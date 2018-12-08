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

	bool isMsgWndVisible(){ return ui.dockWidget->isVisible(); }
	void setMsgWndVisible(bool visible){ ui.dockWidget->setVisible(visible); }

private:
	void SetWindowBackground();

private slots:
	void OpenListClicked();
	void InputClicked();
	void OutputClicked();
	void OKClicked();
	void TabChanged(int idx);

private:
	Ui::FileExtractClass ui;

	//QStringListModel *m_model;
	ListViewModel *m_model;
	QStringList m_list;
};

#endif // FILEEXTRACT_H
