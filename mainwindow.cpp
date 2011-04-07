#include "mainwindow.h"
#include <QGraphicsItem>
#include <QIntValidator>

#include "routeritem.h"
#include "computeritem.h"
#include "lanitem.h"
#include "wireitem.h"

namespace gui{

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent) {
	setWindowTitle("Aranei WAN Simulator v.0.2");

	net = new core::Network;
	typeStop = net->eventType(core::Network::TimeOver);

	net->registerEventHandler(this);

	rir_manager = new RIRManager(this); // Я надеюсь он сам удалится

	workspace = new Workspace(net, rir_manager, this);
	workspace->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

	createElements();
	createMenu();
	createPropertyBrowser();
	createLogReader();
	createIpNetworkBrowser();

	setCentralWidget(workspace);

	connect(workspace, SIGNAL(objectChanged(QGraphicsItem*)),
			this, SLOT(changePropertyObject(QGraphicsItem*)));

	connect(this, SIGNAL(changeIpNetworkBrowser(QGraphicsItem*)),
			ipNetworkBrowser, SLOT(selectDevice(QGraphicsItem*)));
	connect(logReader, SIGNAL(deviceSelected(int)), workspace,
			SIGNAL(selectItemInLog(int)));

	this->setWindowIcon(QIcon(":icon/aranei.png"));

}

void MainWindow::createIpNetworkBrowser() {
	ipNetworkBrowser = new IPNetworksBrowser(rir_manager);

	dockIPNetworksBrowser = new QDockWidget(trUtf8("IP4 сети"), this);
	dockIPNetworksBrowser->setAllowedAreas(Qt::LeftDockWidgetArea
										 | Qt::RightDockWidgetArea
										 | Qt::TopDockWidgetArea
										 | Qt::BottomDockWidgetArea);
	dockIPNetworksBrowser->setWidget(ipNetworkBrowser);
	addDockWidget(Qt::RightDockWidgetArea, dockIPNetworksBrowser);
}

void MainWindow::createPropertyBrowser(){

	dockPropertyBrowser = new QDockWidget(trUtf8("Свойства элемента"), this);

	propertyBrowser = new AutoPropertyBrowser(this);
	
	dockPropertyBrowser->setAllowedAreas(Qt::LeftDockWidgetArea
										 | Qt::RightDockWidgetArea
										 | Qt::TopDockWidgetArea
										 | Qt::BottomDockWidgetArea);
	
	dockPropertyBrowser->setWidget(propertyBrowser);
	
	addDockWidget(Qt::RightDockWidgetArea, dockPropertyBrowser);

}

