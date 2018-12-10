#ifndef ARCHIVECHECKWND_H
#define ARCHIVECHECKWND_H

#include <QWidget>
#include "ui_archivecheckwnd.h"
#include "commonoperate.h"
#include <QMenu>
#include <QAction>

class CheckItemModel;
class ArchiveCheckWnd : public QWidget
{
	Q_OBJECT

public:
	ArchiveCheckWnd(QWidget *parent = 0);
	~ArchiveCheckWnd();

	private slots:
	void on_pushButton_scanFolder_clicked();
	void on_pushButton_runCheck_clicked();
	void on_tableView_clicked(QModelIndex idx);
	void on_tableView_doubleClicked(QModelIndex idx);
	void clicked_rightMenu(const QPoint &pos);
	void clicked_addItem();
	void clicked_deleteItem();
	void clicked_modifyItem();
	void on_pushButton_infoWnd_clicked();
	void on_pushButton_clearInfoWnd_clicked();

private:
	//执行检查
	void runCheck(QString inputFolder);
	//以单景影像文件夹为单位进行检查
	ErrorMsgStruct checkImageFolder(const QString& imageDirPath, const QList<CheckItemStruct>& checkItems,
		Qt::CaseSensitivity isFileNameCs = Qt::CaseInsensitive, Qt::CaseSensitivity isSuffixCs = Qt::CaseInsensitive);
	//从配置文件读取检查项
	void readCheckItems(QList<CheckItemStruct>& checkItems);
	//将检查项写入配置文件
	void writeCheckItems(const QList<CheckItemStruct>& checkItems);

private:
	Ui::ArchiveCheckWnd ui;
	CheckItemModel* m_pDataModel;

	QMenu *m_rightMenu;
	QAction *m_addItem;
	QAction *m_modifyItem;
	QAction *m_delteItem;
};

class CheckItemModel :public QAbstractTableModel
{
	Q_OBJECT
public:
	CheckItemModel();
	~CheckItemModel();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

	void addItem(CheckItemStruct item)
	{
		m_itemList << item;
		flushView();
	}
	void deleteItem(int idx)
	{
		if (idx >= 0 && idx < m_itemList.size())
		{
			m_itemList.removeAt(idx);
			flushView();
		}
	}
	void deleteItem(QModelIndex idx)
	{
		deleteItem(idx.row());
	}
	void deleteItems(QList<int> idxs);
	void deleteItems(QModelIndexList idxs);

	void modifyItem(int idx, CheckItemStruct item)
	{
		if (idx >=0 && idx < m_itemList.size())
		{
			m_itemList.removeAt(idx);
			m_itemList.insert(idx, item);
			flushView(idx);
		}
	}
	void clickItem(int idx)
	{
		if (idx >= 0 && idx < m_itemList.size())
		{
			CheckItemStruct item = m_itemList.at(idx);
			item._isCheck = !item._isCheck;
			modifyItem(idx, item);
		}
	}

	void setHead(QStringList head) { m_headData = head; }
	void setInfos(const QList<CheckItemStruct>& prjList)
	{
		beginResetModel();
		m_itemList = prjList;
		endResetModel();
	}
	QList<CheckItemStruct> getInofs() const { return m_itemList; }

	void reset(){ m_itemList.clear(); }

	void flushView()
	{
		beginResetModel();
		endResetModel();
	}
	void flushView(int row)
	{
		QModelIndex topleft = index(row, 0);
		QModelIndex bottomRight = index(row, columnCount() - 1);
		emit dataChanged(topleft, bottomRight, QVector<int>() << Qt::DisplayRole);
	}

private:
	QStringList m_headData;
	QList<CheckItemStruct> m_itemList;
};

#endif // ARCHIVECHECKWND_H
