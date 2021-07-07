#pragma once

#include <QWidget>
#include "ui_SignalDisplay.h"
#include "qwt_plot_curve.h"
#include "qmath.h"
#include "qwt_plot.h"
#include "qwt_legend.h"
#include "QTimer.h"
#include "QDateTime.h"
#include "qwt_point_data.h"

class SignalDisplay : public QWidget
{
	Q_OBJECT

public:
	SignalDisplay(QWidget *parent = Q_NULLPTR);
	~SignalDisplay();

public:
	QwtPlotCurve* curve;

	double time[50] = { 0 };
	double val[50];

private slots:
	void timerUpdate();
private:
	Ui::SignalDisplay ui;
};
