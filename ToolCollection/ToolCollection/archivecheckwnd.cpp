#include "archivecheckwnd.h"
#include "globalconfig.h"
#include "codedetect.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTime>
#include <QTextStream>
#include "checkitemwnd.h"

#pragma execution_character_set("utf-8")

extern CommonOperate * g_COperate;

ArchiveCheckWnd::ArchiveCheckWnd(QWidget *parent)
	: QWidget(parent)
	, m_pDataModel(NULL)
{
	ui.setupUi(this);

	QList<CheckItemStruct> checkItems;
	readCheckItems(checkItems);
	if (checkItems.size() == 0)
	{
		checkItems << CheckItemStruct(true, Required, "*P.img");
		checkItems << CheckItemStruct(true, Required, "*P.rrd");
		checkItems << CheckItemStruct(true, Required, "*M.img");
		checkItems << CheckItemStruct(true, Required, "*M.rrd");
		checkItems << CheckItemStruct(true, Required, "*F.img");
		checkItems << CheckItemStruct(true, Required, "*F.rrd");
		checkItems << CheckItemStruct(true, Required, "*T.xml");
		checkItems << CheckItemStruct(true, Required, "*Y.xml");
		checkItems << CheckItemStruct(true, Optional, "*P.ige");
		checkItems << CheckItemStruct(true, Optional, "*M.ige");
		checkItems << CheckItemStruct(true, Optional, "*F.ige");
	}

	m_pDataModel = new CheckItemModel;
	ui.tableView->setModel(m_pDataModel);
	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	//ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableView->horizontalHeader()->setStretchLastSection(true);
	ui.tableView->setColumnWidth(0, 45);
	m_pDataModel->setInfos(checkItems);

	ui.tableView->setContextMenuPolicy(Qt::CustomContextMenu);
	m_rightMenu = new QMenu;
	m_addItem = new QAction("添加", this);
	m_delteItem = new QAction("删除", this);
	m_modifyItem = new QAction("修改", this);

	m_rightMenu->addAction(m_addItem);
	m_rightMenu->addAction(m_delteItem);
	m_rightMenu->addAction(m_modifyItem);

	connect(ui.tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(clicked_rightMenu(QPoint)));
	connect(m_addItem, SIGNAL(triggered()), this, SLOT(clicked_addItem()));
	connect(m_delteItem, SIGNAL(triggered()), this, SLOT(clicked_deleteItem()));
	connect(m_modifyItem, SIGNAL(triggered()), this, SLOT(clicked_modifyItem()));
}

ArchiveCheckWnd::~ArchiveCheckWnd()
{
	if (m_pDataModel != NULL)
	{
		writeCheckItems(m_pDataModel->getInofs());
		delete m_pDataModel;
		m_pDataModel = NULL;
	}
}

void ArchiveCheckWnd::on_pushButton_scanFolder_clicked()
{
	QString dir = GlobalConfig::GetProperty("ARCHIVEPATH");
	QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
		dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (path.isEmpty())
	{
		return;
	}
	GlobalConfig::SetProperty("ARCHIVEPATH", path);
	ui.lineEdit_folder->setText(path);
}

