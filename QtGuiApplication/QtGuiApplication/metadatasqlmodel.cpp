#include "metadatasqlmodel.h"
#include "QColor"
#include "QSqlRecord"
#include "marineproject.h"

#pragma execution_character_set("utf-8")

MetadataSqlModel::MetadataSqlModel(QSqlDatabase db, CMarineProject* prj, const MSFileEnum& type)
	: QSqlTableModel(0, db)
	, m_pMarinePrj(prj)
	, m_fileType(type)
	, m_fileNOCol(-1)
	, m_editFlag(false)
{
	m_extraHead << "文件名称";
	if (m_fileType == MSFileEnum::RasterFile)
	{
		m_extraHead << "是否打开";
	}
	m_noEditFields << MS_CHARTNO << MS_DATANO << MS_FILENO << "文件名称" << "是否打开";
	//交换列顺序
	//QSqlTableModel::moveColumn(QModelIndex(), 1, QModelIndex(), 0);
}

MetadataSqlModel::~MetadataSqlModel()
{

}

int MetadataSqlModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return m_extraHead.size() + record().count();
}

int MetadataSqlModel::rowCount(const QModelIndex &parent /* = QModelIndex() */) const
{
	return QSqlTableModel::rowCount(parent);
}

QVariant MetadataSqlModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
	if (role != Qt::DisplayRole)
	{
		return QVariant();
	}
	if (orientation == Qt::Vertical)
	{
		return section + 1;
	}
	if (section < m_extraHead.size())
	{
		return m_extraHead.at(section);
	}
	return QSqlTableModel::headerData(section - m_extraHead.size(), orientation, role);

	//if (section >= record().count())
	//{
	//	return m_extraHead.at(section - m_originalHead.size());
	//}
	//return QSqlTableModel::headerData(section, orientation, role);
}

bool MetadataSqlModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role /* = Qt::EditRole */)
{
	if (orientation == Qt::Vertical)
	{
		return QSqlTableModel::setHeaderData(section, orientation, role);
	}
	if (section < m_extraHead.size())
	{
		return true;
	}
	return QSqlTableModel::setHeaderData(section - m_extraHead.size(), orientation, role);

	//if (section >= m_originalHead.size())
	//{
	//	return true;
	//}
	//return QSqlTableModel::setHeaderData(section, orientation, role);
}

Qt::ItemFlags MetadataSqlModel::flags(const QModelIndex& index) const
{
	if (m_editFlag)
	{
		return QSqlTableModel::flags(index) | Qt::ItemIsEditable;
	}
	QString field_string = headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();
	if (m_noEditFields.contains(field_string, Qt::CaseInsensitive))
	{
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	}
	int row = index.row();
	int column = index.column();
	QModelIndex idx = QSqlTableModel::index(row, column - m_extraHead.size());
	return QSqlTableModel::flags(idx) | Qt::ItemIsEditable;
}

QVariant MetadataSqlModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	int row = index.row();
	int column = index.column();
	if (role == Qt::TextAlignmentRole)
	{
		return int(Qt::AlignHCenter | Qt::AlignVCenter);
	}
	if (role == Qt::UserRole)
	{
		return QVariant();
	}
	if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
	{
		if (m_editFlag)
		{
			return QSqlTableModel::data(index, role);
		}
		if (column < m_extraHead.size())
		{
			QModelIndex NOIdx = QSqlTableModel::index(row, m_fileNOCol);
			QString fileNO = QSqlTableModel::data(NOIdx, Qt::DisplayRole).toString();
			MSFileStruct curfile = getMSFile(m_fileType, fileNO);
			QString field_string = headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
			if (field_string == m_extraHead.first())
			{
				if (role == Qt::ToolTipRole)
				{
					return curfile._relativePath;
				}
				else
				{
					return curfile.fileName();
				}
			}
			else if (field_string == m_extraHead.last())
			{
				return (curfile._isOpen) ? QString("√") : QString("×");
			}
		}
		else
		{
			QModelIndex idx = QSqlTableModel::index(row, column - m_extraHead.size());
			return QSqlTableModel::data(idx, role);
		}

		//if (column >= m_originalHead.size())
		//{
		//	QModelIndex NOIdx = QSqlTableModel::index(row, m_fileNOCol);
		//	QString fileNO = QSqlTableModel::data(NOIdx, Qt::DisplayRole).toString();
		//	MSFileStruct curfile = getMSFile(m_fileType, fileNO);
		//	QString field_string = headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
		//	if (field_string == m_extraHead.first())
		//	{
		//		if (role == Qt::ToolTipRole)
		//		{
		//			return curfile._relativePath;
		//		}
		//		else
		//		{
		//			return curfile.fileName();
		//		}
		//	}
		//	else if (field_string == m_extraHead.last())
		//	{
		//		return (curfile._isOpen) ? QString("√") : QString("×");
		//	}
		//}
		//else
		//{
		//	return QSqlTableModel::data(index, role);
		//}
	}
	if (role == Qt::BackgroundColorRole)
	{
		QModelIndex NOIdx = QSqlTableModel::index(row, m_fileNOCol);
		QString fileNO = QSqlTableModel::data(NOIdx, Qt::DisplayRole).toString();
		MSFileStruct curfile = getMSFile(m_fileType, fileNO);
		if (!curfile.isExist())
		{
			return QColor(255, 99, 71);
		}
		QColor res = row % 2 == 0 ? QColor(229, 244, 236) : QColor(230, 245, 250);
		return res;
	}
	return QVariant();
}

