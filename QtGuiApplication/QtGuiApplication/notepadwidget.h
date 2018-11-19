#ifndef SHOWWIDGET_H
#define SHOWWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit>

class NotepadWidget : public QWidget
{
	Q_OBJECT

public:
	NotepadWidget(QWidget *parent);
	~NotepadWidget();

public:
	QImage _img;
	QLabel *_img_label;
	QTextEdit *_text;
};

#endif // SHOWWIDGET_H
