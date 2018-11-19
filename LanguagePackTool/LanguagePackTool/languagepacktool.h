#ifndef __LANGUAGEPACKTOOL_H__
#define __LANGUAGEPACKTOOL_H__

#include <QtWidgets/QMainWindow>
#include <QDomDocument> 
#include "ui_languagepacktool.h"
#include "languagepack.h"

class LanguagePackTool : public QMainWindow
{
	Q_OBJECT

public:
	LanguagePackTool(QWidget *parent = 0);
	~LanguagePackTool();

private:
	/*! \brief 根据配置参数获取CfgProfile对象
	*/
	CfgProfile getCfgProfile();
	/*! \brief 根据config更新配置参数
		\param [in] config 配置对象
	*/
	void updateConfigParams(const CfgProfile config);
	/*! \brief 根据配置参数更新窗体控件的值
	*/
	void updateWindowCtrls();

	void closeEvent(QCloseEvent *event);

	void SetWindowBackground();

private slots:
	void on_btn_OpenConfig_clicked();
	void on_btn_SaveConfig_clicked();
	void on_btn_Folder_clicked();
	void on_btn_Type_clicked();
	void on_btn_Exclude_clicked();
	void on_btn_ExtractLanguagePack_clicked();
	void on_btn_ApplyLanguagePack_clicked();
	void on_btn_OpenLanguageXml_clicked();
	void on_btn_SaveLanguageXml_clicked();
	void on_btn_ChineseSelect_clicked();
	void on_btn_Last_clicked();
	void on_btn_Next_clicked();
	void on_btn_NextUnfinished_clicked();
	void on_lineEdit_Translate_textEdited(QString);
	void on_btn_OpenOldLan_clicked();
	void on_btn_OpenNewLan_clicked();
	void on_btn_UpdateLanguagePack_clicked();

private:
	Ui::LanguagePackToolClass ui;

	LanguagePack _languagepackobj;				//LanguagePack对象

	QString _config_path;						//配置文件路径
	QStringList _folder_dir;					//处理文件夹
	QStringList _exclude_dir;					//排除文件夹
	QStringList _file_list;						//处理文件
	QList<QString> _type_list;					//处理文件类型
	QStringList _lan_list;						//语言
	QString _regx;								//正则表达式

	QString _lanxml_path;						//语言包路径
	QList<LanguageObj> _lanobj_list;			//languageobj列表
	//LanguageObj _cur_lanobj;					//当前languageobj
	int _cur_lanindex;							//当前languageobj index
	bool _is_textchanged;						//标记lineEdit_Translate字符是否改变
};

#endif // LANGUAGEPACKTOOL_H
