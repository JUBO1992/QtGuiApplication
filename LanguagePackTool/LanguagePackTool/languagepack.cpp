#include <QDir>
#include <QProgressDialog>
#include <QTextCodec>
#include <QCoreApplication>
#include "languagepack.h"
#include "fileread.h"

LanguagePack::LanguagePack(QObject *parent)
	: QObject(parent)
{

}

LanguagePack::~LanguagePack()
{

}

void LanguagePack::updateLanguagePack(QString oldfile, QString newfile)
{
	updateFile(oldfile, newfile);
}

QString LanguagePack::extractLanguagePack(const CfgProfile &config, QString xmlpath)
{
	QStringList folderDir = config.folder_dir;
	QStringList excludeDir = config.exclude_dir;
	QStringList fileList = config.file_list;
	QList<QString>  typeList = config.type_list;
	QStringList languagelist = config.language_list;
	QString regx = config.regx;

	//获取所有文件夹中指定后缀文件
	for (int i = 0; i < folderDir.size(); ++i)
	{
		cdDir(folderDir[i], fileList, typeList, excludeDir);
		godir(folderDir[i], fileList, typeList, excludeDir);
	}
	if (fileList.size() <= 0)
	{
		return QString::fromLocal8Bit("没有搜索到满足条件的文件！");
	}

	std::map<QString, QStringList> words = extractChinese(fileList, regx);
	if (words.size() <= 0)
	{
		return QString::fromLocal8Bit("提取结果为空！");
	}

	QFile wfile(xmlpath);
	if (!wfile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return QString::fromLocal8Bit("文件写入失败！");
	}
	QTextStream out(&wfile);
	out.setCodec(QTextCodec::codecForName("utf-8")); //设置输出文件编码格式
	out.setGenerateByteOrderMark(true); //with BOM
	QDomDocument document;
	QString strHeader("version=\"1.0\"");
	document.appendChild(document.createProcessingInstruction("xml", strHeader));
	QDomElement root_elem = document.createElement("LanguagePack");
	document.appendChild(root_elem);
	QDomText text = document.createTextNode("");
	root_elem.appendChild(text);
	int num = words.size(); //得到tag总数
	int index = 0;
	for (std::map<QString, QStringList>::iterator it = words.begin(); it != words.end(); ++it)
	{
		QDomElement ID = document.createElement("ID");
		QDomText text2 = document.createTextNode(it->first);
		ID.appendChild(text2);
		root_elem.appendChild(ID);

		for (int i = 0; i < it->second.size(); ++i)
		{
			QDomElement env = document.createElement("Env");
			QDomText textenv = document.createTextNode(it->second[i]);
			env.appendChild(textenv);
			ID.appendChild(env);
		}

		for (int i = 0; i < languagelist.size(); ++i)
		{
			QDomElement prc = document.createElement(languagelist.at(i));
			QDomText text3 = document.createTextNode("");
			prc.appendChild(text3);
			ID.appendChild(prc);
		}
		++index;
	}
	document.save(out, 4);
	wfile.close();
	return "success";
}

QString LanguagePack::applyLanguagePack(const CfgProfile &config, QString xmlpath, QString language)
{
	QStringList folderDir = config.folder_dir;
	QStringList excludeDir = config.exclude_dir;
	QStringList fileList = config.file_list;
	QList<QString>  typeList = config.type_list;
	QStringList languagelist = config.language_list;
	QString regx = config.regx;

	std::map<QString, QString> languageMap;
	QList<LanguageObj> lanobj_list;
	if (!readLanguageFile(xmlpath, language, lanobj_list))
	{
		return QString::fromLocal8Bit("语言包读取失败！");
	}
	for (int i = 0; i < lanobj_list.size(); ++i)
	{
		languageMap[lanobj_list[i].chinese] = lanobj_list[i].foreign;
	}

	//获取所有文件夹中指定后缀文件
	for (int i = 0; i < folderDir.size(); ++i)
	{
		cdDir(folderDir[i], fileList, typeList, excludeDir);
		godir(folderDir[i], fileList, typeList, excludeDir);
	}
	if (fileList.size() <= 0)
	{
		return QString::fromLocal8Bit("没有搜索到满足条件的文件！");
	}

	int filesum = fileList.size();
	QProgressDialog process;
	process.setLabelText(tr("processing..."));
	process.setRange(0, filesum);
	process.setModal(true);
	process.setMinimumDuration(0);
	process.setCancelButtonText(tr("cancel"));
	for (int i = 0; i < filesum; ++i)
	{
		process.setValue(i);
		QCoreApplication::processEvents();
		if (process.wasCanceled())
		{
			return QString::fromLocal8Bit("中断更新操作！");
		}

		FileRead *fileRead = new FileRead(fileList[i], FileRead::ReadAndWrite);
		fileRead->updateStr2(languageMap, regx);
		delete fileRead;
	}
	process.setValue(filesum);
	QCoreApplication::processEvents();
	return QString::fromLocal8Bit("success");
}