void ArchiveCheckWnd::on_pushButton_runCheck_clicked()
{
	QString inputFolder = ui.lineEdit_folder->text();
	QFileInfo info(inputFolder);
	if (!info.isDir() || !info.exists())
	{
		QMessageBox::information(this, "提示", "检查目录有误！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	runCheck(inputFolder);
}

void ArchiveCheckWnd::clicked_rightMenu(const QPoint &pos)
{
	QModelIndex index = ui.tableView->indexAt(pos);
	if (index.isValid())
	{
		m_addItem->setEnabled(true);
		m_modifyItem->setEnabled(true);
	}
	else
	{
		m_addItem->setEnabled(true);
		m_modifyItem->setEnabled(false);
	}
	QModelIndexList list = ui.tableView->selectionModel()->selectedRows();
	if (list.size() > 0)
	{
		m_delteItem->setEnabled(true);
	}
	else
	{
		m_delteItem->setEnabled(false);
	}
	m_rightMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
}

void ArchiveCheckWnd::clicked_addItem()
{
	QModelIndex index = ui.tableView->currentIndex();
	CheckItemWnd itemWnd(this);
	if (itemWnd.exec())
	{
		CheckItemStruct item = itemWnd.getCheckItem();
		m_pDataModel->addItem(item);
	}
}

void ArchiveCheckWnd::clicked_deleteItem()
{
	//if (QMessageBox::Yes != QMessageBox::information(this, "提示", "是否确认删除该记录？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
	//{
	//	return;
	//}
	//QModelIndex index = ui.tableView->currentIndex();
	//int row = index.row();
	//m_pDataModel->deleteItem(row);

	QModelIndexList list = ui.tableView->selectionModel()->selectedRows();
	m_pDataModel->deleteItems(list);
}

void ArchiveCheckWnd::clicked_modifyItem()
{
	QModelIndex index = ui.tableView->currentIndex();
	if (!index.isValid())
	{
		return;
	}
	int row = index.row();
	CheckItemStruct item = m_pDataModel->getInofs().at(row);
	CheckItemWnd itemWnd(this, item);
	if (itemWnd.exec())
	{
		CheckItemStruct item = itemWnd.getCheckItem();
		m_pDataModel->modifyItem(row, item);
	}
}

void ArchiveCheckWnd::on_pushButton_infoWnd_clicked()
{
	bool visible = g_COperate->IsMsgWndVisible();
	g_COperate->SetMsgWndVisible(!visible);
}

void ArchiveCheckWnd::on_pushButton_clearInfoWnd_clicked()
{
	g_COperate->MsgClear();
}

void ArchiveCheckWnd::on_tableView_clicked(QModelIndex idx)
{
	int row = idx.row();
	int col = idx.column();
	if (col == 0)
	{
		m_pDataModel->clickItem(row);
	}
}

void ArchiveCheckWnd::on_tableView_doubleClicked(QModelIndex idx)
{
	int row = idx.row();
	CheckItemStruct item = m_pDataModel->getInofs().at(row);
	CheckItemWnd itemWnd(this, item);
	if (itemWnd.exec())
	{
		CheckItemStruct item = itemWnd.getCheckItem();
		m_pDataModel->modifyItem(row, item);
	}
}

void ArchiveCheckWnd::runCheck(QString inputFolder)
{
	QList<ErrorMsgStruct> errList;
	CheckLevel ckLevel = CheckLevel::ProjectLevel;
	QStringList otherErrMsg;	//记录其他错误信息

	QDateTime curTime = QDateTime::currentDateTime();
	QString strStartTime = QString("检查开始：%1\r\n").arg(curTime.toString("yyyy.MM.dd hh:mm:ss"));
	g_COperate->MsgPrint(strStartTime);

	QDir projectDir(inputFolder);
	QString dirName = projectDir.dirName();
	QString filePrefix = dirName;
	int sIdx = filePrefix.indexOf("-");
	int eIdx = filePrefix.lastIndexOf("-");
	if (eIdx > sIdx && eIdx < filePrefix.size()-1)
	{//文件前缀为文件夹名去掉首尾-
		filePrefix = filePrefix.left(sIdx) + filePrefix.mid(sIdx + 1, eIdx - sIdx - 1) + filePrefix.mid(eIdx + 1);
	}
	else
	{//文件前缀为文件夹名去掉-
		filePrefix.remove("-");
	}
	projectDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
	projectDir.setSorting(QDir::DirsFirst);
	QFileInfoList fileList = projectDir.entryInfoList(); // 获取所有的文件信息
	foreach(QFileInfo file, fileList)
	{
		QString fileName = file.fileName();
		if (file.isFile() && dirName.size() > 10 && fileName.startsWith(filePrefix, Qt::CaseInsensitive))
		{
			ckLevel = CheckLevel::ImageLevel;
			break;
		}
		else if (file.isDir() && dirName.size() >= 2 && dirName.size() <= 10
			&& fileName.size() > 10 && fileName.contains(dirName, Qt::CaseInsensitive))
		{
			ckLevel = CheckLevel::SensorLevel;
			break;
		}
	}

	if (ckLevel == CheckLevel::ImageLevel)
	{
		QString imgFolder = inputFolder;
		ErrorMsgStruct errMsg = checkImageFolder(imgFolder, m_pDataModel->getInofs());
		if (errMsg.HasError())
		{
			errList << errMsg;
			g_COperate->MsgPrint(errMsg);
		}
	}
	else if (ckLevel == CheckLevel::SensorLevel)
	{
		foreach(QFileInfo file, fileList)
		{
			if (!file.isDir())
			{
				//此处是否需要报错？？
				otherErrMsg << QString("%1目录下存在多余文件：%2").arg(projectDir.dirName()).arg(file.fileName());
				continue;
			}
			QString imgFolder = file.absoluteFilePath();
			ErrorMsgStruct errMsg = checkImageFolder(imgFolder, m_pDataModel->getInofs());
			if (errMsg.HasError())
			{
				errList << errMsg;
				g_COperate->MsgPrint(errMsg);
			}
		}
	}
	else if (ckLevel == CheckLevel::ProjectLevel)
	{
		foreach(QFileInfo sensorFInfo, fileList)
		{
			if (!sensorFInfo.isDir())
			{
				//此处是否需要报错？？
				otherErrMsg << QString("%1目录下存在多余文件：%2").arg(projectDir.dirName()).arg(sensorFInfo.fileName());
				continue;
			}
			QString sensorFolder = sensorFInfo.absoluteFilePath();
			QDir sensorDir(sensorFolder);
			sensorDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
			sensorDir.setSorting(QDir::DirsFirst);
			QFileInfoList imgFolderList = sensorDir.entryInfoList(); // 获取所有的文件信息
			foreach(QFileInfo imgFInfo, imgFolderList)
			{
				if (!imgFInfo.isDir())
				{
					//此处是否需要报错？？
					otherErrMsg << QString("%1目录下存在多余文件：%2").arg(sensorDir.dirName()).arg(imgFInfo.fileName());
					continue;
				}
				QString imgFolder = imgFInfo.absoluteFilePath();
				ErrorMsgStruct errMsg = checkImageFolder(imgFolder, m_pDataModel->getInofs());
				if (errMsg.HasError())
				{
					errList << errMsg;
					g_COperate->MsgPrint(errMsg);
				}
			}
		}
	}
	if (otherErrMsg.size() > 0)
	{
		otherErrMsg.insert(0, QString("其他错误信息如下："));
		g_COperate->MsgPrint(otherErrMsg.join("\r\n"));
	}

	curTime = QDateTime::currentDateTime();
	QString strEndTime = QString("检查结束：%1\r\n").arg(curTime.toString("yyyy.MM.dd hh:mm:ss"));
	g_COperate->MsgPrint(strEndTime);

	if (errList.size() == 0 && otherErrMsg.size() == 0)
	{
		QMessageBox::information(this, "提示", "检查未发现错误！", QMessageBox::Ok, QMessageBox::Ok);
	}
	else
	{//将检查记录输出到文件
		QString sFilePath = inputFolder + "/checkresult.txt";

		QFile file(sFilePath);
		//方式：Append为追加，WriteOnly，ReadOnly  
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			QMessageBox::critical(NULL, "Tip", "无法创建文件！");
			return;
		}
		QTextStream out(&file);
		out << strStartTime.replace("\r\n", "\n");
		out << endl;
		for (int i = 0; i < errList.size(); ++i)
		{
			out << g_COperate->GetErrMsg(errList.at(i)).replace("\r\n", "\n");
			out << endl;
		}
		if (otherErrMsg.size() > 0)
		{
			out << otherErrMsg.join("\n");
			out << endl;
		}
		out << strEndTime.trimmed().replace("\r\n", "\n");
		out << endl;

		out.flush();
		file.close();
		QString msg = QString("检查记录已输出到：%1/checkresult.txt文件中！").arg(inputFolder);
		QMessageBox::information(this, "提示", msg, QMessageBox::Ok, QMessageBox::Ok);
	}
}

ErrorMsgStruct ArchiveCheckWnd::checkImageFolder(const QString& imageDirPath, const QList<CheckItemStruct>& checkItems,
	Qt::CaseSensitivity isFileNameCs/* = Qt::CaseInsensitive*/, Qt::CaseSensitivity isSuffixCs/* = Qt::CaseInsensitive*/)
{
	QFileInfo info(imageDirPath);
	if (!info.isDir() || !info.exists()/* || checkItems.size() <= 0*/)
	{
		return ErrorMsgStruct();
	}
	QDir imageDir(imageDirPath);
	QString parentFolder = ParentDirName(imageDirPath);
	QString imageFolder = imageDir.dirName();		//影像文件夹名称
	QString imagePrefix = imageFolder;	
	int sIdx = imagePrefix.indexOf("-");
	int eIdx = imagePrefix.lastIndexOf("-");
	if (eIdx > sIdx && eIdx < imagePrefix.size() - 1)
	{//影像规范名称//文件前缀为文件夹名去掉首尾-
		imagePrefix = imagePrefix.left(sIdx) + imagePrefix.mid(sIdx + 1, eIdx - sIdx - 1) + imagePrefix.mid(eIdx + 1);
	}
	else
	{//影像规范名称//文件前缀为文件夹名去掉-
		imagePrefix = imagePrefix.remove("-");
	}

	QStringList requirdItems, optinalItems;
	for (int i = 0; i < checkItems.size(); ++i)
	{
		CheckItemStruct checkItem = checkItems.at(i);
		if (!checkItem._isCheck || checkItem._condition.isEmpty())
		{
			continue;
		}
		if (checkItem._state == Required)
		{
			requirdItems << checkItem._condition.replace("*", imagePrefix);
		}
		else if (checkItem._state == Optional)
		{
			optinalItems << checkItem._condition.replace("*", imagePrefix);
		}
	}

	ErrorMsgStruct errMsg;
	errMsg._parentFolder = parentFolder;
	errMsg._imageFolder = imageFolder;
	imageDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
	QFileInfoList fileList = imageDir.entryInfoList(); // 获取所有的文件信息
	foreach(QFileInfo file, fileList)
	{ //遍历文件信息
		if (file.isFile())
		{ 
			QString fileName;
			fileName = file.fileName();
			if (!fileName.startsWith(imagePrefix, isFileNameCs))
			{
				errMsg._extraFiles << fileName;
				continue;
			}
			//if (!requirdItems.contains(fileName) &&
			//	!optinalItems.contains(fileName))
			if (!QStrListContainsQStr(requirdItems,fileName,isFileNameCs,isSuffixCs) &&
				!QStrListContainsQStr(optinalItems, fileName, isFileNameCs, isSuffixCs))
			{
				//暂时都归为多余文件
				//errMsg._illegalFiles << fileName;
				errMsg._extraFiles << fileName;
				continue;
			}
			//if (requirdItems.contains(fileName))
			//{
			//	requirdItems.removeOne(fileName);
			//}
			int index = QStrListIndexOfQStr(requirdItems, fileName, isFileNameCs, isSuffixCs);
			if (index != -1)
			{
				requirdItems.removeAt(index);
			}
		}
		else
		{
			errMsg._extraFiles << file.fileName();
		}
	}
	errMsg._missingFiles << requirdItems;
	return errMsg;
}

void ArchiveCheckWnd::readCheckItems(QList<CheckItemStruct>& checkItems)
{
	QString itemsConfig = GlobalConfig::GetProperty("CHECKIMTESCONFIG");
	QStringList items = itemsConfig.split("||", QString::SkipEmptyParts);
	for (int i = 0; i < items.size(); ++i)
	{
		QString item = items.at(i);
		QStringList parts = item.split("|", QString::SkipEmptyParts);
		if (parts.size() == 3)
		{
			CheckItemStruct checkItem;
			checkItem._isCheck = parts.at(0).trimmed().toInt();
			checkItem._state = (CheckStateEnum)parts.at(1).trimmed().toInt();
			checkItem._condition = parts.at(2).trimmed();
			checkItems << checkItem;
		}
	}
}

void ArchiveCheckWnd::writeCheckItems(const QList<CheckItemStruct>& checkItems)
{
	QString itemsConfig;
	QStringList items;
	for (int i = 0; i < checkItems.size(); ++i)
	{
		CheckItemStruct checkItem = checkItems.at(i);
		items << QString("%1|%2|%3").arg(checkItem._isCheck).arg(checkItem._state).arg(checkItem._condition);
	}
	itemsConfig = items.join("||");
	GlobalConfig::SetProperty("CHECKIMTESCONFIG", itemsConfig);
}



CheckItemModel::CheckItemModel()
{
	m_headData << "选择" << "模式" << "检查项";
}

CheckItemModel::~CheckItemModel()
{

}

int CheckItemModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return m_itemList.size();
}

int CheckItemModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return 3;
}

QVariant CheckItemModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	if (!index.isValid())
	{
		return QVariant();
	}
	int row = index.row();
	int col = index.column();
	if (role == Qt::TextAlignmentRole)
	{
		return int(Qt::AlignHCenter | Qt::AlignVCenter);
	}
	else if (role == Qt::UserRole)
	{
		return QVariant();
	}
	else if (role == Qt::DisplayRole || role == Qt::EditRole /*|| role == Qt::ToolTipRole*/)
	{
		switch (col)
		{
		case 0:
			return (m_itemList.at(row)._isCheck) ? QString("√") : QString();
			break;
		case 1:
			return (m_itemList.at(row)._state == CheckStateEnum::Required) ? QString("必选") : QString("可选");
			break;
		case 2:
			return m_itemList.at(row)._condition;
			break;
		default:
			break;
		}
	}
	else if (role == Qt::BackgroundColorRole)
	{
		//QColor res = row % 2 == 0 ? QColor(229, 244, 236) : QColor(230, 245, 250);
		//return res;
		QColor res = (m_itemList.at(row)._state == CheckStateEnum::Required) ? QColor(238, 203, 173) : QColor(230, 245, 250);
		return res;
	}
	return QVariant();
}

QVariant CheckItemModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
	if (role != Qt::DisplayRole)
	{
		return QVariant();
	}
	if (orientation == Qt::Vertical)
	{
		return section + 1;
	}
	if (section >= m_headData.size())
	{
		return QVariant();
	}
	return m_headData.at(section);
}

Qt::ItemFlags CheckItemModel::flags(const QModelIndex &index) const
{
	int col = index.column();
	int row = index.row();
	if (col == 0)
	{
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
	else if (col == 1 || col == 2)
	{
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
	return QAbstractTableModel::flags(index);
}

void CheckItemModel::deleteItems(QList<int> idxs)
{
	qSort(idxs.begin(), idxs.end());
	for (int i = idxs.size() - 1; i >= 0; --i)
	{
		int idx = idxs.at(i);
		if (idx >= 0 && idx < m_itemList.size())
		{
			m_itemList.removeAt(idx);
		}
	}
	flushView();
}

void CheckItemModel::deleteItems(QModelIndexList idxs)
{
	qSort(idxs.begin(), idxs.end());
	for (int i = idxs.size() - 1; i >= 0; --i)
	{
		int idx = idxs.at(i).row();
		if (idx >= 0 && idx < m_itemList.size())
		{
			m_itemList.removeAt(idx);
		}
	}
	flushView();
}
