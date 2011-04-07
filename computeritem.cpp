#include "computeritem.h"
#include <iostream>
#include <QGraphicsSceneWheelEvent>

namespace gui{

ComputerItem::ComputerItem(int id, QSize size)
	:computer(new logic::Computer), angle(0), id(id), size(size)
{
	setFlag(ItemIsMovable);
	setFlag(ItemSendsGeometryChanges);
	setFlag(ItemIsSelectable);
	setFlag(ItemIsFocusable);
}

void ComputerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
						 QWidget *widget){
	Q_UNUSED(option);
	Q_UNUSED(widget);

	setZValue(isSelected() ? 6 : 5);


	painter->setBrush(Qt::white);
	painter->setPen(QPen(Qt::black, scale()));

	qreal sizeW = size.width();
	qreal sizeH = size.height();

	painter->drawRoundedRect(-sizeW/2, -sizeH/2,
							 sizeW, sizeH*0.6,
							 sizeW/20, sizeH/20);
	painter->drawRect(-sizeW/2+sizeW/10,
					  -sizeH/2+sizeW/10,
					  sizeW-sizeW/5,
					  0.6*sizeH-sizeW/5);
	painter->drawRect(-0.2*sizeW/2, 0.2*sizeH/2,
					  0.2*sizeW, 0.2*sizeH);
	painter->drawRect(-sizeW/2, 0.3*sizeH,
					  sizeW, 0.2*sizeH);
	if(isSelected()){
		painter->setPen(QPen(QColor(0, 0, 30, 140), scale(), Qt::DotLine));
		painter->setBrush(QColor(0, 0, 255, 30));
		painter->drawRect(-sizeW/2-5, -sizeH/2-5,
						  sizeW+10, sizeH+10);
	}
}

QRectF ComputerItem::boundingRect() const{
	qreal penWidth = scale();
	return QRectF(-size.width()/2+penWidth/2, -size.height()/2+penWidth/2,
				  size.width()+penWidth, size.height()+penWidth);
}

void ComputerItem::wheelEvent(QGraphicsSceneWheelEvent *event){
	angle += 3*event->delta()/8;
	setRotation((int)angle % 360);
}

void ComputerItem::setSize(QSize newSize){
	//Устанавливаем размеры не ниже минимального
	if(size != newSize){
		size = newSize;
		update();
	}
}

} //namespace gui
