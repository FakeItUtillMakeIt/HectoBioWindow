#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_HectoBioWindow.h"

#include <QLineEdit>
#include <qwt_plot.h>
#include <QWT/qwt.h>
#include <qwt_plot_curve.h>
#pragma comment(lib,"USB2069.lib")

#include <time.h>
#include "TimeSerialData.h"
#include "Hdf5Read.h"
//#include "Circuit.h"
#include "usb2069.h"
#include <QThread>

#define CONTINUE_ACQ 1
#define SINGLE_ACQ 2
#define DEFAULT_DEVICE_NUM 0
#define MAX_SEGMENT 5

//读数据线程
class ReadThread :public QThread
{
	Q_OBJECT
public:
	ReadThread(QObject* obj);

signals:
	void readFinish(QString line);
protected:
	void run() Q_DECL_OVERRIDE;
private:
	QObject* m_obj;
};


//显示线程
class DisplayThread :public QThread
{
	Q_OBJECT
public:
	DisplayThread(QObject* obj);
signals:
	void displayFinish(QString line);
protected:
	void run() Q_DECL_OVERRIDE;
private:
	QObject* m_obj;

};



class HectoBioWindow : public QMainWindow
{
    Q_OBJECT

public:
    HectoBioWindow(QWidget *parent = Q_NULLPTR);

	//默认设置连接一个设备
	bool device_flag;
	UCHAR devNum;
	HANDLE linkdevice;

	USB2069_PARA_INIT para_init;

	int selected_chn_sum = 0;
	int acq_flag = CONTINUE_ACQ;

	bool disp_show_flag = false;
	bool disp_stop_flag = false;
	bool disp_up_flag = false;
	bool disp_down_flag = false;

	bool read_AD_flag = false;

	const float DEFAULT_DPI = 192.0;
	float winDpiScale();

	void init_window();
	bool selectedChn[24];
	int trig_mode = 0;
	int trig_source = 0;
	int sample_freq = 24000;
	int trig_level = 100;
	int trig_length = 100;
	int trig_delay = 0;

	int da_freq = 1000;
	int da_sample = 1000;
	int da_cycle = 0;

	//DA
	INT DA0 = 0;
	INT DA1 = 1;
	INT DA2 = 2;
	INT DA3 = 3;

	//电路驱动类
	//Circuit* circuit_set;
	long read_data_length = 1024 * 100;
	bool softTrig = false;
	USHORT* dataBuff[MAX_SEGMENT];
	BOOL NewSegmentData[MAX_SEGMENT];
	int ReadIndex;//数据采集线程当前缓冲区索引号
	int CurrentIndex;//数据处理线程当前缓冲区索引号
	BOOL ADRun;//正在采集标志
	HANDLE hEvent;//事件
	int display_chn;//显示的通道
	ULONG discnt;//显示的点数
	ULONG samcnt;

	//显示读取线程
	ReadThread* m_readThread;
	DisplayThread* m_displayThread;

	

	//读取和显示数据
	bool ctn_display = false;
	void readAD_data();
	void displayAD_data();
	//更新显示的AD数据
	bool updateAD_data(PUSHORT data_buf);

	BOOL readUSB(PUSHORT pBuf, int bufsize);
	void setTrigPara(bool set);


	//数据保存类
	Hdf5Read* hdf5_op;
	const char* db_name = "Dataset";

signals:
	void startReadThread();
	void startDisplayThread();

private slots:
	//信号显示窗口槽函数
	void on_disp_btn();

	void on_earse_btn();
	void on_stop_btn();
	void on_hurry_btn();
	void on_slow_btn();

	//test
	void test_readthread(QString line);
	void test_displaythread(QString line);

private slots:
	//menu子窗口槽函数
	void on_compare_btn();
	void on_signalView_btn();

private slots:
	//通道子功能槽函数
	void on_selectAll_btn();
	void on_cancelAll_btn();
	void on_selectChnOK_btn();


	//采集参数功能槽函数
	void on_acqparaOK_btn();

	//连续采集功能槽函数
	void on_startCtnAcq_btn();
	void on_stopCtnAcq_btn();

	//单次采集槽函数
	void on_singleAcq_btn();

	//DA0-DA3功能槽函数
	void on_DA0_btn();
	void on_DA1_btn();
	void on_DA2_btn();
	void on_DA3_btn();

	void on_baselineRcv_btn();
	void on_baselineRd_btn();

	//输出信息
	void on_saveoutinfo_btn();
	void on_clearinfo_btn();

	//测试使用
	//保存
	void save_testSignal_btn();

private:
    Ui::HectoBioWindowClass ui;

	QLineEdit* text;
	void timerEvent(QTimerEvent*);
	double time[500];
	double val[500] = { 0 };
	USHORT* ad_data;

	QwtPlotCurve* curve = new QwtPlotCurve("signal");

	int timer = 0;
	int time_dur = 50;

	double* savetestSignal;


private:
	//电路驱动部分
	//void init_circuit(Circuit* circuit_set);
	void init_circuit();

	

	void saveDataAsStream(QString& save_name, double* savedata);
	void saveDataAsText(QString& save_name, double* savedata);
	void saveDataAsHdf5(const char* save_name, double* savedata);
};

