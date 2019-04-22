#include "progressbase.h"
#include "progressbar.h"

#pragma execution_character_set("utf-8")

ProgressTracker::ProgressTracker(const QString& info/* = QString()*/)
:_info(info)
{

}

ProgressTracker::~ProgressTracker()
{

}

void ProgressTracker::CreateProgress(const QString& info /*= QString()*/)
{
	QString msg = info;
	if (!_info.isEmpty())msg = _info;
	ProgressBar::getInstance()->createProgress(msg);
}

void ProgressTracker::CloseProgress()
{
	ProgressBar::getInstance()->closeProgress();
}

void ProgressTracker::SetProgressValue(const int& curStep, const int& totalStep)
{
	ProgressBar::getInstance()->setProgressValue(curStep, totalStep);
}

void ProgressTracker::SetProgressText(const QString& info /*= QString()*/)
{
	QString msg = info;
	if (!_info.isEmpty())msg = _info;
	ProgressBar::getInstance()->setProgressText(msg);
}

void ProgressTracker::AutoProgress(const int& mode, const QString& info /*= QString()*/)
{

}