void MainWindow::createLogReader(){

	dockLogReader = new QDockWidget(trUtf8("Журнал"), this);

	logReader = new LogReader(net->log(), this);

	dockLogReader->setAllowedAreas(Qt::LeftDockWidgetArea
										 | Qt::RightDockWidgetArea
										 | Qt::TopDockWidgetArea
										 | Qt::BottomDockWidgetArea);
	
	dockLogReader->setWidget(logReader);

	addDockWidget(Qt::BottomDockWidgetArea, dockLogReader);

	dockLogReader->setMinimumSize(100,100);
	dockLogReader->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	logReader->setMinimumSize(100,100);
	logReader->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

void MainWindow::createElements(){
	toolElements= new QToolBar(trUtf8("Устройства"),this);

	toolElements->setAllowedAreas(Qt::LeftToolBarArea
								  | Qt::RightToolBarArea
								  | Qt::TopToolBarArea
								  | Qt::BottomToolBarArea);

	createElementsActions();

	toolElements->addAction(actionWire);
	toolElements->addAction(actionComputer);
	toolElements->addAction(actionLan);
	toolElements->addAction(actionRouter);
	toolElements->setIconSize(QSize(32,32));

	addToolBar(Qt::LeftToolBarArea, toolElements);
}

void MainWindow::createMenu(){
	toolMenu= new QToolBar("Menu",this);

	toolMenu->setAllowedAreas(Qt::LeftToolBarArea
							  | Qt::RightToolBarArea
							  | Qt::TopToolBarArea
							  | Qt::BottomToolBarArea);

	createMenuActions();

	toolMenu->addAction(actionNew);
	toolMenu->addAction(actionOpen);
	toolMenu->addAction(actionSave);
	toolMenu->addAction(actionDeleteItems);
	toolMenu->addSeparator();
	toolMenu->addAction(actionPlay);
	toolMenu->addAction(actionPause);
	toolMenu->addAction(actionStop);
	toolMenu->addWidget(new QLabel(trUtf8("Время выполнения:"),this));
	toolMenu->addWidget(timeRunNet);
	toolMenu->setIconSize(QSize(25,25));

	addToolBar(Qt::TopToolBarArea, toolMenu);
}

void MainWindow::createMenuActions(){
	actionNew = new QAction(trUtf8("Новая сеть"),this);
	actionNew->setIcon(QIcon(":icon/new.png"));
	connect(actionNew, SIGNAL(triggered()), workspace, SLOT(newWorkspace()));

	actionOpen = new QAction(trUtf8("Открыть"),this);
	actionOpen->setIcon(QIcon(":icon/open.png"));

	actionSave = new QAction(trUtf8("Сохранить"),this);
	actionSave->setIcon(QIcon(":icon/save.png"));

	actionDeleteItems = new QAction(trUtf8("Удалить выделенные элементы"),this);
	actionDeleteItems->setIcon(QIcon(":icon/delete_items.png"));
	connect(actionDeleteItems, SIGNAL(triggered()),workspace, SLOT(deleteSelItems()));

	actionPlay = new QAction(trUtf8("Запустить симуляцию"),this);
	actionPlay->setIcon(QIcon(":icon/play.png"));
	connect(actionPlay, SIGNAL(triggered()), this, SLOT(startRunNet()));

	actionPause = new QAction(trUtf8("Приостановить симуляцию"),this);
	actionPause->setIcon(QIcon(":icon/pause.png"));
	connect(actionPause, SIGNAL(triggered()), this, SLOT(pauseRunNet()));
	actionPause->setDisabled(true);

	actionStop = new QAction(trUtf8("Остановить симуляцию"),this);
	actionStop->setIcon(QIcon(":icon/stop.png"));
	connect(actionStop, SIGNAL(triggered()), this, SLOT(stopRunNet()));
	actionStop->setDisabled(true);

	timeRunNet = new QDoubleSpinBox(this);
	timeRunNet->setRange(0, INT_MAX);
	timeRunNet->setSingleStep(0.001);
	timeRunNet->setDecimals(3);
}

void MainWindow::createElementsActions(){
	actionWire = new QAction(trUtf8("Провод"),this);
	actionWire->setIcon(QIcon(":icon/wire.png"));
	connect(actionWire, SIGNAL(triggered()),workspace, SLOT(createWire()));

	actionRouter = new QAction(trUtf8("Маршрутизатор"),this);
	actionRouter->setIcon(QIcon(":icon/router.png"));
	connect(actionRouter, SIGNAL(triggered()), workspace, SLOT(createRouter()));

	actionLan = new QAction(trUtf8("Локальная сеть"),this);
	actionLan->setIcon(QIcon(":icon/lan.png"));
	connect(actionLan, SIGNAL(triggered()),workspace, SLOT(createLan()));

	actionComputer = new QAction(trUtf8("Компьютер"),this);
	actionComputer->setIcon(QIcon(":icon/computer.png"));
	connect(actionComputer, SIGNAL(triggered()),workspace, SLOT(createComputer()));
}

//Устанавливаем свойство выбранного обьекта в браузере свойств
void MainWindow::changePropertyObject(QGraphicsItem *item){
	//Выставляем валидаторы для браузеров свойств
	//Показываем лог устройства и передаем IP4 браузеру

	if(item){
		if(item->data(5).toString() == "Port")
			item = item->parentItem();

		emit changeIpNetworkBrowser(item);

		QObject* obj = dynamic_cast<QObject*>(item);
		propertyBrowser->setObject(obj);

		if(item->data(1).toString() == "Router"){
			propertyBrowser->setValidator("Width", new QIntValidator(50, INT_MAX, propertyBrowser));
			propertyBrowser->setValidator("Height", new QIntValidator(20, INT_MAX, propertyBrowser));
			propertyBrowser->setValidator("numberPorts", new QIntValidator(0, 16, propertyBrowser));

			RouterItem* r = qgraphicsitem_cast<RouterItem*>(item);
			logReader->showDevice(r->getId());
		}
		if(item->data(2).toString() == "LAN"){
			propertyBrowser->setValidator("Width", new QIntValidator(50, INT_MAX, propertyBrowser));
			propertyBrowser->setValidator("Height", new QIntValidator(40, INT_MAX, propertyBrowser));
			propertyBrowser->setValidator("sizeLan", new QIntValidator(0, 0, propertyBrowser));

			LanItem* l = qgraphicsitem_cast<LanItem*>(item);
			logReader->showDevice(l->getId());
		}
		if(item->data(3).toString() == "Computer"){
			propertyBrowser->setValidator("Width", new QIntValidator(30, INT_MAX, propertyBrowser));
			propertyBrowser->setValidator("Height", new QIntValidator(30, INT_MAX, propertyBrowser));

			ComputerItem* c = qgraphicsitem_cast<ComputerItem*>(item);
			logReader->showDevice(c->getId());
		}
		if(item->data(4).toString() == "Wire"){
			propertyBrowser->setValidator("Noise", new QIntValidator(0, 100, propertyBrowser));
			propertyBrowser->setValidator("Length", new QDoubleValidator(1.0, std::numeric_limits<double>::max(),
																		 2, propertyBrowser));

			WireItem* w = qgraphicsitem_cast<WireItem*>(item);
			logReader->showDevice(w->getId());
		}
	}
	else{
		//Если выбрали сцены изменим браузеры свойст и логов
		propertyBrowser->setObject(0);
		logReader->goHome();
	}
}

//Запуск сети, блокируем кнопки
void MainWindow::startRunNet(){
	actionPause->setEnabled(true);
	actionStop->setEnabled(true);

	actionNew->setDisabled(true);
	actionDeleteItems->setDisabled(true);
	actionPlay->setDisabled(true);

	actionWire->setDisabled(true);
	actionLan->setDisabled(true);
	actionComputer->setDisabled(true);
	actionRouter->setDisabled(true);

	propertyBrowser->setDisabled(true);

	net->start(0.003,0.005,timeRunNet->value());
	workspace->runNetTimer();
}

//Приостановка работы сети
void MainWindow::pauseRunNet(){
	actionPlay->setEnabled(true);

	actionPause->setDisabled(true);

	net->pause();
}

//Остановка работы сети, разблокируем кнопки
void MainWindow::stopRunNet(){
	actionPause->setDisabled(true);
	actionStop->setDisabled(true);

	actionNew->setEnabled(true);
	actionDeleteItems->setEnabled(true);
	actionPlay->setEnabled(true);

	actionWire->setEnabled(true);
	actionLan->setEnabled(true);
	actionComputer->setEnabled(true);
	actionRouter->setEnabled(true);
	propertyBrowser->setEnabled(true);

	net->stop();
	workspace->stopNetTimer();
}

bool MainWindow::event(QEvent *event){
	if(typeStop == event->type()){
		stopRunNet();
		event->setAccepted(true);
		return true;
	}

	QMainWindow::event(event);

	return false;
}

MainWindow::~MainWindow(){
	delete net;
}

} //namespace gui
