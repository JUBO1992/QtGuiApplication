#ifndef PROJECTREGISTERWND_H
#define PROJECTREGISTERWND_H

#include <QDialog>
#include <QSqlDatabase>
#include <QPointer>
#include "ui_projectregisterwnd.h"
#include "marinebase.h"
#include "metadatasqlmodel.h"
#include "marineproject.h"


//项目登记窗口
class ProjectRegisterWnd : public QDialog
{
	Q_OBJECT

public:
	ProjectRegisterWnd(QWidget *parent = 0, QString projectID = QString());
	~ProjectRegisterWnd();

private:
	void initWnd();
	void clearComboBox();
	void updateWnd();
	void destory();
	bool initModel(bool isSQL = false);

	private slots:
	void on_pushButton_saveProject_clicked();
	void on_pushButton_delete_clicked();
	void on_pushButton_cancel_clicked();
	void on_comboBox_projectID_currentIndexChanged(int);

	void on_pushButton_newDatabase_clicked();
	void on_pushButton_importDatabase_clicked();

	void on_pushButton_importRaster_clicked();
	void on_pushButton_updateRasterMD_clicked();
	void on_pushButton_saveRasterMD_clicked();

private:
	Ui::ProjectRegisterWnd ui;
	QMap<QString, MarinePrjStruct> m_projectMap;
	//MarinePrjStruct m_curProject;
	CMarineProject* m_pMarinePrj;		//当前海测工程

	QSqlDatabase m_rasterMDdb;			//栅格元数据库
	QSqlDatabase m_surveyMDdb;			//测量元数据库
	QSqlDatabase m_archiveMDdb;			//档案元数据库

	QPointer<MetadataSqlModel> m_pRasterModel;
	QPointer<MetadataSqlModel> m_pSurveyModel;
	QPointer<MetadataSqlModel> m_pArchiveModel;

	bool m_isUpdateWnd;
};

#endif // PROJECTREGISTERWND_H
