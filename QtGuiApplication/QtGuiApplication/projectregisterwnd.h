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
	void opendb();
	void destory();
	bool initRasterModel(bool isSQL = false);
	bool initSurveyModel(bool isSQL = false);
	bool initArchiveModel(bool isSQL = false);

	void closeEvent(QCloseEvent *);

private slots:
	void on_pushButton_saveProject_clicked();
	void on_pushButton_delete_clicked();
	void on_pushButton_cancel_clicked();
	void on_comboBox_projectID_currentIndexChanged(int);
	void on_comboBox_projectID_editTextChanged(QString);

	void on_pushButton_newDatabase_clicked();
	void on_pushButton_importDatabase_clicked();

	void on_pushButton_importRaster_clicked();
	void on_pushButton_updateRasterMD_clicked();
	void on_pushButton_saveRasterMD_clicked();
	void on_tableView_raster_clicked(QModelIndex idx);

	void on_pushButton_importSurvey_clicked();
	void on_pushButton_updateSurveyMD_clicked();
	void on_pushButton_saveSurveyMD_clicked();
	void on_tableView_survey_clicked(QModelIndex idx);

	void on_pushButton_importArchive_clicked();
	void on_pushButton_updateArchiveMD_clicked();
	void on_pushButton_saveArchiveMD_clicked();
	void on_tableView_archive_clicked(QModelIndex idx);

	void clicked_rightMenu(const QPoint &pos);
	void clicked_deleteAction();

private:
	Ui::ProjectRegisterWnd ui;
	QMap<QString, MarinePrjStruct> m_projectMap;
	//MarinePrjStruct m_curProject;
	CMarineProject* m_pCurMarinePrj;		//当前海测工程

	QSqlDatabase m_rasterMDdb;			//栅格元数据库
	QSqlDatabase m_surveyMDdb;			//测量元数据库
	QSqlDatabase m_archiveMDdb;			//档案元数据库

	QPointer<MetadataSqlModel> m_pRasterModel;
	QPointer<MetadataSqlModel> m_pSurveyModel;
	QPointer<MetadataSqlModel> m_pArchiveModel;

	bool m_isUpdateWnd;

	QMenu *m_pRightMenu;
	QAction *m_pDeleteAction;

};

#endif // PROJECTREGISTERWND_H
