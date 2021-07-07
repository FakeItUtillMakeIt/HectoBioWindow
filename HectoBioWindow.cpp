#include "HectoBioWindow.h"
#include "QMessageBox"
#include <string>
#include "QString"
#include "qwt_plot.h"
#include "qwt_plot_zoomer.h"
#include "qwt_plot_panner.h"
#include "qwt_plot_magnifier.h"

#include "CompareWidgets.h"
#include "QAction"
#include "signaView.h"
#include "QFileDialog.h"
#include <QPushButton>
#include <fstream>
#include <QBitmap>
#include <QThread>
#include <windows.h>
#include "stdio.h"


//#include "usb2069.h"


using namespace std;

HectoBioWindow::HectoBioWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	//初始化界面
	init_window();

	//初始化电路连接
	init_circuit();

	//USB2069_InitAD(linkdevice,)

	//初始化数据保存类
	hdf5_op = new Hdf5Read();

	//初始化主界面设置
	curve->setPen(Qt::blue, 2);
	//更新初始数据 从电路出来的数据     电路到之后
	updateAD_data(NULL);

	ui.signalPlot->setTitle(QwtText("SIGNAL PLOT"));
	ui.signalPlot->setAxisTitle(QwtPlot::yLeft, "current / mA");
	ui.signalPlot->setAxisTitle(QwtPlot::xBottom, "time / mS");


	//menubar加动作设为Signal
	QAction* act = new QAction(("Compare widgets"), ui.menuSignal_compare_widget);
	act->setCheckable(true);
	act->setChecked(false);
	ui.menuSignal_compare_widget->addAction(act);

	QAction* act1 = new QAction(("signal select widgets"), ui.menu_signalView_widget);
	act1->setCheckable(true);
	act1->setChecked(false);
	ui.menu_signalView_widget->addAction(act1);

	/*****************************************************************/
	//比较窗口
	/*****************************************************************/
	//connect(ui.menuSignal_compare_widget, &QMenu::hovered, this, &HectoBioWindow::on_compare_btn);
	connect(ui.menuSignal_compare_widget, &QMenu::triggered, this, &HectoBioWindow::on_compare_btn);
	//挑选信号窗口
	//connect(ui.menu_signalView_widget, &QMenu::hovered, this, &HectoBioWindow::on_signalView_btn);
	connect(ui.menu_signalView_widget, &QMenu::triggered, this, &HectoBioWindow::on_signalView_btn);

	//主界面 
	//通道  全选按钮
	connect(ui.chnselectAll, &QPushButton::clicked, this, &HectoBioWindow::on_selectAll_btn);
	//取消全选按钮
	connect(ui.chncancelAll, &QPushButton::clicked, this, &HectoBioWindow::on_cancelAll_btn);
	//确定
	connect(ui.selectchnOK, &QPushButton::clicked, this, &HectoBioWindow::on_selectChnOK_btn);
	//采集参数  确定
	connect(ui.acqparaBtnOK, &QPushButton::clicked, this, &HectoBioWindow::on_acqparaOK_btn);
	//连续采集  启动
	connect(ui.ctnscqStart, &QPushButton::clicked, this, &HectoBioWindow::on_startCtnAcq_btn);
	//结束
	connect(ui.ctnscqEnd, &QPushButton::clicked, this, &HectoBioWindow::on_stopCtnAcq_btn);

	//显示

	//单次采集 结束
	connect(ui.singleacqEnd, &QPushButton::clicked, this, &HectoBioWindow::on_singleAcq_btn);
	//DA0 DA0
	connect(ui.btnDA0, &QPushButton::clicked, this, &HectoBioWindow::on_DA0_btn);
	//DA1  DA1
	connect(ui.btnDA1, &QPushButton::clicked, this, &HectoBioWindow::on_DA1_btn);
	//DA2	DA2
	connect(ui.btnDA2, &QPushButton::clicked, this, &HectoBioWindow::on_DA2_btn);
	//DA3	DA3
	connect(ui.btnDA3, &QPushButton::clicked, this, &HectoBioWindow::on_DA3_btn);

	//基线
	//基线读取
	connect(ui.baselineReadBtn, &QPushButton::clicked, this, &HectoBioWindow::on_baselineRd_btn);
	//基线校正
	connect(ui.baselineRecoverBtn, &QPushButton::clicked, this, &HectoBioWindow::on_baselineRcv_btn);

	//输出信息
	//保存输出信息
	connect(ui.saveTextbtn, &QPushButton::clicked, this, &HectoBioWindow::on_saveoutinfo_btn);
	//清除
	connect(ui.clearTextbtn, &QPushButton::clicked, this, &HectoBioWindow::on_clearinfo_btn);


	//信号显示功能  显示按钮
	connect(ui.signaldispDisp, &QPushButton::clicked, this, &HectoBioWindow::on_disp_btn);
	//清除按钮
	connect(ui.signalearseDisp, &QPushButton::clicked, this, &HectoBioWindow::on_earse_btn);
	//暂停按钮
	connect(ui.signalstopDisp, &QPushButton::clicked, this, &HectoBioWindow::on_stop_btn);
	//加速按钮
	connect(ui.signaldispHurry, &QPushButton::clicked, this, &HectoBioWindow::on_hurry_btn);
	//减速按钮 
	connect(ui.signaldispSlow, &QPushButton::clicked, this, &HectoBioWindow::on_slow_btn);


	//测试使用
	//保存按钮
	connect(ui.savetestSignalBtn, &QPushButton::clicked, this, &HectoBioWindow::save_testSignal_btn);

}


float HectoBioWindow::winDpiScale() {
	HDC screen = GetDC(NULL);
	FLOAT dpiX = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSX));
	ReleaseDC(0, screen);
	return dpiX / DEFAULT_DPI;
}



