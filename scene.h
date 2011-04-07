#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include "../core/threadsafe.h"
#include "smartscroll.h"

class QMouseEvent;
class QScrollBar;

namespace core { class Network; class NetworkDevice; }

namespace gui {

class LanItem;
class RouterItem;
class ComputerItem;
class WireItem;
class PortItem;

class RIRManager;

class Scene : public QGraphicsScene {
	Q_OBJECT
signals:
	void objectChanged(QGraphicsItem* item);
public slots:
	void disconnect(PortItem* port);
	void selectItemInLog(int id);
	void setSizeScene();
public:
	Scene(core::Network* net, RIRManager *manager, SmartScroll* hBar,
		  SmartScroll* vBar,QObject *parent = 0);
	~Scene();

	void addRouter(QPointF pos);
	void addLan(QPointF pos);
	void addWire(PortItem* portS, PortItem* portD);
	void addComputer(QPointF pos);

	int getNumberPort(QGraphicsItem* port);
	ThreadSafe<core::NetworkDevice> getLogicDevice(QGraphicsItem* parentItem);

	void deleteDevice(QGraphicsItem* item);
	void deleteWire(WireItem* wire);
	void deletePort(QGraphicsItem* item);
	void disablePort(PortItem* port);

	void deleteSelectedDevice();

	void scrollScene(QPoint delta);

	void clearNet();
	void selectAll();
private:
	core::Network *net;
	RIRManager *rir_manager;

	//Храню скролбары потому что часто обращаюсь
	SmartScroll* hBar;
	SmartScroll* vBar;
};

} //namespace gui

#endif // SCENE_H
