#ifndef MYBMW_H
#define MYBMW_H

#include <QWidget>
#include <QtGui>

class MyBMW : public QWidget
{
	Q_OBJECT

public:
	MyBMW(QWidget *parent);
	~MyBMW();
protected:
	void paintEvent(QPaintEvent *);

	//重写sizeHint()
	QSize sizeHint() const
	{
		return QSize(300, 300);
	}

private:
	void drawUnderCircle(QPainter* painter);//画外圆

	void drawBMW(QPainter* painter);//画宝马

private:
	QTimer* m_updateTimer;//定时器时间

	qreal   m_angle;    //旋转角度
	qreal   m_outerRadius;//外半径

	private slots:
	void UpdateAngle();//自定义槽，更新角度旋转
	
};

#endif // MYBMW_H
