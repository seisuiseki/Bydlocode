#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "autopropertybrowser.h"
#include "logreader.h"
#include "workspace.h"
#include "../core/network.h"
#include "ipnetworksbrowser.h"
#include "rirmanager.h"

#include <limits>

#include <QtGui/QMainWindow>
#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <QtGui/QPalette>
#include <QtGui/QIcon>
#include <QtGui/QDockWidget>
#include <QtGui/QGraphicsView>
#include <QtGui/QLayout>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLabel>

namespace gui{

class MainWindow : public QMainWindow{
	Q_OBJECT
signals:
	void changeIpNetworkBrowser(QGraphicsItem* item);
public slots:
	void changePropertyObject(QGraphicsItem* item);
	void startRunNet();
	void pauseRunNet();
	void stopRunNet();
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
	core::Network *net;
	QEvent::Type typeStop;
	Workspace* workspace;

	LogReader* logReader;

	AutoPropertyBrowser* propertyBrowser;

	RIRManager *rir_manager;
	IPNetworksBrowser *ipNetworkBrowser;

	QToolBar* toolElements;
	QToolBar* toolMenu;
	QDockWidget* dockPropertyBrowser;
	QDockWidget* dockLogReader;
	QDockWidget* dockIPNetworksBrowser;

	QAction* actionRouter;
	QAction* actionLan;
	QAction* actionComputer;
	QAction* actionWire;

	QAction* actionNew;
	QAction* actionOpen;
	QAction* actionSave;
	QAction* actionDeleteItems;

	QAction* actionPlay;
	QAction* actionPause;
	QAction* actionStop;
	QDoubleSpinBox* timeRunNet;

	void createElementsActions();
	void createMenuActions();

	void createElements();
	void createMenu();
	void createPropertyBrowser();
	void createLogReader();
	void createIpNetworkBrowser();
protected:
	bool event(QEvent *event);
};

} //namespace gui

#endif // MAINWINDOW_H
