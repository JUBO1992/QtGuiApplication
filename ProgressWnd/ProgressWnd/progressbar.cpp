#include "progressbar.h"

#pragma execution_character_set("utf-8")

ProgressBar* ProgressBar::m_progress = NULL;
std::mutex ProgressBar::m_mutex;

ProgressBar::ProgressBar(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	this->setAttribute(Qt::WA_TranslucentBackground, true);

	m_move = false;
}

ProgressBar::~ProgressBar()
{

}

ProgressBar* ProgressBar::getInstance()
{
	if (!m_progress)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (!m_progress)
		{
			m_progress = new ProgressBar();
		}
	}
	return m_progress;
}

void ProgressBar::createProgress(const QString& info /*= QString()*/)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_info = info;
	QString msg = QString("%1 [%2]\%").arg(info).arg(0);
	getInstance()->ui.label->setText(msg);
	getInstance()->show();
}

void ProgressBar::setProgressText(const QString& info /*= QString()*/)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_info = info;
}

void ProgressBar::setProgressValue(const int& curStep, const int& totalStep)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	int value = curStep * 100 / totalStep;
	if (value != getInstance()->ui.progressBar->value())
	{
		getInstance()->ui.progressBar->setValue(value);
		QString msg = QString("%1 [%2]\%").arg(m_info).arg(value);
		getInstance()->ui.label->setText(msg);
		qApp->processEvents();
	}
}

void ProgressBar::closeProgress()
{
	getInstance()->hide();
}

void ProgressBar::mousePressEvent(QMouseEvent *event)
{
	//当鼠标左键点击时.
	if (event->button() == Qt::LeftButton)
	{
		m_move = true;
		//记录鼠标的世界坐标.
		m_startPoint = event->globalPos();
		//记录窗体的世界坐标.
		m_windowPoint = this->frameGeometry().topLeft();
	}
}

void ProgressBar::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		//移动中的鼠标位置相对于初始位置的相对位置.
		QPoint relativePos = event->globalPos() - m_startPoint;
		//然后移动窗体即可.
		this->move(m_windowPoint + relativePos);
	}
}

void ProgressBar::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		//改变移动状态.
		m_move = false;
	}
}
