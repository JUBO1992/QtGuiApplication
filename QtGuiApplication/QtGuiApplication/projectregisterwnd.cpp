#include "projectregisterwnd.h"
#include <QSqlDatabase>
#include <QSqlField>
#include <QCompleter>
#include <QListWidgetItem>
#include <QDesktopServices>
#include <QMenu>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include "msurveymethodwnd.h"
#include "filenamesetwnd.h"
#include "marineprjfunc.h"
#include "extrafilescheckwnd.h"

#pragma execution_character_set("utf-8")

extern QString g_curOpenPrjID;

ProjectRegisterWnd::ProjectRegisterWnd(QWidget *parent, QString projectID)
	: QDialog(parent)
	, m_pCurMarinePrj(new CMarineProject())
	, m_pRasterModel(NULL)
	, m_pSurveyModel(NULL)
	, m_pArchiveModel(NULL)
	, m_isUpdateWnd(true)
{
	ui.setupUi(this);


	ui.listWidget_vector->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tableView_raster->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tableView_survey->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tableView_archive->setContextMenuPolicy(Qt::CustomContextMenu);
	m_pRightMenu = new QMenu;
	m_pDeleteAction = new QAction("删除", this);
	m_pAddAction = new QAction("导入", this);
	m_pOpenDirAction = new QAction("打开所在目录", this);

	m_pRightMenu->addAction(m_pDeleteAction);
	m_pRightMenu->addAction(m_pAddAction);
	m_pRightMenu->addAction(m_pOpenDirAction);

	connect(ui.listWidget_vector, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(clicked_rightMenu(QPoint)));
	connect(ui.tableView_raster, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(clicked_rightMenu(QPoint)));
	connect(ui.tableView_survey, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(clicked_rightMenu(QPoint)));
	connect(ui.tableView_archive, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(clicked_rightMenu(QPoint)));
	connect(m_pDeleteAction, SIGNAL(triggered()), this, SLOT(clicked_deleteAction()));
	connect(m_pAddAction, SIGNAL(triggered()), this, SLOT(clicked_addAction()));
	connect(m_pOpenDirAction, SIGNAL(triggered()), this, SLOT(clicked_openDirAction()));

	//限制输入，其中不能出现<>\/:*?"|
	QRegExp rx = QRegExp("[^<>\\\\/:*?\"|]*");
	QRegExpValidator *validator = new QRegExpValidator(rx);
	ui.comboBox_projectID->setValidator(validator);
	ui.comboBox_projectName->setValidator(validator);

	UpdateProjectMenu(m_projectMap);
	auto it = m_projectMap.find(projectID);
	if ( it != m_projectMap.end())
	{
		m_pCurMarinePrj->setNewMarinePrj(it.value());
	}
	ui.tabWidget_survey->setCurrentIndex(0);
	initWnd();
}

