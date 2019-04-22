#ifndef PROGRESSBASE_H
#define PROGRESSBASE_H

#include <QString>

class ProgressBase
{
public:
	ProgressBase(){}
	virtual ~ProgressBase(){}

	virtual void CreateProgress(const QString& info = QString()) = 0;
	virtual void CloseProgress() = 0;
	virtual void SetProgressValue(const int& curStep, const int& totalStep) = 0;
	virtual void SetProgressText(const QString& info = QString()) = 0;

	//1-创建；2-信息；3-关闭
	virtual void AutoProgress(const int& mode, const QString& info = QString()) = 0;
};

class ProgressTracker :public ProgressBase
{
public:
	ProgressTracker(const QString& info = QString());
	virtual ~ProgressTracker();

	virtual void CreateProgress(const QString& info = QString()) override;
	virtual void CloseProgress() override;
	virtual void SetProgressValue(const int& curStep, const int& totalStep) override;
	virtual void SetProgressText(const QString& info = QString()) override;

	//1-创建；2-信息；3-关闭
	virtual void AutoProgress(const int& mode, const QString& info = QString()) override;

private:
	QString _info;
};

#endif // PROGRESSBASE_H
