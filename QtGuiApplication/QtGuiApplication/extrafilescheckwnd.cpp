#include "extrafilescheckwnd.h"s
#include "marineproject.h"
#include <QListWidgetItem>
#include <QMenu>
#include <QMessageBox>

#pragma execution_character_set("utf-8")

ExtraFilesCheckWnd::ExtraFilesCheckWnd(QWidget *parent)
	: QDialog(parent)
	, m_pCurMarinePrj(NULL)
	, m_fType(MSFileEnum::RasterFile)
{
	ui.setupUi(this);

	ui.listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	m_pRightMenu = new QMenu;
	m_pAddAction = new QAction("添加到元数据", this);
	m_pDeleteAction = new QAction("删除多余文件", this);

	m_pRightMenu->addAction(m_pAddAction);
	m_pRightMenu->addAction(m_pDeleteAction);

	connect(ui.listWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(clicked_rightMenu(QPoint)));
	connect(m_pAddAction, SIGNAL(triggered()), this, SLOT(clicked_addAction()));
	connect(m_pDeleteAction, SIGNAL(triggered()), this, SLOT(clicked_deleteAction()));

}

ExtraFilesCheckWnd::~ExtraFilesCheckWnd()
{

}

void ExtraFilesCheckWnd::updateWnd()
{
	if (m_pCurMarinePrj == NULL) return;

	m_extraList.clear();
	m_extraList = m_pCurMarinePrj->getExtraFiles(m_fType);
	ui.listWidget->clear();
	for (int i = 0; i < m_extraList.size(); ++i)
	{
		QString curfile = m_extraList.at(i);
		QListWidgetItem* item = new QListWidgetItem;
		item->setSizeHint(QSize(100, 25));
		item->setText(curfile);
		item->setToolTip(curfile);
		ui.listWidget->addItem(item);
		if (i % 2 == 0)
		{
			item->setBackgroundColor(QColor(229, 244, 236));
		}
		else
		{
			item->setBackgroundColor(QColor(230, 245, 250));
		}
	}
	int cnt = ui.listWidget->count();
}

void ExtraFilesCheckWnd::showDialog()
{
	if (ui.listWidget->count() == 0)
	{
		QMessageBox::information(this, "提示", "没有多余文件！");
		return;
	}
	exec();
}

void ExtraFilesCheckWnd::on_pushButton_addToMd_clicked()
{
	if (ui.listWidget->count() == 0)
	{
		QMessageBox::information(this, "提示", "列表为空！");
		return;
	}

	QStringList fileList;
	QList<QListWidgetItem*> list = ui.listWidget->selectedItems();
	if (list.size() == 0)
	{
		if (QMessageBox::Yes != QMessageBox::information(this, "提示", QString("确认添加列表中所有文件到元数据？"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
		{
			return;
		}
		for (int i = 0; i < m_extraList.size(); ++i)
		{
			fileList << m_pCurMarinePrj->projectPath() + "/" + m_extraList.at(i);
		}
	}
	else
	{
		for (int i = 0; i < list.size(); ++i)
		{
			fileList << m_pCurMarinePrj->projectPath() + "/" + list.at(i)->text();
		}
	}

	if (m_fType == MSFileEnum::RasterFile)
	{
		m_pCurMarinePrj->addRasters(fileList);
	}
	else if (m_fType == MSFileEnum::SurveyFile)
	{
		m_pCurMarinePrj->addSurveys(fileList);
	}
	else if (m_fType == MSFileEnum::ArchiveFile)
	{
		m_pCurMarinePrj->addArchives(fileList);
	}

	updateWnd();
	QMessageBox::information(this, "提示", "更新完成！");
}

void ExtraFilesCheckWnd::on_pushButton_delExtra_clicked()
{
	if (ui.listWidget->count() == 0)
	{
		QMessageBox::information(this, "提示", "列表为空！");
		return;
	}

	QStringList fileList;
	QList<QListWidgetItem*> list = ui.listWidget->selectedItems();
	if (list.size() == 0)
	{
		if (QMessageBox::Yes != QMessageBox::information(this, "提示", QString("确认删除列表中所有文件？"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
		{
			return;
		}
		for (int i = 0; i < m_extraList.size(); ++i)
		{
			fileList << m_pCurMarinePrj->projectPath() + "/" + m_extraList.at(i);
		}
	}
	else
	{
		for (int i = 0; i < list.size(); ++i)
		{
			fileList << m_pCurMarinePrj->projectPath() + "/" + list.at(i)->text();
		}
	}

	for (int i = 0; i < fileList.size(); ++i)
	{
		MSDelFileOrDir(fileList.at(i));
	}

	updateWnd();
	QMessageBox::information(this, "提示", "删除完成！");
}

void ExtraFilesCheckWnd::on_pushButton_Cancel_clicked()
{
	reject();
}

void ExtraFilesCheckWnd::clicked_rightMenu(const QPoint &pos)
{
	m_pRightMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
}

void ExtraFilesCheckWnd::clicked_addAction()
{
	on_pushButton_addToMd_clicked();
}

void ExtraFilesCheckWnd::clicked_deleteAction()
{
	on_pushButton_delExtra_clicked();

}
