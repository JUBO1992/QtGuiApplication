#ifndef MARINEPROJECT_H
#define MARINEPROJECT_H

#include <QObject>
#include "marinebase.h"

class CMarineProject : public QObject
{
	Q_OBJECT

public:
	CMarineProject(MarinePrjStruct marine = MarinePrjStruct());
	~CMarineProject();

	MarinePrjStruct getMarinePrjStruct(){ return m_project; }
	/*	@brief 将MarinePrjStruct设置为新的marine，同时读取marine相关信息
		此接口主要操作是读取数据库中marine项目的相关信息，操作等同于初始化对象
		@param[in] marine
	*/
	void setNewMarinePrj(const MarinePrjStruct marine);
	/*
		@brief 根据传入的marine更新MarinePrjStruct，同时更新数据库
		此接口主要操作是根据marine更新MarinePrjStruct项目的相关信息
		@param[in] marine
	*/
	void updateCurMarinePrj(const MarinePrjStruct marine);

	//返回CMarineProject对象是否有效
	bool isValid()
	{
		return m_project._prjID.isEmpty() ? false : true;
	}

	//返回工程路径
	QString projectPath() const
	{
		return m_project.projectPath();
	}
	//返回矢量数据所在路径
	QString vectorPath() const
	{
		return m_project.vectorPath();
	}
	//返回栅格数据所在路径
	QString rasterPath() const
	{
		return m_project.rasterPath();
	}
	//返回测量数据所在路径
	QString surveyPath(const MSurveyMethod& method = MSurveyMethod::MSUnknown) const
	{
		return m_project.surveyPath(method);
	}
	//返回档案数据所在路径
	QString archivePath() const
	{
		return m_project.archivePath();
	}
	//返回栅格元数据库路径
	QString rasterMDPath() const
	{
		return m_project.rasterMDPath();
	}
	//返回测量元数据库路径
	QString surveyMDPath() const
	{
		return m_project.surveyMDPath();
	}
	//返回档案元数据库路径
	QString archiveMDPath() const
	{
		return m_project.archiveMDPath();
	}

	void getVectorList(QMap<QString, MSFileStruct>& vectorFiles)
	{
		vectorFiles = m_vectorFiles;
	}

	void getRasterList(QMap<QString, MSFileStruct>& rasterFiles)
	{
		rasterFiles = m_rasterFiles;
	}
	void addRasters(const QStringList& filePathArr);
	//根据ChartNO列表删除项目中对应栅格文件，列表为空则全部删除
	void deleteRasters(const QStringList& fileNOArr = QStringList());
	void addRasters(const QString& dirpath);

	void getSurveyList(QMap<QString, MSFileStruct>& surveyFiles)
	{
		surveyFiles = m_surveyFiles;
	}
	void addSurveys(const QStringList& filePathArr, const MSurveyMethod& method = MSUnknown);
	//根据DataNO列表删除项目中对应测量文件，列表为空则根据method删除对应测量方式下的所有文件
	//DataNO列表和method都不指定则清空所有测量文件
	void deleteSurveys(const QStringList& fileNOArr = QStringList(), const MSurveyMethod& method = MSUnknown);
	void addSurveys(const QString& dirPath, const MSurveyMethod& method = MSUnknown);

	void getArchiveList(QMap<QString, MSFileStruct>& archiveFiles)
	{
		archiveFiles = m_archiveFiles;
	}
	void addArchives(const QStringList& filePathArr);
	//根据FileNO列表删除项目中对应档案文件，列表为空则全部删除
	void deleteArchives(const QStringList& fileNOArr = QStringList());
	void addArchives(const QString& dirpath);

	//fileNO为更新文件原编号，newName为新文件名，isOpen标记是否打开
	void updateMSFile(const QString& fileNO, const QString& newName, const bool& isOpen = false);
	//提交文件更新信息到数据库，如果文件名更改则同时修改文件名
	bool commitMSFile();

private:
	void getProjectFiles();

private:
	MarinePrjStruct m_project;


	QMap<QString, MSFileStruct> m_vectorFiles;	//矢量文件列表
	QMap<QString, MSFileStruct> m_rasterFiles;	//栅格文件列表
	QMap<QString, MSFileStruct> m_surveyFiles;	//测量文件列表
	QMap<QString, MSFileStruct> m_archiveFiles;	//档案文件列表

	QMap<QString, MSFileStruct> m_updateFiles;	//更新文件列表

};

#endif // MARINEPROJECT_H
