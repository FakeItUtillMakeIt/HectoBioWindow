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

using namespace std;

HectoBioWindow::HectoBioWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	//初始化界面
	this->init_window();
	/*int newWidth = this->width();
	int newHight = this->height();

	float scale = winDpiScale();
	newWidth = newWidth * scale;
	newHight = newHight * scale;
	this->setFixedWidth(newWidth);
	this->setFixedHeight(newHight);*/

	//初始化电路连接
	circuit_set = new Circuit();
	this->init_circuit(circuit_set);

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

	//比较窗口

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

//根据界面初始化电路部分
void HectoBioWindow::init_circuit(Circuit* circuit_set) {
	//初始化电路连接参数
	//如果无电路连接，则主界面可设为不可编辑（暂时调试阶段无设计）
	/*************************************************
		通道及其他参数根据界面选择进行更新和设置
	***************************************************/

	//先检查是否连接设备
	device_flag = circuit_set->judge_link_device();
	if (device_flag)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("已连接设备,设备号为："));
		ui.textBrowser->append(QString::number(device_flag));
	}
	else
	{
		ui.textBrowser->append(QString::fromLocal8Bit("未检测到输入设备,请连接设备"));
		return;
	}

	for (int i = 0; i < 24; i++)
	{
		circuit_set->channel[i] = selectedChn[i];
	}
	//采集参数的确定必须要等按采集参数模块的确定按钮
	circuit_set->trig_mode = trig_mode;
	circuit_set->trig_source = trig_source;
	circuit_set->sample_freq = sample_freq;
	circuit_set->trig_level = trig_level;
	circuit_set->trig_length = trig_length;
	circuit_set->delay = trig_delay;

	bool flag = circuit_set->init_start_AD_device();

	char* ret_info = circuit_set->ret_flag;
	ui.textBrowser->append(QString::fromLocal8Bit(ret_info));


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
	//触发源  触发源初始化为外正沿触发
	ui.trigSource->setEditable(false);

	ui.trigSource->addItem(QString::fromLocal8Bit("外正沿触发"));
	ui.trigSource->addItem(QString::fromLocal8Bit("外负沿触发"));
	ui.trigSource->addItem(QString::fromLocal8Bit("边沿触发"));
	//采样率  采样率初始化为0
	ui.sampleFreq->setText(QString::number(0));
	//触发电平 触发电平初始化为0
	ui.trigLevel->setText(QString::number(0));
	//触发长度
	ui.trigLength->setText(QString::number(0));
	//延时

	ui.delayTime->setText(QString::number(0));

	//连续采集
	//显示
	ui.ctnscqDisplay->setCheckable(true);
	ui.ctnscqDisplay->setCheckState(Qt::Unchecked);
	//触发模式
	ui.ctnacqMutliSelcet->setMaxVisibleItems(10);
	ui.ctnacqMutliSelcet->addItem(QString::fromLocal8Bit("外正沿触发"));
	ui.ctnacqMutliSelcet->addItem(QString::fromLocal8Bit("外负沿触发"));
	ui.ctnacqMutliSelcet->addItem(QString::fromLocal8Bit("边沿触发"));

	//单次采集
	//采集时长
	ui.singleacqTime->setText(QString::number(0));

	/********************************
	//DA部分暂时不清楚是用来做什么的？
	*********************************/
	//DA0

	ui.da0MutliSelect->setMaxVisibleItems(10);
	ui.da0MutliSelect->addItem(QString::fromLocal8Bit("正弦波"));
	ui.da0MutliSelect->addItem(QString::fromLocal8Bit("方波"));

	ui.da0freq->setText(QString::number(0));
	ui.da0samplepoint->setText(QString::number(0));
	ui.da0cycle->setText(QString::number(0));
	ui.da0start->setCheckable(true);
	ui.da0start->setCheckState(Qt::Unchecked);

	//DA1

	ui.da1MutliSelect->setMaxVisibleItems(10);
	ui.da1MutliSelect->addItem(QString::fromLocal8Bit("正弦波"));
	ui.da1MutliSelect->addItem(QString::fromLocal8Bit("方波"));

	ui.da1freq->setText(QString::number(0));
	ui.da1samplepoint->setText(QString::number(0));
	ui.da1cycle->setText(QString::number(0));
	ui.da1start->setCheckable(true);
	ui.da1start->setCheckState(Qt::Unchecked);
	//DA2

	ui.da2MutliSelect->setMaxVisibleItems(10);
	ui.da2MutliSelect->addItem(QString::fromLocal8Bit("正弦波"));
	ui.da2MutliSelect->addItem(QString::fromLocal8Bit("方波"));

	ui.da2freq->setText(QString::number(0));
	ui.da2samplepoint->setText(QString::number(0));
	ui.da2cycle->setText(QString::number(0));
	ui.da2start->setCheckable(true);
	ui.da2start->setCheckState(Qt::Unchecked);
	//DA3

	ui.da3MutliSelect->setMaxVisibleItems(10);
	ui.da3MutliSelect->addItem(QString::fromLocal8Bit("正弦波"));
	ui.da3MutliSelect->addItem(QString::fromLocal8Bit("方波"));

	ui.da3freq->setText(QString::number(0));
	ui.da3samplepoint->setText(QString::number(0));
	ui.da3cycle->setText(QString::number(0));
	ui.da3start->setCheckable(true);
	ui.da3start->setCheckState(Qt::Unchecked);


	ui.baseline_input->setText(QString::fromLocal8Bit("输入校正偏移"));

	//界面适配
	//通道部分
	/*ui.channelGroup->setGeometry(ui.channelGroup->x()* scale, ui.channelGroup->y()* scale,
		ui.channelGroup->width()* scale, ui.channelGroup->height()* scale);
	ui.gridGroupBox_3->setGeometry(ui.gridGroupBox_3->x()*scale,ui.gridGroupBox_3->y()*scale,
		ui.gridGroupBox_3->width()*scale,ui.gridGroupBox_3->height()*scale);

	ui.gridGroupBox_2->setGeometry(ui.gridGroupBox_2->x()*scale, ui.gridGroupBox_2->y()*scale,
		ui.gridGroupBox_2->width()* scale, ui.gridGroupBox_2->height()* scale);
	ui.gridGroupBox_4->setGeometry(ui.gridGroupBox_4->x()* scale, ui.gridGroupBox_4->y()* scale,
		ui.gridGroupBox_4->width()* scale, ui.gridGroupBox_4->height()* scale);

	QCheckBox* all_chncheckBox[24] = { ui.CH1,ui.CH2 ,ui.CH3 ,ui.CH4 ,ui.CH5 ,ui.CH6,
	ui.CH7,ui.CH8 ,ui.CH9 ,ui.CH10 ,ui.CH11 ,ui.CH12,
	ui.CH13,ui.CH14 ,ui.CH15 ,ui.CH16 ,ui.CH17 ,ui.CH18,
	ui.CH19,ui.CH20 ,ui.CH21 ,ui.CH22 ,ui.CH23 ,ui.CH24 };



	selected_chn_sum = 0;
	for (auto tmp : all_chncheckBox)
	{
		tmp->setGeometry(tmp->x()*scale,tmp->y()*scale,tmp->width()*scale,tmp->height()*scale);
		tmp->setBaseSize(tmp->size()*scale);
		tmp->setIconSize(tmp->iconSize() * scale);
		tmp->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
	}
	ui.channelGroup->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);



	ui.trigModel->setFixedSize(ui.trigModel->width()* scale, ui.trigModel->height()* scale);
	ui.trigSource->setFixedSize(ui.trigSource->width()* scale, ui.trigSource->height()* scale);
	ui.sampleFreq->setFixedSize(ui.sampleFreq->width()* scale, ui.sampleFreq->height()* scale);
	ui.trigLevel->setFixedSize(ui.trigLevel->width()* scale, ui.trigLevel->height()* scale);
	ui.trigLength->setFixedSize(ui.trigLength->width()* scale, ui.trigLength->height()* scale);
	ui.delayTime->setFixedSize(ui.delayTime->width()* scale, ui.delayTime->height()* scale);
	ui.ctnscqDisplay->setFixedSize(ui.ctnscqDisplay->width()* scale, ui.ctnscqDisplay->height()* scale);
	ui.ctnacqMutliSelcet->setFixedSize(ui.ctnacqMutliSelcet->width()* scale, ui.ctnacqMutliSelcet->height()* scale);
	ui.singleacqTime->setFixedSize(ui.singleacqTime->width()* scale, ui.singleacqTime->height()* scale);
	ui.da0MutliSelect->setFixedSize(ui.da0MutliSelect->width()* scale, ui.da0MutliSelect->height()* scale);
	ui.da1MutliSelect->setFixedSize(ui.da1MutliSelect->width()* scale, ui.da1MutliSelect->height()* scale);
	ui.da2MutliSelect->setFixedSize(ui.da2MutliSelect->width()* scale, ui.da2MutliSelect->height()* scale);
	ui.da3MutliSelect->setFixedSize(ui.da3MutliSelect->width()* scale, ui.da3MutliSelect->height()* scale);

	ui.baseline_input->setFixedSize(ui.baseline_input->width()* scale, ui.baseline_input->height()* scale);


	*/

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

	QPixmap pmap(":/HectoBioWindow/res/DNA.png");

	QPalette plt;

	plt.setBrush(cmpwindow->backgroundRole(), QBrush(pmap));

	cmpwindow->setPalette(plt);
	cmpwindow->setMask(pmap.mask());
	//pmap.createMaskFromColor(Qt::color0);

	cmpwindow->setAutoFillBackground(false);

	cmpwindow->show();
}

