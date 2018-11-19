#ifndef DRAWWINDOW_H
#define DRAWWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include "ui_drawwindow.h"
#include "drawwidget.h"

class DrawWindow : public QMainWindow
{
	Q_OBJECT

public:
	DrawWindow(QWidget *parent = 0);
	~DrawWindow();
	void initToolBar();

public slots:
	void showStyle();
	void showColor();

private:
	Ui::DrawWindow ui;

	DrawWidget *_draw_widget;
	QLabel *_style_label;
	QComboBox *_style_comboBox;
	QLabel *_width_label;
	QSpinBox *_width_spinBox;
	QToolButton *_color_btn;
	QToolButton *_clear_btn;
};

#endif // DRAWWINDOW_H
