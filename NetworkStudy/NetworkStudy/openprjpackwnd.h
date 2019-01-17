#ifndef OPENPRJPACKWND_H
#define OPENPRJPACKWND_H

#include <QDialog>
#include "ui_openprjpackwnd.h"
#include <QAbstractTableModel>
#include "ynsdbase.h"

class StageListModel;
class OpenPrjPackWnd : public QDialog
{
	Q_OBJECT

public:
	OpenPrjPackWnd(QWidget *parent = 0);
	~OpenPrjPackWnd();


private:

private slots :
	void on_pushButton_select_clicked();
	void on_pushButton_ok_clicked();
	void on_pushButton_cancel_clicked();
	void on_tableView_clicked(QModelIndex);
	void on_tableView_doubleClicked(QModelIndex);

private:
	Ui::OpenPrjPackWnd ui;
	StageListModel* m_pModel;
	QString m_curStage;		//当前阶段
	YNSDPrjInfo m_prjInfo;	//工程信息
};

class StageListModel :public QAbstractTableModel
{
	Q_OBJECT
public:
	StageListModel();
	~StageListModel();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

	void setInfos(const QStringList& stageData)
	{
		beginResetModel();
		m_stageData = stageData;
		endResetModel();
	}
	QStringList getInfos(){ return m_stageData; }

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
	QStringList m_stageData;
	int m_curIdx;
};

#endif // OPENPRJPACKWND_H