/*************************************************
   林立安提出的打分功能
***************************************************/
//打分界面槽函数
void HectoBioWindow::on_signalView_btn() {
	signaView* signalviewWindow = new signaView();
	signalviewWindow->setWindowTitle(QString::fromLocal8Bit("信号打分（供内部实验人员操作）"));
	signalviewWindow->show();
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
}

/*************************************************
   通道全部取消选择
***************************************************/
void HectoBioWindow::on_cancelAll_btn() {
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
}

/*************************************************
   选中通道槽函数
***************************************************/
void HectoBioWindow::on_selectChnOK_btn() {

	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("未检测到输入设备,请连接设备"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("未检测到输入设备,请连接设备"));
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
		ui.textBrowser->append(QString::fromLocal8Bit("请进行下一步"));
	}
	else
	{
		ui.textBrowser->append(QString::fromLocal8Bit("请选择通道,然后进行下一步"));
	}

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

}


/*************************************************
  //连续采集功能槽函数
***************************************************/
void HectoBioWindow::on_startCtnAcq_btn() {
	//开始连续采集之前需判断设备是否在线
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
	/*****************************************************
	* //如果选择通道，读取AD数据
	则调用信号显示功能部分（显示，根据传递的通道数显示）
	******************************************************/
	acq_flag = CONTINUE_ACQ;
	PUSHORT data_buf = NULL;
	//data_buf 为实时数据
	//从电路输出直接读
	data_buf = new USHORT[circuit_set->read_data_length];
	bool circuit_read_flag = circuit_set->read_AD(data_buf, circuit_set->read_data_length);

	//更新显示区信号  判断AD读取是否成功
	if (!circuit_read_flag)
	{
		ui.textBrowser->append(QString::fromLocal8Bit(circuit_set->ret_flag));
		circuit_set->stop_AD_device();
		return;
	}
	updateAD_data(data_buf);

}

