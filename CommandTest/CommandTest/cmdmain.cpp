#include "commandtest_global.h"
#include <QStringList>
#include "commandtest.h"

//添加单个命令
void AddCommand(QStringList & arrNames, QList<void*> & arrFun, QStringList& explainList, QString csName, void* pFun, const QString &explain = QString())
{
	arrNames.push_back(csName);
	arrFun.push_back(pFun);
	explainList.push_back(explain);
}

//注册命令函数
extern "C" void COMMANDTEST_EXPORT CmdRegister(QStringList & arrNames, QList<void*> & arrFun, QStringList& explainList)
{
	AddCommand(arrNames, arrFun, explainList, "Test_Func6", test_func6, "test6 help content");
	AddCommand(arrNames, arrFun, explainList, "Test_Func7", test_func7, "test7 help content");
	AddCommand(arrNames, arrFun, explainList, "Test_Func8", test_func8, "test8 help content");
	AddCommand(arrNames, arrFun, explainList, "Test_Func9", test_func9, "test9 help content");
	AddCommand(arrNames, arrFun, explainList, "Test_Func0", test_func0, "test0 help content");
}

//入口点函数
extern "C" bool COMMANDTEST_EXPORT CmdEntryPoint()
{
	//for now do nothing
	return true;
}
