#include "metadatasqlmodel.h"

MetadataSqlModel::MetadataSqlModel(QSqlDatabase db)
	: QSqlTableModel(0, db)
{

}

MetadataSqlModel::~MetadataSqlModel()
{

}