//初始化界面函数
void HectoBioWindow::init_window() {
	//适配分辨率
	int newWidth = this->width();
	int newHight = this->height();

	float scale = winDpiScale();
	newWidth = newWidth * scale;
	newHight = newHight * scale;
	/*this->setFixedWidth(newWidth);
	this->setFixedHeight(newHight);*/
	this->setMinimumWidth(newWidth);
	this->setMinimumHeight(newHight);


	//通道初始化，全部初始化为未选中
	ui.CH1->setCheckState(Qt::Unchecked);
	ui.CH2->setCheckState(Qt::Unchecked);
	ui.CH3->setCheckState(Qt::Unchecked);
	ui.CH4->setCheckState(Qt::Unchecked);
	ui.CH5->setCheckState(Qt::Unchecked);
	ui.CH6->setCheckState(Qt::Unchecked);
	ui.CH7->setCheckState(Qt::Unchecked);
	ui.CH8->setCheckState(Qt::Unchecked);
	ui.CH9->setCheckState(Qt::Unchecked);
	ui.CH10->setCheckState(Qt::Unchecked);
	ui.CH11->setCheckState(Qt::Unchecked);
	ui.CH12->setCheckState(Qt::Unchecked);
	ui.CH13->setCheckState(Qt::Unchecked);
	ui.CH14->setCheckState(Qt::Unchecked);
	ui.CH15->setCheckState(Qt::Unchecked);
	ui.CH16->setCheckState(Qt::Unchecked);
	ui.CH17->setCheckState(Qt::Unchecked);
	ui.CH18->setCheckState(Qt::Unchecked);
	ui.CH19->setCheckState(Qt::Unchecked);
	ui.CH20->setCheckState(Qt::Unchecked);
	ui.CH21->setCheckState(Qt::Unchecked);
	ui.CH22->setCheckState(Qt::Unchecked);
	ui.CH23->setCheckState(Qt::Unchecked);
	ui.CH24->setCheckState(Qt::Unchecked);


	//采集参数
	//触发模式  触发模式初始化为连续触发
	ui.trigModel->setEditable(false);

	ui.trigModel->addItem(QString::fromLocal8Bit("连续"));
	ui.trigModel->addItem(QString::fromLocal8Bit("后触发"));
	ui.trigModel->addItem(QString::fromLocal8Bit("延时"));

	ui.trigModel->setCurrentIndex(0);
	//触发源  触发源初始化为外正沿触发
	ui.trigSource->setEditable(false);

	ui.trigSource->addItem(QString::fromLocal8Bit("外正沿触发"));
	ui.trigSource->addItem(QString::fromLocal8Bit("外负沿触发"));
	ui.trigSource->addItem(QString::fromLocal8Bit("边沿触发"));

	ui.trigSource->setCurrentIndex(0);
	//采样率  采样率初始化为0
	ui.sampleFreq->setText(QString::number(sample_freq));
	//触发电平 触发电平初始化为0
	ui.trigLevel->setText(QString::number(trig_level));
	//触发长度
	ui.trigLength->setText(QString::number(trig_length));
	//延时

	ui.delayTime->setText(QString::number(trig_delay));

	//连续采集
	//显示
	ui.ctnscqDisplay->setCheckable(true);
	ui.ctnscqDisplay->setCheckState(Qt::Unchecked);
	ui.ctnscqStart->setEnabled(true);
	ui.ctnscqEnd->setEnabled(false);

	//触发模式
	ui.ctnacqMutliSelcet->setMaxVisibleItems(10);
	ui.ctnacqMutliSelcet->addItem(QString::fromLocal8Bit("外正沿触发"));
	ui.ctnacqMutliSelcet->addItem(QString::fromLocal8Bit("外负沿触发"));
	ui.ctnacqMutliSelcet->addItem(QString::fromLocal8Bit("边沿触发"));

	ui.ctnacqMutliSelcet->setCurrentIndex(0);

	//单次采集
	//采集时长
	ui.singleacqEnd->setEnabled(true);
	ui.singleacqTime->setText(QString::number(0));

	/********************************
	//DA部分暂时不清楚是用来做什么的？
	*********************************/
	//DA0

	ui.da0MutliSelect->setMaxVisibleItems(10);
	ui.da0MutliSelect->addItem(QString::fromLocal8Bit("正弦波"));
	ui.da0MutliSelect->addItem(QString::fromLocal8Bit("方波"));

	ui.da0freq->setText(QString::number(da_freq));
	ui.da0samplepoint->setText(QString::number(da_sample));
	ui.da0cycle->setText(QString::number(da_cycle));
	ui.da0start->setCheckable(true);
	ui.da0start->setCheckState(Qt::Unchecked);

	//DA1

	ui.da1MutliSelect->setMaxVisibleItems(10);
	ui.da1MutliSelect->addItem(QString::fromLocal8Bit("正弦波"));
	ui.da1MutliSelect->addItem(QString::fromLocal8Bit("方波"));

	ui.da1freq->setText(QString::number(da_freq));
	ui.da1samplepoint->setText(QString::number(da_sample));
	ui.da1cycle->setText(QString::number(da_cycle));
	ui.da1start->setCheckable(true);
	ui.da1start->setCheckState(Qt::Unchecked);
	//DA2

	ui.da2MutliSelect->setMaxVisibleItems(10);
	ui.da2MutliSelect->addItem(QString::fromLocal8Bit("正弦波"));
	ui.da2MutliSelect->addItem(QString::fromLocal8Bit("方波"));

	ui.da2freq->setText(QString::number(da_freq));
	ui.da2samplepoint->setText(QString::number(da_sample));
	ui.da2cycle->setText(QString::number(da_cycle));
	ui.da2start->setCheckable(true);
	ui.da2start->setCheckState(Qt::Unchecked);
	//DA3

	ui.da3MutliSelect->setMaxVisibleItems(10);
	ui.da3MutliSelect->addItem(QString::fromLocal8Bit("正弦波"));
	ui.da3MutliSelect->addItem(QString::fromLocal8Bit("方波"));

	ui.da3freq->setText(QString::number(da_freq));
	ui.da3samplepoint->setText(QString::number(da_sample));
	ui.da3cycle->setText(QString::number(da_cycle));
	ui.da3start->setCheckable(true);
	ui.da3start->setCheckState(Qt::Unchecked);


	ui.baseline_input->setText(QString::fromLocal8Bit("输入校正偏移"));

	

	return;
}

/*************************************************
   顾老师提出的比较功能
***************************************************/
//比较界面槽函数

void HectoBioWindow::on_compare_btn() {
	CompareWidgets* cmpwindow = new CompareWidgets();
	cmpwindow->setWindowTitle(QString::fromLocal8Bit("多信号比较界面"));
	
	cmpwindow->setWindowIcon(QIcon(QStringLiteral("res/hectobio1.png")));

	cmpwindow->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	/*QPixmap pmap(":/HectoBioWindow/res/backgroud.png");

	QPalette plt;

	plt.setBrush(cmpwindow->backgroundRole(), QBrush(pmap));

	cmpwindow->setPalette(plt);
	cmpwindow->setMask(pmap.mask());*/
	//pmap.createMaskFromColor(Qt::color0);

	cmpwindow->setAutoFillBackground(false);

	cmpwindow->show();

	return;
}

/*************************************************
   林立安提出的打分功能
***************************************************/
//打分界面槽函数
void HectoBioWindow::on_signalView_btn() {
	signaView* signalviewWindow = new signaView();
	signalviewWindow->setWindowTitle(QString::fromLocal8Bit("信号打分（供内部实验人员操作）"));

	signalviewWindow->setWindowIcon(QIcon(QStringLiteral("res/hectobio1.png")));

	signalviewWindow->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	/*QPixmap pmap(":/HectoBioWindow/res/backgroud.png");

	QPalette plt;

	plt.setBrush(signalviewWindow->backgroundRole(), QBrush(pmap));

	signalviewWindow->setPalette(plt);
	signalviewWindow->setMask(pmap.mask());*/
	//pmap.createMaskFromColor(Qt::color0);

	signalviewWindow->setAutoFillBackground(false);

	signalviewWindow->show();

	return;
}

