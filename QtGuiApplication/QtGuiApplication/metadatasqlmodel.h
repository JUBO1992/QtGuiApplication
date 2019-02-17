#ifndef METADATASQLMODEL_H
#define METADATASQLMODEL_H

#include <QSqlTableModel>
#include "marinebase.h"

class CMarineProject;

//元数据TableView Model
class MetadataSqlModel : public QSqlTableModel
{
	Q_OBJECT

public:
	MetadataSqlModel(QSqlDatabase db, CMarineProject* prj, const MSFileEnum& type);
	~MetadataSqlModel();

	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	int rowCount(const QModelIndex &parent /* = QModelIndex() */) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role /* = Qt::EditRole */) override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	void setTable(const QString &tableName) override;

	//设置不可编辑字段
	void setNoEditFields(const QStringList& strlist);

	//根据list获取文件编号
	QStringList getFileNOList(const QModelIndexList& list);

	//click
	void itemClicked(const QModelIndex& idx);

	//提交Index表的更改
	void submitIndex();

	void flushView(int row)
	{
		QModelIndex topleft = index(row, 0);
		QModelIndex bottomRight = index(row, columnCount() - 1);
		emit dataChanged(topleft, bottomRight, QVector<int>() << Qt::DisplayRole);
	}

private:
	MSFileStruct getMSFile(const MSFileEnum& type, const QString& fileNO) const ;

private:
	QStringList m_noEditFields;	//不可编辑字段
	QStringList m_extraHead;	//额外添加字段
	QStringList m_originalHead;	//table原始字段

	MSFileEnum m_fileType;		//当前表格的文件类型
	CMarineProject* m_pMarinePrj;

	int m_fileNOCol;			//fileNO所在列索引

	bool m_editFlag;			//标记父类调用还是手动调用
};

#endif // METADATASQLMODEL_H
