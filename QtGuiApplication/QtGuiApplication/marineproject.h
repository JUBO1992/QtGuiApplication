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
	QString surveyPath() const
	{
		return m_project.surveyPath();
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
	//获得下一个栅格文件编号
	QString getNextRasterNO();
	void addRasters(const QStringList& rasterPathArr);
	void deleteRasters(const QStringList& rasterNOArr );
	void addRaster(const QString& rasterPath);
	void deleteRaster(const QString& rasterNO);

	void getSurveyList(QMap<QString, MSFileStruct>& surveyFiles)
	{
		surveyFiles = m_surveyFiles;
	}
	//获得下一个测量文件编号
	QString getNextSurveyNO();
	void addSurvey(QString sourcePath);
	void deleteSurvey(MSFileStruct surveyFile);

	void getArchiveList(QMap<QString, MSFileStruct>& archiveFiles)
	{
		archiveFiles = m_archiveFiles;
	}
	//获得下一个档案文件编号
	QString getNextArchiveNO();
	void addArchive(QString sourcePath);
	void deleteArchive(MSFileStruct archiveFile);

private:
	void getProjectFiles();


private:
	MarinePrjStruct m_project;

	QMap<QString, MSFileStruct> m_vectorFiles;	//矢量文件列表
	QMap<QString, MSFileStruct> m_rasterFiles;	//栅格文件列表
	QMap<QString, MSFileStruct> m_surveyFiles;	//测量文件列表
	QMap<QString, MSFileStruct> m_archiveFiles;	//档案文件列表

};

#endif // MARINEPROJECT_H
