/*! \file drawwidget.h
    Copyright (c) 2018,广东南方数码科技有限公司
	All rights reserved.
	\brief 绘图widget头文件
	\author created by JUBO,bo.ju@southgis.com
	\version v1.0
	\date 2018年5月17日
*/

#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>

/*! \brief DrawWidget class
	绘图Widget
*/
class DrawWidget : public QWidget
{
	Q_OBJECT

public:
	explicit DrawWidget(QWidget *parent = 0);
	~DrawWidget();
	void mousePressEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void paintEvent(QPaintEvent *);
	void resizeEvent(QResizeEvent *);

signals:
	;
public slots:
	void setStyle(int);
	void setWidth(int);
	void setColor(QColor);
	void clear();

private:
	QPixmap *_pix;
	QPoint _start_pos;
	QPoint _end_pos;
	int _style;
	int _width;
	QColor _color;
};

#endif // DRAWWIDGET_H
