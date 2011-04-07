#ifndef PORTITEM_H
#define PORTITEM_H

#include <QGraphicsItem>
#include <QPainter>

namespace gui{

class PortItem : public QGraphicsItem{

private:
	int size;
public:
	bool isConnect;
	PortItem(QGraphicsItem *parent = 0) : QGraphicsItem(parent),
										size(5), isConnect(false){}
protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	QRectF boundingRect() const;
};

} //namespace gui

#endif // PORTITEM_H