/******************************
初始化电路
*************************************/
void HectoBioWindow::init_circuit() {

	linkdevice = INVALID_HANDLE_VALUE;
	BOOL dopen = FALSE;
	devNum = 0;
	for (int i = 0; i < 32; i++)
	{
		linkdevice = USB2069_Link(i);
		if (linkdevice != INVALID_HANDLE_VALUE)
		{
			devNum = i;

			para_init.ADFREQ = 0;
			for (int k=0;k<24;k++)
			{
				para_init.lEnCh[k] = false;
			}
			para_init.TriggerDelay = 0;
			para_init.TriggerLength = 0;
			para_init.TriggerLevel = 0;
			para_init.TriggerMode = 0;
			para_init.TriggerSource = 0;

			
			ui.textBrowser->append(QString::fromLocal8Bit("已连接设备号:")+QString::number(devNum));
			qDebug() << linkdevice << endl;
			device_flag = true;

			return;
		}
	}
	ui.textBrowser->append(QString::fromLocal8Bit("未连接设备，请检查或重新插拔设备"));
	device_flag = false;
	return;
}


/*************************************************
   通道全部使能
***************************************************/
//通道子功能槽函数
void HectoBioWindow::on_selectAll_btn() {
	ui.CH1->setCheckState(Qt::Checked);
	ui.CH2->setCheckState(Qt::Checked);
	ui.CH3->setCheckState(Qt::Checked);
	ui.CH4->setCheckState(Qt::Checked);
	ui.CH5->setCheckState(Qt::Checked);
	ui.CH6->setCheckState(Qt::Checked);
	ui.CH7->setCheckState(Qt::Checked);
	ui.CH8->setCheckState(Qt::Checked);
	ui.CH9->setCheckState(Qt::Checked);
	ui.CH10->setCheckState(Qt::Checked);
	ui.CH11->setCheckState(Qt::Checked);
	ui.CH12->setCheckState(Qt::Checked);
	ui.CH13->setCheckState(Qt::Checked);
	ui.CH14->setCheckState(Qt::Checked);
	ui.CH15->setCheckState(Qt::Checked);
	ui.CH16->setCheckState(Qt::Checked);
	ui.CH17->setCheckState(Qt::Checked);
	ui.CH18->setCheckState(Qt::Checked);
	ui.CH19->setCheckState(Qt::Checked);
	ui.CH20->setCheckState(Qt::Checked);
	ui.CH21->setCheckState(Qt::Checked);
	ui.CH22->setCheckState(Qt::Checked);
	ui.CH23->setCheckState(Qt::Checked);
	ui.CH24->setCheckState(Qt::Checked);

	return;
}

/*************************************************
   通道全部取消选择
***************************************************/
void HectoBioWindow::on_cancelAll_btn() {
	
	QCheckBox* all_chncheckBox[24] = { ui.CH1,ui.CH2 ,ui.CH3 ,ui.CH4 ,ui.CH5 ,ui.CH6,
	ui.CH7,ui.CH8 ,ui.CH9 ,ui.CH10 ,ui.CH11 ,ui.CH12,
	ui.CH13,ui.CH14 ,ui.CH15 ,ui.CH16 ,ui.CH17 ,ui.CH18,
	ui.CH19,ui.CH20 ,ui.CH21 ,ui.CH22 ,ui.CH23 ,ui.CH24 };
	for (int i=0;i<24;i++)
	{
		all_chncheckBox[i]->setCheckState(Qt::Unchecked);
	}

	return;
}

/*************************************************
   选中通道槽函数
***************************************************/
void HectoBioWindow::on_selectChnOK_btn() {
	//
	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("未检测到输入设备,请连接设备"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("未检测到输入设备,请连接设备"));
		//init_circuit(circuit_set);
		return;
	}

	QCheckBox* all_chncheckBox[24] = { ui.CH1,ui.CH2 ,ui.CH3 ,ui.CH4 ,ui.CH5 ,ui.CH6,
	ui.CH7,ui.CH8 ,ui.CH9 ,ui.CH10 ,ui.CH11 ,ui.CH12,
	ui.CH13,ui.CH14 ,ui.CH15 ,ui.CH16 ,ui.CH17 ,ui.CH18,
	ui.CH19,ui.CH20 ,ui.CH21 ,ui.CH22 ,ui.CH23 ,ui.CH24 };

	int count = 0;
	selected_chn_sum = 0;
	for (auto tmp : all_chncheckBox)
	{

		if (tmp->checkState() == Qt::Checked)
		{
			selectedChn[count] = true;
			selected_chn_sum += 1;
		}
		else
		{
			selectedChn[count] = false;
		}
		count++;
	}
	if (selected_chn_sum > 0)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("当前选中通道为："));
		QString selectchn_str = "";
		for (int i=0;i<24;i++)
		{
			if (selectedChn[i])
			{
				selectchn_str += QString::number(i)+" ";
				
			}
		}
		ui.textBrowser->append(selectchn_str);
		ui.textBrowser->append(QString::fromLocal8Bit("请进行下一步"));
	}
	else
	{
		ui.textBrowser->append(QString::fromLocal8Bit("请选择通道,然后进行下一步"));
	}

	return;
}

/*************************************************
   //采集参数功能槽函数
***************************************************/
void HectoBioWindow::on_acqparaOK_btn() {

	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("未检测到输入设备,请连接设备"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("未检测到输入设备,请连接设备"));
		return;
	}

	trig_mode = ui.trigModel->currentIndex();
	trig_source = ui.trigSource->currentIndex();
	sample_freq = ui.sampleFreq->text().toInt();
	trig_level = ui.trigLevel->text().toInt();
	trig_length = ui.trigLength->text().toInt();
	trig_delay = ui.delayTime->text().toInt();

	QMessageBox::warning(this, QString::fromLocal8Bit("请确认采集参数无误"), QString::fromLocal8Bit("请确认采集参数无误"), QMessageBox::Yes);

	ui.textBrowser->append(QString::fromLocal8Bit("按下采集参数确定按钮\n触发模式："));
	ui.textBrowser->append((ui.trigModel->itemText(trig_mode)));
	ui.textBrowser->append(QString::fromLocal8Bit("触发源："));
	ui.textBrowser->append((ui.trigSource->itemText(trig_source)));
	ui.textBrowser->append(QString::fromLocal8Bit("采样频率："));
	ui.textBrowser->append(QString::number(sample_freq));
	ui.textBrowser->append(QString::fromLocal8Bit("触发电平："));
	ui.textBrowser->append(QString::number(trig_level));
	ui.textBrowser->append(QString::fromLocal8Bit("触发长度："));
	ui.textBrowser->append(QString::number(trig_length));
	ui.textBrowser->append(QString::fromLocal8Bit("延时："));
	ui.textBrowser->append(QString::number(trig_delay));

	return;
}


