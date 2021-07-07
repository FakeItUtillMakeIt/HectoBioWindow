//可以用于第三种数据显示方法，用户操作界面和显示界面分离
//0716,未使用

#include "SignalDisplay.h"


SignalDisplay::SignalDisplay(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	for (int i=0;i<50;i++)
	{
		val[i] = 0;
		time[i] = i;
	}

	ui.qwtPlot->setTitle("mV/ms");
	ui.qwtPlot->setAxisTitle(QwtPlot::xBottom, "t/ms");
	ui.qwtPlot->setAxisTitle(QwtPlot::yLeft, "voltage/mv");
	ui.qwtPlot->setAxisAutoScale(QwtPlot::xBottom,true);
	ui.qwtPlot->setAxisAutoScale(QwtPlot::yLeft, true);

	ui.qwtPlot->insertLegend(new QwtLegend(), QwtPlot::RightLegend);//曲线标签

	curve = new QwtPlotCurve();
	curve->setPen(Qt::blue, 2);
	curve->setSamples(time, val, 500);
	curve->setCurveAttribute(QwtPlotCurve::Fitted, true);//平滑
	curve->attach(ui.qwtPlot);

	qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &SignalDisplay::timerUpdate);
	timer->start(100);

}

SignalDisplay::~SignalDisplay()
{
	delete &ui;
}

void SignalDisplay::timerUpdate() {
	for (int i=0;i<=48;i++)
	{
		val[i] = val[i + 1];
	}
	val[49] = qrand() % 50;
	curve->setSamples(time, val, 500);
	ui.qwtPlot->replot();
}



