#ifndef COMPUTERITEM_H
#define COMPUTERITEM_H

#include "computer.h"

#include "../core/threadsafe.h"
#include <QGraphicsItem>
#include <QPainter>


namespace gui{

class ComputerItem : public QObject, public QGraphicsItem{

Q_OBJECT
Q_INTERFACES(QGraphicsItem)

Q_PROPERTY(QSize size READ getSize WRITE setSize)
Q_PROPERTY(logic::TaskTable table READ getTaskTable WRITE setTaskTable)

private:
	ThreadSafe<logic::Computer> computer;

	qreal angle;
	int id;
	QSize size;

	QSize getSize(){ return size; }
	void setSize(QSize newSize);
public:
	bool isConnected;

	void setId(int newId) { id = newId; }
	int getId() { return id; }

	ComputerItem(int id=0, QSize size = QSize(50, 50));
	ThreadSafe<logic::Computer> getComputer(){ return computer; }

	logic::TaskTable getTaskTable() { return computer->taskTable; }
	void setTaskTable(const logic::TaskTable& table) { computer->taskTable = table; }

	int getSizeH(){ return size.height(); }
protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	QRectF boundingRect() const;
	void wheelEvent(QGraphicsSceneWheelEvent *event);
};

} //namespace gui

#endif // COMPUTERITEM_H

