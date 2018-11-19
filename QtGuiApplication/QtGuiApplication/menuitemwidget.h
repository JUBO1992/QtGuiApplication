#ifndef MENUITEMWIDGET_H
#define MENUITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>

class MenuItemWidget : public QWidget
{
	Q_OBJECT

public:
	MenuItemWidget(const QPixmap& icon, const QString& text, QWidget *parent = 0)
	{
		QLabel* label_Icon = new QLabel(this);
		label_Icon->setFixedSize(32, 32);
		label_Icon->setScaledContents(true);
		label_Icon->setPixmap(icon);

		QLabel* label_Text = new QLabel(text, this);

		QHBoxLayout* layout = new QHBoxLayout;
		layout->setContentsMargins(20, 6, 50, 6);
		layout->setSpacing(10);
		layout->addWidget(label_Icon);
		layout->addWidget(label_Text);
		setLayout(layout);

		setFixedWidth(240);
	}
	~MenuItemWidget()
	{}

protected:
	void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE{
		QStyleOption opt;
		opt.init(this);
		QPainter p(this);
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
	}

private:
};

#endif // MENUITEMWIDGET_H
