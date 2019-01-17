#include "openprjpackwnd.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QAbstractItemView>

//#include "idatainterface.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

//extern iDataInterface* g_pInterface;
extern QString g_current_packetpath;

OpenPrjPackWnd::OpenPrjPackWnd(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	m_pModel = new StageListModel;
	ui.tableView->setModel(m_pModel);
	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	ui.tableView->horizontalHeader()->setStretchLastSection(true);
	ui.tableView->setColumnWidth(0, 45);

	QStringList data;
	data << "内业解译" << "外业调查" << "内业建库";
	m_pModel->setInfos(data);

	if (!g_current_packetpath.isEmpty())
	{
		ui.lineEdit_packpath->setText(g_current_packetpath);
		QString xmlpath = g_current_packetpath + "/项目信息.xml";
		if (QFileInfo(xmlpath).exists())
		{
			if (readConfigFile(xmlpath, m_prjInfo))
			{
				m_curStage = m_prjInfo._xmjd;
				int index = m_pModel->getInfos().indexOf(m_curStage);
				m_pModel->setCurIdx(index);
			}
		}
	}

	QStringList tmpList;
	QString templatepath = qApp->applicationDirPath() + "/system/TEMPLESETFILE";
	QDir tmpDir(templatepath);
	if (tmpDir.exists())
	{
		tmpDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
		QFileInfoList fileInfoList = tmpDir.entryInfoList();
		foreach(QFileInfo fileInfo, fileInfoList)
		{
			if (fileInfo.fileName().endsWith(".db", Qt::CaseInsensitive))
			{
				tmpList << fileInfo.fileName();
			}
		}
	}

	tmpList.insert(0, "默认配置");
	ui.comboBox_template->addItems(tmpList);
}

OpenPrjPackWnd::~OpenPrjPackWnd()
{
	if (m_pModel)
	{
		delete m_pModel;
		m_pModel = NULL;
	}
}

void OpenPrjPackWnd::on_pushButton_select_clicked()
{
	QString tempdir = "";// g_pInterface->property("YNSD_PRJPACK_PATH").toString();
	tempdir = g_current_packetpath.isEmpty() ? tempdir : g_current_packetpath;
	QString path = QFileDialog::getExistingDirectory(this, tr("选择文件夹"),
		tempdir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (path.isEmpty())
	{
		return;
	}
	//g_pInterface->setProperty("YNSD_PRJPACK_PATH", path);
	ui.lineEdit_packpath->setText(path);
	QString xmlpath = path + "/项目信息.xml";
	if (!QFileInfo(xmlpath).exists())
	{
		QMessageBox::information(this, "提示", "项目包不完整！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	bool flag = readConfigFile(xmlpath, m_prjInfo);
	if (!flag)
	{
		QMessageBox::information(this, "提示", "项目信息配置文件格式不正确！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	m_curStage = m_prjInfo._xmjd;
	int index = m_pModel->getInfos().indexOf(m_curStage);
	m_pModel->setCurIdx(index);

}

void OpenPrjPackWnd::on_pushButton_ok_clicked()
{
	if (m_curStage.isEmpty())
	{
		QMessageBox::information(this, "提示", "请选择项目阶段！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	QString packpath = ui.lineEdit_packpath->text();
	QString stagepath = packpath;
	if (m_pModel->getInfos().indexOf(m_curStage) == 0)
	{//内业解译
		stagepath += "/01内业解译/01解译数据";
	}
	else if (m_pModel->getInfos().indexOf(m_curStage) == 1)
	{//外业调查
		stagepath += "/02外业调查/01调查数据";
	}
	else if (m_pModel->getInfos().indexOf(m_curStage) == 2)
	{//内业建库
		stagepath += "/03内业建库/01基础数据";
	}
	QString filename = QFileDialog::getOpenFileName(this, "请选择数据文件", stagepath, "mdb 文件(*.mdb)");
	if (filename.isEmpty())
	{
		return;
	}
	accept();
	g_current_packetpath = packpath;
	//下面打开mdb文件
	QString templatepath;
	if (ui.comboBox_template->currentIndex() == 0)
	{
		templatepath = "";
	}
	else
	{
		templatepath = ui.comboBox_template->currentText();
	}

	bool flag = true;
	if (templatepath.isEmpty())
	{
		//flag = g_pInterface->OpenDoc(filename, false, templatepath, 1);
	}
	else
	{
		//flag = g_pInterface->OpenDoc(filename, false, templatepath, 2);
	}
	//if (flag)
	//{
	//	g_pInterface->iDataPrintf(QString("'%1' 打开成功！").arg(QFileInfo(filename).fileName()));
	//}
	//else
	//{
	//	g_pInterface->iDataPrintf(QString("'%1' 打开失败！").arg(QFileInfo(filename).fileName()));
	//}
}

void OpenPrjPackWnd::on_pushButton_cancel_clicked()
{
	reject();
}

void OpenPrjPackWnd::on_tableView_clicked(QModelIndex idx)
{
	int row = idx.row();
	if (row >= 0 && row < m_pModel->getInfos().size())
	{
		m_pModel->setCurIdx(row);
		m_curStage = m_pModel->getInfos().at(row);
	}
	else
	{
		m_pModel->setCurIdx(-1);
		m_curStage = QString();
	}
}

void OpenPrjPackWnd::on_tableView_doubleClicked(QModelIndex idx)
{
	int row = idx.row();
	if (row >= 0 && row < m_pModel->getInfos().size())
	{
		m_pModel->setCurIdx(row);
		m_curStage = m_pModel->getInfos().at(row);
	}
	else
	{
		m_pModel->setCurIdx(-1);
		m_curStage = QString();
	}
	on_pushButton_ok_clicked();
}

/*---------------------------------StageListModel类--------------------------------------*/

StageListModel::StageListModel()
{
	m_headData << "选择" << "项目阶段";
}

StageListModel::~StageListModel()
{

}

int StageListModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return m_stageData.size();
}

int StageListModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return 2;
}

QVariant StageListModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
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
	else if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
	{
		switch (col)
		{
		case 0:
			return  (m_curIdx == row) ? QString("√") : QString();
			break;
		case 1:
			return m_stageData.at(row);
			break;
		default:
			break;
		}
	}
	else if (role == Qt::BackgroundColorRole)
	{
		QColor res = row % 2 == 0 ? QColor(229, 244, 236) : QColor(230, 245, 250);
		return res;
	}
	return QVariant();
}

QVariant StageListModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
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

Qt::ItemFlags StageListModel::flags(const QModelIndex &index) const
{
	int col = index.column();
	int row = index.row();
	if (col == 0)
	{
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
	else if (col == 1)
	{
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
	return QAbstractTableModel::flags(index);

}

void StageListModel::setCurIdx(int idx)
{
	m_curIdx = idx;
	for (int i = 0; i < m_stageData.size(); ++i)
	{
		flushView(i);
	}
}
