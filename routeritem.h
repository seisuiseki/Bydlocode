#ifndef ROUTERITEM_H
#define ROUTERITEM_H

#include "router.h"
#include "../core/threadsafe.h"

#include <portitem.h>
#include <QGraphicsItem>
#include <QPainter>

namespace gui{

class RouterItem : public QObject, public QGraphicsItem{

Q_OBJECT
Q_INTERFACES(QGraphicsItem)

Q_PROPERTY(QSize size READ getSize WRITE setSize)
Q_PROPERTY(int numberPorts READ getSizeRouter WRITE changeNumberPorts)
Q_PROPERTY(logic::RoutingTable table READ getRoutingTable WRITE setRoutingTable)

signals:
	void portDeleted(PortItem* port);
private:
	ThreadSafe<logic::Router> router;

	qreal angle;
	int id;
	QSize size;
	int numberPorts;

	QSize getSize(){ return size; }
	void setSize(QSize newSize);
	int getSizeRouter(){ return numberPorts; }

	void setPortsPosition();
	void changeNumberPorts(int newSize);
	logic::RoutingTable getRoutingTable() {return router->table; }
	void setRoutingTable(logic::RoutingTable table) { router->table = table; }
public:
	void setId(int newId) { id = newId; }
	int getId() { return id; }

	RouterItem(int id=0, QSize size = QSize(90, 20));

	ThreadSafe<logic::Router> getRouter(){ return router; }
protected:
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	void wheelEvent(QGraphicsSceneWheelEvent *event);
};

} //namespace gui

#endif // ROUTERITEM_H
