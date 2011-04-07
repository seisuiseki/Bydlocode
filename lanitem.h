#ifndef LANITEM_H
#define LANITEM_H

#include "lan.h"
#include "../core/threadsafe.h"

#include <QGraphicsItem>
#include <QPainter>

namespace gui{

class LanItem : public QObject, public QGraphicsItem{

Q_OBJECT
Q_INTERFACES(QGraphicsItem)

Q_PROPERTY(QSize size READ getSize WRITE setSize)
Q_PROPERTY(int sizeLan READ getSizeLan)

private:
	ThreadSafe<logic::Lan> lan;

	qreal angle;
	int id;
	QSize size;

	QSize getSize() const { return size; }
	void setSize(QSize newSize);
	int getSizeLan() const { return childItems().size(); }
public:
	void setSizePorts() { lan->resizePort(childItems().size()); }

	void setId(int newId) { id = newId; }
	int getId() { return id; }

	LanItem(int id=0, QSize size = QSize(75, 45));

	ThreadSafe<logic::Lan> getLan(){ return lan; }
protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	QRectF boundingRect() const;
	void wheelEvent(QGraphicsSceneWheelEvent *event);
};

} //namespace gui

#endif // LANITEM_H
