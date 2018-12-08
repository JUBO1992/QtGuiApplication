#ifndef PROJECTOPENWND_H
#define PROJECTOPENWND_H

#include <QDialog>
#include "ui_projectopenwnd.h"
#include "marinebase.h"
#include <QAbstractTableModel>
#include <QStringListModel>

class MarinePrjModel;

//项目打开窗口
class ProjectOpenWnd : public QDialog
{
	Q_OBJECT

public:
	ProjectOpenWnd(QWidget *parent = 0);
	~ProjectOpenWnd();

private:
	void initWnd();
	void clearTableSelect();

private slots:
	void on_pushButton_open_clicked();
	void on_pushButton_close_clicked();
	void on_tableView_clicked(QModelIndex);
	void on_tableView_doubleClicked(QModelIndex);
	void on_tableWidget_clicked(QModelIndex);
	void on_tableWidget_doubleClicked(QModelIndex);

private:
	Ui::ProjectOpenWnd ui;
	MarinePrjModel* m_dataModel;				
	MarinePrjStruct m_selPrj;		//记录当前选中的工程
	QMap<QString, MarinePrjStruct> m_prjList;	//记录海测工程列表
};

class MarinePrjModel :public QAbstractTableModel
{
	Q_OBJECT
public:
	MarinePrjModel();
	~MarinePrjModel();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

	void setHead(QStringList head) { m_headData = head; }
	void setInfos(const QList<MarinePrjStruct>& prjList)
	{ 
		beginResetModel();
		m_prjList = prjList;
		endResetModel();
	}
	QList<MarinePrjStruct> getInofs() const { return m_prjList; }

	void reset(){ m_prjList.clear(); }

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

	void setCurIdx(int idx);
	int getCurIdx(){ return m_curIdx; }
private:
	QStringList m_headData;
	QList<MarinePrjStruct> m_prjList;
	int m_curIdx;
};

#endif // PROJECTOPENWND_H