bool LanguagePack::readConfigFile(QString cfgfile, CfgProfile &config)
{
	QDomDocument doc;
	QFile profile(cfgfile); //cfgfile为xml文件路径  
	if (!profile.open(QIODevice::ReadOnly))
	{
		return false;
	}
	if (!doc.setContent(&profile))
	{
		profile.close();
		return false;
	}

	QDomElement root = doc.documentElement();
	if ("Profile" != root.tagName())//不是配置文件格式
	{
		return false;
	}

	int folderNum = 0;
	int excludeNum = 0;
	int fileNum = 0;
	int typeNum = 0;
	int languageNum = 0;
	QStringList folderDir;
	QStringList excludeDir;
	QStringList fileList;
	QList<QString>  typeList;
	QStringList languagelist;
	QString regx;

	QDomNode node = root.firstChildElement("folder");
	folderNum = node.firstChildElement("Number").text().toInt();
	QDomElement foldernode = node.firstChildElement("Path");
	if (folderNum != 0)
	{
		folderDir << foldernode.text();
	}
	for (int i = 1; i < folderNum; ++i)
	{
		folderDir << foldernode.nextSiblingElement("Path").text();
		foldernode = foldernode.nextSiblingElement("Path");
	}
	node = root.firstChildElement("Exclude");
	excludeNum = node.firstChildElement("Number").text().toInt();
	QDomElement excludenode = node.firstChildElement("Path");
	if (excludeNum != 0)
	{
		excludeDir << excludenode.text();
	}
	for (int i = 1; i < excludeNum; ++i)
	{
		excludeDir << excludenode.nextSiblingElement("Path").text();
		excludenode = excludenode.nextSiblingElement("Path");
	}
	node = root.firstChildElement("File");
	fileNum = node.firstChildElement("Number").text().toInt();
	QDomElement filenode = node.firstChildElement("Path");
	if (fileNum != 0)
	{
		fileList << filenode.text();
	}
	for (int i = 1; i < fileNum; ++i)
	{
		fileList << filenode.nextSiblingElement("Path").text();
		filenode = filenode.nextSiblingElement("Path");
	}
	node = root.firstChildElement("Type");
	typeNum = node.firstChildElement("Number").text().toInt();
	QDomElement typenode = node.firstChildElement("postfix");
	if (typeNum != 0)
	{
		typeList << typenode.text();
	}
	for (int i = 1; i < typeNum; ++i)
	{
		typeList << typenode.nextSiblingElement("postfix").text();
		typenode = typenode.nextSiblingElement("postfix");
	}
	node = root.firstChildElement("Translate");
	languageNum = node.firstChildElement("Number").text().toInt();
	QDomElement languagenode = node.firstChildElement("Language");
	if (languageNum != 0)
	{
		languagelist << languagenode.text();
	}
	for (int i = 1; i < languageNum; ++i)
	{
		languagelist << languagenode.nextSiblingElement("Language").text();
		languagenode = languagenode.nextSiblingElement("Language");
	}
	regx = root.firstChildElement("Fliter").firstChildElement("Reg").text();
	//regx.replace("\\u4e00", "\u4e00");
	//regx.replace("\\u9fa5", "\u9fa5");
	profile.close();

	config.folder_dir = folderDir;
	config.exclude_dir = excludeDir;
	config.file_list = fileList;
	config.type_list = typeList;
	config.language_list = languagelist;
	config.regx = regx;
	return true;
}

