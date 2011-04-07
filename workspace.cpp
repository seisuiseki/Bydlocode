#include "mainwindow.h"
#include "workspace.h"
#include "scene.h"
#include "smartscroll.h"

#include <math.h>
#include <QMessageBox>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QGraphicsSceneWheelEvent>
#include <QScrollBar>

#include "portitem.h"
#include "lanitem.h"
#include "wireitem.h"

namespace gui{

const int scrollStep = 10;
const int wheelScaleStep = 120;

Workspace::Workspace(core::Network* net, RIRManager *manager, QWidget *parent)
	: QGraphicsView(parent), net(net), deviceForCreate(empty){

	curLan = QPixmap(":images/lan.png");
	curRouter = QPixmap(":images/router.png");
	curWire = QPixmap(":images/wire.png");
	curComputer = QPixmap(":images/computer.png");

	portS = 0;
	portD = 0;

	setAlignment(Qt::AlignCenter);
	setCacheMode(CacheBackground);
	setTransformationAnchor(AnchorUnderMouse);
	setViewportUpdateMode(FullViewportUpdate);
	setRenderHints(QPainter::Antialiasing);
	setResizeAnchor(AnchorUnderMouse);

	netStarted = false;

	SmartScroll* vBar = new SmartScroll();
	setVerticalScrollBar(vBar);
	SmartScroll* hBar = new SmartScroll();
	setHorizontalScrollBar(hBar);

	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	scene = new Scene(net, manager, hBar, vBar,this);

	setScene(scene);

	connect(scene, SIGNAL(objectChanged(QGraphicsItem*)),
			this, SIGNAL(objectChanged(QGraphicsItem*)));

	connect(vBar, SIGNAL(actionComplete()), scene, SLOT(setSizeScene()));
	connect(hBar, SIGNAL(actionComplete()), scene, SLOT(setSizeScene()));

	QObject::connect(&wireTimer, SIGNAL(timeout()), scene, SLOT(update()));
	QObject::connect(&netTimer, SIGNAL(timeout()), scene, SLOT(update()));

	connect(this, SIGNAL(selectItemInLog(int)), scene, SLOT(selectItemInLog(int)));
}

void Workspace::createLan(){
	setCursor(curLan);
	deviceForCreate = lan;
}

void Workspace::createWire(){
	setCursor(curWire);
	deviceForCreate = wireS;
}

void Workspace::createRouter(){
	setCursor(curRouter);
	deviceForCreate = router;
}

void Workspace::createComputer(){
	setCursor(curComputer);
	deviceForCreate = computer;
}

void Workspace::deleteSelItems(){
	scene->deleteSelectedDevice();
}

void Workspace::newWorkspace(){
		scene->clearNet();
}

void Workspace::runNetTimer(){
	if(netTimer.timerId() == -1){
		if(wireTimer.timerId() != -1)
			wireTimer.stop();
		netTimer.start(1000/33);
		netStarted = true;

		//На случай если нажали перед созданием на рабочей области
		//Сбрасываем перед анимацией все переменные параметры для
		//создания
		deviceForCreate = empty;
		setCursor(Qt::ArrowCursor);
		portS = 0;
		portD = 0;
	}
}

void Workspace::stopNetTimer(){
	netStarted = false;
	netTimer.stop();
	scene->update();
}

void Workspace::mousePressEvent(QMouseEvent* event){

	//Если нажали правую кнопку мыши
	if (event->button() == Qt::RightButton){
		//Сбросить создание новых устройств
		if(deviceForCreate != empty){
			setCursor(Qt::ArrowCursor);
			if(portS)
				scene->disablePort(portS);
			if(portD)
				scene->disablePort(portD);

			deviceForCreate = empty;
		}

		//Начинаем таскание по сцене
		lastMousePos = event->globalPos();
		setCursor(Qt::ClosedHandCursor);
		scrollHandDrag = true;

		//Чтобы не работало выделение
		setDragMode(NoDrag);
	}

	//Если мы прекратили перемещение по сцене
	if (event->button() == Qt::LeftButton && scrollHandDrag){
		scrollHandDrag = false;
		setCursor(Qt::ArrowCursor);
	}

	if (event->button() == Qt::LeftButton && deviceForCreate != empty){
		createDevice(event->pos());
	}

	//Если нажали просто левую кнопку, без создания элемента
	if(event->button() == Qt::LeftButton){
		setDragMode(RubberBandDrag);

		//Передаем обьект браузеру свойств
		emit objectChanged(itemAt(event->pos()));
	}

	QGraphicsView::mousePressEvent(event);
}


void Workspace::mouseMoveEvent(QMouseEvent *event){

	//Обновляем размеры сцены при переноске элемента
	//Только если мышь владеет элементом
	if(scene->mouseGrabberItem() && !scrollHandDrag){
		scene->setSizeScene();
		scene->update(scene->mouseGrabberItem()->boundingRect());
	}

	//Запоминаем положение для рисования провода
	if(!netStarted)
		cursor = event->pos();

	//Прокручиваем сцену
	if(event->buttons() == Qt::RightButton && scrollHandDrag){
		scene->scrollScene(event->globalPos() - lastMousePos);
		scene->setSizeScene();
		lastMousePos = event->globalPos();
	}

	QGraphicsView::mouseMoveEvent(event);
}

void Workspace::mouseReleaseEvent(QMouseEvent *event){
	//Меняем курсор
	if(event->button() == Qt::RightButton && scrollHandDrag){
		setCursor(Qt::OpenHandCursor);
	}

	QGraphicsView::mouseReleaseEvent(event);
}

void Workspace::wheelEvent(QWheelEvent *event){

	//Если нажата клавиша контрол, крутим элемент
	if(event->modifiers() && Qt::Key_Control){
		QGraphicsItem* item = scene->focusItem();
		if(item){
			//Формируем сообщение для отправки итему
			QGraphicsSceneWheelEvent wheelEvent(QEvent::GraphicsSceneWheel);
			wheelEvent.setWidget(viewport());
			wheelEvent.setScenePos(item->pos());
			wheelEvent.setScreenPos(mapFromScene(item->pos()));
			wheelEvent.setButtons(event->buttons());
			wheelEvent.setModifiers(event->modifiers());
			wheelEvent.setDelta(event->delta());
			wheelEvent.setAccepted(false);
			scene->sendEvent(item, &wheelEvent);
		}
	}
	else{
		//Масштабируем сцену
		scaleView(event->delta());
		scene->setSizeScene();
	}
}

void Workspace::keyPressEvent(QKeyEvent *event){
	switch(event->key()){
		//Нажата клавиша Delete, удалим
		//выделенные элементы
		case Qt::Key_Delete:
		{
			//Если сеть не работает, то можно удалить
			if(net->state() == core::Network::Stopped)
				scene->deleteSelectedDevice();
			break;
		}
		case Qt::Key_F5:
		{
			scene->update();
			break;
		}
		//Если нажали Ctrl+A, то выделить всё
		case Qt::Key_A:
		{
			if(event->modifiers() && Qt::Key_Control)
				scene->selectAll();
			break;
		}
		//Устанавливаем фокус на элемент под курсором
		case Qt::Key_Control:
			scene->setFocusItem(itemAt(cursor));
			break;
		//Перемезаемся по сцене стрелочками
		case Qt::Key_Up:
			scene->scrollScene(QPoint(0,scrollStep));
			scene->setSizeScene();
			break;
		case Qt::Key_Down:
			scene->scrollScene(QPoint(0,-scrollStep));
			scene->setSizeScene();
			break;
		case Qt::Key_Left:
			scene->scrollScene(QPoint(scrollStep,0));
			scene->setSizeScene();
			break;
		case Qt::Key_Right:
			scene->scrollScene(QPoint(-scrollStep,0));
			scene->setSizeScene();
			break;
		//Увеличение и уменьшение сцены
		case Qt::Key_Plus:
			if(event->modifiers() && Qt::Key_Control){
				setTransformationAnchor(AnchorViewCenter);
				scaleView(wheelScaleStep);
				scene->setSizeScene();
				setTransformationAnchor(AnchorUnderMouse);
			}
			break;
		case Qt::Key_Minus:
			if(event->modifiers() && Qt::Key_Control){
				setTransformationAnchor(AnchorViewCenter);
				scaleView(-wheelScaleStep);
				scene->setSizeScene();
				setTransformationAnchor(AnchorUnderMouse);
			}
			break;
	}
	QGraphicsView::keyPressEvent(event);
}

void Workspace::keyReleaseEvent(QKeyEvent *event){
	switch(event->key()){
		//Снимаем фокус с элемента после прокрутки
		case Qt::Key_Control:
			scene->clearFocus();
			break;
	}

	QGraphicsView::keyReleaseEvent(event);
}

void Workspace::paintEvent(QPaintEvent *event){
	QGraphicsView::paintEvent(event);

	//Рисуем провод
	if(deviceForCreate == wireD){
		QPainter *painter = new QPainter();
		painter->begin(viewport());
		painter->setRenderHint(QPainter::Antialiasing);

		painter->setPen(QPen(Qt::black, this->transform().m11()));
		painter->drawLine(mapFromScene(portS->mapToScene(0,0)) ,cursor);

		painter->end();
	}

	//Рисуем пакеты
	if(netStarted)
		drawPackets();
}

//Масштабирование вида
void Workspace::scaleView(int delta){
	qreal scale = pow((double)2, delta / 240.0);
	qreal factor = matrix().scale(scale, scale).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.07 || factor > 100)
		return;

