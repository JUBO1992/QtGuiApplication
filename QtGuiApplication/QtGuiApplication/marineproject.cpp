#include "marineproject.h"
#include <QSqlDatabase>
#include <QSqlQuery>

CMarineProject::CMarineProject(MarinePrjStruct marine)
	: m_project(marine)
{
	getProjectFiles();
}

CMarineProject::~CMarineProject()
{

}

void CMarineProject::setNewMarinePrj(const MarinePrjStruct marine)
{
	m_project = marine;
	getProjectFiles();
}

void CMarineProject::updateCurMarinePrj(const MarinePrjStruct marine)
{
	UpdateMarinePrj(marine, m_project._prjID);
	m_project = marine;
	getProjectFiles();
}

void CMarineProject::getProjectFiles()
{
	m_vectorFiles.clear();
	m_rasterFiles.clear();
	m_surveyFiles.clear();
	m_archiveFiles.clear();
	if (!isValid()) return;

	GetMarinePrjFiles(m_project, MSFileEnum::VectorFile, m_vectorFiles);
	GetMarinePrjFiles(m_project, MSFileEnum::RasterFile, m_rasterFiles);
	GetMarinePrjFiles(m_project, MSFileEnum::SurveyFile, m_surveyFiles);
	GetMarinePrjFiles(m_project, MSFileEnum::ArchiveFile, m_archiveFiles);
}

void CMarineProject::addRasters(const QStringList& rasterPathArr)
{

}

void CMarineProject::deleteRasters(const QStringList& rasterNOArr)
{

}

void CMarineProject::addRaster(const QString& rasterPath)
{
	addRasters(QStringList() << rasterPath);
}

void CMarineProject::deleteRaster(const QString& rasterNO)
{
	deleteRasters(QStringList() << rasterNO);
}
