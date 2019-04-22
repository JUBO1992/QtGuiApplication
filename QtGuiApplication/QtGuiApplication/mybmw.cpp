#include "mybmw.h"

//内部圆半径比例因子
#define RADIUS_FACTOR 0.8

//外部圆开始和停止颜色
#define OUTER_CIRCLE_START_COLOR QColor(65,65,65)
#define OUTER_CIRCLE_END_COLOR QColor(89,89,89)

//内部圆的蓝色
#define BLUE_CIRCLE_START_COLOR QColor(0,133,203)
#define BLUE_CIRCLE_END_COLOR QColor(0,118,177)
//内部圆的白色
#define WHITE_CIRCLE_START_COLOR QColor(255,255,255)
#define WHITE_CIRCLE_END_COLOR QColor(233,233,233)

MyBMW::MyBMW(QWidget *parent)
	: QWidget(parent)
{
	m_updateTimer = new QTimer(this);
	m_updateTimer->setInterval(50);//间隔，微妙微单位，大家可以改一下这个值看看转动速度。
	connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(UpdateAngle()));
	m_updateTimer->start();//启动定时器

	m_angle = 0;
	m_outerRadius = 0;
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);//无窗体
	setAttribute(Qt::WA_TranslucentBackground);//背景透明
	setGeometry(100, 100, 300, 300);
}

MyBMW::~MyBMW()
{

}
void MyBMW::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);//设置反锯齿
	drawUnderCircle(&painter);//画外部圆
	drawBMW(&painter);//画宝马
	//qApp->processEvents();
}

void MyBMW::drawUnderCircle(QPainter *painter)
{
	painter->save();

	m_outerRadius = width() > height() ? (qreal)height() / 2 - 4 : (qreal)width() / 2 - 4;//求最小的那个值。自己可以测试一下减4个像素与不减的区别

	QPointF TopLeft(rect().center().x() - m_outerRadius, rect().center().y() - m_outerRadius);
	QPointF BottomRight(rect().center().x() + m_outerRadius, rect().center().y() + m_outerRadius);
	QRectF CircleRect(TopLeft, BottomRight);//大圆矩形

	painter->setPen(Qt::NoPen);//大家可以注释掉这行看一下，有什么差别？
	QRadialGradient CircleGradient(CircleRect.center(), m_outerRadius, CircleRect.center());//设置渐变
	CircleGradient.setColorAt(0.0, OUTER_CIRCLE_START_COLOR);
	CircleGradient.setColorAt(1.0, OUTER_CIRCLE_END_COLOR);
	painter->setBrush(CircleGradient);

	painter->drawEllipse(CircleRect);//画椭圆，其实就是画圆。特殊椭圆而已

	painter->restore();
}

void MyBMW::drawBMW(QPainter *painter)
{

	painter->save();

	//坐标转换的方法和下面直接用painter的rotate方法一样
	//QTransform t;
	//t.translate(rect().center().x(),rect().center().y());// move to center
	//t.rotate(m_angle,Qt::ZAxis);//绕Z轴旋转
	//painter->setTransform(t);

	painter->translate(rect().center().x(), rect().center().y());// move to center
	painter->rotate(m_angle);//旋转

	qreal InnerRadius = m_outerRadius * RADIUS_FACTOR;//内半径
	QPointF tTopLeft(-InnerRadius, -InnerRadius);
	QPointF tBottomRight(InnerRadius, InnerRadius);
	QRectF  tRect(tTopLeft, tBottomRight);

	qreal dAngle = 90 * 16;//为什么乘以16？自己查看文档怎么画pie的。
	qreal StartAngle = 0;

	painter->setPen(Qt::NoPen);
	for (int AngleIndex = 0; AngleIndex < 4; AngleIndex++)
	{
		//交叉蓝色白色
		QRadialGradient PieGradient(tRect.center(), m_outerRadius, tRect.center());
		if (AngleIndex % 2)//蓝色
		{
			PieGradient.setColorAt(0.0, BLUE_CIRCLE_START_COLOR);
			PieGradient.setColorAt(1.0, BLUE_CIRCLE_END_COLOR);
		}
		else//白色
		{
			PieGradient.setColorAt(0.0, WHITE_CIRCLE_START_COLOR);
			PieGradient.setColorAt(1.0, WHITE_CIRCLE_END_COLOR);
		}
		painter->setBrush(PieGradient);
		painter->drawPie(tRect, StartAngle, dAngle);

		//角度增加90度
		StartAngle += dAngle;
	}

	painter->restore();
}

void MyBMW::UpdateAngle()
{
	m_angle += 1;
	if (m_angle > 360)
	{
		m_angle = 0;
	}

	//m_angle = ((m_angle + 1) % 360);与上面几行功能一样
	//update();//刷新控件，会调用paintEvent函数
	//repaint();
	paintEvent(0);
}