/*************************************************
  //连续采集功能槽函数
***************************************************/
void HectoBioWindow::on_startCtnAcq_btn() {
	//开始连续采集之前需判断设备是否在线
	if (1)
	{
		if (!device_flag)
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("未检测到输入设备,请连接设备"), QMessageBox::Yes);
			ui.textBrowser->append(QString::fromLocal8Bit("未检测到输入设备,请连接设备"));
			return;
		}
	//是否选择通道  如果没有选择通道  报错
		if (selected_chn_sum == 0)
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("请候选检测通道"), QMessageBox::Yes);
			ui.textBrowser->append(QString::fromLocal8Bit("请勾选检测通道"));
			return;
		}
	}
	
	/*****************************************************
	* //如果选择通道，读取AD数据
	则调用信号显示功能部分（显示，根据传递的通道数显示）
	******************************************************/
	ui.textBrowser->append(QString::fromLocal8Bit("连续采集模式"));
	for (int i=0;i<24;i++)
	{
		para_init.lEnCh[i] = selectedChn[i];
	}
	para_init.ADFREQ = sample_freq;
	para_init.TriggerDelay = trig_delay;
	para_init.TriggerLength = trig_length;
	para_init.TriggerLevel = trig_level;
	para_init.TriggerMode = trig_mode;
	para_init.TriggerSource = trig_source;

	ctn_display = ui.ctnscqDisplay->checkState();
	

 	if (USB2069_InitAD(linkdevice, &para_init))
	{
		;
		ui.textBrowser->append(QString::fromLocal8Bit("初始化AD成功"));
	}
	//禁止采集参数
	setTrigPara(false);

	//记得添加数据更新和图像实时显示
	//放在线程中
	if (para_init.TriggerMode==TRIG_SRC_SOFT)
	{
		softTrig = true;
		samcnt = trig_length * TRIG_UNIT;
	}
	else
	{
		samcnt = read_data_length;
	}
	samcnt *= selected_chn_sum;
	if (samcnt>READ_MAX_LEN)
	{
		samcnt = READ_MAX_LEN;
	}
	
	int i = 0;
	//重新分配缓冲区
	if (dataBuff)
	{
		for (i=0;i<MAX_SEGMENT;i++ )
		{
			//delete[] dataBuff[i];
			dataBuff[i] = NULL;
		}
	}
	//多缓冲
	for (i=0;i<MAX_SEGMENT;i++)
	{
		dataBuff[i] = new USHORT[samcnt];//每个缓冲区存放一个通道AD数据
	}
	//初始化多缓冲标志
	for (i=0;i<MAX_SEGMENT;i++)
	{
		NewSegmentData[i] = FALSE;
	}
	ReadIndex = 0;

	ADRun = TRUE;
	display_flag = ui.ctnscqDisplay->isChecked();
	save_flag = !display_flag;
	//创建读取线程和显示线程
	if (ADRun && save_flag)
	{
		m_readThread = new ReadThread(this);
		//修改传递信号 需要传递设备号和通道号
		connect(this, SIGNAL(startReadThread(QString,HANDLE&)), m_readThread, SLOT(recvMegFromMain(QString,HANDLE&)));
		//m_readThread->start();
		//
		//connect(this->m_readThread, SIGNAL(finished()), this, SLOT(FinishThread()));
		emit startReadThread(QString("你好，保存子线程"),linkdevice);
		m_readThread->start();
	}
	else if (ADRun && display_flag)
	{
		//初始化signalPlot
		//on_disp_btn();
		//displayThread = new DisplayThread(this);
		displayUpdateTimer = new QTimer(this);
		connect(displayUpdateTimer, &QTimer::timeout, this, &HectoBioWindow::timerUpdate);
		displayUpdateTimer->start(1000);//1s更新一次
		//disp_show_flag = true;
		//dispay_timer = this->startTimer(100);
		//curve->setSamples(time, val, sam);
		curve->attach(ui.signalPlot);
		ui.signalPlot->replot();

		m_displayThread = new DisplayThread(this);
		connect(this, SIGNAL(startDisplayThread(QString,HANDLE&)), m_displayThread, SLOT(recvMegFromMain(QString,HANDLE&)));
		//
		m_displayThread->start();
		//connect(this->m_displayThread, SIGNAL(finished()), this, SLOT(FinishThread()));
		emit startDisplayThread(QString("你好，显示子线程"),linkdevice);
		m_displayThread->start();
		
	}
	

	return;
}

void HectoBioWindow::timerUpdate() {
	double* display_time = new double[read_data_length];
	double* display_val = new double[read_data_length];
	PUSHORT inBuffer = new USHORT[read_data_length];
	bool status=USB2069_ReadAD(linkdevice, inBuffer, read_data_length);

	if (status)
	{
		for (int i = 0; i < sample_freq; i++)
		{
			display_time[i] = i;
			double level = double(inBuffer[i] - 32768) * (1.0 / 32768.0) * 10;
			level = (level * (double)1000 + 0.5977) / 0.9854;
			display_val[i] = level;

			qDebug() << inBuffer[i] << level << "mV" << endl;
		}


		curve->setSamples(display_time, display_val, sample_freq);
		//curve->setSamples(time, displayDataBuff[CurrentIndex], 500);
		ui.signalPlot->replot();
	}
	else
	{
		return;
	}
	
	delete[] display_val;
	delete[] display_time;
}


void HectoBioWindow::readAD_data() {
	PSHORT inBuffer = NULL;
	ULONG status = FALSE;

	//放进线程里面
	bool stutas = USB2069_ReadAD(linkdevice, dataBuff[0], read_data_length);

	/*const char* savefile = "C:\\Users\\LJ\\Desktop\\信号文件\\CH1.h5";
	FILE* fp = fopen(savefile, "w");

	fwrite(dataBuff[0], sizeof(unsigned short), 2400, fp);
	ofstream out(savefile);*/

	double data[240];
	for (int i = 0; i < 240; i++)
	{
		data[i] = (dataBuff[0][i] - ceil(65535 / 2)) * (1 / (ceil(65535 / 2))) * 10;
		data[i] = data[i] * 1000;
		data[i] = (data[i] + 0.5977) / 0.9854;
	}


}
	


void HectoBioWindow::displayAD_data() {


	//放进线程里面
	bool stutas = USB2069_ReadAD(linkdevice, dataBuff[0], read_data_length);

	/*const char* savefile = "C:\\Users\\LJ\\Desktop\\信号文件\\CH1.h5";
	FILE* fp = fopen(savefile, "w");

	fwrite(dataBuff[0], sizeof(unsigned short), 2400, fp);
	ofstream out(savefile);*/

	double data[240];
	for (int i = 0; i < 240; i++)
	{
		data[i] = (dataBuff[0][i] - ceil(65535 / 2)) * (1 / (ceil(65535 / 2))) * 10;
		data[i] = data[i] * 1000;
		data[i] = (data[i] + 0.5977) / 0.9854;
	}

	if (stutas)
	{
		saveDataAsText(QString::fromLocal8Bit("C:\\Users\\LJ\\Desktop\\信号文件\\CH1.h5"), data);
		//saveDataAsStream(QString::fromLocal8Bit("C:\\Users\\LJ\\Desktop\\信号文件\\CH1.h5"), data);
		//saveDataAsHdf5("C:\\Users\\LJ\\Desktop\\信号文件\\CH1.h5", data);
	}
}




