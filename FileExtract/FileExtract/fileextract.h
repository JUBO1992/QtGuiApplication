#ifndef FILEEXTRACT_H
#define FILEEXTRACT_H

#include <QtWidgets/QMainWindow>
#include <QStringListModel>
#include "ui_fileextract.h"
#include "listviewmodel.h"

class FileExtract : public QMainWindow
{
	Q_OBJECT

public:
	FileExtract(QWidget *parent = 0);
	~FileExtract();

private:
	void SetWindowBackground();

private slots:
	void OpenListClicked();
	void InputClicked();
	void OutputClicked();
	void OKClicked();

private:
	Ui::FileExtractClass ui;

	//QStringListModel *m_model;
	ListViewModel *m_model;
	QStringList m_list;
};

#endif // FILEEXTRACT_H
