#ifndef EXTRAFILESCHECKWND_H
#define EXTRAFILESCHECKWND_H

#include <QDialog>
#include "ui_extrafilescheckwnd.h"
#include "marinebase.h"

class CMarineProject;

class ExtraFilesCheckWnd : public QDialog
{
	Q_OBJECT

public:
	ExtraFilesCheckWnd(QWidget *parent = 0);
	~ExtraFilesCheckWnd();

	void setParameters(CMarineProject* pMarine = NULL, const MSFileEnum& ftype = MSFileEnum::RasterFile)
	{
		m_pCurMarinePrj = pMarine;
		m_fType = ftype;
		updateWnd();
	}

	void updateWnd();
	void showDialog();

private slots:
	void on_pushButton_addToMd_clicked();
	void on_pushButton_delExtra_clicked();
	void on_pushButton_Cancel_clicked();

	void clicked_rightMenu(const QPoint &pos);
	void clicked_addAction();
	void clicked_deleteAction();


private:
	Ui::ExtraFilesCheckWnd ui;
	CMarineProject* m_pCurMarinePrj;//海测工程
	MSFileEnum m_fType;				//文件类型
	QStringList m_extraList;		//记录多余文件相对路径

	QMenu* m_pRightMenu;
	QAction *m_pAddAction;
	QAction *m_pDeleteAction;
};

#endif // EXTRAFILESCHECKWND_H
