#include "notepadwidget.h"
#include <QHBoxLayout>

NotepadWidget::NotepadWidget(QWidget *parent)
	: QWidget(parent)
{
	_img_label = new QLabel;
	_img_label->setScaledContents(true);
	_text = new QTextEdit;

	QHBoxLayout *main_layout = new QHBoxLayout(this);
	main_layout->addWidget(_img_label);
	main_layout->addWidget(_text);
}

NotepadWidget::~NotepadWidget()
{

}
