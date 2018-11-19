#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include "languagepacktool.h"
#include "listviewdialog.h"
#include "searchlistdialog.h"
#include "globalconfig.h"

ListViewDialog *listViewDlg;
SearchListDialog *searchDlg;

LanguagePackTool::LanguagePackTool(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	SetWindowBackground();
	//this->setFixedSize(568, 445);
	//this->setWindowFlags(Qt::WindowCloseButtonHint);
	//this->setStatusTip(QString::fromLocal8Bit("文件编码需要为UTF-8！"));
	ui.btn_Folder->setStyleSheet("text-align: left;");
	ui.btn_Type->setStyleSheet("text-align: left;");
	ui.btn_Exclude->setStyleSheet("text-align: left;");
	QFont font("Consolas", 10);
	ui.label_Env->setFont(font);
	//ui.btn_ChineseSelect->setStyleSheet("text-align: left;");
	ui.btn_ExtractLanguagePack->setStatusTip(QString::fromLocal8Bit("根据当前配置，提取语言包！"));
	ui.btn_ApplyLanguagePack->setStatusTip(QString::fromLocal8Bit("将语言包应用到当前设置！"));
	ui.btn_UpdateLanguagePack->setStatusTip(QString::fromLocal8Bit("根据已有语言包更新新语言包！"));
	QObject::connect(ui.btn_OpenConfig, SIGNAL(clicked()), this, SLOT(on_btn_OpenConfig_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.btn_SaveConfig, SIGNAL(clicked()), this, SLOT(on_btn_SaveConfig_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.btn_Folder, SIGNAL(clicked()), this, SLOT(on_btn_Folder_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.btn_Type, SIGNAL(clicked()), this, SLOT(on_btn_Type_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.btn_Exclude, SIGNAL(clicked()), this, SLOT(on_btn_Exclude_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.btn_ExtractLanguagePack, SIGNAL(clicked()), this, SLOT(on_btn_ExtractLanguagePack_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.btn_ApplyLanguagePack, SIGNAL(clicked()), this, SLOT(on_btn_ApplyLanguagePack_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.btn_OpenLanguageXml, SIGNAL(clicked()), this, SLOT(on_btn_OpenLanguageXml_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.btn_SaveLanguageXml, SIGNAL(clicked()), this, SLOT(on_btn_SaveLanguageXml_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.btn_ChineseSelect, SIGNAL(clicked()), this, SLOT(on_btn_ChineseSelect_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.btn_Last, SIGNAL(clicked()), this, SLOT(on_btn_Last_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.btn_Next, SIGNAL(clicked()), this, SLOT(on_btn_Next_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.btn_NextUnfinished, SIGNAL(clicked()), this, SLOT(on_btn_NextUnfinished_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.textEdit_Translate, SIGNAL(textEdited(QString)), this, SLOT(on_lineEdit_Translate_textEdited(QString)), Qt::UniqueConnection);
	QObject::connect(ui.btn_OpenOldLan, SIGNAL(clicked()), this, SLOT(on_btn_OpenOldLan_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.btn_OpenNewLan, SIGNAL(clicked()), this, SLOT(on_btn_OpenNewLan_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.btn_UpdateLanguagePack, SIGNAL(clicked()), this, SLOT(on_btn_UpdateLanguagePack_clicked()), Qt::UniqueConnection);

	_config_path = "";
	_folder_dir.clear();
	_exclude_dir.clear();
	_file_list.clear();
	_type_list.clear();
	_lan_list.clear();
	_lan_list.push_back("English");
	_regx = QString::fromLocal8Bit(".*[一-龥].*");

	_lanxml_path = "";
	_lanobj_list.clear();
	_cur_lanindex = -1;
	_is_textchanged = false;
	updateWindowCtrls();

	listViewDlg = new ListViewDialog(ListModel::Text, this);
	searchDlg = new SearchListDialog(this);

	//linEdit存在setValidator，textEdit不存在setValidator
	//限制lineEdit_Translate输入格式为,不能出现<>&
	//QRegExp rx = QRegExp("([^<>&])*");
	//QRegExpValidator *validator = new QRegExpValidator(rx);
	//ui.textEdit_Translate->setValidator(validator);

	ui.textEdit_Chinese->setAcceptRichText(false);
	ui.textEdit_Translate->setAcceptRichText(false);
	ui.textEdit_Translate->setPlaceholderText(QString::fromLocal8Bit("*.ui文件中<>&三个符号需要转义：<用&lt;转义，>用&gt;转义，&用&amp;转义\n*.h、*.cpp文件中<>&不需要转义，但要注意\"和\\的转义问题：\"用\\\"转义，\\用\\\\转义"));
	ui.textEdit_Translate->setStatusTip(QString::fromLocal8Bit("*.ui文件注意<>&的转义，*.h、*.cpp注意\"和\\的转义\n<（&lt; ） >（&gt; ） &（&amp; ） \"（\\\"） \\（\\\\）"));

}

LanguagePackTool::~LanguagePackTool()
{
	_config_path = "";
	_folder_dir.clear();
	_exclude_dir.clear();
	_file_list.clear();
	_type_list.clear();
	_lan_list.clear();
	_regx = "";

	_lanxml_path = "";
	_lanobj_list.clear();
	_cur_lanindex = -1;
	_is_textchanged = false;
}

CfgProfile LanguagePackTool::getCfgProfile()
{
	CfgProfile config;
	config.folder_dir = _folder_dir;
	config.exclude_dir = _exclude_dir;
	config.file_list = _file_list;
	config.type_list = _type_list;
	config.language_list = _lan_list;
	config.regx = _regx;
	return config;
}

void LanguagePackTool::updateConfigParams(const CfgProfile config)
{
	_folder_dir = config.folder_dir;
	_exclude_dir = config.exclude_dir;
	_file_list = config.file_list;
	_type_list = config.type_list;
	_lan_list = config.language_list;
	_regx = config.regx;
}

void LanguagePackTool::updateWindowCtrls()
{
	//tab 0 控件更新
	ui.lineEdit_ConfigPath->setText(_config_path);
	QString str = "";
	for (int i = 0; i < _folder_dir.size(); ++i)
	{
		str += _folder_dir[i] + ";";
	}
	ui.btn_Folder->setText(str);
	ui.btn_Folder->setStatusTip(str);
	str = "";
	for (int i = 0; i < _type_list.size(); ++i)
	{
		str += _type_list[i] + ";";
	}
	ui.btn_Type->setText(str);
	ui.btn_Type->setStatusTip(str);
	str = "";
	for (int i = 0; i < _exclude_dir.size(); ++i)
	{
		str += _exclude_dir[i] + ";";
	}
	ui.btn_Exclude->setText(str);
	ui.btn_Exclude->setStatusTip(str);
	ui.lineEdit_Regx->setText(_regx);
	//tab 1 控件更新
	LanguageObj lanobj;
	if (_cur_lanindex >= 0 && _cur_lanindex < _lanobj_list.size())
	{
		lanobj = _lanobj_list[_cur_lanindex];
	}
	ui.label_LanguageXml->setText(_lanxml_path);
	ui.textEdit_Chinese->setPlainText(lanobj.chinese);
	ui.textEdit_Chinese->setToolTip(lanobj.chinese);

	str = "";
	for (int i = 0; i < lanobj.env.size(); ++i)
	{
		str += lanobj.env[i] + "\n";
	}
	ui.label_Env->setText(str);
	ui.textEdit_Translate->setPlainText(lanobj.foreign);
	ui.textEdit_Translate->setToolTip(lanobj.foreign);
	str = "";
	str = QString::fromLocal8Bit("记录总数：") + QString::number(_lanobj_list.size())
		+ QString::fromLocal8Bit("；当前记录：") + QString::number(_cur_lanindex + 1);
	ui.label_RecordHit->setText(str);
}

void LanguagePackTool::closeEvent(QCloseEvent *event)
{
	if (!_is_textchanged)
	{
		event->accept();
		return;
	}
	switch (QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("内容已修改，是否更新当前语言包?"), 
		QString::fromLocal8Bit("是"), QString::fromLocal8Bit("否"), QString::fromLocal8Bit("取消"), 0, 2))
	{
	case 0:
		on_btn_SaveLanguageXml_clicked();
		event->accept();
		break;
	case 1:
		event->accept();
		break;
	case 2:
	default:
		event->ignore();
		break;
	}
}

void LanguagePackTool::SetWindowBackground()
{
	QColor qclr = QColor(245, 245, 245);//默认背景
	QString color = GlobalConfig::GetProperty("BACKCOLOR");
	QStringList clist = color.split(QRegExp(","));
	if (color.isEmpty() || clist.size() != 3)
	{
		GlobalConfig::SetProperty("BACKCOLOR", "245,245,245");
	}
	else
	{
		int r = clist[0].toInt() % 256;
		int g = clist[1].toInt() % 256;
		int b = clist[2].toInt() % 256;
		qclr = QColor(r, g, b);
	}
	QPalette palette;
	palette.setBrush(this->backgroundRole(), qclr);
	this->setPalette(palette);

	//QFile file("LanguagePackTool.cfg");
	//if (!file.exists())
	//{
	//	QPalette palette;
	//	palette.setBrush(this->backgroundRole(), QColor(245, 245, 245));
	//	this->setPalette(palette);
	//	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	//	{
	//		return;
	//	}
	//	QTextStream out(&file);
	//	out << QString("%1 %2 %3").arg(245).arg(245).arg(245) << endl;
	//	out.flush();
	//}
	//else
	//{
	//	if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
	//	{
	//		return;
	//	}
	//	QString str = file.readLine();
	//	QStringList slist = str.split(QRegExp("\\s+"));
	//	if (slist.size() < 3)
	//	{
	//		QPalette palette;
	//		palette.setBrush(this->backgroundRole(), QColor(245, 245, 245));
	//		this->setPalette(palette);
	//		file.reset();
	//		QTextStream out(&file);
	//		out << QString("%1 %2 %3").arg(245).arg(245).arg(245) << endl;
	//		out.flush();
	//	}
	//	else
	//	{
	//		int r = slist[0].toInt() % 256;
	//		int g = slist[1].toInt() % 256;
	//		int b = slist[2].toInt() % 256;
	//		QPalette palette;
	//		palette.setBrush(this->backgroundRole(), QColor(r, g, b));
	//		this->setPalette(palette);
	//	}
	//}
	//file.close();
}

void LanguagePackTool::on_btn_OpenConfig_clicked()
{
	QString dir = GlobalConfig::GetProperty("CONFIGPATH");
	QString filename = QFileDialog::getOpenFileName(this, "Open", dir, "Xml files(*.xml)");
	if (!filename.isEmpty())
	{
		//GlobalConfig::SetProperty("CONFIGPATH", QFileInfo(filename).absolutePath() + "/");
		GlobalConfig::SetProperty("CONFIGPATH", filename);
		CfgProfile config;
		bool flag = _languagepackobj.readConfigFile(filename, config);
		if (flag)
		{
			_config_path = filename;
			updateConfigParams(config);
			updateWindowCtrls();
		}
		else
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
				QString::fromLocal8Bit("打开配置文件失败！"), QMessageBox::Ok, QMessageBox::Ok);
		}
	}
}

void LanguagePackTool::on_btn_SaveConfig_clicked()
{
	QString dir = GlobalConfig::GetProperty("CONFIGPATH");
	QString filename = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存配置文件"),
		dir, "Xml files(*.xml)");
	if (!filename.isEmpty())
	{
		GlobalConfig::SetProperty("CONFIGPATH", QFileInfo(filename).absolutePath() + "/");
		//GlobalConfig::SetProperty("CONFIGPATH", filename);
		bool flag = _languagepackobj.writeConfigFile(filename, getCfgProfile());
	}
}

void LanguagePackTool::on_btn_Folder_clicked()
{
	//ListViewDialog dlg(ListModel::Folder, this);
	//dlg.setStringList(_folder_dir);
	//dlg.exec();
	//if (dlg.getConfirmFlag())
	//{
	//	dlg.getStringList(_folder_dir);
	//}
	//updateWindowCtrls();

	listViewDlg->setListModel(ListModel::Folder);
	listViewDlg->setStringList(_folder_dir);
	listViewDlg->exec();
	if (listViewDlg->getConfirmFlag())
	{
		listViewDlg->getStringList(_folder_dir);
	}
	updateWindowCtrls();
}

void LanguagePackTool::on_btn_Type_clicked()
{
	//ListViewDialog dlg(ListModel::Text, this);
	//dlg.setStringList(_type_list);
	//dlg.exec();
	//if (dlg.getConfirmFlag())
	//{
	//	dlg.getStringList(_type_list);
	//}
	//updateWindowCtrls();

	listViewDlg->setListModel(ListModel::Text);
	listViewDlg->setStringList(_type_list);
	listViewDlg->exec();
	if (listViewDlg->getConfirmFlag())
	{
		listViewDlg->getStringList(_type_list);
	}
	updateWindowCtrls();
}

void LanguagePackTool::on_btn_Exclude_clicked()
{
	//ListViewDialog dlg(ListModel::Folder, this);
	//dlg.setStringList(_exclude_dir);
	//dlg.exec();
	//if (dlg.getConfirmFlag())
	//{
	//	dlg.getStringList(_exclude_dir);
	//}
	//updateWindowCtrls();

	listViewDlg->setListModel(ListModel::Folder);
	listViewDlg->setStringList(_exclude_dir);
	listViewDlg->exec();
	if (listViewDlg->getConfirmFlag())
	{
		listViewDlg->getStringList(_exclude_dir);
	}
	updateWindowCtrls();
}

void LanguagePackTool::on_btn_ExtractLanguagePack_clicked()
{
	CfgProfile config = getCfgProfile();
	//处理文件、处理类型、过滤关键字不能为空
	if (config.folder_dir.size() <=0 || config.type_list.size() <= 0 || config.regx.isEmpty())
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
			QString::fromLocal8Bit("缺少参数！"), QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	QString dir = GlobalConfig::GetProperty("EXTRACTLANPATH");
	QString filename = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存语言包"),
		dir, "Xml files(*.xml)");
	if (!filename.isEmpty())
	{
		GlobalConfig::SetProperty("EXTRACTLANPATH", QFileInfo(filename).absolutePath() + "/");
		//GlobalConfig::SetProperty("EXTRACTLANPATH", filename);
		QString err = _languagepackobj.extractLanguagePack(config, filename);
		if (err == "success")
		{
			QMessageBox::information(this, QString::fromLocal8Bit("Information!"),
				QString::fromLocal8Bit("提取完成！"), QMessageBox::Ok, QMessageBox::Ok);
		}
		else
		{
			QMessageBox::information(this, QString::fromLocal8Bit("Information!"),
				QString::fromLocal8Bit("提取出错-") + err, QMessageBox::Ok, QMessageBox::Ok);
		}
	}
}

void LanguagePackTool::on_btn_ApplyLanguagePack_clicked()
{
	CfgProfile config = getCfgProfile();
	//处理文件、处理类型、过滤关键字不能为空
	if (config.folder_dir.size() <= 0 || config.type_list.size() <= 0 || config.regx.isEmpty())
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
			QString::fromLocal8Bit("缺少参数！"), QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	QString dir = GlobalConfig::GetProperty("APPLYLANPATH");
	QString filename = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择语言包"),
		dir, "Xml files(*.xml)");
	if (!filename.isEmpty())
	{
		//GlobalConfig::SetProperty("APPLYLANPATH", QFileInfo(filename).absolutePath() + "/");
		GlobalConfig::SetProperty("APPLYLANPATH", filename);
		QString err = _languagepackobj.applyLanguagePack(config, filename, "English");
		if (err == "success")
		{
			QMessageBox::information(this, QString::fromLocal8Bit("Information!"),
				QString::fromLocal8Bit("应用完成！"), QMessageBox::Ok, QMessageBox::Ok);
		}
		else
		{
			QMessageBox::information(this, QString::fromLocal8Bit("Information!"),
				QString::fromLocal8Bit("应用出错-") + err, QMessageBox::Ok, QMessageBox::Ok);
		}
	}
}

void LanguagePackTool::on_btn_OpenLanguageXml_clicked()
{
	//if (!_lanxml_path.isEmpty())
	//{
	//	if (QMessageBox::question(this, QString::fromLocal8Bit("Qestion!"),
	//		QString::fromLocal8Bit("是否更新当前语言包？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
	//	{
	//		on_btn_SaveLanguageXml_clicked();
	//	}
	//}

	QString dir = GlobalConfig::GetProperty("OPENLANPATH");
	QString filename = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("打开语言包"),
		dir, "Xml files(*.xml)");
	if (!filename.isEmpty())
	{
		//GlobalConfig::SetProperty("OPENLANPATH", QFileInfo(filename).absolutePath() + "/");
		GlobalConfig::SetProperty("OPENLANPATH", filename);
		_lanobj_list.clear();
		bool flag = _languagepackobj.readLanguageFile(filename, "English", _lanobj_list);
		if (flag )
		{
			if (_lanobj_list.size() > 0)
			{
				_lanxml_path = filename;
				_cur_lanindex = 0;
				updateWindowCtrls();
			}
		}
		else
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
				QString::fromLocal8Bit("读取语言包文件失败！"), QMessageBox::Ok, QMessageBox::Ok);
		}
	}
}

void LanguagePackTool::on_btn_SaveLanguageXml_clicked()
{
	if (_cur_lanindex >= 0 && _cur_lanindex < _lanobj_list.size())
	{
		_lanobj_list[_cur_lanindex].foreign = ui.textEdit_Translate->toPlainText();
		updateWindowCtrls();
	}

	bool flag = _languagepackobj.updateLanguageFile(_lanxml_path, "English", _lanobj_list);
	if (flag)
	{
		_is_textchanged = false;
		QMessageBox::information(this, QString::fromLocal8Bit("Information!"),
			QString::fromLocal8Bit("更新完成！"), QMessageBox::Ok, QMessageBox::Ok);
	}
	else
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
			QString::fromLocal8Bit("更新失败！"), QMessageBox::Ok, QMessageBox::Ok);
	}
}

void LanguagePackTool::on_btn_ChineseSelect_clicked()
{
	if (_cur_lanindex >= 0 && _cur_lanindex < _lanobj_list.size())
	{
		_lanobj_list[_cur_lanindex].foreign = ui.textEdit_Translate->toPlainText();
		updateWindowCtrls();
	}

	//SearchListDialog dlg(this);
	//dlg.setCurrentIndex(_cur_lanindex);//在setStringList前调用
	//dlg.setStringList(_lanobj_list);
	//dlg.exec();
	//int index = dlg.getCurrentIndex();
	//if (index >= 0 && index < _lanobj_list.size())
	//{
	//	_cur_lanindex = index;
	//}
	//updateWindowCtrls();

	searchDlg->setCurrentIndex(_cur_lanindex);//在setStringList前调用
	searchDlg->setStringList(_lanobj_list);
	searchDlg->exec();
	int index = searchDlg->getCurrentIndex();
	if (index >= 0 && index < _lanobj_list.size())
	{
		_cur_lanindex = index;
	}
	updateWindowCtrls();

	ui.textEdit_Translate->setFocus();
}

void LanguagePackTool::on_btn_Last_clicked()
{
	if (_cur_lanindex >= 0 && _cur_lanindex < _lanobj_list.size())
	{
		_lanobj_list[_cur_lanindex].foreign = ui.textEdit_Translate->toPlainText();
		if (_cur_lanindex == 0)
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
				QString::fromLocal8Bit("已为第一项！"), QMessageBox::Ok, QMessageBox::Ok);
		}
		else
		{
			_cur_lanindex = _cur_lanindex - 1;
		}
	}
	updateWindowCtrls();
}

void LanguagePackTool::on_btn_Next_clicked()
{
	if (_cur_lanindex >= 0 && _cur_lanindex < _lanobj_list.size())
	{
		_lanobj_list[_cur_lanindex].foreign = ui.textEdit_Translate->toPlainText();
		if (_cur_lanindex == _lanobj_list.size() - 1)
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
				QString::fromLocal8Bit("已为最后一项！"), QMessageBox::Ok, QMessageBox::Ok);
		}
		else
		{
			_cur_lanindex = _cur_lanindex + 1;
		}
	}
	updateWindowCtrls();
}