	//Чтобы при уменьшении не прыгало

	QRect rectView = rect();
	scene->setSceneRect(-(rectView.width()/0.001)/2, -(rectView.height()/0.001)/2,
				 rectView.width()/0.001, rectView.height()/0.001);

	this->scale(scale, scale);
}

//Задача: Найти подключаемый порт
PortItem* Workspace::getItemnPort(QPoint pos){
	QGraphicsItem* item = itemAt(pos);
	if(item){
		//Если мы попали в роутер, но не в его порт, то подключаемся
		//к первому свободному
		if(item->data(1).toString() == "Router"){
			foreach(QGraphicsItem* it, item->childItems()){
				PortItem* port = qgraphicsitem_cast<PortItem*>(it);
				if(!port->isConnect){
					port->isConnect = true;
					port->update();
					return port;
				}
			}
			QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("У устройства нету свободных портов."));
		}
		//Если попали в локальную сеть
		if(item->data(2).toString() == "LAN"){
			LanItem* l = qgraphicsitem_cast<LanItem*>(item);
			//Если мы соединяем Lan с Lan, то мы не соеденяем их
			if(portS)
				if(portS->parentItem()->data(2).toString() == "LAN")
					return 0;
			//В противном случаи подключаем
			PortItem *port = new PortItem(item);
			port->setVisible(false);
			port->isConnect = true;
			port->setData(5, "Port");

			l->setSizePorts();

			return port;
		}
		//Соединяем рабочие станции
		if(item->data(3).toString() == "Computer"){

			PortItem* port = qgraphicsitem_cast<PortItem*>(item->childItems().first());
			//если к чему-то подключен, отключаем
			if(port->isConnect){
				scene->disconnect(port);
			}

			port->isConnect = true;
			//port->update();
			return port;
		}
		//Соединяем порты
		if(item->data(5).toString() == "Port"){
			PortItem* port = qgraphicsitem_cast<PortItem*>(item);

			//если к чему-то подключен, отключаем
			if(port->isConnect){
				scene->disconnect(port);
			}

			port->isConnect = true;
			//port->update();
			return port;
		}
	}
	return 0; //Не над итемом
}

