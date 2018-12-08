#ifndef METADATASQLMODEL_H
#define METADATASQLMODEL_H

#include <QSqlTableModel>

//元数据TableView Model
class MetadataSqlModel : public QSqlTableModel
{
	Q_OBJECT

public:
	MetadataSqlModel(QSqlDatabase db);
	~MetadataSqlModel();

private:
	
};

#endif // METADATASQLMODEL_H
