#include "fileread.h"
#include <map>
#include <QTextStream>
#include <QTextCodec>
#include <QByteArray>
#include <QRegExp>
#include <QTextEncoder>
#include "codedetect.h"

FileRead::FileRead(QString RegionDir, OpenFlag of)
{
	//int n = RegionDir.toStdString().find_last_of('/');
	int n = RegionDir.lastIndexOf("/");//路径包含中文时此结果和上面的不一致
	fileDir = RegionDir.mid(n + 1);
	readstrFile(RegionDir,of);
	doemit = false;
	isSame = false;
}

FileRead::~FileRead()
{
	strFile.close();
}

void FileRead::readstrFile(QString RegionDir, OpenFlag of)
{
	strFile.setFileName(RegionDir);
	if (of == Readonly)
	{
		strFile.open(QIODevice::ReadOnly);
	}
	else
		strFile.open(QIODevice::ReadWrite);
	if (strFile.fileName().mid(strFile.fileName().lastIndexOf(".")) == ".ui")
		isUiFile = true;
	else
		isUiFile = false;
}

void FileRead::extractStr(std::map<QString, QStringList> &words,QString regx)
{
	EncodingType type = CodeDetect::GetEncodingType(strFile);
	int row = 0;
	while (!strFile.atEnd())
	{
		//QStringList line = QString(strFile.readLine()).split('\r');
		//QString s = line[0];
		QByteArray byarr = strFile.readLine();
		QString s = QString();
		if (type == ENCODINGTYPE_UTF8)
		{
			s = QString::fromUtf8(byarr);
		}
		else
		{
			s = QString::fromLocal8Bit(byarr);
		}

		++row;  //行数+1
		
		if (s.indexOf('#',0)==0)//判断第一个字符是不是 #
		{
			continue;
		}
		if (doemit)//当前位于注释块中/**/
		{
			if (s.trimmed().indexOf("*/") == -1)
			{
				doemit = true;
				continue;
			}
			else
			{
				s = s.section("*/", 1, 1);
				doemit = false;
			}
		}
		if (s.trimmed().startsWith("/*"))//只有在/*打头的时候才认为开始了块注释
		{
			if (s.trimmed().indexOf("*/") == -1)
			{
				doemit = true;
				continue;
			}
			else
			{
				s = s.section("*/", 1, 1);
				doemit = false;
			}
		}
		s = s.section("//", 0, 0);//不严谨

		QString allLine = s + "    file:" + fileDir + "  row:" + QString::number(row);
		QString ttmp = s.trimmed();
		int i = 0;
		s = subQStr(ttmp,i);
		while (!s.isEmpty())
		{
			QString temstr = s.trimmed(); //去掉字符串中左右多余空格
			QRegExp rx(regx);
			if (analysisStr(temstr) && rx.exactMatch(temstr))
			{
				std::map<QString, QStringList>::iterator it;
				it = words.find(temstr);
				if (it == words.end())
				{
					words.insert(std::make_pair(temstr, QStringList() << allLine));
				}
				else
				{
					it->second << allLine;
				}
			}
			s = subQStr(ttmp, i);
		}
	}

}

void FileRead::updateStr(std::map<QString, QString> &languageMap, QString regx)
{
	QTextStream out(&strFile);
	out.setCodec(QTextCodec::codecForName("utf-8"));   //设置输出文件编码格式
	out.setGenerateByteOrderMark(true);
	
	//读取文件中的所有内容
	QString allFile = strFile.readAll();
	//清空原有文件内容
	strFile.resize(0);
	int i = 0;
	//文件行数
	int lineNum = 0;
	QString s1 = allFile.section("\r\n", i, i);
	while (lineNum!=-1)
	{
		lineNum = allFile.indexOf("\r\n", lineNum + 1);
		QString s = s1;
		if (s.indexOf('#', 0) == 0)
		{
			out << s1 << endl;
			++i;
			s1 = allFile.section("\r\n", i, i);
			continue;
		}
		//判断是不是注释
		if (s.indexOf("/*", 0) != -1)
		{
			s = s.section("/*", 0, 0);
			doemit = true;
		}
		if (s.indexOf("*/", 0) != -1)
		{
			s = s.section("*/", 1, 1);
			doemit = false;
		}
		if (doemit == true)
		{
			out << s1 << endl;
			++i;
			s1 = allFile.section("\r\n", i, i);
			continue;
		}

		s = s.section("//", 0, 0);

		QString ttmp = s;
		int j = 0;
		s = subQStr(ttmp,j);
		//s = trope(s);
		while (!s.isEmpty())
		{
			QString temstr = s.trimmed(); //去掉字符串中左右多余空格
			int olenth = s.size();
			QString ostr = s;
			QRegExp rx(regx);
			if (analysisStr(temstr) && rx.exactMatch(temstr))
			{
				if (!isUiFile)
				{
					temstr = trope(temstr);
				}
				std::map<QString, QString>::iterator iter = languageMap.find(temstr);
				if (languageMap.end() != iter)//判断iter是否存在！！！
				{
					QString after = iter->second;
					if (after != "")
					{
						if (!isUiFile)
						{
							temstr = _trope(temstr);
						}
						int pot = s.indexOf(temstr, 0);
						s.replace(pot, temstr.size(), after);
						int pot2 = s1.indexOf(ostr, 0);
						s1.replace(pot2, olenth, s);
					}
				}
			}
			s = subQStr(ttmp, j);
			//s = trope(s);
		}
		out << s1 << endl;
		++i;
		s1 = allFile.section("\r\n", i, i);
	}
}

