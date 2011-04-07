#include "lanitem.h"

#include <QGraphicsSceneWheelEvent>

namespace gui{

LanItem::LanItem(int id, QSize size)
	: lan(new logic::Lan(0)), angle(0), id(id), size(size)
{
	setFlag(ItemIsMovable);
	setFlag(ItemSendsGeometryChanges);
	setFlag(ItemIsSelectable);
	setFlag(ItemIsFocusable);
}

void LanItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
						 QWidget *widget){
	Q_UNUSED(option);
	Q_UNUSED(widget);

	setZValue(isSelected() ? 6 : 5);

	painter->setBrush(Qt::white);
	painter->setPen(QPen(Qt::black, scale()));

	qreal sizeW = size.width();
	qreal sizeH = size.height();

	painter->drawRoundedRect(-sizeW/2, -sizeH/2,
							 sizeW, sizeH,
							 sizeW*0.225, sizeW*0.225);
	painter->setFont(QFont("Arial", sizeH/3));
	painter->setPen(QPen(Qt::black, 1));
	painter->drawText(-sizeW/2, -sizeH/2,
					  sizeW,sizeH,Qt::AlignCenter, "LAN" );
	if(isSelected()){
		painter->setPen(QPen(QColor(0, 0, 30, 140), scale(), Qt::DotLine));
		painter->setBrush(QColor(0, 0, 255, 30));
		painter->drawRect(-sizeW/2-5, -sizeH/2-5,
						  sizeW+10, sizeH+10);
	}
}

QRectF LanItem::boundingRect() const{
	qreal penWidth = scale();
	return QRectF(-size.width()/2+penWidth/2, -size.height()/2+penWidth/2,
				  size.width()+penWidth, size.height()+penWidth);
}

void LanItem::wheelEvent(QGraphicsSceneWheelEvent *event){
	angle += 3*event->delta()/8;
	setRotation((int)angle % 360);
}

void LanItem::setSize(QSize newSize){
	//Устанавливаем размеры не ниже минимального
	if(size != newSize){
		size = newSize;
		update();
	}
}

} //namespace gui