void LanguagePackTool::on_btn_NextUnfinished_clicked()
{
	if (_cur_lanindex >= 0 && _cur_lanindex < _lanobj_list.size())
	{
		_lanobj_list[_cur_lanindex].foreign = ui.textEdit_Translate->toPlainText();
		if (_cur_lanindex == _lanobj_list.size() - 1)
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
				QString::fromLocal8Bit("已为最后一项！"), QMessageBox::Ok, QMessageBox::Ok);
			updateWindowCtrls();
			return;
		}
		else
		{
			for (int i = _cur_lanindex + 1; i < _lanobj_list.size(); i++)
			{
				if (_lanobj_list[i].foreign.isEmpty())
				{
					_cur_lanindex = i;
					updateWindowCtrls();
					return;
				}
			}
		}
		QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
			QString::fromLocal8Bit("后续项均已填写！"), QMessageBox::Ok, QMessageBox::Ok);
	}
}

void LanguagePackTool::on_lineEdit_Translate_textEdited(QString)
{
	if (_cur_lanindex >= 0)
	{
		_is_textchanged = true;
	}
}

void LanguagePackTool::on_btn_OpenOldLan_clicked()
{
	QString dir = GlobalConfig::GetProperty("OLDLANPATH");
	QString oldfile = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择已有语言包"),
		dir, "Xml files(*.xml)");
	if (oldfile.isEmpty())
	{
		return;
	}
	if (!_languagepackobj.isLanguageFile(oldfile))
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
			QString::fromLocal8Bit("此文件非语言包文件！"), QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	//GlobalConfig::SetProperty("OLDLANPATH", QFileInfo(oldfile).absolutePath() + "/");
	GlobalConfig::SetProperty("OLDLANPATH", oldfile);
	ui.lineEdit_OldLanguagePack->setText(oldfile);
}