bool LanguagePack::writeConfigFile(QString cfgfile, const CfgProfile &config)
{
	QFile wfile(cfgfile);
	if (!wfile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return false;
	}
	QTextStream out(&wfile);
	out.setCodec(QTextCodec::codecForName("utf-8")); //设置输出文件编码格式
	out.setGenerateByteOrderMark(true); //with BOM
	QDomDocument document;
	QString strHeader("version=\"1.0\"");
	document.appendChild(document.createProcessingInstruction("xml", strHeader));
	// Profile标签
	QDomElement root_elem = document.createElement("Profile");
	document.appendChild(root_elem);
	// folder标签
	QDomElement folder_elem = document.createElement("folder");
	root_elem.appendChild(folder_elem);
	QDomElement number_elem = document.createElement("Number");
	QDomText text = document.createTextNode(QString::number(config.folder_dir.size()));
	number_elem.appendChild(text);
	folder_elem.appendChild(number_elem);
	for (int i = 0; i < config.folder_dir.size(); i++)
	{
		QDomElement path_elem = document.createElement("Path");
		QDomText text2 = document.createTextNode(config.folder_dir[i]);
		path_elem.appendChild(text2);
		folder_elem.appendChild(path_elem);
	}
	// Exclude标签
	QDomElement exclude_elem = document.createElement("Exclude");
	root_elem.appendChild(exclude_elem);
	number_elem = document.createElement("Number");
	text = document.createTextNode(QString::number(config.exclude_dir.size()));
	number_elem.appendChild(text);
	exclude_elem.appendChild(number_elem);
	for (int i = 0; i < config.exclude_dir.size(); i++)
	{
		QDomElement path_elem = document.createElement("Path");
		QDomText text2 = document.createTextNode(config.exclude_dir[i]);
		path_elem.appendChild(text2);
		exclude_elem.appendChild(path_elem);
	}
	// File标签
	QDomElement file_elem = document.createElement("File");
	root_elem.appendChild(file_elem);
	number_elem = document.createElement("Number");
	text = document.createTextNode(QString::number(config.file_list.size()));
	number_elem.appendChild(text);
	file_elem.appendChild(number_elem);
	for (int i = 0; i < config.file_list.size(); i++)
	{
		QDomElement path_elem = document.createElement("Path");
		QDomText text2 = document.createTextNode(config.file_list[i]);
		path_elem.appendChild(text2);
		file_elem.appendChild(path_elem);
	}
	// Type标签
	QDomElement type_elem = document.createElement("Type");
	root_elem.appendChild(type_elem);
	number_elem = document.createElement("Number");
	text = document.createTextNode(QString::number(config.type_list.size()));
	number_elem.appendChild(text);
	type_elem.appendChild(number_elem);
	for (int i = 0; i < config.type_list.size(); i++)
	{
		QDomElement postfix_elem = document.createElement("postfix");
		QDomText text2 = document.createTextNode(config.type_list[i]);
		postfix_elem.appendChild(text2);
		type_elem.appendChild(postfix_elem);
	}
	// Translate标签
	QDomElement translate_elem = document.createElement("Translate");
	root_elem.appendChild(translate_elem);
	number_elem = document.createElement("Number");
	text = document.createTextNode(QString::number(config.language_list.size()));
	number_elem.appendChild(text);
	translate_elem.appendChild(number_elem);
	for (int i = 0; i < config.language_list.size(); i++)
	{
		QDomElement language_elem = document.createElement("Language");
		QDomText text2 = document.createTextNode(config.language_list[i]);
		language_elem.appendChild(text2);
		translate_elem.appendChild(language_elem);
	}
	// Filter标签
	QDomElement filter_elem = document.createElement("Fliter");
	root_elem.appendChild(filter_elem);
	QDomElement reg_elem = document.createElement("Reg");
	text = document.createTextNode(config.regx);
	reg_elem.appendChild(text);
	filter_elem.appendChild(reg_elem);

	document.save(out, 4);
	wfile.close();
	return true;
}

bool LanguagePack::findElementByIDtag(QString xmlpath, QString idtext, QDomElement &ID)
{
	QDomDocument doc;
	QFile profile(xmlpath);  
	if (!profile.open(QIODevice::ReadOnly))
	{
		return false;
	}
	if (!doc.setContent(&profile))
	{
		profile.close();
		return false;
	}

	QDomElement root = doc.documentElement();
	root.firstChildElement("LanguagePack");
	QDomNode n = root.firstChild();
	while (!n.isNull())
	{
		QDomElement e = n.toElement();
		if (!e.isNull())
		{
			QString tag_name = e.tagName();
			if (0 == tag_name.compare("ID"))
			{
				QDomNode child = n.firstChild();
				while (!child.isNull())
				{
					if (child.isText())
					{
						QString text = child.toText().data();
						if (0 == text.compare(idtext))
						{
							ID = e;
							return true;
						}
					}
				}
			}
		}
		n = n.nextSibling();
	}
	profile.close();
	return false;
}

