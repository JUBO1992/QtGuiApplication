#ifndef __LANGUAGEPACK_H__
#define __LANGUAGEPACK_H__

#include <QObject>
#include <QDomDocument> 
#include <QString>
#include <QTextStream>
#include <QIODevice>
#include <QList>
#include <QDomDocument> 

/*! \brief 配置描述结构体
*/
struct CfgProfile
{
	QStringList folder_dir;					//处理文件夹
	QStringList exclude_dir;				//排除文件夹
	QStringList file_list;					//处理文件
	QList<QString> type_list;				//处理文件类型
	QStringList language_list;				//语言
	QString regx;							//正则表达式
};

/*! \brief 语言包对象结构体
*/
struct LanguageObj
{
	QString chinese;						//中文
	QStringList env;						//中文出现的环境
	QString foreign;						//外文

	bool operator==(const LanguageObj b) const
	{
		if (this->chinese == b.chinese
			&& this->env == b.env
			&& this->foreign == b.foreign)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

class LanguagePack : public QObject
{
	Q_OBJECT

public:
	LanguagePack(QObject *parent = 0);
	~LanguagePack();
	/*! \brief 更新语言包
		\pre 需要两个已有xml语言包文件
		\param [in] oldpath 已有xml文件语言包
		\param [in] newpath 待更新xml文件语言包
	*/
	void updateLanguagePack(QString oldfile, QString newfile);
	/*! \brief 提取生成xml语言包
		\param [in] config 配置参数结构体
		\param [in] xmlpath 语言包路径
		\return 成功返回"success"，失败返回错误信息
	*/
	QString extractLanguagePack(const CfgProfile &config, QString xmlpath);
	/*! \brief 应用已有xml语言包中的指定语言language
		\param [in] config 配置参数结构体
		\param [in] newpath 语言包路径
		\param [in] language 语言类别
		\return 成功返回"success"，失败返回错误信息
	*/
	QString applyLanguagePack(const CfgProfile &config, QString xmlpath, QString language);
	/*! \brief 读取配置文件
		\param [in] cfgfile 配置文件路径
		\param [out] &config 配置参数结构体
	*/
	bool readConfigFile(QString cfgfile, CfgProfile &config);
	/*! \brief 读取配置文件
		\param [in] cfgfile 配置文件路径
		\param [in] &config 配置参数结构体
	*/
	bool writeConfigFile(QString cfgfile, const CfgProfile &config);
	/*! \brief 根据ID标签查找该标签元素
		\param [in] xmlpath 语言包路径
		\param [in] idtext ID标签text
		\param [out] ID 语言包ID标签
	*/
	bool findElementByIDtag(QString xmlpath, QString idtext, QDomElement &ID);
	/*! \brief 读取xmlpath语言包，获取LanguageObj列表
		\param [in] xmlpath 语言包路径
		\param [in] languagetag Language标签
		\param [out] languagelist LanguageObj列表
	*/
	bool readLanguageFile(QString xmlpath, QString languagetag, QList<LanguageObj> &languagelist);
	/*! \brief 根据LanguageObj列表，更新xmlpath语言包
		\param [in] xmlpath 语言包路径
		\param [in] languagetag Language标签
		\param [in] languagelist LanguageObj列表
	*/
	bool updateLanguageFile(QString xmlpath, QString languagetag, const QList<LanguageObj> &languagelist);

	bool isConfigFile(QString cfgfile);
	bool isLanguageFile(QString xmlpath);

private:
	/*! \brief 根据正则表达式regx提取filelist文件中的中文字符串
		\param [in] filelist 文件列表
		\param [in] regx 正则表达式
		\return 返回中文字符串列表
	*/
	std::map<QString, QStringList> extractChinese(QStringList filelist, QString regx);
	/*! \brief 根据chinese查找languagelist相应元素，返回index
		\param [in] languagelist
		\param [in] chinese
		\return -1表示未找到
	*/
	int findIndexByChinese(const QList<LanguageObj> &languagelist, QString chinese);

	void getOldMap(const QDomDocument &old_doc, QMap<QString, QString> &old_map);
	void replaceNew(QDomDocument &new_doc, QMap<QString, QString> &old_map);
	void updateFile(QString old_file_path, QString new_file_path);
	void godir(QString folderDir, QStringList &filenameall, QList<QString> &typeList, QStringList excludeDir);
	bool cdDir(QString folderDir, QStringList &filenameall, QList<QString> &typeList, QStringList excludeDir);
};

#endif // __LANGUAGEPACK_H__
