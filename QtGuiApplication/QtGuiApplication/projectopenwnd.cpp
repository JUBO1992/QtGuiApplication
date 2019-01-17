#include "projectopenwnd.h"
//#include "idatainterface.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QScrollBar>
#include "marineproject.h"

//extern iDataInterface* g_pInterface;

ProjectOpenWnd::ProjectOpenWnd(QWidget *parent)
	: QDialog(parent)
	, m_dataModel(NULL)
{
	ui.setupUi(this);
	QColor qclr(129, 168, 211);
	QPalette palette;
	palette.setBrush(this->backgroundRole(), qclr);
	this->setPalette(palette);

	//读取海测工程列表
	ReadProjectMenu(m_prjList);
	//初始化窗口
	initWnd();
}

ProjectOpenWnd::~ProjectOpenWnd()
{
	if (m_dataModel)
	{
		delete m_dataModel;
		m_dataModel = NULL;
	}
}

void ProjectOpenWnd::initWnd()
{
	m_dataModel = new MarinePrjModel;
	ui.tableView->setModel(m_dataModel);
	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	ui.tableView->horizontalHeader()->setStretchLastSection(true);
	ui.tableView->setColumnWidth(0, 45);
	m_dataModel->setInfos(m_prjList.values());

	QStringList recntPrjs = MSGetProperty("RecentMarinePrjs").toString().split("|", QString::SkipEmptyParts);
	QStringList headerLabels;
	ui.tableWidget->setColumnCount(2);
	ui.tableWidget->setRowCount(recntPrjs.size());
	headerLabels << QString("选择") << QString("项目编号");
	ui.tableWidget->setHorizontalHeaderLabels(headerLabels);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
	ui.tableWidget->setColumnWidth(0, 45);
	ui.tableWidget->verticalHeader()->setVisible(false);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget->horizontalScrollBar()->setVisible(false);
	for (int i = 0; i < recntPrjs.size(); ++i)
	{
		QTableWidgetItem *newItem = new QTableWidgetItem("");
		newItem->setTextAlignment(Qt::AlignCenter | Qt::AlignHCenter);
		ui.tableWidget->setItem(i, 0, newItem);

		newItem = new QTableWidgetItem(recntPrjs.at(i));
		newItem->setTextAlignment(Qt::AlignCenter | Qt::AlignHCenter);
		newItem->setData(0, recntPrjs.at(i));
		auto it = m_prjList.find(recntPrjs.at(i));
		if (it != m_prjList.end())
		{
			newItem->setToolTip(it.value()._prjName);
		}
		ui.tableWidget->setItem(i, 1, newItem);
	}
}

void ProjectOpenWnd::clearTableSelect()
{
	m_dataModel->setCurIdx(-1);
	int rowSize = ui.tableWidget->rowCount();
	for (int i = 0; i < rowSize; ++i)
	{
		ui.tableWidget->item(i, 0)->setText(QString());
	}
}