bool MetadataSqlModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
	if (role != Qt::EditRole)
	{//限定setData只支持Qt::EditRole
		return false;
	}
	int row = index.row();
	int column = index.column();
	if (column < m_extraHead.size())
	{
		return true;
	}
	QModelIndex idx = QSqlTableModel::index(row, column - m_extraHead.size());
	m_editFlag = true;
	bool isSuccess = QSqlTableModel::setData(idx, value, role);
	m_editFlag = false;
	return isSuccess;

	//if (column >= m_originalHead.size())
	//{
	//	return true;
	//}
	//bool isSuccess = QSqlTableModel::setData(index, value, role);
	//return isSuccess;
}

void MetadataSqlModel::setTable(const QString &tableName)
{
	QSqlTableModel::setTable(tableName);
	QString fileNO;
	if (m_fileType == MSFileEnum::RasterFile)
	{
		fileNO = MS_CHARTNO;
	}
	else if (m_fileType == MSFileEnum::SurveyFile)
	{
		fileNO = MS_DATANO;
	}
	else if (m_fileType == MSFileEnum::ArchiveFile)
	{
		fileNO = MS_FILENO;
	}
	QSqlRecord rcd = record();
	for (int i = 0; i < rcd.count(); ++i)
	{
		if (rcd.fieldName(i).compare(fileNO,Qt::CaseInsensitive) == 0)
		{
			m_fileNOCol = i;
		}
		m_originalHead.push_back(rcd.fieldName(i));
	}
}

void MetadataSqlModel::setNoEditFields(const QStringList& strlist)
{
	m_noEditFields << strlist;
}

QStringList MetadataSqlModel::getFileNOList(const QModelIndexList& list)
{
	QStringList fileNOList;
	for (int i = 0; i < list.size(); ++i)
	{
		QModelIndex idx = list.at(i);
		QModelIndex NOIdx = QSqlTableModel::index(idx.row(), m_fileNOCol);
		QString fileNO = QSqlTableModel::data(NOIdx, Qt::DisplayRole).toString();
		if (!fileNO.isEmpty() && !fileNOList.contains(fileNO))
		{
			fileNOList.push_back(fileNO);
		}
	}
	return fileNOList;
}

void MetadataSqlModel::itemClicked(const QModelIndex& idx)
{
	int row = idx.row();
	int column = idx.column();
	if (MSFileEnum::RasterFile == m_fileType)
	{
		QString field_string = headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
		if (field_string != m_extraHead.last())
		{
			return;
		}
		QModelIndex NOIdx = QSqlTableModel::index(row, m_fileNOCol);
		QString fileNO = QSqlTableModel::data(NOIdx, Qt::DisplayRole).toString();
		MSFileStruct curfile = getMSFile(m_fileType, fileNO);
		if (curfile.isValid())
		{
			m_pMarinePrj->updateMSFile(fileNO, QString(), !curfile._isOpen);
			flushView(row);
		}
	}
}

void MetadataSqlModel::submitIndex()
{
	if (!m_pMarinePrj)
	{
		return;
	}
	m_pMarinePrj->commitMSFile();
}

MSFileStruct MetadataSqlModel::getMSFile(const MSFileEnum& type, const QString& fileNO) const
{
	if (!m_pMarinePrj)
	{
		return MSFileStruct();
	}
	MSFileStruct curfile;
	QMap<QString, MSFileStruct> files;
	if (m_fileType == MSFileEnum::RasterFile)
	{
		m_pMarinePrj->getRasterList(files);
	}
	else if (m_fileType == MSFileEnum::SurveyFile)
	{
		m_pMarinePrj->getSurveyList(files);
	}
	else if (m_fileType == MSFileEnum::ArchiveFile)
	{
		m_pMarinePrj->getArchiveList(files);
	}
	auto it = files.find(fileNO);
	if (it != files.end())
	{
		curfile = it.value();
	}
	return curfile;
}
