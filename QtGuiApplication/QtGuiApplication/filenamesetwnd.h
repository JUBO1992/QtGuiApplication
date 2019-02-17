#ifndef FILENAMESETWND_H
#define FILENAMESETWND_H

#include <QDialog>
#include "ui_filenamesetwnd.h"

class FileNameSetWnd : public QDialog
{
	Q_OBJECT

public:
	FileNameSetWnd(QWidget *parent = 0, const QString& filename = QString());
	~FileNameSetWnd();

	QString getFileName(){ return m_fileName; }
	QString getTemplate(){ return m_template; }

private slots:
	void on_pushButton_ok_clicked();
	void on_pushButton_cancel_clicked();
	void on_comboBox_template_currentTextChanged(QString);
private:
	Ui::FileNameSetWnd ui;

	QString m_fileName;
	QString m_template;
};

#endif // FILENAMESETWND_H
