#include "cs_coordsystemstr.h"
#pragma execution_character_set("utf-8")

CoordSystemStr::CoordSystemStr()
: _wkid(-1)
, _cs_type(NONECS)
, _cs_unit(_cs_type)
{

}

CoordSystemStr::~CoordSystemStr()
{

}


CSParamObject* parseQStr2CSPObj(const QString& str)
{
	CSParamObject* obj = new CSParamObject();
	//由外及里，递归处理

	//先获取最外层括号的索引，必须是对应的括号，此处取法不严谨（默认str只对应一个根节点）
	int firstbracket = str.indexOf("[");
	int lastbracket = str.lastIndexOf("]");
	if (firstbracket == -1 || lastbracket == -1 || lastbracket < firstbracket)
	{
		delete obj;
		obj = NULL;
		return obj;
	}
	//到这儿说明至少还有个mark，但是mark也有可能为空
	QString mark = str.left(firstbracket).trimmed();
	obj->_mark = mark;
	QString content = str.mid(firstbracket + 1, lastbracket - firstbracket - 1);//提取内容，不包括头尾括号
	if (content.contains(","))//说明不只具有name
	{
		int index = content.indexOf(",");
		obj->_name = content.left(index).remove("\"").trimmed();
		if (content.contains("["))//说明还有children
		{
			//这里要把children字符串分割出来，单个解析，然后将返回的结果push到children中去
			QString subStr = content.mid(index + 1);//去除名称和逗号剩余的部分
			int startindex = 0;
			int endindex = 0;
			int count = 0;//用于寻找对应的括号
			for (int i = 0; i < subStr.size(); ++i)
			{
				if (subStr.at(i) != '[' && subStr.at(i) != ']')
					continue;
				if (subStr.at(i) == '[')
				{
					++count;
				}
				else if (subStr.at(i) == ']')
				{
					--count;
				}

				if (count < 0) break;//说明[]不匹配，跳出
				else if (count > 0) continue;//说明还未匹配到
				else if (count == 0)//即找到对应的括号索引了
				{
					endindex = i;
					QString childStr = subStr.mid(startindex, endindex - startindex + 1);//提取子对象内容
					CSParamObject* child = parseQStr2CSPObj(childStr);
					if (child)
					{
						child->_parent = obj;
						obj->_children << child;
					}
					//查找逗号，如果后面还有逗号则首末还有child，如果找不到则认为再没有child
					index = subStr.indexOf(",", endindex);
					if (index == -1) break;//找不到则认为再没有child
					i = index + 1;
					startindex = i;
					endindex = 0;
					count = 0;
				}
			}
		}
		else//说明只有values
		{
			QStringList values = content.split(",", QString::KeepEmptyParts);
			for (int i = 1; i < values.size(); ++i)
			{
				obj->_values << values[i].trimmed();
			}
		}
	}
	else//说明只有name
	{
		obj->_name = content.remove("\"").trimmed();
		return obj;
	}
	return obj;
}

QString parseCSPObj2QStr(const CSParamObject* obj)
{
	if (obj == NULL) return QString();
	QString str;
	if (obj->_children.isEmpty())//说明是叶子节点
	{
		QString valuesStr;
		for (int i = 0; i < obj->_values.size(); ++i)
		{
			valuesStr += QString(",%1").arg(obj->_values[i]);
		}
		str = QString("%1[\"%2\"%3]").arg(obj->_mark).arg(obj->_name).arg(valuesStr);
	}
	else//说明存在子节点
	{
		QString childrenStr;
		for (int i = 0; i < obj->_children.size(); ++i)
		{
			childrenStr += QString(",%1").arg(parseCSPObj2QStr(obj->_children[i]));
		}
		str = QString("%1[\"%2\"%3]").arg(obj->_mark).arg(obj->_name).arg(childrenStr);
	}
	return str;
}

CSParamObject* findCSPObjByMark(const CSParamObject* obj, const QString & mark)
{
	if (obj == NULL) return NULL;
	CSParamObject* pCsObj = const_cast<CSParamObject*>(obj);
	if (obj->_mark.toUpper() == mark.toUpper())
		return pCsObj;
	
	if (obj->_children.isEmpty())//说明是叶子节点
	{
		return NULL;//没有找到
	}
	else//说明存在子节点
	{
		for (int i = 0; i < obj->_children.size(); ++i)
		{
			CSParamObject* pCurObj = obj->_children[i];
			CSParamObject* pTmpObj = findCSPObjByMark(pCurObj, mark);
			if (pTmpObj != NULL)
			{
				return pTmpObj;
			}
		}
	}
	return NULL;
}