bool LanguagePack::readLanguageFile(QString xmlpath, QString languagetag, QList<LanguageObj> &languagelist)
{
	QList<LanguageObj> llist;
	QDomDocument doc;
	QFile profile(xmlpath); //xmlpath为语言包-xml文件路径  
	if (!profile.open(QIODevice::ReadOnly))
	{
		return false;
	}
	if (!doc.setContent(&profile))
	{
		profile.close();
		return false;
	}

	QDomElement root = doc.documentElement();
	if ("LanguagePack" != root.tagName())//不是语言包文件格式
	{
		return false;
	}

	QDomNode n = root.firstChildElement("ID");
	while (!n.isNull())
	{
		QDomElement e = n.toElement(); // try to convert the node to an element.
		if (!e.isNull())
		{
			QString tag_name = e.tagName();
			if (0 == tag_name.compare("ID"))
			{
				QDomNode id_child = n.firstChild();
				QString chinese, foreign;
				QStringList env;

				while (!id_child.isNull())
				{
					if (id_child.isElement())
					{
						QDomElement id_element = id_child.toElement();
						if ("Env" == id_element.tagName())
						{
							env.push_back(id_element.text().trimmed());
						}
						else if (languagetag == id_element.tagName())
						{
							foreign = id_element.text();
						}
					}
					else if (id_child.isText())
					{
						chinese = id_child.toText().data();
					}

					id_child = id_child.nextSibling();
				}

				if (!chinese.isEmpty())
				{
					LanguageObj obj;
					obj.chinese = chinese;
					obj.env = env;
					obj.foreign = foreign;
					llist.push_back(obj);
				}
			}
		}
		n = n.nextSibling();
	}
	profile.close();
	languagelist = llist;
	return true;
}

bool LanguagePack::updateLanguageFile(QString xmlpath, QString languagetag, const QList<LanguageObj> &languagelist)
{
	QDomDocument doc;
	QFile file(xmlpath);
	
	if (!file.open(QFile::ReadOnly))
	{
		return false;
	}
	if (!doc.setContent(&file))
	{
		file.close();
		return false;
	}

	QDomElement root = doc.documentElement();
	if ("LanguagePack" != root.tagName())//不是语言包文件格式
	{
		return false;
	}

	QDomNode n = root.firstChildElement("ID");
	while (!n.isNull())
	{
		QDomElement e = n.toElement(); // try to convert the node to an element.
		if (!e.isNull())
		{
			QString tag_name = e.tagName();
			if (0 == tag_name.compare("ID"))
			{
				QDomNode id_child = n.firstChild();
				QString chinese, foreign;
				QStringList env;
				QDomElement foreign_ele;

				while (!id_child.isNull())
				{
					if (id_child.isText())
					{
						chinese = id_child.toText().data();
					}					
					else if (id_child.isElement())
					{
						QDomElement id_element = id_child.toElement();
						if (languagetag == id_element.tagName())
						{
							foreign_ele = id_element;
						}
					}
					int index = findIndexByChinese(languagelist, chinese);
					foreign = languagelist[index].foreign;
					if (index != -1)
					{
						QDomNode oldnode = foreign_ele.firstChild();
						if (!oldnode.isNull())//节点有值的情况
						{
							foreign_ele.firstChild().setNodeValue(foreign);
							QDomNode newnode = foreign_ele.firstChild();
							foreign_ele.replaceChild(newnode, oldnode);
						}
						else//节点无值的情况
						{
							QDomText foreign_text = doc.createTextNode(foreign);
							foreign_ele.appendChild(foreign_text);
						}
					}						
					id_child = id_child.nextSibling();
				}
			}
		}
		n = n.nextSibling();
	}

	QFile modfile(xmlpath);
	modfile.open(QFile::WriteOnly | QFile::Truncate);
	QTextStream out(&modfile);	
	out.setCodec(QTextCodec::codecForName("utf-8")); //设置输出文件编码格式
	out.setGenerateByteOrderMark(true); //with BOM
	out.reset();
	doc.save(out, 4);
	file.close();
	modfile.close();
	return true;
}

