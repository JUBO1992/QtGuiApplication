#ifndef FILEEXTRACT2_H
#define FILEEXTRACT2_H

#include <QWidget>
#include "ui_fileextract2.h"
#include "filedirlistmodel.h"

class FileExtract2 : public QWidget
{
	Q_OBJECT

public:
	FileExtract2(QWidget *parent = 0);
	~FileExtract2();

	private slots:
	void scanInputClicked();
	void scanOutputClicked();
	void scanListClicked();
	void textEidt_textChanged();
	void searchClicked();
	void searchResultClicked();
	void deleteClicked();
	void extractClicked();

	void on_checkBox_fuzzy_clicked();
	void on_checkBox_recursion_clicked();
	void on_checkBox_searchFile_clicked();
	void on_checkBox_searchDir_clicked();

private:
	void searchFileOrDir();
	void extractFileOrDir();
	/*! \breif 将处理结果写入txt文件
	*/
	void writeProcessResult();

private:
	Ui::FileExtract2 ui;
	FileDirListModel *m_model;
	QStringList m_list;
	QMap<QString, QStringList> m_searchresult;

	bool m_searchdir;
};

#endif // FILEEXTRACT2_H
