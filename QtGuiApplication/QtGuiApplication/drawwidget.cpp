#include "drawwidget.h"
#include <QtGui>
#include <QPen>

DrawWidget::DrawWidget(QWidget *parent)
	: QWidget(parent)
{
	setAutoFillBackground(true);
	setPalette(QPalette(Qt::white));
	_pix = new QPixmap(size());
	_pix->fill(Qt::white);
	setMinimumSize(600, 400);
}

DrawWidget::~DrawWidget()
{

}

void DrawWidget::setStyle(int s)
{
	_style = s;
}

void DrawWidget::setWidth(int w)
{
	_width = w;
}

void DrawWidget::setColor(QColor c)
{
	_color = c;
}

void DrawWidget::mousePressEvent(QMouseEvent *e)
{
	_start_pos = e->pos();
}

void DrawWidget::mouseMoveEvent(QMouseEvent *e)
{
	QPainter *painter = new QPainter;

	QPen pen;
	pen.setStyle((Qt::PenStyle)_style);
	pen.setWidth(_width);
	pen.setColor(_color);

	painter->begin(_pix);
	painter->setPen(pen);
	painter->drawLine(_start_pos, e->pos());
	painter->end();
	_start_pos = e->pos();
	update();
}

void DrawWidget::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.drawPixmap(QPoint(0, 0), *_pix);
}

void DrawWidget::resizeEvent(QResizeEvent *e)
{
	if (height()>_pix->height() || width()>_pix->width())
	{
		QPixmap *newpix = new QPixmap(size());
		newpix->fill(Qt::white);
		QPainter p(newpix);
		p.drawPixmap(QPoint(0, 0), *_pix);
		_pix = newpix;
	}
	QWidget::resizeEvent(e);
}

void DrawWidget::clear()
{
	QPixmap *clearpix = new QPixmap(size());
	clearpix->fill(Qt::white);
	_pix = clearpix;
	update();
}