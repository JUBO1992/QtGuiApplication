#include "projectregisterwnd.h"
//#include "idatainterface.h"
#include <QSqlDatabase>
#include <QSqlField>
#include <QSqlRecord>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QSqlQuery>
#include <QSqlError>

#pragma execution_character_set("utf-8")

//extern iDataInterface* g_pInterface;

ProjectRegisterWnd::ProjectRegisterWnd(QWidget *parent, QString projectID)
	: QDialog(parent)
	, m_pMarinePrj(new CMarineProject())
	, m_pRasterModel(NULL)
	, m_pSurveyModel(NULL)
	, m_pArchiveModel(NULL)
	, m_isUpdateWnd(true)
{
	ui.setupUi(this);
	ReadProjectMenu(m_projectMap);
	auto it = m_projectMap.find(projectID);
	if ( it != m_projectMap.end())
	{
		m_pMarinePrj->setNewMarinePrj(it.value());
	}
	initWnd();
}

ProjectRegisterWnd::~ProjectRegisterWnd()
{
	if (m_pMarinePrj)
	{
		delete m_pMarinePrj;
		m_pMarinePrj = NULL;
	}
	destory();
}

void ProjectRegisterWnd::initWnd()
{
	clearComboBox();
	updateWnd();
}

void ProjectRegisterWnd::clearComboBox()
{
	ui.comboBox_projectID->clear();
	ui.comboBox_projectName->clear();
	ui.comboBox_projectType->clear();
	ui.comboBox_projectEngineer->clear();
	ui.comboBox_surveyTeam->clear();
	ui.comboBox_surveyPrj->clear();
	ui.comboBox_surveyCoor->clear();
	ui.comboBox_surveyType->clear();

	m_isUpdateWnd = false;
	ui.comboBox_projectID->addItem(QString(""));
	ui.comboBox_projectID->addItems(m_projectMap.keys());
	//ui.comboBox_surveyPrj->addItems(QStringList() << "高斯-克吕格");
	//ui.comboBox_surveyCoor->addItems(QStringList() << "CGCS2000");
	m_isUpdateWnd = true;

}

void ProjectRegisterWnd::updateWnd()
{
	ui.tab_2->setEnabled(false);
	ui.tab_3->setEnabled(false);
	ui.tab_4->setEnabled(false);
	ui.tab_5->setEnabled(false);
	ui.pushButton_delete->setEnabled(false);
	if (m_pMarinePrj->isValid())
	{
		MarinePrjStruct curProject = m_pMarinePrj->getMarinePrjStruct();
		ui.comboBox_projectID->setCurrentText(curProject._prjID);
		ui.comboBox_projectName->setCurrentText(curProject._prjName);
		ui.comboBox_projectType->setCurrentText(curProject._prjType);
		ui.comboBox_projectEngineer->setCurrentText(curProject._prjEngineer);
		ui.comboBox_surveyTeam->setCurrentText(curProject._surveyTeam);
		ui.comboBox_surveyPrj->setCurrentText(curProject._surveyPrj);
		ui.comboBox_surveyCoor->setCurrentText(curProject._surveyCoor);
		ui.comboBox_surveyType->setCurrentText(curProject._surveyType);
		ui.tab_2->setEnabled(true);
		ui.pushButton_delete->setEnabled(true);
		this->setWindowTitle(QString("项目登记-当前项目：%1").arg(curProject._prjID));

		destory();

		QString rasterMD = curProject.rasterMDPath();
		m_rasterMDdb = QSqlDatabase::addDatabase("QSQLITE", "raterMDdb");
		m_rasterMDdb.setDatabaseName(rasterMD);
		if (m_rasterMDdb.open())
		{//能够正常打开元数据库文件才将面板置为可用
			ui.tab_3->setEnabled(true);
			m_pRasterModel = new MetadataSqlModel(m_rasterMDdb);
			m_pRasterModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
			if (!initModel())
			{
				ui.tableView_raster->setModel(NULL);
			}
			ui.tableView_raster->setModel(m_pRasterModel);
			m_pRasterModel->setSort(0, Qt::AscendingOrder);
		}

		QString surveyMD = curProject.surveyMDPath();
		m_surveyMDdb = QSqlDatabase::addDatabase("QSQLITE", "surveyMDdb");
		m_surveyMDdb.setDatabaseName(surveyMD);
		if (m_surveyMDdb.open())
		{
			ui.tab_4->setEnabled(true);
		}


		QString archiveMD = curProject.archiveMDPath();
		m_archiveMDdb = QSqlDatabase::addDatabase("QSQLITE", "archiveMDdb");
		m_archiveMDdb.setDatabaseName(archiveMD);
		if (m_archiveMDdb.open())
		{
			ui.tab_5->setEnabled(true);
		}
	}
	else
	{
		this->setWindowTitle("项目登记");
		ui.comboBox_projectID->setCurrentText(QString());
		ui.comboBox_projectName->setCurrentText(QString());
		ui.comboBox_projectType->setCurrentText(QString());
		ui.comboBox_projectEngineer->setCurrentText(QString());
		ui.comboBox_surveyTeam->setCurrentText(QString());
		ui.comboBox_surveyPrj->setCurrentText(QString());
		ui.comboBox_surveyCoor->setCurrentText(QString());
		ui.comboBox_surveyType->setCurrentText(QString());
	}
}