bool LanguagePack::isConfigFile(QString cfgfile)
{
	QDomDocument doc;
	QFile profile(cfgfile); //cfgfile为xml文件路径  
	if (!profile.open(QIODevice::ReadOnly))
	{
		return false;
	}
	if (!doc.setContent(&profile))
	{
		profile.close();
		return false;
	}

	QDomElement root = doc.documentElement();
	profile.close();
	if ("Profile" != root.tagName())//不是配置文件格式
	{
		return false;
	}
	return true;
}

bool LanguagePack::isLanguageFile(QString xmlpath)
{
	QDomDocument doc;
	QFile profile(xmlpath); //xmlpath为语言包-xml文件路径  
	if (!profile.open(QIODevice::ReadOnly))
	{
		return false;
	}
	if (!doc.setContent(&profile))
	{
		profile.close();
		return false;
	}

	QDomElement root = doc.documentElement();
	profile.close();
	if ("LanguagePack" != root.tagName())//不是语言包文件格式
	{
		return false;
	}
	return true;
}

std::map<QString, QStringList> LanguagePack::extractChinese(QStringList filelist, QString regx)
{
	int filesum = filelist.size();
	QProgressDialog process;
	process.setLabelText(tr("processing..."));
	process.setRange(0, filesum);
	process.setModal(true);
	process.setMinimumDuration(0);
	process.setCancelButtonText(tr("cancel"));
	std::map<QString, QStringList> words;
	for (int i = 0; i < filesum; ++i)
	{
		process.setValue(i);
		QCoreApplication::processEvents();
		if (process.wasCanceled())
		{
			words.clear();
			break;
		}

		FileRead *fileRead = new FileRead(filelist[i]);
		fileRead->extractStr(words, regx);
		delete fileRead;
	}
	process.setValue(filesum);
	QCoreApplication::processEvents();
	return words;
}

int LanguagePack::findIndexByChinese(const QList<LanguageObj> &languagelist, QString chinese)
{
	int index = -1;
	for (int i = 0; i < languagelist.size(); ++i)
	{
		if (chinese == languagelist[i].chinese)
		{
			index = i;
			break;
		}
	}
	return index;
}

void LanguagePack::getOldMap(const QDomDocument &old_doc, QMap<QString, QString> &old_map)
{
	QDomNode n = old_doc.firstChild();
	while (!n.isNull())
	{
		QDomElement e = n.toElement(); // try to convert the node to an element.
		if (!e.isNull())
		{
			QString tag_name = e.tagName();
			if (0 == tag_name.compare("LanguagePack"))
			{
				n = n.firstChild();;
				break;
			}
		}
		n = n.nextSibling();
	}

	while (!n.isNull())
	{
		QDomElement e = n.toElement(); // try to convert the node to an element.
		if (!e.isNull())
		{
			QString tag_name = e.tagName();
			if (0 == tag_name.compare("ID"))
			{
				QDomNode id_child = n.firstChild();
				QString chinese, english;
				QStringList env;

				while (!id_child.isNull())
				{
					if (id_child.isElement())
					{
						QDomElement id_element = id_child.toElement();
						if ("Env" == id_element.tagName())
						{
							env.push_back(id_element.text());
						}
						else if ("English" == id_element.tagName())
						{
							english = id_element.text();
						}
					}
					else if (id_child.isText())
					{
						chinese = id_child.toText().data();
					}

					id_child = id_child.nextSibling();
				}

				if (!chinese.isEmpty() && !english.isEmpty())
				{
					old_map.insert(chinese, english);
				}
			}

		}
		n = n.nextSibling();
	}
}