/*************************************************
   //停止连续采集槽函数
***************************************************/
void HectoBioWindow::on_stopCtnAcq_btn() {
	if (1)
	{
		if (!device_flag)
			{
				QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("未检测到输入设备,请连接设备"), QMessageBox::Yes);
				ui.textBrowser->append(QString::fromLocal8Bit("未检测到输入设备,请连接设备"));
				return;
			}
			//如果没有选择通道  报错
			if (selected_chn_sum == 0)
			{
				QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("请候选检测通道"), QMessageBox::Yes);
				ui.textBrowser->append(QString::fromLocal8Bit("请勾选检测通道"));
				return;
			}
			//如果还没开始检测就点结束，报错启动检测，或pass
			if (USB2069_StopAD(linkdevice, devNum))
			{
				ui.textBrowser->append(QString::fromLocal8Bit("关闭AD成功"));
			}
	}
	
	if (ADRun && save_flag)
	{
		connect(this, SIGNAL(stopReadThread(bool)), m_readThread, SLOT(recvStopSignal(bool)));
		connect(m_readThread, SIGNAL(readFinish(QString)), this, SLOT(test_readthread(QString)));
		save_flag = false;
		display_flag = false;
		m_readThread->quit();
		m_readThread->destroyed();
		emit stopReadThread(true);
		emit m_readThread->readFinish("save end");
		//
	}
	else if (ADRun && display_flag)
	{
		
		connect(this, SIGNAL(stopDisplayThread(bool)), m_displayThread, SLOT(recvStopSignal(bool)));
		connect(m_displayThread, SIGNAL(displayFinish(QString)), this, SLOT(test_displaythread(QString)));
		
		save_flag = false;
		display_flag = false;
		m_displayThread->quit();
		m_displayThread->destroyed();
		emit stopDisplayThread(true);
		emit m_displayThread->displayFinish("display end");
		//this->killTimer(dispay_timer);
		displayUpdateTimer->stop();
	}
	
	//emit readFinish(QString("have benn displayed"));
	//QThread::msleep(15);
	//emit displayFinish(QString("have benn displayed"));
	//QThread::msleep(15);
	
	//使能采集参数按钮
	setTrigPara(true);

	return;
}

void HectoBioWindow::setTrigPara(bool set) {
	//通道
	QCheckBox* all_chncheckBox[24] = { ui.CH1,ui.CH2 ,ui.CH3 ,ui.CH4 ,ui.CH5 ,ui.CH6,
	ui.CH7,ui.CH8 ,ui.CH9 ,ui.CH10 ,ui.CH11 ,ui.CH12,
	ui.CH13,ui.CH14 ,ui.CH15 ,ui.CH16 ,ui.CH17 ,ui.CH18,
	ui.CH19,ui.CH20 ,ui.CH21 ,ui.CH22 ,ui.CH23 ,ui.CH24 };
	for (int i = 0; i < 24; i++)
	{
		all_chncheckBox[i]->setEnabled(set);
	}
	ui.chnselectAll->setEnabled(set);
	ui.chncancelAll->setEnabled(set);
	ui.selectchnOK->setEnabled(set);

	//连续采集参数
	ui.trigModel->setEnabled(set);
	ui.trigSource->setEnabled(set);
	ui.trigLength->setEnabled(set);
	ui.sampleFreq->setEnabled(set);
	ui.trigLevel->setEnabled(set);
	ui.delayTime->setEnabled(set);
	ui.acqparaBtnOK->setEnabled(set);
	//连续采集和单次采集互斥
	ui.ctnscqStart->setEnabled(set);
	ui.ctnscqEnd->setEnabled(!set);
	ui.singleacqEnd->setEnabled(set);
	ui.ctnscqDisplay->setEnabled(set);
	ui.ctnacqMutliSelcet->setEnabled(set);
	ui.singleacqTime->setEnabled(set);
}

/*************************************************
   //单次采集槽函数
***************************************************/
void HectoBioWindow::on_singleAcq_btn() {
	
	ui.ctnscqStart->setEnabled(false);
	ui.ctnscqEnd->setEnabled(false);
	ui.singleacqEnd->setEnabled(false);

	if (0)
	{
		if (!device_flag)
			{
				QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("未检测到输入设备,请连接设备"), QMessageBox::Yes);
				ui.textBrowser->append(QString::fromLocal8Bit("未检测到输入设备,请连接设备"));
				return;
			}
			//如果没有选择通道  报错
			if (selected_chn_sum == 0)
			{
				QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("请候选检测通道"), QMessageBox::Yes);
				ui.textBrowser->append(QString::fromLocal8Bit("请勾选检测通道"));
				return;
			}
	}
	

	if (0)
	{
		//如果还没开始检测就点结束，报错启动检测，或pass
			acq_flag = SINGLE_ACQ;
			ui.textBrowser->append(QString::fromLocal8Bit("单次采集模式："));

			for (int i = 0; i < 24; i++)
			{
				para_init.lEnCh[i] = selectedChn[i];
			}
			para_init.ADFREQ = sample_freq;
			para_init.TriggerDelay = trig_delay;
			para_init.TriggerLength = trig_length;
			para_init.TriggerLevel = trig_level;
			para_init.TriggerMode = trig_mode;
			para_init.TriggerSource = trig_source;

			//读取数据长度，软件触发只读取一个触发长度
			if (para_init.TriggerMode==TRIG_SRC_SOFT)
			{
				samcnt = para_init.TriggerLength * TRIG_UNIT;
			}
			else
			{
				samcnt = (ULONG)(para_init.TriggerLength * para_init.ADFREQ);
			}
			samcnt *= selected_chn_sum;


			if (USB2069_InitAD(linkdevice, &para_init))
			{
				ui.textBrowser->append(QString::fromLocal8Bit("初始化AD成功"));
			}

			PUSHORT inBuffer = NULL;
			inBuffer = new USHORT[samcnt];
			readUSB(inBuffer, samcnt);

			LONG bufover=0;
			if (!USB2069_GetBufOver(linkdevice,&bufover))
			{
				ui.textBrowser->append(QString::fromLocal8Bit("读取缓存溢出位失败"));
			}
			if (bufover==1)
			{
				ui.textBrowser->append(QString::fromLocal8Bit("缓存溢出"));
			}


			//记得添加数据更新和图像实时显示
			/*Sleep(ui.singleacqTime->text().toInt());*/

			//结束采集
			if (USB2069_StopAD(linkdevice,devNum))
			{
				ui.textBrowser->append(QString::fromLocal8Bit("关闭AD成功"));
				goto savedata;
			}
		savedata:
			double* buf = new double[samcnt];
			int chcnt = 0;
			ULONG sscnt = samcnt / selected_chn_sum;
			for (int k = 0; k < 24; k++)
			{
				if (para_init.lEnCh[k])
				{
					LONGLONG sst = 0;
					//先取数据
					for (int j = 0; j < (int)sscnt; j++)
					{
						buf[j] = inBuffer[selected_chn_sum * j + chcnt];
						sst += (USHORT)(buf[j]);
					}

					char filename[100];

					sprintf(filename, "C:\\Users\\LJ\\Desktop\\信号文件\\CH%d.h5", (k + 1));

					saveDataAsHdf5(filename, buf);
				}
				chcnt++;

			}
	}
	
	//
	setTrigPara(false);


	return;
}

