#include "msurveymethodwnd.h"

MSurveyMethodWnd::MSurveyMethodWnd(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.radioButton->setChecked(true);
}

MSurveyMethodWnd::~MSurveyMethodWnd()
{

}

void MSurveyMethodWnd::on_pushButton_ok_clicked()
{
	if (ui.radioButton->isChecked())
	{
		m_mehtod = MSurveyMethod::MultiBeam;
	}
	else if (ui.radioButton_2->isChecked())
	{
		m_mehtod = MSurveyMethod::SingleBeam;
	}
	else if (ui.radioButton_3->isChecked())
	{
		m_mehtod = MSurveyMethod::ADCP;
	}
	else if (ui.radioButton_4->isChecked())
	{
		m_mehtod = MSurveyMethod::SideSonar;
	}
	else if (ui.radioButton_5->isChecked())
	{
		m_mehtod = MSurveyMethod::TideGuage;
	}
	else if (ui.radioButton_6->isChecked())
	{
		m_mehtod = MSurveyMethod::SoundVelocimeter;
	}
	else if (ui.radioButton_7->isChecked())
	{
		m_mehtod = MSurveyMethod::SedimentSampler;
	}
	else if (ui.radioButton_8->isChecked())
	{
		m_mehtod = MSurveyMethod::GPS;
	}
	else if (ui.radioButton_9->isChecked())
	{
		m_mehtod = MSurveyMethod::Magnetometer;
	}
	accept();
}

void MSurveyMethodWnd::on_pushButton_cancel_clicked()
{
	reject();
}
