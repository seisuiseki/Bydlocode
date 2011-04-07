#include "portitem.h"

namespace gui{

void PortItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
						 QWidget *widget){

	Q_UNUSED(option);
	Q_UNUSED(widget);

	if(isConnect){
		painter->setBrush(Qt::black);
	}
	else{
		painter->setBrush(Qt::white);
	}

	painter->setPen(QPen(Qt::black, scale()));
	painter->drawRect(-size/2, -size/2,
					  size, size);

	if(parentItem()->isSelected()){
		painter->setPen(QColor(0, 0, 230, 30));
		painter->setBrush(QColor(0, 0, 255, 30));
		painter->drawRect(-size/2, -size/2,
						  size, size);
	}
}

QRectF PortItem::boundingRect() const{
	qreal penWidth = scale();
	return QRectF(-size/2+penWidth/2, -size/2+penWidth/2,
				  size+penWidth, size+penWidth);
}

} //namespace gui