void ProjectRegisterWnd::destory()
{
	ui.tab_3->setEnabled(false);
	ui.tab_4->setEnabled(false);
	ui.tab_5->setEnabled(false);
	if (m_pRasterModel)
	{
		m_pRasterModel->submitAll();
		delete m_pRasterModel;
		m_pRasterModel = NULL;
	}
	if (m_pSurveyModel)
	{
		m_pSurveyModel->submitAll();
		delete m_pSurveyModel;
		m_pSurveyModel = NULL;
	}
	if (m_pArchiveModel)
	{
		m_pArchiveModel->submitAll();
		delete m_pArchiveModel;
		m_pArchiveModel = NULL;
	}
	if (m_rasterMDdb.isOpen())
	{
		m_rasterMDdb.close();
	}
	if (m_surveyMDdb.isOpen())
	{
		m_surveyMDdb.close();
	}
	if (m_archiveMDdb.isOpen())
	{
		m_archiveMDdb.close();
	}
}

bool ProjectRegisterWnd::initModel(bool isSQL /*= false*/)
{
	if (!m_pRasterModel) return false;
	ui.tableView_raster->setSelectionBehavior(QAbstractItemView::SelectItems);
	m_pRasterModel->setTable(MS_RASTERMD_TABLE);
	QSqlRecord record = m_rasterMDdb.record(MS_RASTERMD_TABLE);
	m_pRasterModel->select();
	for (int i = 0; i < record.count(); ++i)
	{
		m_pRasterModel->setHeaderData(i, Qt::Horizontal, record.fieldName(i), Qt::UserRole);
	}
	return true;
}