void LanguagePack::replaceNew(QDomDocument &new_doc, QMap<QString, QString> &old_map)
{
	QDomNode n = new_doc.firstChild();
	while (!n.isNull())
	{
		QDomElement e = n.toElement(); // try to convert the node to an element.
		if (!e.isNull())
		{
			QString tag_name = e.tagName();
			if (0 == tag_name.compare("LanguagePack"))
			{
				n = n.firstChild();;
				break;
			}
		}
		n = n.nextSibling();
	}

	while (!n.isNull())
	{
		QDomElement e = n.toElement(); // try to convert the node to an element.
		if (!e.isNull())
		{
			QString tag_name = e.tagName();
			if (0 == tag_name.compare("ID"))
			{
				QDomNode id_child = n.firstChild();

				QString chinese, english;
				QStringList env;
				QDomElement english_ele;

				while (!id_child.isNull())
				{


					if (id_child.isText())
					{
						chinese = id_child.toText().data();
					}
					else if (id_child.isElement())
					{
						QDomElement id_element = id_child.toElement();

						if ("English" == id_element.tagName())
						{
							english_ele = id_element;
						}
					}

					if (old_map.contains(chinese))
					{
						QDomNode oldnode = english_ele.firstChild();
						if (!oldnode.isNull())//节点有值的情况
						{
							english_ele.firstChild().setNodeValue(old_map[chinese]);
							QDomNode newnode = english_ele.firstChild();
							english_ele.replaceChild(newnode, oldnode);
						}
						else//节点无值的情况
						{
							QDomText english_text = new_doc.createTextNode(old_map[chinese]);
							english_ele.appendChild(english_text);
						}
					}

					id_child = id_child.nextSibling();
				}
			}

		}
		n = n.nextSibling();
	}
	//new_doc.save()
}

void LanguagePack::updateFile(QString old_file_path, QString new_file_path)
{
	QDomDocument old_doc;
	QDomDocument new_doc;
	QFile old_file(old_file_path); //filepath为xml文件路径  
	QFile new_file(new_file_path);

	if (!old_file.open(QIODevice::ReadOnly))
	{
		qDebug(QString::fromLocal8Bit("错误:配置文件目录有误！").toStdString().c_str());
		return;
	}

	if (!new_file.open(QIODevice::ReadOnly))
	{
		qDebug(QString::fromLocal8Bit("错误:配置文件目录有误！").toStdString().c_str());
		return;
	}

	if (!old_doc.setContent(&old_file))
	{
		qDebug(QString::fromLocal8Bit("错误:配置文件目录有误！").toStdString().c_str());
		old_file.close();
		return;
	}

	if (!new_doc.setContent(&new_file))
	{
		qDebug(QString::fromLocal8Bit("错误:配置文件目录有误！").toStdString().c_str());
		new_file.close();
		return;
	}

	QMap<QString, QString> old_map;
	getOldMap(old_doc, old_map);
	replaceNew(new_doc, old_map);

	QFile modify_file(new_file_path);
	modify_file.open(QFile::WriteOnly | QFile::Truncate);
	QTextStream out(&modify_file);
	out.setCodec(QTextCodec::codecForName("utf-8")); //设置输出文件编码格式
	out.setGenerateByteOrderMark(true); //with BOM	out.reset();
	new_doc.save(out, 4);
	old_file.close();
	new_file.close();
	modify_file.close();
}

void LanguagePack::godir(QString folderDir, QStringList &filenameall, QList<QString> &typeList, QStringList excludeDir)
{
	QDir dir(folderDir);
	if (!dir.exists())
	{
		return;
	}
	QStringList filter;
	for (int i = 0; i < typeList.size(); ++i)
	{
		filter << typeList[i];
	}
	dir.setNameFilters(filter);
	QList<QFileInfo> *fileInfo = new QList<QFileInfo>(dir.entryInfoList(filter));
	int num = fileInfo->count();
	for (int j = 0; j < num; ++j)
	{
		QString RegionDir = folderDir + "/" + (fileInfo->at(j).fileName());
		bool bl = true;
		for (int k = 0; k < excludeDir.size(); ++k)
		{
			QFileInfo fileinfo2(excludeDir.at(k));
			if (fileInfo->at(j) == fileinfo2)
			{
				bl = false;
			}
		}
		if (bl)
		{
			filenameall << RegionDir;
		}

	}
	delete fileInfo;
}

bool LanguagePack::cdDir(QString folderDir, QStringList &filenameall, QList<QString> &typeList, QStringList excludeDir)
{
	QDir dir(folderDir);
	if (!dir.exists())
	{
		return false;
	}

	for (int k = 0; k < excludeDir.size(); ++k)
	{
		QDir dir2(excludeDir.at(k));
		if (dir == dir2)
		{
			return false;
		}
	}
	dir.setFilter(QDir::Dirs | QDir::NoSymLinks);
	if (dir.count() == 2)
	{
		return true;
	}
	for (int j = 2; j < dir.count(); ++j)
	{
		QString RegionDir = folderDir + "/" + dir[j];
		if (cdDir(RegionDir, filenameall, typeList, excludeDir))
		{
			godir(RegionDir, filenameall, typeList, excludeDir);
		}
	}
	return true;
}
