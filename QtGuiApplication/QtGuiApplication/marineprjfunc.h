//将海测工程相关操作函数移至marineprjfunc.h

#ifndef MARINEPRJFUNC_H
#define MARINEPRJFUNC_H

#include "marinebase.h"


/*
	@brief 读取ProjectMenu表，获取海测工程列表
	注意：该函数不会清空prjlist
	@param[out] prjlist为海测工程列表
*/
void ReadProjectMenu(QList<MarinePrjStruct>& prjlist);

/*
	@brief 读取ProjectMenu表，获取海测工程列表
	注意：该函数不会清空prjMap
	@param[out] prjMap为海测工程列表（key为海测工程编号，value为海测工程详细信息）
*/
void ReadProjectMenu(QMap<QString, MarinePrjStruct>& prjMap);

/*
	@brief 读取ProjectMenu表，更新海测工程列表
	注意：该函数会清空prjMap后重新追加
	@param[out] prjMap为海测工程列表（key为海测工程编号，value为海测工程详细信息）
*/
void UpdateProjectMenu(QMap<QString, MarinePrjStruct>& prjMap);

/*
	@brief 创建海测工程
	@param[in] marine为要创建的海测工程
	@return 返回是否成功
*/
bool CreateMarinePrj(const MarinePrjStruct& marine);

/*
	@brief 删除海测工程
	@param[in] marine为要删除的海测工程
	@return 返回是否成功
*/
bool DeleteMarinePrj(const MarinePrjStruct& marine);

/*
	@brief 打开海测工程
	@param[in] marine为要打开的海测工程
	@return 返回是否成功
*/
bool OpenMarinePrj(const MarinePrjStruct& marine);

/*
	@brief 关闭海测工程
	@param[in] marine为要关闭的海测工程
	@return 返回是否成功
*/
bool CloseMarinePrj(const MarinePrjStruct& marine);

/*
	@brief 更新海测工程
	@param[in] marine记录更新信息
	@param[out] marinePrjID为待更新的工程编号
	@return 返回是否成功
*/
bool UpdateMarinePrj(const MarinePrjStruct& marine, const QString& marinePrjID);

/*
	@brief 获取海测工程下的文件列表
	注意：该函数不会清空files
	@param[in] marine
	@param[in] type 文件类型
	@param[out] files 读取到的文件列表（key为文件编号 value为文件信息）
	@param[in] method type == SurveyFile时有效
*/
void GetMarinePrjFiles(const MarinePrjStruct& marine, const MSFileEnum& type, QMap<QString, MSFileStruct>& files,
	const MSurveyMethod& method = MSurveyMethod::MSUnknown);

/*
	@brief 向海测工程添加文件，返回添加文件的编号列表
	注意：1该函数对VectorFile类型无效；2该函数内部对文件是否存在不做判断，会尝试覆盖已有文件
	3应该保证传入的files不存在已有Index，否则会导致同一文件存在多个Index
	@param[in] marine
	@param[in] type 添加的文件类型，当type == SurveyFile时需要指定method参数
	@param[in] files 要添加的文件列表（记录文件全路径），这些文件将被拷贝到海测工程下
	@param[in] method 当添加测量文件时，用于指定测量文件由何种测量手段获得
	@return 返回文件编号列表，如果添加失败则返回空列表
*/
QStringList AddFilesToMarinePrj(const MarinePrjStruct& marine, const MSFileEnum& type, const QStringList& files,
	const MSurveyMethod& method = MSurveyMethod::MSUnknown);

/*
	@brief 向海测工程添加文件，返回添加文件的编号列表
	注意：1该函数对VectorFile类型无效；2该函数内部对文件是否存在不做判断，会尝试覆盖已有文件
	@param[in] marine
	@param[in] type 添加的文件类型，当type == SurveyFile时需要指定method参数
	@param[in] dirpath 要添加的文件所在目录路径，该目录下所有文件及文件夹将被拷贝到目标路径下
	@param[in] method 当添加测量文件时，用于指定测量文件由何种测量手段获得
	@return 返回文件编号列表，如果添加失败则返回空列表
*/
QStringList AddDirToMarinePrj(const MarinePrjStruct& marine, const MSFileEnum& type, const QString& dirpath,
	const MSurveyMethod& method = MSurveyMethod::MSUnknown);

/*
	@brief 查询海测工程文件
	注意：1该函数不会清空files；2该函数对VectorFile类型无效
	@param[in] marine
	@param[in] type 要查询的文件类型
	@param[in] fileNOList 要查询的文件编号（如果为空则获取该类型的所有文件类别）
	@param[out] files 查询到的文件信息（key为文件编号 value为文件信息）
	@param[in] method 如果type == SurveyFile且fileNOList为空则需要指定method，如果不指定则获取'测量文件'目录下所有内容
*/
void FindFilesInMarinePrj(const MarinePrjStruct& marine, const MSFileEnum& type, const QStringList& fileNOList, QMap<QString, MSFileStruct>& files,
	const MSurveyMethod& method = MSurveyMethod::MSUnknown);

/*
	@brief 从海测工程删除文件
	注意：该函数对VectorFile类型无效
	@param[in] marine
	@param[in] type 要删除的文件类型
	@param[in] fileNOList 要删除的文件编号（如果为空则清空相应目录下文件及文件夹）
	@param[in] method 如果type == SurveyFile且fileNOList为空则需要指定method，如果不指定则删除'测量文件'目录下所有内容
	@return 返回是否删除成功
*/
bool DeleteFilesFromMarinePrj(const MarinePrjStruct& marine, const MSFileEnum& type, const QStringList& fileNOList = QStringList(),
	const MSurveyMethod& method = MSurveyMethod::MSUnknown);

/*
	@brief 更新海测工程中文件信息，此处只针对Index表进行信息更新（如果文件名变化同时修改文件名），不支持修改文件编号
	@param[in] marine
	@param[in] files 要更新的文件列表，key为文件编号，value为文件信息
	@return
*/
bool UpdateFilesInMarinePrj(const MarinePrjStruct& marine, const QMap<QString, MSFileStruct>& files);

/*
	@brief 生成idata工程文件*.idata
*/
bool CreateiDataPrjFile(const QStringList& mdbs, const QStringList& imgs, const QString prjfilepath);

/*
	@brief 更新idata工程文件*.idata
*/
bool UpdateiDataPrjFile(const MarinePrjStruct& marine);

#endif // MARINEPRJFUNC_H
