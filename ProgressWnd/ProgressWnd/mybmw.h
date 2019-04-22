#ifndef MYBMW_H
#define MYBMW_H

#include <QWidget>
#include <QtGui>

class MyBMW : public QWidget
{
	Q_OBJECT

public:
	MyBMW(QWidget *parent = 0);
	~MyBMW();

	private slots:
	void UpdateAngle();//自定义槽，更新角度旋转

protected:
	//!override sizeHint
	QSize sizeHint() const { return QSize(300, 300); }

	//!override paintEvent
	void paintEvent(QPaintEvent *);

	//!override mousePressEvent
	void mousePressEvent(QMouseEvent *event);
	//!override mouseMoveEvent
	void mouseMoveEvent(QMouseEvent *event);
	//!override mouseReleaseEvent
	void mouseReleaseEvent(QMouseEvent *event);


private:
	void drawUnderCircle(QPainter* painter);//画外圆
	void drawBMW(QPainter* painter);//画宝马

private:
	QTimer* m_updateTimer;//定时器时间

	qreal   m_angle;    //旋转角度
	qreal   m_outerRadius;//外半径

	bool m_move;
	QPoint m_startPoint;
	QPoint m_windowPoint;
};

#endif // MYBMW_H