void ProjectRegisterWnd::on_pushButton_saveProject_clicked()
{
	enum SavePrjEnum
	{
		CreateNew = 0,	//新建记录
		OverWrite = 1,	//覆盖同ID记录
		ModifyCur = 2,	//修改当前记录
	};
	SavePrjEnum saveFlag = CreateNew;
	MarinePrjStruct marine;
	marine._prjID = ui.comboBox_projectID->currentText();
	marine._prjName = ui.comboBox_projectName->currentText();
	if (marine._prjID.isEmpty() || marine._prjName.isEmpty())
	{
		QMessageBox::information(this, "提示", "工程编号和工程名称不能为空！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	marine._prjType = ui.comboBox_projectType->currentText();
	marine._prjEngineer = ui.comboBox_projectEngineer->currentText();
	marine._surveyTeam = ui.comboBox_surveyTeam->currentText();
	marine._surveyPrj = ui.comboBox_surveyPrj->currentText();
	marine._surveyCoor = ui.comboBox_surveyCoor->currentText();
	marine._surveyType = ui.comboBox_surveyType->currentText();
	if (m_projectMap.find(marine._prjID) != m_projectMap.end())
	{
		if (QMessageBox::Yes != QMessageBox::information(this, "提示", "同编号工程已存在，是否覆盖？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
		{
			return;
		}
		saveFlag = OverWrite;
	}
	else if (m_pMarinePrj->isValid())
	{
		QMessageBox box(QMessageBox::Information, "提示", "请选择新建或修改当前项目");
		box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		box.setButtonText(QMessageBox::Yes, "新建");
		box.setButtonText(QMessageBox::No, "修改当前项目");
		int rst = box.exec();
		if (rst == QMessageBox::Yes)
		{
			saveFlag = CreateNew;
		}
		else if (rst == QMessageBox::No)
		{
			saveFlag = ModifyCur;
		}
		else
		{
			return;
		}
	}

	if (CreateNew == saveFlag)
	{
		bool flag = CreateMarinePrj(marine);
		if (flag)				
			QMessageBox::information(this, "提示", "新建成功！", QMessageBox::Ok, QMessageBox::Ok);
		else
			QMessageBox::information(this, "提示", "新建失败！", QMessageBox::Ok, QMessageBox::Ok);
	}
	else if (OverWrite == saveFlag)
	{
		bool flag = UpdateMarinePrj(marine, marine._prjID);
		if (flag)
			QMessageBox::information(this, "提示", "覆盖成功！", QMessageBox::Ok, QMessageBox::Ok);
		else
			QMessageBox::information(this, "提示", "覆盖失败！", QMessageBox::Ok, QMessageBox::Ok);
	}
	else if (ModifyCur == saveFlag)
	{
		MarinePrjStruct curProject = m_pMarinePrj->getMarinePrjStruct();
		bool flag = UpdateMarinePrj(marine, curProject._prjID);
		if (flag)
			QMessageBox::information(this, "提示", "更新成功！", QMessageBox::Ok, QMessageBox::Ok);
		else
			QMessageBox::information(this, "提示", "更新失败！", QMessageBox::Ok, QMessageBox::Ok);
	}

	m_pMarinePrj->setNewMarinePrj(marine);
	m_projectMap.clear();
	ReadProjectMenu(m_projectMap);
	updateWnd();
}

void ProjectRegisterWnd::on_pushButton_delete_clicked()
{
	//如果当前项目已打开需要提示先关闭当前项目才可删除
	if (QMessageBox::Yes != QMessageBox::information(this, "提示", "是否删除整个项目文件？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
	{
		return;
	}
	MarinePrjStruct curProject = m_pMarinePrj->getMarinePrjStruct();
	bool flag = DeleteMarinePrj(curProject);
	initWnd();
	m_pMarinePrj->setNewMarinePrj(MarinePrjStruct());
}

void ProjectRegisterWnd::on_pushButton_cancel_clicked()
{
	reject();
}

void ProjectRegisterWnd::on_comboBox_projectID_currentIndexChanged(int idx)
{
	if (!m_isUpdateWnd)
	{
		return;
	}
	QString prjid = ui.comboBox_projectID->currentText();
	auto it = m_projectMap.find(prjid);
	if (it != m_projectMap.end())
	{
		MarinePrjStruct curProject = it.value();
		m_pMarinePrj->setNewMarinePrj(curProject);
	}
	else
	{
		MarinePrjStruct curProject;
		curProject._prjID = prjid;//这里prjid为空
		m_pMarinePrj->setNewMarinePrj(curProject);
	}
	updateWnd();
}

void ProjectRegisterWnd::on_pushButton_newDatabase_clicked()
{
	//根据 海测.db 创建mdb数据库
	MarinePrjStruct curProject = m_pMarinePrj->getMarinePrjStruct();
	QString vectorpath = curProject.vectorPath();
	if (!QDir(vectorpath).exists())
	{
		QMessageBox::information(this, "提示", "无矢量数据文件夹，请重建项目！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	QString mdbpath = vectorpath + "/" + curProject._prjID + ".mdb";
}

void ProjectRegisterWnd::on_pushButton_importDatabase_clicked()
{
	MarinePrjStruct curProject = m_pMarinePrj->getMarinePrjStruct();
	QString vectorpath = curProject.vectorPath();
	if (!QDir(vectorpath).exists())
	{
		QMessageBox::information(this, "提示", "无矢量数据文件夹，请重建项目！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	QString mdbfilepath = QFileDialog::getOpenFileName(this, "请选择要导入的矢量数据库", ".", "(*.mdb)");
	if (mdbfilepath.isEmpty())
	{
		return;
	}
	QString topath = vectorpath + "/" + curProject._prjID + ".mdb";
	if (QFile(topath).exists())
	{
		if (QMessageBox::Yes != QMessageBox::information(this, "提示", "数据库已存在，是否覆盖？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
		{
			return;
		}
	}
	CopyFileOrDir(mdbfilepath, topath);
}

void ProjectRegisterWnd::on_pushButton_importRaster_clicked()
{
	QString tempdir;// = g_pInterface->property("MS_RASTER_PATH").toString();
	QStringList filenames = QFileDialog::getOpenFileNames(this, "文件选择", tempdir, "tif 文件(*.tif *.tiff);;所有文件(*.*)");
	if (filenames.size() > 0)
	{
		tempdir = QFileInfo(filenames.at(0)).absolutePath();
		//g_pInterface->setProperty("MS_RASTER_PATH", tempdir);
		QSqlRecord record = m_rasterMDdb.record(MS_RASTERMD_TABLE);
		QStringList fields;
		//fields << "CatalogFrameID" << "ChartNO" << "ChartName" << "Scale" << "Xmin" << "XMax" << "YMin" << "YMax"
		//	<< "Type" << "UpdateTime" << "Resolution" << "SatelliteName" << "SatelliteDesc" << "CoordinateSystem" << "Projection" << "ImageRegion"
		//	<< "ShootTime";
		int count = record.count();
		for (int i = 0; i < record.count(); ++i)
		{
			fields << record.fieldName(i);
		}
		QStringList markList;
		for (int i = 0; i < fields.size(); ++i)
		{
			markList.append("?");
		}
		QSqlQuery query(m_rasterMDdb);
		m_rasterMDdb.transaction();
		for (int i = 0; i < filenames.size(); ++i)
		{
			QString insert_sql = "INSERT INTO [" + MS_RASTERMD_TABLE + "] (" + fields.join(",") 
				+ ") VALUES (" + markList.join(",") + ")";
			query.prepare(insert_sql);
			MarinePrjStruct curProject = m_pMarinePrj->getMarinePrjStruct();
			for (int j = 0; j < record.count(); ++j)
			{
				QSqlField field = record.field(j);
				QString fieldname = field.name();
				if (fieldname.compare("ChartNO", Qt::CaseInsensitive) == 0)
				{
					QString chartNO = curProject._prjID + "R" + QString("%1").arg(i, 4, 10, QChar('0'));
					query.addBindValue(QString(chartNO));
				}
				else
				{
					query.addBindValue(QString("ceshi"));
				}
			}
			if (!query.exec())
			{
				QString errmsg = m_rasterMDdb.lastError().text();
				//g_pInterface->iDataPrintf(errmsg);
				return;
			}
		}
		m_rasterMDdb.commit();
		m_pRasterModel->select();
	}
}

void ProjectRegisterWnd::on_pushButton_updateRasterMD_clicked()
{

}

void ProjectRegisterWnd::on_pushButton_saveRasterMD_clicked()
{
	m_pRasterModel->submitAll();
	m_pRasterModel->select();
	QMessageBox::information(this, "提示", "更新完成！", QMessageBox::Ok, QMessageBox::Ok);
}
