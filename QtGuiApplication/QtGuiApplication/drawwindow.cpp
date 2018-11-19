#include "drawwindow.h"
#include <QColorDialog>
#include <QToolBar>

DrawWindow::DrawWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	setWindowTitle(tr("Paint Window"));
	setWindowState(Qt::WindowMaximized);

	_draw_widget = new DrawWidget();
	setCentralWidget(_draw_widget);

	initToolBar();
	showStyle();
	_draw_widget->setWidth(_width_spinBox->value());
	_draw_widget->setColor(Qt::black);
}

DrawWindow::~DrawWindow()
{

}

void DrawWindow::initToolBar()
{
	QToolBar *toolBar = addToolBar("ToolBar");

	_style_label = new QLabel(tr("LineStyle:"));
	_style_comboBox = new QComboBox;
	_style_comboBox->addItem(tr("SolidLine"), static_cast<int>(Qt::SolidLine));
	_style_comboBox->addItem(tr("DashLine"), static_cast<int>(Qt::DashLine));
	_style_comboBox->addItem(tr("DotLine"), static_cast<int>(Qt::DotLine));
	_style_comboBox->addItem(tr("DashDotLine"), static_cast<int>(Qt::DashDotLine));
	_style_comboBox->addItem(tr("DashDotDotLine"), static_cast<int>(Qt::DashDotDotLine));
	connect(_style_comboBox, SIGNAL(activated(int)), this, SLOT(showStyle()));

	_width_label = new QLabel(tr("LineWidth:"));
	_width_spinBox = new QSpinBox;
	connect(_width_spinBox, SIGNAL(valueChanged(int)), _draw_widget, SLOT(setWidth(int)));

	_color_btn = new QToolButton;
	QPixmap pixmap(20, 20);
	pixmap.fill(Qt::black);
	_color_btn->setIcon(QIcon(pixmap));
	connect(_color_btn, SIGNAL(clicked()), this, SLOT(showColor()));

	_clear_btn = new QToolButton();
	_clear_btn->setText(tr("Clear"));
	connect(_clear_btn, SIGNAL(clicked()), _draw_widget, SLOT(clear()));

	toolBar->addWidget(_style_label);
	toolBar->addWidget(_style_comboBox);
	toolBar->addWidget(_width_label);
	toolBar->addWidget(_width_spinBox);
	toolBar->addWidget(_color_btn);
	toolBar->addWidget(_clear_btn);
}

void DrawWindow::showStyle()
{
	_draw_widget->setStyle(_style_comboBox->itemData(
		_style_comboBox->currentIndex(), Qt::UserRole).toInt());
}

void DrawWindow::showColor()
{
	QColor color = QColorDialog::getColor(static_cast<int>(Qt::black), this);
	if (color.isValid())
	{
		_draw_widget->setColor(color);
		QPixmap p(20, 20);
		p.fill(color);
		_color_btn->setIcon(QIcon(p));
	}
}