void ProjectOpenWnd::on_pushButton_open_clicked()
{
	if (m_selPrj._prjID.isEmpty())
	{
		QMessageBox::information(this, "提示", "工程不存在，请重新选择！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	QStringList recntPrjs = MSGetProperty("RecentMarinePrjs").toString().split("|", QString::SkipEmptyParts);
	int index = recntPrjs.indexOf(m_selPrj._prjID);
	if (index != -1)
	{
		recntPrjs.removeAt(index);
	}
	recntPrjs.insert(0, m_selPrj._prjID);
	if (recntPrjs.size() >= 5)
	{
		recntPrjs = recntPrjs.mid(0, 4);
	}
	MSSetProperty("RecentMarinePrjs", recntPrjs.join("|"));

	//QString msg = QString("工程编号：%1，工程名称：%2").arg(m_selPrj._prjID).arg(m_selPrj._prjName);
	//QMessageBox::information(this, "提示", msg, QMessageBox::Ok, QMessageBox::Ok);
	CMarineProject projct(m_selPrj);
	QMap<QString, MSFileStruct> vectorFiles;
	projct.getVectorList(vectorFiles);
	if (vectorFiles.size() == 0)
	{
		QMessageBox::information(this, "提示", "该项目下不存在矢量数据！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	accept();
	bool isSuccess = OpenMarinePrj(m_selPrj);
	if (!isSuccess)
	{
		QMessageBox::information(this, "提示", "文件打开失败！", QMessageBox::Ok, QMessageBox::Ok);
	}
}

void ProjectOpenWnd::on_pushButton_close_clicked()
{
	reject();
}

void ProjectOpenWnd::on_tableView_clicked(QModelIndex idx)
{
	clearTableSelect();
	int row = idx.row();
	QList<MarinePrjStruct> prjList = m_dataModel->getInofs();
	if (row >= 0 && row < prjList.size())
	{
		m_selPrj = prjList.at(row);
		m_dataModel->setCurIdx(row);
	}
	else
	{
		m_selPrj = MarinePrjStruct();
		m_dataModel->setCurIdx(-1);
	}
}

void ProjectOpenWnd::on_tableView_doubleClicked(QModelIndex idx)
{
	clearTableSelect();
	int row = idx.row();
	QList<MarinePrjStruct> prjList = m_dataModel->getInofs();
	if (row >= 0 && row < prjList.size())
	{
		m_selPrj = prjList.at(row);
		m_dataModel->setCurIdx(row);
	}
	else
	{
		m_selPrj = MarinePrjStruct();
		m_dataModel->setCurIdx(-1);
	}
	on_pushButton_open_clicked();
}

void ProjectOpenWnd::on_tableWidget_clicked(QModelIndex idx)
{
	clearTableSelect();
	int row = idx.row();
	ui.tableWidget->item(row, 0)->setText(QString("√"));
	QString prjID = ui.tableWidget->item(row, 1)->data(0).toString();
	auto it = m_prjList.find(prjID);
	if (it != m_prjList.end())
	{
		m_selPrj = it.value();
	}
	else
	{
		m_selPrj = MarinePrjStruct();
	}
}

void ProjectOpenWnd::on_tableWidget_doubleClicked(QModelIndex idx)
{
	clearTableSelect();
	int row = idx.row();
	ui.tableWidget->item(row, 0)->setText(QString("√"));
	QString prjID = ui.tableWidget->item(row, 1)->data(0).toString();
	auto it = m_prjList.find(prjID);
	if (it != m_prjList.end())
	{
		m_selPrj = it.value();
	}
	else
	{
		m_selPrj = MarinePrjStruct();
	}
	on_pushButton_open_clicked();
}

/*---------------------------------MarinePrjModel--------------------------------------*/
MarinePrjModel::MarinePrjModel()
{
	m_headData << "选择" << "项目编号" << "项目名称";
}

MarinePrjModel::~MarinePrjModel()
{

}

int MarinePrjModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return m_prjList.size();
}

int MarinePrjModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return 3;
}

QVariant MarinePrjModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
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
	else if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
	{
		switch (col)
		{
		case 0:
			return  (m_curIdx == row) ? QString("√") : QString();
			break;
		case 1:
			return m_prjList.at(row)._prjID;
			break;
		case 2:
			return m_prjList.at(row)._prjName;
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

QVariant MarinePrjModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
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

Qt::ItemFlags MarinePrjModel::flags(const QModelIndex &index) const
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

bool MarinePrjModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
	if (!index.isValid())
	{
		return false;
	}
	return QAbstractTableModel::setData(index, value, role);
}

bool MarinePrjModel::removeRows(int row, int count, const QModelIndex &parent /*= QModelIndex()*/)
{
	return false;
}

void MarinePrjModel::setCurIdx(int idx)
{
	m_curIdx = idx;
	for (int i = 0; i < m_prjList.size(); ++i)
	{
		flushView(i);
	}
}
