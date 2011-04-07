#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QGraphicsView>
#include <QTimer>

namespace core { class Network; }

namespace gui {

class Scene;
class PortItem;
class RIRManager;

enum TypeDevice{
		router,
		lan ,
		computer,
		wireS,
		wireD,
		empty
};

class Workspace : public QGraphicsView {
Q_OBJECT

signals:
	void objectChanged(QGraphicsItem* item);
	void selectItemInLog(int id);
public slots:
	void createWire();
	void createRouter();
	void createComputer();
	void createLan();

	void newWorkspace();
	void deleteSelItems();
private:
	core::Network* net;

	TypeDevice deviceForCreate;

	//Координаты курсора для скроллирования мышью
	QPoint lastMousePos;

	//Координаты курсора для рисования провода
	QPoint cursor;
	bool scrollHandDrag;

	QPixmap curLan;
	QPixmap curRouter;
	QPixmap curWire;
	QPixmap curComputer;
	
	PortItem* portS;
	PortItem* portD;
	PortItem* getItemnPort(QPoint pos);

	void drawPackets();

	QTimer wireTimer;
	QTimer netTimer;
	bool netStarted;

	Scene *scene;
public:
	Workspace(core::Network* net, RIRManager *manager, QWidget *parent = 0);

	void runNetTimer();
	void stopNetTimer();
	void pauseNetTimer();
protected:
	void paintEvent(QPaintEvent *event);
	void wheelEvent(QWheelEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

	void scaleView(int delta);
	void createDevice(QPoint pos);
};

} //namespace gui

#endif // WORKSPACE_H