void FileRead::updateStr2(std::map<QString, QString> &languageMap, QString regx)
{
	QTextStream out(&strFile);
	out.setGenerateByteOrderMark(true);//utf-8 with bom
	out.setCodec(QTextCodec::codecForName("utf-8"));//设置输出文件编码格式
	
	EncodingType type = CodeDetect::GetEncodingType(strFile);
	QStringList lineList;
	while (!strFile.atEnd())
	{
		QByteArray byarr = strFile.readLine();
		QString str = QString();
		if (type == ENCODINGTYPE_UTF8)
		{
			str = QString::fromUtf8(byarr);
		}
		else
		{
			str = QString::fromLocal8Bit(byarr);
		}
		if (str.endsWith("\n"))
			str.chop(1);
		lineList << str;
	}
	strFile.resize(0);//清空文件

	for (int m = 0; m < lineList.size(); ++m)
	{
		QString s1 = lineList[m];
		QString s = s1;
		if (s.indexOf('#', 0) == 0)
		{
			out << s1 << endl;
			continue;
		}
		if (doemit)//当前位于注释块中/**/
		{
			if (s.trimmed().indexOf("*/") == -1)
			{
				doemit = true;
				out << s1 << endl;
				continue;
			}
			else
			{
				s = s.section("*/", 1, 1);
				doemit = false;
			}
		}
		if (s.trimmed().startsWith("/*"))//只有在/*打头的时候才认为开始了块注释
		{
			if (s.trimmed().indexOf("*/") == -1)
			{
				doemit = true;
				out << s1 << endl;
				continue;
			}
			else
			{
				s = s.section("*/", 1, 1);
				doemit = false;
			}
		}
		s = s.section("//", 0, 0);//不严谨

		QString ttmp = s.trimmed();
		int j = 0;
		s = subQStr(ttmp, j);
		while (!s.isEmpty())
		{
			QString temstr = s.trimmed(); //去掉字符串中左右多余空格
			int olenth = s.size();
			QString ostr = s;
			QRegExp rx(regx);
			if (analysisStr(temstr) && rx.exactMatch(temstr))
			{
				std::map<QString, QString>::iterator iter = languageMap.find(temstr);
				if (languageMap.end() != iter)//判断iter是否存在！！！
				{
					QString after = iter->second;
					if (after != "")
					{
						int pot = s.indexOf(temstr, 0);
						s.replace(pot, temstr.size(), after);
						int pot2 = s1.indexOf(ostr, 0);
						s1.replace(pot2, olenth, s);
					}
				}
			}
			s = subQStr(ttmp, j);
		}
		out << s1 << endl;
	}
}

bool FileRead::analysisStr(QString str)
{
	int nlen = str.count();
	if (str == "OK" || str == "Yes" || str == "NO" || str == "Cancel")
	{
		return true;
	}
	if (nlen > 7)
	{
		return true;
	}
	for (int i = 0; i < nlen; ++i)
	{
		if (str.at(i) >= 0 && str.at(i) <= 127)
		{
		}
		else
		{
			return true;
		}
			
	}
	return false;
}

QString FileRead::subQStr(QString str,int &pos)
{
	QString out;
	if (isUiFile)
	{
		if (pos >= str.length())
		{
			out == NULL;
			return out;
		}
		int start = str.indexOf("<string>", pos);
		int end = start + 1;
		end = str.indexOf("</string>", end);
		if (start == -1 && end == -1)
		{
			out = str;
			pos = str.length();
		}
		else if (start != -1 && end == -1)
		{
			start = start + 8;
			out = str.mid(start);
			pos = str.length();
		}
		else if (start == -1 && end != -1)
		{
			out = str.left(end);
			pos = end + 9;
		}
		else
		{
			start = start + 8;
			out = str.mid(start, end - start);
			pos = end + 9;
		}
	}
	else
	{
		int start = str.indexOf('\"', pos);
		if (start == -1)
		{
			out == NULL;
			return out;
		}
		int end = start + 1;
		while (1)
		{
			int index = str.indexOf("\\\"", end);//寻找\"的位置
			int index2 = str.indexOf("\\\\\"", end);//寻找\\"的位置
			int index3 = str.indexOf('\"', end);//寻找"的位置

			if (index3 == -1)
			{
				out == NULL;
				return out;
			}
			if (index - index2 == 1)
			{
				end = index3;
				break;
			}
			else if (index3 - index == 1)
			{
				end = index3 + 1;
				continue;
			}
			else
			{
				end = index3;
				break;
			}
		}
		out = str.mid(start + 1, end - start - 1);
		pos = end + 1;
	}
	return out;
}

//QT在读写xml时已经进行特殊字符的转义，因此无需再用下面的函数进行转义
QString FileRead::trope(QString str)
{
	str = str.replace('&',"&amp;");
	str = str.replace('<',"&lt;");
	str = str.replace('>',"&gt;");
	//str = str.replace('\"',"&quot;");
	//str = str.replace('\'',"&apos;");
	return str;
}
QString FileRead::_trope(QString str)
{
	str = str.replace("&amp;", "&");
	str = str.replace("&lt;", "<");
	str = str.replace("&gt;", ">");
	//str = str.replace("&quot;", "\"");
	//str = str.replace("&apos;", "\'");
	return str;
}