BOOL HectoBioWindow::readUSB(PUSHORT pBuf, int bufsize) {
	//一次读完samcnt长度数据，例如一个触发长度，如果samcnt大于READ_MAX_LEN，则分为多次读取
	ULONG DataOver = 0; //缓存区溢出指示
	ULONG rlen = 0;//每次读取长度
	ULONG alen = 0;//已经读取长度
	int rcnt = 0;//总共需要读取的次数
	if ((bufsize % READ_MAX_LEN) == 0)//如果读取的长度，刚好是最大允许读取长度的整数倍
		rcnt = (bufsize / READ_MAX_LEN);
	else
		rcnt = (bufsize / READ_MAX_LEN) + 1;
	//分多次读取，每次读最大长度是READ_MAX_LEN
	for (int i = 0; i < rcnt; i++)
	{
		if (i == (rcnt - 1))//如果是最后一次
		{
			if ((bufsize % READ_MAX_LEN) == 0)//如果刚好是整数倍
				rlen = READ_MAX_LEN;
			else
				rlen = bufsize % READ_MAX_LEN;
		}
		else
		{
			rlen = READ_MAX_LEN;
		}
		//读数 
		if (!USB2069_ReadAD(linkdevice, pBuf + alen, rlen))
		{
			ui.textBrowser->append(QString::fromLocal8Bit("读取数据失败"));
			
			break;
			return FALSE;
		}
		//判断数据是否溢出
		alen += rlen;
	}
	return TRUE;
}


/*************************************************
   //DA0-DA3功能槽函数
   4通道独立DA转换器，DA操作和AD相互独立
   DA：数据来自PC自己产生的模拟波形   AD：数据来自采集卡--->PC
***************************************************/

void HectoBioWindow::on_DA0_btn() {
	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("未检测到输入设备,请连接设备"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("未检测到输入设备,请连接设备"));
		return;
	}
	//如果没有选择通道  报错
	if (selected_chn_sum == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("请候选检测通道"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("请勾选检测通道"));
		return;
	}
	//选择正弦波或方波
	int da0index=0;
	da0index= ui.da0MutliSelect->currentIndex();
	long da0freq = da_freq;
	da0freq=ui.da0freq->text().toDouble();
	int da0point = da_sample;
	da0point= ui.da0samplepoint->text().toInt();
	int da0cycle = da_cycle;
	da0cycle=ui.da0cycle->text().toInt();
	bool da0enable = 0;
	da0enable= ui.da0start->checkState();
	bool da0enable_cycle = false;
	bool stopflag = false;



	PUSHORT data_buf = NULL;
	data_buf=new USHORT[read_data_length];
	
	if (da0cycle > 0)
	{
		da0enable_cycle = true;
	}

	bool ret_DA_flag = 0;

	ret_DA_flag=USB2069_SetDA(linkdevice, DA0, da0enable, &da0freq, da0enable_cycle, da0cycle, stopflag, data_buf, da0point);

	if (ret_DA_flag)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("DA0设置完毕"));
		
	}
	delete[] data_buf;


	return;

}
void HectoBioWindow::on_DA1_btn() {
	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("未检测到输入设备,请连接设备"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("未检测到输入设备,请连接设备"));
		return;
	}
	//如果没有选择通道  报错
	if (selected_chn_sum == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("请候选检测通道"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("请勾选检测通道"));
		return;
	}
	//选择正弦波或方波
	int da1index = 0;
	da1index = ui.da1MutliSelect->currentIndex();
	long da1freq = da_freq;
	da1freq = ui.da1freq->text().toDouble();
	int da1point = da_sample;
	da1point = ui.da1samplepoint->text().toInt();
	int da1cycle = da_cycle;
	da1cycle = ui.da1cycle->text().toInt();
	bool da1enable = 0;
	da1enable = ui.da1start->checkState();
	bool da1enable_cycle = false;
	bool stopflag = 1;
	PUSHORT data_buf = new USHORT[read_data_length];

	if (da1cycle > 0)
	{
		da1enable_cycle = true;
	}

	bool ret_DA_flag = USB2069_SetDA(linkdevice, DA1, da1enable, &da1freq, da1enable_cycle, da1cycle, stopflag, data_buf, da1point);
	if (ret_DA_flag)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("DA1设置完毕"));
	}
	delete[] data_buf;

	return;
}
void HectoBioWindow::on_DA2_btn() {
	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("未检测到输入设备,请连接设备"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("未检测到输入设备,请连接设备"));
		return;
	}
	//如果没有选择通道  报错
	if (selected_chn_sum == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("请候选检测通道"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("请勾选检测通道"));
		return;
	}
	//选择正弦波或方波
	int da2index = 0;
	da2index = ui.da2MutliSelect->currentIndex();
	long da2freq = da_freq;
	da2freq = ui.da2freq->text().toDouble();
	int da2point = da_sample;
	da2point = ui.da2samplepoint->text().toInt();
	int da2cycle = da_cycle;
	da2cycle = ui.da2cycle->text().toInt();
	bool da2enable = 0;
	da2enable = ui.da2start->checkState();
	bool da2enable_cycle = false;
	bool stopflag = 1;
	PUSHORT data_buf = new USHORT[read_data_length];

	USHORT offset_value = ui.da_offset_voltage->text().toInt();
	
	for (int i=0;i<read_data_length;i++)
	{
		data_buf[i] = offset_value;
	}

	if (da2cycle > 0)
	{
		da2enable_cycle = true;
	}

	bool ret_DA_flag = 0;
	ret_DA_flag = USB2069_SetDA(linkdevice, DA2, da2enable, &da2freq, da2enable_cycle, da2cycle, stopflag, data_buf, da2point);
	if (ret_DA_flag)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("DA2设置完毕"));
		ui.textBrowser->append(ui.da_offset_voltage->text());
	}
	delete[] data_buf;

	return;
}
void HectoBioWindow::on_DA3_btn() {
	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("未检测到输入设备,请连接设备"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("未检测到输入设备,请连接设备"));
		return;
	}
	//如果没有选择通道  报错
	if (selected_chn_sum == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("请候选检测通道"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("请勾选检测通道"));
		return;
	}
	//选择正弦波或方波
	int da3index = 0;
	da3index = ui.da3MutliSelect->currentIndex();
	long da3freq = da_freq;
	da3freq = ui.da3freq->text().toDouble();
	int da3point = da_sample;
	da3point = ui.da3samplepoint->text().toInt();
	int da3cycle = da_cycle;
	da3cycle = ui.da3cycle->text().toInt();
	bool da3enable = 0;
	da3enable = ui.da3start->checkState();
	bool da3enable_cycle = false;
	bool stopflag = 1;
	PUSHORT data_buf = new USHORT[read_data_length];

	if (da3cycle > 0)
	{
		da3enable_cycle = true;
	}

	bool ret_DA_flag = 0;
	ret_DA_flag = USB2069_SetDA(linkdevice, DA3, da3enable, &da3freq, da3enable_cycle, da3cycle, stopflag, data_buf, da3point);
	if (ret_DA_flag)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("DA3设置完毕"));
	}
	delete[] data_buf;

	return;
}

