#ifndef WIREITEM_H
#define WIREITEM_H

#include "wire.h"
#include "../core/threadsafe.h"

#include "portitem.h"
#include <QGraphicsLineItem>
#include <QPainter>

namespace gui{

class WireItem : public QObject, public QGraphicsLineItem{

Q_OBJECT
Q_INTERFACES(QGraphicsItem)

Q_PROPERTY(int Noise READ getWireNoise WRITE setWireNoise)
Q_PROPERTY(double Length READ getWireLength WRITE setWireLength)
private:
	ThreadSafe<logic::Wire> wire;

	int id;

	//Рома накосячил с названием переменной
	void setWireLength(double length){ wire->lenght = length; }
	double getWireLength(){ return wire->lenght; }

	void setWireNoise(int noise){ wire->noise = noise; }
	int getWireNoise(){ return wire->noise; }
public:
	void setId(int newId) { id = newId; }
	int getId() { return id; }

	PortItem* portS;
	PortItem* portD;

	WireItem(PortItem* portS, PortItem* portD, int id=0);

	ThreadSafe<logic::Wire> getWire(){ return wire; }
protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

} //namespace gui

#endif // WIREITEM_H


