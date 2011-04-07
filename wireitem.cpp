#include "wireitem.h"
#include "portitem.h"

namespace gui{

WireItem::WireItem(PortItem* portS, PortItem* portD, int id)
	:wire(new logic::Wire), id(id), portS(portS), portD(portD)
{
	setPen(QPen(QColor(0,0,0), 6));
	setFlag(ItemSendsGeometryChanges);
	setFlag(ItemIsSelectable);
}

void WireItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){

	setZValue(isSelected() ? 4 : 3);

	Q_UNUSED(option);
	Q_UNUSED(widget);

	painter->setPen(QPen(Qt::black, scale()));

	QPointF pS = mapFromItem(portS,0,0);
	QPointF pD = mapFromItem(portD,0,0);

	painter->drawLine(pS, pD);

	setLine(QLineF(pS, pD));

	if(isSelected()){
		painter->setPen(QPen(QColor(0, 0, 255, 30), scale()*6));
		painter->drawLine(pS, pD);
	}
}

} //namespace gui