/*************************************************
   //零偏
//基线读取槽函数
***************************************************/
void HectoBioWindow::on_baselineRd_btn() {
	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("未检测到输入设备,请连接设备"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("未检测到输入设备,请连接设备"));
		return;
	}
	//如果没有选择通道  报错
	if (selected_chn_sum == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("请候选检测通道"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("请勾选检测通道"));
		return;
	}
	//选择通道后
	

	QString offset_text;
	
	long val[24] = { 0 };
	//为24个通道中选中的通道均设置零偏
	for (LONG i=0;i<24;i++)
	{
		
		if (selectedChn[i])
		{
			if (!USB2069_ADoffset(linkdevice, READOFFSET, i, &val[i]));
			{
				ui.textBrowser->append(QString::fromLocal8Bit("读取零偏失败"));
				return;
			}
		}
		
	}
	ui.textBrowser->append(QString::fromLocal8Bit("读取零偏成功"));
	
	for (int i=0;i<24;i++)
	{
		if (selectedChn[i])
		{
			char output[100];
			sprintf(output, "OF[%d]=%d\r\n", i, val[i]);
			offset_text += output;
		}
		
	}
	ui.textBrowser->append(offset_text);

	return;
}

/*************************************************
   //基线校正槽函数
***************************************************/

void HectoBioWindow::on_baselineRcv_btn() {
	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("未检测到输入设备,请连接设备"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("未检测到输入设备,请连接设备"));
		return;
	}
	//如果没有选择通道  报错
	if (selected_chn_sum == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("请候选检测通道"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("请勾选检测通道"));
		return;
	}

	
	
	long recover_offset;
	try
	{
		recover_offset = ui.baseline_input->text().toDouble();
		
		for (LONG i=0;i<24;i++)
		{
			if (selectedChn[i])
			{
				if (!USB2069_ADoffset(linkdevice, WRITEOFFSET, i, &recover_offset)) {
					ui.textBrowser->append(QString::fromLocal8Bit("写入零偏失败"));
					return;
				}
			}
			
		}
		ui.textBrowser->append(QString::fromLocal8Bit("写入零偏成功\r")+ui.baseline_input->text());
			

	}
	catch (...)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("输入校正偏移：double类型"));
	}

	return;
}


/*************************************************
  //输出信息
//保存输出信息
***************************************************/
void HectoBioWindow::on_saveoutinfo_btn() {
	QFileDialog* filedialog = new QFileDialog(this);
	filedialog->setWindowTitle("save file");
	filedialog->setDirectory(".");
	//filedialog->show();

	filedialog->setNameFilterDetailsVisible(true);
	filedialog->setNameFilter("Serials File(*.xml *.html *.json *.txt");

	QString save_name = filedialog->getSaveFileName();
	ui.textBrowser->append(save_name);

	if (save_name == "")
	{
		return;
	}

	QFile file(save_name);
	file.open(QIODevice::WriteOnly);
	QTextStream textstream(&file);
	QString str = ui.textBrowser->toPlainText();
	textstream << str;
	ui.textBrowser->clear();
	ui.textBrowser->append(str);
	QString Last_FileContent = str;
	file.close();

	return;
}

/*************************************************
   清空textBrower
***************************************************/
void HectoBioWindow::on_clearinfo_btn() {
	ui.textBrowser->clear();
}

/*************************************************
  //定时器
//随机产生初始信号，仅用于电路未到时测试
***************************************************/
void HectoBioWindow::timerEvent(QTimerEvent*) {
	//savetestSignal = new double[500];
	//USB2069_ReadAD(linkdevice, dataBuff[0], 500);
	//
	//double level;
	//for (int i = 0; i < 499; i++)
	//{
	//	level = double(dataBuff[0][i] - 32768) * (1.0 / 32768.0) * 10;
	//	level = (level * (double)1000 + 0.5977) / 0.9854;
	//	val[i] = val[i + 1];
	//	savetestSignal[i] = val[i];
	//}
	//val[499] = level;
	//qDebug() << level << "mV"<<endl;
	//curve->setSamples(time, val, 500);
	////curve->setSamples(time, displayDataBuff[CurrentIndex], 500);
	//ui.signalPlot->replot();
	double* display_time=new double[sample_freq];
	double* display_val = new double[sample_freq];
	USB2069_ReadAD(linkdevice, dataBuff[0], sample_freq);

	double level;
	for (int i = 0; i < sample_freq; i++)
	{
		display_time[i] = i;
		level = double(dataBuff[0][i] - 32768) * (1.0 / 32768.0) * 10;
		level = (level * (double)1000 + 0.5977) / 0.9854;
		display_val[i] = level;
		qDebug() << dataBuff[0][i] << "mV" << endl;
	}

	curve->setSamples(display_time, display_val, sample_freq);
	//curve->setSamples(time, displayDataBuff[CurrentIndex], 500);
	ui.signalPlot->replot();
	delete[] display_val;
	delete[] display_time;
	return;
}


/*************************************************
   信号显示子模块
   //信号显示槽函数
***************************************************/
void HectoBioWindow::on_disp_btn() {
	disp_show_flag = true;
	//ui.signaldispOpenGL->DrawWindowBackground();
	//QwtPlot* myplot = new QwtPlot("curve display");
	curve->setSamples(time, val, 500);
	curve->attach(ui.signalPlot);
	ui.signalPlot->replot();
	ui.textBrowser->append(QString::fromLocal8Bit("开始显示"));
}
/*************************************************
   //删除信号槽函数
***************************************************/
void HectoBioWindow::on_earse_btn() {
	if (!disp_show_flag)
	{
		return;
	}

	this->killTimer(timer);
	curve->setData(0);
	//ui.signalPlot->detachItems();
	ui.signalPlot->replot();
	ui.textBrowser->append(QString::fromLocal8Bit("清除显示"));

	return;

}
/*************************************************
  //暂停信号槽函数
***************************************************/
void HectoBioWindow::on_stop_btn() {
	if (!disp_show_flag)
	{
		return;
	}
	if (!disp_up_flag || !disp_down_flag)
	{
	}
	disp_stop_flag = disp_stop_flag ^ true;
	this->killTimer(timer);
	//ui.signalPlot->update();
	//ui.signalPlot->close();

	//ui.signalPlot->replot();
	ui.textBrowser->append(QString::fromLocal8Bit("暂定信号显示"));

	//暂停后可以放大，缩小等功能

	QwtPlotZoomer* zoomer = new QwtPlotZoomer(ui.signalPlot->canvas());
	//选框颜色设置为红色
	zoomer->setRubberBandPen(QColor(Qt::red));

	zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
	zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton);
	zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);

	zoomer->setEnabled(true);

	//delete内存
	for (int i = 0; i < MAX_SEGMENT; i++)
	{
		delete[] dataBuff[i];
	}

	return;

}

/*************************************************
   //加速信号读出槽函数
***************************************************/
void HectoBioWindow::on_hurry_btn() {
	if (!disp_show_flag)
	{
		return;
	}
	this->killTimer(timer);
	if (time_dur <= 0)
	{
		time_dur = time_dur;
	}
	else
	{
		time_dur = time_dur - 10;
	}

	timer = this->startTimer(time_dur);
	//ui.textBrowser->append("hurry up draw signal plot");

	std::string text = std::to_string(time_dur);

	ui.textBrowser->append(QString::fromLocal8Bit("当前显示速率：") + QString::fromStdString(text));

	return;
}

