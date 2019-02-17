#ifndef COMPRESSTOOL_H
#define COMPRESSTOOL_H

#include "baseoperatelib_global.h"
#include <QString>

class BASEOPERATELIB_EXPORT CompressTool
{
	enum CompressLevel
	{
		Level0,
		Level1,
		Level2,
		Level3,
		Level4,
		Level5,
		Level6,
		Level7,
		Level8,
		Level9,
	};
public:
	CompressTool();
	~CompressTool();

	static bool compress(QString src, QString dst);
	static bool unCompress(QString src, QString dst);

private:
	
};

#endif // COMPRESSTOOL_H
