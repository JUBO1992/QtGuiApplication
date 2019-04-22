#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QWidget>
#include "ui_progressbar.h"
#include <mutex>
#include <QMouseEvent>

class ProgressBar : public QWidget
{
	Q_OBJECT

private:
	ProgressBar(QWidget *parent = 0);
	~ProgressBar();

public:
	static ProgressBar* getInstance();

	void createProgress(const QString& info = QString());
	void setProgressText(const QString& info = QString());
	void setProgressValue(const int& curStep, const int& totalStep);
	void closeProgress();

	//1-创建；2-信息；3-关闭
	void autoProgress(const int& mode, const QString& info = QString());

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

private:
	Ui::ProgressBar ui;
	static std::mutex m_mutex;
	static ProgressBar* m_progress;
	QString m_info;

	bool m_move;
	QPoint m_startPoint;
	QPoint m_windowPoint;
};

#endif // PROGRESSBAR_H
