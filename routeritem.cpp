#include "routeritem.h"
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsScene>

namespace gui{

RouterItem::RouterItem(int id, QSize size)
	:router(new logic::Router(4)), angle(0), id(id), size(size), numberPorts(0)
{
	setFlag(ItemIsMovable);
	setFlag(ItemSendsGeometryChanges);
	setFlag(ItemIsSelectable);
	setFlag(ItemIsFocusable);

	changeNumberPorts(4);
}

void RouterItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
						 QWidget *widget){

	Q_UNUSED(option);
	Q_UNUSED(widget);

	setZValue(isSelected() ? 2 : 1);

	painter->setBrush(Qt::white);
	painter->setPen(QPen(Qt::black, scale()));

	qreal sizeW = size.width();
	qreal sizeH = size.height();

	painter->drawRoundedRect(-sizeW/2, -sizeH/2,
							 sizeW, sizeH,
							 sizeW/45, sizeH/4);

	if(isSelected()){
		painter->setPen(QPen(QColor(0, 0, 30, 140), scale(), Qt::DotLine));
		painter->setBrush(QColor(0, 0, 255, 30));
		painter->drawRect(-sizeW/2-5, -sizeH/2-5,
						  sizeW+10, sizeH+10);
	}
}

QRectF RouterItem::boundingRect() const{
	qreal penWidth = scale();
	return QRectF(-size.width()/2+penWidth/2, -size.height()/2+penWidth/2,
				  size.width()+penWidth, size.height()+penWidth);
}

void RouterItem::wheelEvent(QGraphicsSceneWheelEvent *event){
	angle += 3*event->delta()/8;
	setRotation((int)angle % 360);;
}

void RouterItem::changeNumberPorts(int newSize){
	if(numberPorts != newSize){

		numberPorts = newSize;
		int oldSize = childItems().count();

		if(oldSize < numberPorts){
			for(int i=oldSize; i<numberPorts; ++i){
				PortItem *port = new PortItem(this);
				port->setData(5, "Port");
			}
		}

		if(oldSize > numberPorts){
			QList<QGraphicsItem *> list = childItems();
			while(list.size() != numberPorts){

				//удаляем все связи
				PortItem* port = qgraphicsitem_cast<PortItem*>(list.last());
				emit portDeleted(port);

				this->scene()->removeItem(list.last());
				delete list.last();
				list.removeLast();
			}
		}
		setPortsPosition();
	}
}

void RouterItem::setSize(QSize newSize){
	//Устанавливаем размеры не ниже минимального
	if(size != newSize){
		size = newSize;

		setPortsPosition();
	}
}

void RouterItem::setPortsPosition(){

	//Длинна роутера по размерам порта
	int sizeOfPorts = (2*numberPorts+1)*(size.height()/4);
	
	//Если размеры роутера не устраивают по количеству портов то меняем их
	size.setWidth(qMax(sizeOfPorts, size.width()));

	//Расчитываем интервал между портами
	qreal inter = size.width()/(numberPorts+1);
	int i=0;
	foreach(QGraphicsItem* port, childItems()){
		qreal pos = (((qreal)numberPorts-1)/2)*inter;
		port->setPos(i*inter-pos, 0);
		++i;
	}
}

} //namespace gui