void LanguagePackTool::on_btn_OpenNewLan_clicked()
{
	QString dir = GlobalConfig::GetProperty("NEWLANPATH");
	QString newfile = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择待更新的语言包"),
		dir, "Xml files(*.xml)");
	if (newfile.isEmpty())
	{
		return;
	}
	if (!_languagepackobj.isLanguageFile(newfile))
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
			QString::fromLocal8Bit("此文件非语言包文件！"), QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	//GlobalConfig::SetProperty("NEWLANPATH", QFileInfo(newfile).absolutePath() + "/");
	GlobalConfig::SetProperty("NEWLANPATH", newfile);
	ui.lineEdit_NewLanguagePack->setText(newfile);
}

void LanguagePackTool::on_btn_UpdateLanguagePack_clicked()
{
	QString oldfile = ui.lineEdit_OldLanguagePack->text();
	QString newfile = ui.lineEdit_NewLanguagePack->text();
	if (oldfile.isEmpty() || newfile.isEmpty())
	{
		QMessageBox::information(this, QString::fromLocal8Bit("Information!"),
			QString::fromLocal8Bit("缺少参数！"), QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	_languagepackobj.updateLanguagePack(oldfile, newfile);
	QMessageBox::information(this, QString::fromLocal8Bit("Information!"),
		QString::fromLocal8Bit("更新完成！"), QMessageBox::Ok, QMessageBox::Ok);
}