//Проходим все устройства, находим провод
//Смотрим все инфо о подсветке
//Используем параметрическое уровнения исходя из
//Прогресса находим положение пакета на прямой
void Workspace::drawPackets(){

	QPainter *painter = new QPainter();
	painter->begin(viewport());
	painter->setRenderHint(QPainter::Antialiasing);

	qreal _scale = this->transform().m11();

	foreach(QGraphicsItem* item, items()){

		WireItem* wire;
		PortItem* outPort;
		PortItem* inPort;
		QPoint outPortPos, inPortPos;
		double x, y;

		if(item->data(4).toString() == "Wire"){
			wire = qgraphicsitem_cast<WireItem*>(item);
			foreach(logic::IMoveFrame::LightInfo packInfo,wire->getWire()->getLightInfo()){

				outPort = packInfo.outPort ? wire->portS: wire->portD;
				inPort = packInfo.inPort ? wire->portS : wire->portD;

				outPortPos = mapFromScene(outPort->mapToScene(0,0));
				inPortPos = mapFromScene(inPort->mapToScene(0,0));

				x = outPortPos.x() + (inPortPos.x() - outPortPos.x()) * packInfo.progress;
				y = outPortPos.y() + (inPortPos.y() - outPortPos.y()) * packInfo.progress;

				painter->setBrush(QBrush(packInfo.color));
				painter->drawEllipse(x-4*_scale, y-4*_scale,
									 8*_scale, 8*_scale);
			}
		}
	}

	painter->end();
}

//Создаем устройство по левому щелчку мыши, если
//есть что создавать
void Workspace::createDevice(QPoint pos){
	switch(deviceForCreate){
		//Создаем компьютер на указанном месте
		case computer:
			scene->addComputer(mapToScene(pos));
			break;
		//Если выбрано создать провод, то сначало
		//Смотрим есть ли элемент по мышью, если есть
		//то включаем рисование провода за курсором мыши
		case wireS:
			portS = getItemnPort(pos);
			if(portS){
				deviceForCreate = wireD;
				cursor = pos;
				wireTimer.start(1000/33);
			}
			break;
		//Создаем провод если выбранное второе устройство не первое
		//и если у них не одинаковый родитель(роутер)
		//Иначе сбрасываем порты на создание.
		case wireD:
			portD = getItemnPort(pos);
			if(portD){
				if((portS != portD) && (portS->parentItem()!=portD->parentItem())){
						scene->addWire(portS, portD);
				}
				else{
					scene->disablePort(portS);
					scene->disablePort(portD);
				}
			}
			else{
				scene->disablePort(portS);
			}
			portS = 0;
			portD = 0;
			wireTimer.stop();
			deviceForCreate = empty;
			break;
		//Создаем сеть
		case lan:
			scene->addLan(mapToScene(pos));
			break;
		//Создаем роутер
		case router:
			scene->addRouter(mapToScene(pos));
			break;
		default:
			break;
	}


	if(deviceForCreate != wireD){
		setCursor(Qt::ArrowCursor);
		deviceForCreate = empty;
	}
}

} //namespace gui