ProjectRegisterWnd::~ProjectRegisterWnd()
{
	if (m_pCurMarinePrj)
	{
		delete m_pCurMarinePrj;
		m_pCurMarinePrj = NULL;
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
	m_isUpdateWnd = false;
	ui.comboBox_projectID->clear();
	ui.comboBox_projectName->clear();
	ui.comboBox_projectType->clear();
	ui.comboBox_projectEngineer->clear();
	ui.comboBox_surveyTeam->clear();
	ui.comboBox_surveyCoor->clear();
	ui.comboBox_surveyPrj->clear();
	ui.comboBox_surveyType->clear();
	ui.dateEdit_surveyStart->setDate(QDate::currentDate());
	ui.dateEdit_surveyEnd->setDate(QDate::currentDate());
	ui.dateEdit_chartStart->setDate(QDate::currentDate());
	ui.dateEdit_chartEnd->setDate(QDate::currentDate());

	QCompleter *completer = new QCompleter(m_projectMap.keys(), this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	ui.comboBox_projectID->setCompleter(completer);
	ui.comboBox_projectID->addItem(QString(""));
	ui.comboBox_projectID->addItems(m_projectMap.keys());
	//ui.comboBox_surveyPrj->addItems(QStringList() << "高斯-克吕格");
	//ui.comboBox_surveyCoor->addItems(QStringList() << "CGCS2000");
	m_isUpdateWnd = true;

}

void ProjectRegisterWnd::updateWnd()
{
	//ui.tab_2->setEnabled(false);
	updateListWidget();
	ui.tab_3->setEnabled(false);
	ui.tab_4->setEnabled(false);
	ui.tab_5->setEnabled(false);
	ui.pushButton_delete->setEnabled(false);
	this->setWindowTitle("项目登记");
	ui.comboBox_projectID->setCurrentText(QString());
	ui.comboBox_projectName->setCurrentText(QString());
	ui.comboBox_projectType->setCurrentText(QString());
	ui.comboBox_projectEngineer->setCurrentText(QString());
	ui.comboBox_surveyTeam->setCurrentText(QString());
	ui.comboBox_surveyCoor->setCurrentText(QString());
	ui.comboBox_surveyPrj->setCurrentText(QString());
	ui.comboBox_surveyType->setCurrentText(QString());
	ui.dateEdit_surveyStart->setDate(QDate::currentDate());
	ui.dateEdit_surveyEnd->setDate(QDate::currentDate());
	ui.dateEdit_chartStart->setDate(QDate::currentDate());
	ui.dateEdit_chartEnd->setDate(QDate::currentDate());
	if (m_pCurMarinePrj->isValid())
	{
		MarinePrjStruct curProject = m_pCurMarinePrj->getMarinePrjStruct();
		this->setWindowTitle(QString("项目登记-当前项目：%1").arg(curProject._prjID));
		ui.comboBox_projectID->setCurrentText(curProject._prjID);
		ui.comboBox_projectName->setCurrentText(curProject._prjName);
		ui.comboBox_projectType->setCurrentText(curProject._prjType);
		ui.comboBox_projectEngineer->setCurrentText(curProject._prjEngineer);
		ui.comboBox_surveyTeam->setCurrentText(curProject._surveyTeam);
		ui.comboBox_surveyCoor->setCurrentText(curProject._surveyCoor);
		ui.comboBox_surveyPrj->setCurrentText(curProject._surveyPrj);
		ui.comboBox_surveyType->setCurrentText(curProject._surveyType);
		ui.dateEdit_surveyStart->setDate(curProject._surveySTime);
		ui.dateEdit_surveyEnd->setDate(curProject._surveyETime);
		ui.dateEdit_chartStart->setDate(curProject._chartSTime);
		ui.dateEdit_chartEnd->setDate(curProject._chartETime);
		ui.pushButton_delete->setEnabled(true);

		destory();
		opendb();//opendb前必须先destory
	}
}

void ProjectRegisterWnd::opendb()
{
	MarinePrjStruct curProject = m_pCurMarinePrj->getMarinePrjStruct();
	QString rasterMD = curProject.rasterMDPath();
	m_rasterMDdb = QSqlDatabase::addDatabase("QSQLITE", "raterMDdb");
	m_rasterMDdb.setDatabaseName(rasterMD);
	if (m_rasterMDdb.open())
	{//能够正常打开元数据库文件才将面板置为可用
		ui.tab_3->setEnabled(true);
		m_pRasterModel = new MetadataSqlModel(m_rasterMDdb, m_pCurMarinePrj, MSFileEnum::RasterFile);
		m_pRasterModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
		if (!initRasterModel())
		{
			ui.tableView_raster->setModel(NULL);
		}
		ui.tableView_raster->setModel(m_pRasterModel);
		//m_pRasterModel->setSort(0, Qt::AscendingOrder);
	}

	QString surveyMD = curProject.surveyMDPath();
	m_surveyMDdb = QSqlDatabase::addDatabase("QSQLITE", "surveyMDdb");
	m_surveyMDdb.setDatabaseName(surveyMD);
	if (m_surveyMDdb.open())
	{
		ui.tab_4->setEnabled(true);
		m_pSurveyModel = new MetadataSqlModel(m_surveyMDdb, m_pCurMarinePrj, MSFileEnum::SurveyFile);
		m_pSurveyModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
		if (!initSurveyModel())
		{
			ui.tableView_survey->setModel(NULL);
		}
		ui.tableView_survey->setModel(m_pSurveyModel);
	}

	QString archiveMD = curProject.archiveMDPath();
	m_archiveMDdb = QSqlDatabase::addDatabase("QSQLITE", "archiveMDdb");
	m_archiveMDdb.setDatabaseName(archiveMD);
	if (m_archiveMDdb.open())
	{
		ui.tab_5->setEnabled(true);
		m_pArchiveModel = new MetadataSqlModel(m_archiveMDdb, m_pCurMarinePrj, MSFileEnum::ArchiveFile);
		m_pArchiveModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
		if (!initArchiveModel())
		{
			ui.tableView_archive->setModel(NULL);
		}
		ui.tableView_archive->setModel(m_pArchiveModel);
	}
}

void ProjectRegisterWnd::destory()
{
	if (m_pRasterModel)
	{
		//m_pRasterModel->submitAll();
		//m_pRasterModel->clear();
		ui.tableView_raster->setModel(NULL);
		delete m_pRasterModel;
		m_pRasterModel = NULL;
	}
	if (m_pSurveyModel)
	{
		//m_pSurveyModel->submitAll();
		//m_pSurveyModel->clear();
		ui.tableView_survey->setModel(NULL);
		delete m_pSurveyModel;
		m_pSurveyModel = NULL;
	}
	if (m_pArchiveModel)
	{
		//m_pArchiveModel->submitAll();
		//m_pArchiveModel->clear();
		ui.tableView_archive->setModel(NULL);
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
	ui.tab_3->setEnabled(false);
	ui.tab_4->setEnabled(false);
	ui.tab_5->setEnabled(false);
}

bool ProjectRegisterWnd::initRasterModel(bool isSQL /*= false*/)
{
	if (!m_pRasterModel) return false;
	ui.tableView_raster->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pRasterModel->setTable(MS_RASTERMD_TABLE);
	m_pRasterModel->select();
	return true;
}

bool ProjectRegisterWnd::initSurveyModel(bool isSQL /*= false*/)
{
	if (!m_pSurveyModel) return false;
	ui.tableView_survey->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pSurveyModel->setTable(MS_SURVEYMD_TABLE);
	m_pSurveyModel->select();
	return true;
}

bool ProjectRegisterWnd::initArchiveModel(bool isSQL /*= false*/)
{
	if (!m_pArchiveModel) return false;
	ui.tableView_archive->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pArchiveModel->setTable(MS_ARCHIVEMD_TABLE);
	m_pArchiveModel->select();
	return true;
}

void ProjectRegisterWnd::showEvent(QShowEvent *)
{
	UpdateProjectMenu(m_projectMap);
	auto it = m_projectMap.find(g_curOpenPrjID);
	if (it != m_projectMap.end())
	{
		m_pCurMarinePrj->setNewMarinePrj(it.value());
	}
	ui.tabWidget_survey->setCurrentIndex(0);
	initWnd();
}

void ProjectRegisterWnd::closeEvent(QCloseEvent *)
{
	destory();
}

void ProjectRegisterWnd::updateListWidget()
{
	if (m_pCurMarinePrj->isValid())
	{
		ui.tab_2->setEnabled(true);
		ui.listWidget_vector->clear();
		QMap<QString, MSFileStruct> files;
		m_pCurMarinePrj->getVectorList(files);
		int i = 0; 
		for (auto it = files.begin(); it != files.end(); ++it)
		{
			QListWidgetItem* item = new QListWidgetItem;
			item->setSizeHint(QSize(100, 25));
			item->setText(it->fileName());
			ui.listWidget_vector->addItem(item);
			if (i%2 == 0)
			{
				item->setBackgroundColor(QColor(229, 244, 236));
			}
			else
			{
				item->setBackgroundColor(QColor(230, 245, 250));
			}
			i++;
		}
	}
	else
	{
		ui.listWidget_vector->clear();
		ui.tab_2->setEnabled(false);
	}
}

void ProjectRegisterWnd::on_pushButton_saveProject_clicked()
{
	enum SavePrjEnum
	{
		CreateNew = 0,	//新建记录
		//OverWrite = 1,	//覆盖同ID记录 目前设计禁止覆盖
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
	marine._surveySTime = ui.dateEdit_surveyStart->date();
	marine._surveyETime = ui.dateEdit_surveyEnd->date();
	marine._chartSTime = ui.dateEdit_chartStart->date();
	marine._chartETime = ui.dateEdit_chartEnd->date();
	//if (m_projectMap.find(marine._prjID) != m_projectMap.end())
	//{
	//	QMessageBox::information(this, "提示", "工程编号已存在，请修改工程编号！", QMessageBox::Ok, QMessageBox::Ok);
	//	return;
	//	//saveFlag = OverWrite;
	//}
	if (m_pCurMarinePrj->isValid())
	{
		if (m_projectMap.find(marine._prjID) != m_projectMap.end())
		{
			saveFlag = ModifyCur;
			if (QMessageBox::Yes != QMessageBox::information(this, "提示", QString("确认更新编号为'%1'的工程？").arg(m_pCurMarinePrj->getMarinePrjStruct()._prjID),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
			{
				return;
			}
		}
		else
		{
			QMessageBox box(QMessageBox::Information, "提示", "请选择新建或更新当前项目");
			box.setStandardButtons(QMessageBox::Yes | QMessageBox::Ok | QMessageBox::Cancel);
			box.setButtonText(QMessageBox::Yes, "新建项目");
			box.setButtonText(QMessageBox::Ok, "修改当前项目");
			box.setButtonText(QMessageBox::Cancel, "取消");
			int rst = box.exec();
			if (rst == QMessageBox::Yes)
			{
				saveFlag = CreateNew;
			}
			else if (rst == QMessageBox::Ok)
			{
				saveFlag = ModifyCur;
			}
			else
			{
				return;
			}
		}
	}

	if (CreateNew == saveFlag)
	{
		bool flag = CreateMarinePrj(marine);
		if (flag)				
			QMessageBox::information(this, "提示", "新建成功！", QMessageBox::Ok, QMessageBox::Ok);
		else
		{
			QMessageBox::information(this, "提示", "新建失败！", QMessageBox::Ok, QMessageBox::Ok);
			return;
		}
	}
	else if (ModifyCur == saveFlag)
	{
		MarinePrjStruct curProject = m_pCurMarinePrj->getMarinePrjStruct();
		destory();
		bool flag = UpdateMarinePrj(marine, curProject._prjID);
		if (flag)
			QMessageBox::information(this, "提示", "更新成功！", QMessageBox::Ok, QMessageBox::Ok);
		else
		{
			QMessageBox::information(this, "提示", "更新失败！", QMessageBox::Ok, QMessageBox::Ok);
			return;
		}
	}

	UpdateProjectMenu(m_projectMap);
	m_pCurMarinePrj->setNewMarinePrj(marine);
	initWnd();
}

void ProjectRegisterWnd::on_pushButton_delete_clicked()
{
	//如果当前项目已打开需要提示先关闭当前项目才可删除
	if (QMessageBox::Yes != QMessageBox::information(this, "提示", "是否删除整个项目文件？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
	{
		return;
	}
	MarinePrjStruct curProject = m_pCurMarinePrj->getMarinePrjStruct();
	destory();//先关闭打开的数据再尝试执行删除操作
	bool flag = DeleteMarinePrj(curProject);
	if (!flag)
	{
		QMessageBox::information(this, "提示", "项目删除失败，请检查是否有工程文件被占用！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	m_pCurMarinePrj->setNewMarinePrj(MarinePrjStruct());
	UpdateProjectMenu(m_projectMap);
	initWnd();
	QMessageBox::information(this, "提示", "项目删除成功！", QMessageBox::Ok, QMessageBox::Ok);
}

void ProjectRegisterWnd::on_pushButton_cancel_clicked()
{
	close();
	//reject();
}

void ProjectRegisterWnd::on_comboBox_projectID_currentIndexChanged(int idx)
{
	if (!m_isUpdateWnd)
	{
		return;
	}
	destory();
	QString prjid = ui.comboBox_projectID->currentText();
	auto it = m_projectMap.find(prjid);
	if (it != m_projectMap.end())
	{
		MarinePrjStruct curProject = it.value();
		m_pCurMarinePrj->setNewMarinePrj(curProject);
	}
	else
	{
		MarinePrjStruct curProject;
		curProject._prjID = prjid;//这里prjid为空
		m_pCurMarinePrj->setNewMarinePrj(curProject);
	}
	updateWnd();
}

void ProjectRegisterWnd::on_comboBox_projectID_editTextChanged(QString)
{
	if (!m_isUpdateWnd)
	{
		return;
	}
}

void ProjectRegisterWnd::on_pushButton_newDatabase_clicked()
{
	//根据 海测.db 创建mdb数据库
	MarinePrjStruct curProject = m_pCurMarinePrj->getMarinePrjStruct();
	QString vectorpath = curProject.vectorPath();
	if (!QDir(vectorpath).exists())
	{
		QMessageBox::information(this, "提示", "无'矢量数据'文件夹，请重建项目！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	FileNameSetWnd wnd(this, curProject._prjID);
	if (wnd.exec() != QDialog::Accepted)
		return;
	m_pCurMarinePrj->createVector(wnd.getFileName(), wnd.getTemplate());
	//需要刷一下listWidget_vector
	updateListWidget();
}

void ProjectRegisterWnd::on_pushButton_importDatabase_clicked()
{
	MarinePrjStruct curProject = m_pCurMarinePrj->getMarinePrjStruct();
	QString vectorpath = curProject.vectorPath();
	if (!QDir(vectorpath).exists())
	{
		QMessageBox::information(this, "提示", "无'矢量数据'文件夹，请重建项目！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	QString tempdir = MSGetProperty("MS_VECTOR_PATH").toString();
	QStringList filenames = QFileDialog::getOpenFileNames(this, "请选择要导入的矢量数据库", tempdir, "mdb files(*.mdb)");
	if (filenames.size() == 0)
	{
		return;
	}
	tempdir = QFileInfo(filenames.at(0)).absolutePath();
	MSSetProperty("MS_VECTOR_PATH", tempdir);
	m_pCurMarinePrj->addVectors(filenames);
	//需要刷一下listWidget_vector
	updateListWidget();
}

void ProjectRegisterWnd::on_listWidget_vector_clicked(QModelIndex idx)
{

}

void ProjectRegisterWnd::on_pushButton_importRaster_clicked()
{
	MarinePrjStruct curProject = m_pCurMarinePrj->getMarinePrjStruct();
	QString rasterpath = curProject.rasterPath();
	if (!QDir(rasterpath).exists())
	{
		QMessageBox::information(this, "提示", "无'栅格数据'文件夹，请重建项目！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	//QString tempdir = g_pInterface->property("MS_RASTER_PATH").toString();
	QString tempdir = MSGetProperty("MS_RASTER_PATH").toString();
	QStringList filenames = QFileDialog::getOpenFileNames(this, "文件选择", tempdir, "tif 文件(*.tif *.tiff);;所有文件(*.*)");
	if (filenames.size() > 0)
	{
		tempdir = QFileInfo(filenames.at(0)).absolutePath();
		//g_pInterface->setProperty("MS_RASTER_PATH", tempdir);
		MSSetProperty("MS_RASTER_PATH", tempdir);
	}
	m_pCurMarinePrj->addRasters(filenames);
	m_pRasterModel->select();
}

void ProjectRegisterWnd::on_pushButton_updateRasterMD_clicked()
{
	ExtraFilesCheckWnd wnd(this);
	wnd.setParameters(m_pCurMarinePrj, MSFileEnum::RasterFile);
	wnd.showDialog();
	m_pRasterModel->select();
}

void ProjectRegisterWnd::on_pushButton_saveRasterMD_clicked()
{
	m_pRasterModel->submitAll();
	m_pRasterModel->submitIndex();
	m_pRasterModel->select();
	QMessageBox::information(this, "提示", "更新完成！", QMessageBox::Ok, QMessageBox::Ok);
}

void ProjectRegisterWnd::on_tableView_raster_clicked(QModelIndex idx)
{
	m_pRasterModel->itemClicked(idx);
}

void ProjectRegisterWnd::on_pushButton_importSurvey_clicked()
{
	MarinePrjStruct curProject = m_pCurMarinePrj->getMarinePrjStruct();
	QString surveypath = curProject.surveyPath();
	if (!QDir(surveypath).exists())
	{
		QMessageBox::information(this, "提示", "无'测量文件'文件夹，请重建项目！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	QString tempdir = MSGetProperty("MS_SURVEY_PATH").toString();
	QString path = QFileDialog::getExistingDirectory(this, tr("选择文件夹"),
		tempdir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (path.isEmpty())
	{
		return;
	}
	tempdir = path;
	MSSetProperty("MS_SURVEY_PATH", tempdir);
	MSurveyMethodWnd methodWnd(this);
	if (methodWnd.exec() == QDialog::Accepted)
	{
		MSurveyMethod method = methodWnd.getMethod();
		m_pCurMarinePrj->addSurveys(path, method);
		m_pSurveyModel->select();
	}
}

void ProjectRegisterWnd::on_pushButton_updateSurveyMD_clicked()
{
	ExtraFilesCheckWnd wnd(this);
	wnd.setParameters(m_pCurMarinePrj, MSFileEnum::SurveyFile);
	wnd.showDialog();
	m_pSurveyModel->select();
}

void ProjectRegisterWnd::on_pushButton_saveSurveyMD_clicked()
{
	m_pSurveyModel->submitAll();
	m_pSurveyModel->select();
	QMessageBox::information(this, "提示", "更新完成！", QMessageBox::Ok, QMessageBox::Ok);
}

void ProjectRegisterWnd::on_tableView_survey_clicked(QModelIndex idx)
{

}

void ProjectRegisterWnd::on_pushButton_importArchive_clicked()
{
	MarinePrjStruct curProject = m_pCurMarinePrj->getMarinePrjStruct();
	QString archivepath = curProject.archivePath();
	if (!QDir(archivepath).exists())
	{
		QMessageBox::information(this, "提示", "无'档案文件'文件夹，请重建项目！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	QString tempdir = MSGetProperty("MS_ARCHIVE_PATH").toString();
	QString path = QFileDialog::getExistingDirectory(this, tr("选择文件夹"),
		tempdir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (path.isEmpty())
	{
		return;
	}
	tempdir = path;
	MSSetProperty("MS_ARCHIVE_PATH", tempdir);
	m_pCurMarinePrj->addArchives(path);
	m_pArchiveModel->select();
}

void ProjectRegisterWnd::on_pushButton_updateArchiveMD_clicked()
{
	ExtraFilesCheckWnd wnd(this);
	wnd.setParameters(m_pCurMarinePrj, MSFileEnum::ArchiveFile);
	wnd.showDialog();
	m_pArchiveModel->select();
}

void ProjectRegisterWnd::on_pushButton_saveArchiveMD_clicked()
{
	m_pArchiveModel->submitAll();
	m_pArchiveModel->select();
	QMessageBox::information(this, "提示", "更新完成！", QMessageBox::Ok, QMessageBox::Ok);
}

void ProjectRegisterWnd::on_tableView_archive_clicked(QModelIndex idx)
{

}

void ProjectRegisterWnd::clicked_rightMenu(const QPoint &pos)
{
	QModelIndexList list;
	if (ui.tabWidget_survey->currentIndex() == 1)
	{
		list = ui.listWidget_vector->selectionModel()->selectedRows();
	}
	else if (ui.tabWidget_survey->currentIndex() == 2)
	{
		list = ui.tableView_raster->selectionModel()->selectedRows();
	}
	else if (ui.tabWidget_survey->currentIndex() == 3)
	{
		list = ui.tableView_survey->selectionModel()->selectedRows();
	}
	else if (ui.tabWidget_survey->currentIndex() == 4)
	{
		list = ui.tableView_archive->selectionModel()->selectedRows();
	}

	if (list.size() > 0)
	{
		m_pDeleteAction->setEnabled(true);
		m_pOpenDirAction->setEnabled(true);
	}
	else
	{
		m_pDeleteAction->setEnabled(false);
		m_pOpenDirAction->setEnabled(false);
	}
	m_pRightMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置

}

void ProjectRegisterWnd::clicked_deleteAction()
{
	QModelIndexList list;
	if (ui.tabWidget_survey->currentIndex() == 1)
	{
		QList<QListWidgetItem*> itemlist = ui.listWidget_vector->selectedItems();
		QStringList fileList;
		for (int i = 0; i < itemlist.size(); ++i)
		{
			fileList << itemlist.at(i)->text();
		}
		if (QMessageBox::Yes != QMessageBox::information(NULL, "提示", "确认删除矢量文件？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
		{
			return;
		}
		m_pCurMarinePrj->deleteVectors(fileList);
		updateListWidget();
	}
	else if (ui.tabWidget_survey->currentIndex() == 2)
	{
		list = ui.tableView_raster->selectionModel()->selectedRows();
		QStringList fileNOList = m_pRasterModel->getFileNOList(list);
		m_pCurMarinePrj->deleteRasters(fileNOList);
		m_pRasterModel->select();
	}
	else if (ui.tabWidget_survey->currentIndex() == 3)
	{
		list = ui.tableView_survey->selectionModel()->selectedRows();
		QStringList fileNOList = m_pSurveyModel->getFileNOList(list);
		m_pCurMarinePrj->deleteSurveys(fileNOList);
		m_pSurveyModel->select();
	}
	else if (ui.tabWidget_survey->currentIndex() == 4)
	{
		list = ui.tableView_archive->selectionModel()->selectedRows();
		QStringList fileNOList = m_pArchiveModel->getFileNOList(list);
		m_pCurMarinePrj->deleteArchives(fileNOList);
		m_pArchiveModel->select();
	}
}

void ProjectRegisterWnd::clicked_addAction()
{
	if (ui.tabWidget_survey->currentIndex() == 1)
	{
		on_pushButton_importDatabase_clicked();
	}
	else if (ui.tabWidget_survey->currentIndex() == 2)
	{
		on_pushButton_importRaster_clicked();
	}
	else if (ui.tabWidget_survey->currentIndex() == 3)
	{
		on_pushButton_importSurvey_clicked();
	}
	else if (ui.tabWidget_survey->currentIndex() == 4)
	{
		on_pushButton_importArchive_clicked();
	}
}

void ProjectRegisterWnd::clicked_openDirAction()
{
	if (ui.tabWidget_survey->currentIndex() == 1)
	{
		QDesktopServices::openUrl(QUrl::fromLocalFile(m_pCurMarinePrj->vectorPath()));
	}
	else if (ui.tabWidget_survey->currentIndex() == 2)
	{
		QDesktopServices::openUrl(QUrl::fromLocalFile(m_pCurMarinePrj->rasterPath()));
	}
	else if (ui.tabWidget_survey->currentIndex() == 3)
	{
		QDesktopServices::openUrl(QUrl::fromLocalFile(m_pCurMarinePrj->surveyPath()));
	}
	else if (ui.tabWidget_survey->currentIndex() == 4)
	{
		QDesktopServices::openUrl(QUrl::fromLocalFile(m_pCurMarinePrj->archivePath()));
	}
}