/*************************************************
   //停止连续采集槽函数
***************************************************/
void HectoBioWindow::on_stopCtnAcq_btn() {
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
}

/*************************************************
   //单次采集槽函数
***************************************************/
void HectoBioWindow::on_singleAcq_btn() {
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
	acq_flag = SINGLE_ACQ;

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
	int da0index = ui.da0MutliSelect->currentIndex();
	double da0freq = ui.da0freq->text().toDouble();
	int da0point = ui.da0samplepoint->text().toInt();
	int da0cycle = ui.da0cycle->text().toInt();
	bool da0enable = ui.da0start->checkState();
	bool da0enable_cycle = false;
	bool stopflag = 1;
	PUSHORT data_buf = new USHORT[circuit_set->read_data_length];

	if (da0cycle > 0)
	{
		da0enable_cycle = true;
	}

	bool ret_DA_flag = circuit_set->set_DA(DA0, da0enable, da0freq, da0enable_cycle, da0cycle, stopflag, data_buf, da0point);
	if (ret_DA_flag)
	{

	}
	delete[] data_buf;

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
	int da1index = ui.da1MutliSelect->currentIndex();
	double da1freq = ui.da1freq->text().toDouble();
	int da1point = ui.da1samplepoint->text().toInt();
	int da1cycle = ui.da1cycle->text().toInt();
	bool da1enable = ui.da1start->checkState();
	bool da1enable_cycle = false;
	bool stopflag = 1;
	PUSHORT data_buf = new USHORT[circuit_set->read_data_length];

	if (da1cycle > 0)
	{
		da1enable_cycle = true;
	}

	bool ret_DA_flag = circuit_set->set_DA(DA1, da1enable, da1freq, da1enable_cycle, da1cycle, stopflag, data_buf, da1point);
	if (ret_DA_flag)
	{

	}
	delete[] data_buf;

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
	int da2index = ui.da2MutliSelect->currentIndex();
	double da2freq = ui.da2freq->text().toDouble();
	int da2point = ui.da2samplepoint->text().toInt();
	int da2cycle = ui.da2cycle->text().toInt();
	bool da2enable = ui.da2start->checkState();
	bool da2enable_cycle = false;
	bool stopflag = 1;
	PUSHORT data_buf = new USHORT[circuit_set->read_data_length];

	if (da2cycle > 0)
	{
		da2enable_cycle = true;
	}

	bool ret_DA_flag = circuit_set->set_DA(DA2, da2enable, da2freq, da2enable_cycle, da2cycle, stopflag, data_buf, da2point);
	if (ret_DA_flag)
	{

	}
	delete[] data_buf;

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
	int da3index = ui.da3MutliSelect->currentIndex();
	double da3freq = ui.da3freq->text().toDouble();
	int da3point = ui.da3samplepoint->text().toInt();
	int da3cycle = ui.da3cycle->text().toInt();
	bool da3enable = ui.da3start->checkState();
	bool da3enable_cycle = false;
	bool stopflag = 1;
	PUSHORT data_buf = new USHORT[circuit_set->read_data_length];

	if (da3cycle > 0)
	{
		da3enable_cycle = true;
	}

	bool ret_DA_flag = circuit_set->set_DA(DA3, da3enable, da3freq, da3enable_cycle, da3cycle, stopflag, data_buf, da3point);
	if (ret_DA_flag)
	{

	}
	delete[] data_buf;

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
	double recover_offset;
	try
	{
		recover_offset = ui.baseline_input->text().toDouble();
		//ui.textBrowser->append(recover_offset);
		char* input_text = (char*)malloc(sizeof(char) * 100);
		sprintf(input_text, "写入零偏：%f", recover_offset);
		circuit_set->recover_ADoffset(recover_offset);
		ui.textBrowser->append(QString::fromLocal8Bit(input_text));
	}
	catch (...)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("输入校正偏移：double类型"));
	}

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
	savetestSignal = new double[500];

	for (int i = 0; i < 499; i++)
	{
		val[i] = val[i + 1];
		savetestSignal[i] = val[i];
	}
	val[499] = qrand() % 500;
	curve->setSamples(time, val, 500);
	ui.signalPlot->replot();


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
	ui.textBrowser->append("now start draw signal plot");
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
	ui.textBrowser->append("eraser signal plot");
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
	ui.textBrowser->append("stop signal plot");

	//暂停后可以放大，缩小等功能

	QwtPlotZoomer* zoomer = new QwtPlotZoomer(ui.signalPlot->canvas());
	//选框颜色设置为红色
	zoomer->setRubberBandPen(QColor(Qt::red));

	zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
	zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton);
	zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);

	zoomer->setEnabled(true);
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

	ui.textBrowser->append(QString::fromStdString(text));

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

	ui.textBrowser->append(QString::fromStdString(text));
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

	if (save_name == "")
	{
		return;
	}
	//保存数据
	//可选保存格式
	//saveDataAsStream(save_name,savetestSignal);

	saveDataAsHdf5(save_name.toStdString().c_str(), savetestSignal);

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
}