#include "scene.h"
#include "mainwindow.h"

#include "../core/networkdevice.h"

#include "lanitem.h"
#include "routeritem.h"
#include "computeritem.h"
#include "wireitem.h"
#include "portitem.h"
#include "rirmanager.h"

#include <QMouseEvent>
#include <QScrollBar>

namespace gui{


Scene::Scene(core::Network* net, RIRManager *manager, SmartScroll* hBar,
			 SmartScroll* vBar,QObject *parent)
	: QGraphicsScene(parent), net(net), rir_manager(manager), hBar(hBar), vBar(vBar) {

	setItemIndexMethod(NoIndex);
	setSceneRect(-210,-150, 420, 300);
}

void Scene::addRouter(QPointF pos){
	RouterItem* router = new RouterItem();
	router->setData(1, "Router");
	router->setPos(pos);

	//Подключаем сигнал для удаления порта
	connect(router, SIGNAL(portDeleted(PortItem*)),this, SLOT(disconnect(PortItem*)));

	//Регистрируем в сети, даем ID
	net->registerDevice(router->getRouter());
	router->setId(router->getRouter()->id());

	addItem(router);
}

void Scene::addLan(QPointF pos){
	LanItem* lan = new LanItem();
	lan->setData(2, "LAN");
	lan->setPos(pos);

	net->registerDevice(lan->getLan());
	lan->setId(lan->getLan()->id());

	addItem(lan);
}

void Scene::addComputer(QPointF pos){
	ComputerItem* computer = new ComputerItem();
	computer->setData(3, "Computer");
	computer->setPos(pos);

	//Регистрируем в сети, даем ID
	net->registerDevice(computer->getComputer());
	computer->setId(computer->getComputer()->id());

	PortItem *port = new PortItem(computer);
	port->setVisible(false);
	port->setData(5, "Port");
	port->setPos(0,0.4*computer->getSizeH());

	addItem(computer);
}

void Scene::addWire(PortItem* portS, PortItem* portD){
	WireItem* wire = new WireItem(portS, portD);
	wire->setData(4, "Wire");

	net->registerDevice(wire->getWire());
	wire->setId(wire->getWire()->id());

	net->connect(getLogicDevice(portS->parentItem()),
				 getNumberPort(portS),
				 wire->getWire(), 0);
	net->connect(getLogicDevice(portD->parentItem()),
				 getNumberPort(portD),
				 wire->getWire(), 1);
	// Уведомляем RIRManager о новом соединении
	rir_manager->connect(portS->parentItem(), getNumberPort(portS),
						 portD->parentItem(), getNumberPort(portD));

	wire->setLine(QLineF(portS->mapToScene(0,0), portD->mapToScene(0,0)));

	addItem(wire);
}

int Scene::getNumberPort(QGraphicsItem* port){

	QList<QGraphicsItem*> list = port->parentItem()->childItems();
	return list.indexOf(port);
}

ThreadSafe<core::NetworkDevice> Scene::getLogicDevice(QGraphicsItem* parentItem){
	if(parentItem->data(1).toString() == "Router"){
		RouterItem* r = qgraphicsitem_cast<RouterItem*>(parentItem);
		return r->getRouter();
	}
	if(parentItem->data(2).toString() == "LAN"){
		LanItem* l = qgraphicsitem_cast<LanItem*>(parentItem);
		return l->getLan();
	}
	if(parentItem->data(3).toString() == "Computer"){
		ComputerItem* c = qgraphicsitem_cast<ComputerItem*>(parentItem);
		return c->getComputer();
	}

	return ThreadSafe<core::NetworkDevice>(); //Возвращаем по умолчанию
}

void Scene::deleteSelectedDevice(){
	if(!selectedItems().isEmpty()){
		foreach(QGraphicsItem* item, selectedItems()){
			if(item->data(4).toString() == "Wire"){
				WireItem* wire = qgraphicsitem_cast<WireItem*>(item);
				deleteWire(wire);
			}
		}
		foreach(QGraphicsItem* item, selectedItems()){
			deleteDevice(item);
		}
		emit objectChanged(0);
	}
}

void Scene::disconnect(PortItem* port){
	foreach(QGraphicsItem* item, items()){
		if(item->data(4).toString() == "Wire"){
			WireItem* wire = qgraphicsitem_cast<WireItem*>(item);
			if(port == wire->portS || port == wire->portD){
				deleteWire(wire);
			}
		}
	}
}

void Scene::deleteWire(WireItem* wire){

	net->disconnect(getLogicDevice(wire->portS->parentItem()),
					getNumberPort(wire->portS),
					wire->getWire(), 0);
	net->disconnect(getLogicDevice(wire->portD->parentItem()),
					getNumberPort(wire->portD),
					wire->getWire(), 1);

	// Уведомляем RIRManager о разъединении
	rir_manager->disconnect(wire->portS->parentItem(), getNumberPort(wire->portS),
							wire->portD->parentItem(), getNumberPort(wire->portD));

	disablePort(wire->portS);
	disablePort(wire->portD);

	//Удаляем из сети сам провод
	removeItem(wire);
	net->removeDevice(wire->getWire());
	delete wire;
}

void Scene::deletePort(QGraphicsItem *item){
	PortItem* port = qgraphicsitem_cast<PortItem*>(item);
	disconnect(port);

	removeItem(port);
	delete port;
}

void Scene::deleteDevice(QGraphicsItem *item){
	//Если мы удаляем выделенные элементы
	//У очистки сцены порты уже будут удалены
	foreach(QGraphicsItem* portIt, item->childItems()){
		PortItem* port = qgraphicsitem_cast<PortItem*>(portIt);
		disconnect(port);

		//Если это Lan, то не удаляем порт, так как
		//уже удалили
		if(!(item->data(2).toString() == "LAN")){
				removeItem(port);
				delete port;
		}
	}

	//Удаляем устройство со сцены и удаляем логически
	removeItem(item);
	net->removeDevice(getLogicDevice(item));
	delete item;
}

void Scene::disablePort(PortItem* port){
	if(port->parentItem()->data(2).toString() == "LAN"){
		LanItem* l = qgraphicsitem_cast<LanItem*>(port->parentItem());

		removeItem(port);
		delete port;
		//Удалили у Lan порт, уменьшим количество в
		//логическом Lan
		l->setSizePorts();
	}
	else{
		port->isConnect = false;
		port->update();
	}
}

void Scene::setSizeScene(){

	QRect rectView = views().first()->rect();

	rectView.setRight(rectView.right() - vBar->width() - 3);
	rectView.setBottom(rectView.bottom() - hBar->height() - 3);

	//Берем сначало размеры вьюва, потом берем размеры вьюва в координатах сцены
	QRectF rectScene = views().first()->mapToScene(rectView).boundingRect();

	QPointF posMin(rectScene.topLeft());
	QPointF posMax(rectScene.bottomRight());

	//Проходим все итемы и смотрим что выходит за сцену

	foreach(QGraphicsItem *item, items()){
		if(!item->parentItem()){
			QPointF curPosItem = item->pos();
			QRectF rectIt = item->boundingRect();

			QPointF tl = curPosItem + (rectIt.topLeft()+QPoint(-10,-10));
			QPointF rb = curPosItem + (rectIt.bottomRight()+QPoint(10,10));

			posMin.setY(qMin(posMin.y(), tl.y()));
			posMin.setX(qMin(posMin.x(), tl.x()));
			posMax.setY(qMax(posMax.y(), rb.y()));
			posMax.setX(qMax(posMax.x(), rb.x()));
		}
	}

	setSceneRect(QRectF(posMin, posMax));
}

void Scene::scrollScene(QPoint delta){
	int hParam = hBar->value() + ((views().first()->isRightToLeft() ? delta.x() : -delta.x()));
	int vParam = vBar->value() - delta.y();

	//Если мы выходим за границы сцены при скроллинге, то увеличиваем её

	if(hParam > hBar->maximum()){
		hBar->setMaximum(hParam);
	}
	if(hParam < hBar->minimum()){
		hBar->setMinimum(hParam);
	}
	if(vParam > vBar->maximum()){
		vBar->setMaximum(vParam);
	}
	if(vParam < vBar->minimum()){
		vBar->setMinimum(vParam);
	}

	hBar->setValue(hParam);
	vBar->setValue(vParam);
}

void Scene::clearNet(){
	//Удаляем сначало провода
	foreach(QGraphicsItem* item, items()){
		if(item->data(4).toString() == "Wire"){
			WireItem* wire = qgraphicsitem_cast<WireItem*>(item);
			deleteWire(wire);
		}
	}
	//Потом все порты
	foreach(QGraphicsItem* item, items()){
		if(item->data(5).toString() == "Port"){
			deletePort(item);
		}
	}
	//И только потом устройства
	foreach(QGraphicsItem* item, items()){
		deleteDevice(item);
	}

	emit objectChanged(0);
}

void Scene::selectAll(){
	foreach(QGraphicsItem* item, items()){
		item->setSelected(true);
	}
}

void Scene::selectItemInLog(int id){
	foreach(QGraphicsItem* item, items()){
		if(!item->parentItem()){
			if(getLogicDevice(item)->id() == id){
				clearSelection();
				item->setSelected(true);
				emit objectChanged(item);
				return;
			}
		}
	}
}

Scene::~Scene(){
	//clearNet();
}


} //namespace gui