/*************************************************
  减速信号读出槽函数
***************************************************/
void HectoBioWindow::on_slow_btn() {
	if (!disp_show_flag)
	{
		return;
	}
	this->killTimer(timer);
	time_dur = time_dur + 10;
	timer = this->startTimer(time_dur);
	//ui.textBrowser->append("slow down draw signal plot");
	std::string text = std::to_string(time_dur);

	ui.textBrowser->append(QString::fromLocal8Bit("当前显示速率：")+QString::fromStdString(text));

	return;
}

/*************************************************
   更新AD数据，电路到之后测试
***************************************************/
bool HectoBioWindow::updateAD_data(PUSHORT data_buf) {

	//示例信号
	//随机生成信号
	for (int i = 0; i < 500; i++)
	{
		time[i] = i;
	}

	ui.signalPlot->setTitle(QwtText("SIGNAL PLOT"));
	ui.signalPlot->setAxisTitle(QwtPlot::yLeft, "current / mA");
	ui.signalPlot->setAxisTitle(QwtPlot::xBottom, "time / mS");

	if (data_buf)
	{
		for (int i = 0; i < 500; i++)
		{
			time[i] = data_buf[i];
		}
	}
	return true;
}

/*************************************************
   //保存信号槽函数
***************************************************/
void HectoBioWindow::save_testSignal_btn() {
	//保存暂停时显示之前的数据
	//保存格式为hdf5
	if (!disp_stop_flag)
	{
		return;
	}

	QFileDialog* filedialog = new QFileDialog(this);
	filedialog->setWindowTitle("save file");
	filedialog->setDirectory(".");
	//filedialog->show();

	filedialog->setNameFilterDetailsVisible(true);
	//filedialog->
	filedialog->setFileMode(QFileDialog::AnyFile);
	filedialog->setViewMode(QFileDialog::Detail);
	//filedialog->setNameFilter(tr("Serials File(*.xml *.html *.json *.txt"));

	QString save_name = filedialog->getSaveFileName(nullptr,nullptr,nullptr, QString(tr("All file(*.*)")));
	ui.textBrowser->append(save_name);

	//保存数据
	//可选保存格式
	//saveDataAsStream(save_name,savetestSignal);
	if (save_name.endsWith("txt"))
	{
		saveDataAsStream(save_name, savetestSignal);
	}
	else if (save_name.endsWith("h5"))
	{
		saveDataAsHdf5(save_name.toStdString().c_str(), savetestSignal);
	}
	
	return;
}

/*************************************************
   //保存信号格式：saveDataStream
   保存为二进制模式，以C++文件方式写
***************************************************/
void HectoBioWindow::saveDataAsStream(QString& save_name, double* savedata) {
	FILE* save_file = fopen(save_name.toStdString().c_str(), "w");

	for (int i = 0; i < 500; i++)
	{
		fwrite(&savedata[i], sizeof(double), 1, save_file);
	}

	fclose(save_file);

	return;

}

/*************************************************
   保存信号，以QTTextStream文件流写
***************************************************/
void HectoBioWindow::saveDataAsText(QString& save_name, double* savedata) {
	//可保存为hdf5文件
	QFile file(save_name);
	file.open(QIODevice::WriteOnly);

	QTextStream textstream(&file);

	for (size_t i = 0; i < 500; i++)
	{
		QString linestr = QString::number(savedata[i]);
		textstream << linestr;
	}


	file.close();

	return;
}

/*************************************************
  保存信号，以HDF5文件写
***************************************************/
void HectoBioWindow::saveDataAsHdf5(const char* save_name, double* savedata) {
	//可保存为hdf5文件
	//调用Hdf5Read类
	int row, col;
	row = 1;
	col = 500;
	hdf5_op = new Hdf5Read();

	hdf5_op->save_file(savedata, row, col, save_name, db_name);

	return;
}

void HectoBioWindow::test_readthread(QString line) {
	ui.textBrowser->append(line);
	
}

void HectoBioWindow::test_displaythread(QString line) {
	ui.textBrowser->append(line);
}

ReadThread::ReadThread(QObject* obj) :
	m_obj(obj) {
	linkdevice = 0;
	this->stop_flag = false;
}

//读取线程
void ReadThread::run() {
	//读取线程需要做的事
	//QString readf = QString::fromLocal8Bit("读取数据中...");
	//读取函数
	

	//读取完毕之后，发出读取完毕信号
	//emit readFinish(readf);

	//emit readFinish(QString("have been save!"));
	while (!stop_flag)
	{
		PUSHORT inBuffer = new USHORT[read_data_length];
		
		//qDebug() << "run in save thread" << endl;
		bool stutas=USB2069_ReadAD(linkdevice, inBuffer, read_data_length);
		double level = double(inBuffer[0] - 32768) * (1.0 / 32768.0) * 10;
		level = (level * (double)1000 + 0.5977) / 0.9854;
		qDebug() << level <<"mv"<< endl;
		QThread::msleep(100);
	}

}

void ReadThread::recvMegFromMain(QString line,HANDLE& linkdevice) {
	qDebug() << "save thread has recieve " << line.toStdString().c_str()<<endl;
	this->linkdevice = linkdevice;
	qDebug()<<linkdevice<<endl;
}

void ReadThread::recvStopSignal(bool stop_flag) {

	//this->wait();
	this->stop_flag = stop_flag;
	
	qDebug() << "stop save thread" << endl;
}


//显示线程
DisplayThread::DisplayThread(QObject* obj):
	m_obj(obj){
	//linkdevice = 0;
	/*SignalDisplay* showWindow = new SignalDisplay(nullptr);
	showWindow->show();*/
}

void DisplayThread::run() {
	//显示线程需要做的事   线程数据传递至qwtplotcurve？
	// 
	//QString displayf = QString::fromLocal8Bit("显示数据中...");
	////显示函数
	//

	////显示完毕（）之后，发出显示完毕信号
	//emit displayFinish(displayf);
	/*emit displayFinish(QString("have benn displayed"));
	QThread::msleep(15);*/

	
	while (!stop_flag)
	{
		//PUSHORT inBuffer = new USHORT[read_data_length];

		////qDebug() << "run in save thread" << endl;
		//bool stutas = USB2069_ReadAD(linkdevice, inBuffer, read_data_length);
		//int discnt = DISPLAY_CNT;
		//
		//double level = 0;
		//for (int i=0;i<discnt;i++)
		//{
		//	level = double(inBuffer[1*i+0] - 32768) * (1.0 / 32768.0) * 10;
		//	level = (level * 1000 + 0.5977) / 0.9854;
		//	
		//}
		//

		//qDebug() << level << "mv" << endl;
		//QThread::msleep(100);
		//发送事件给主窗口显示
		//emit--->传输数据

	}
}

void DisplayThread::recvMegFromMain(QString line,HANDLE& linkdevice) {
	qDebug() << "display thread has recieve" << line.toStdString().c_str() << endl;
	
}

void DisplayThread::recvStopSignal(bool stop_flag) {
	this->stop_flag = stop_flag;

	qDebug() << "stop display thread" << endl;
}
