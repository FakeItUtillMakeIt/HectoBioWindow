//HectoBioWindow V1.0
// Using QT GUI 
//include header file
#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_HectoBioWindow.h"

#include <QLineEdit>
#include <qwt_plot.h>
#include <QWT/qwt.h>
#include <qwt_plot_curve.h>
#include "SignalDisplay.h"
//Using third dynamic library
//AD Card Device Driver Library
#pragma comment(lib,"USB2069.lib")

#include <time.h>
#include "TimeSerialData.h"
#include "Hdf5Read.h"
//#include "Circuit.h"
#include "usb2069.h"
#include <QThread>
#include <QVector>
#include <QTimer>
#include "qmath.h"
#include "qwt_point_data.h"
#include <QDateTime>
#include "QMessageBox"
#include <string>
#include "QString"
//QT Third Library
//QWTPLOT
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
#include <QFile>
#include <QTextStream>
#include <windows.h>
#include "stdio.h"
#include <iostream>
#include <fstream>


using namespace std;

#define DISPLAY_MOTHOD0 0
#define DISPLAY_MOTHOD1 1


#define CONTINUE_ACQ 1
#define SINGLE_ACQ 2
#define DEFAULT_DEVICE_NUM 0
#define MAX_SEGMENT 5
#define DISPLAY_CNT 500
//#define READ_DATA_LENGTH 1024 * 10

static int DISPLAY_PLOT_CNT = 1024 * 100;
static int read_length_base = 1024;
static int update_point_length = 1000;

static int READ_DATA_LENGTH = 1024 * 100;
static QVector<double> display_xs;
static QVector<double> display_ys;
static QVector<double> factDisplayData;
static QwtPointArrayData* display_xy_data;
static int displaySample_freq;
static int update_cycle;
static int data_offset = 0;



//读数据线程
class ReadThread :public QThread
{
	Q_OBJECT
public:
	ReadThread(QObject* obj);
	HANDLE linkdevice;
	USHORT* dataBuff[MAX_SEGMENT];
	BOOL NewSegmentData[MAX_SEGMENT];
	long read_data_length = READ_DATA_LENGTH;
	bool stop_flag=false;
	
	QString savefile_name=NULL;
	

signals:
	void readFinish(QString line);
protected:
	void run() Q_DECL_OVERRIDE;

public slots:
	void recvMegFromMain(QString savefile_name,HANDLE& linkdevice);
	void recvStopSignal(bool stop_flag);
private:
	QObject* m_obj;
};


//显示线程
class DisplayThread :public QThread
{
	Q_OBJECT
public:
	DisplayThread(QObject* obj);
	HANDLE linkdevice;
	/*USHORT* dataBuff[MAX_SEGMENT];
	BOOL NewSegmentData[MAX_SEGMENT];*/
	int read_data_length = READ_DATA_LENGTH;
	bool stop_flag = false;
	bool display_save_flag = false;
	FILE* save_handle = NULL;
	QString savefile_name = NULL;
	QFile* file;
	QTextStream* out;
	
	//const char* txt_file= this->savefile_name.toStdString().c_str();
	//ofstream out;

signals:
	void sendDisplayDataToWindow(QVector<double> displaydata);
	void displayFinish(QString line);
protected:
	void run() Q_DECL_OVERRIDE;

public slots:
	void recvMegFromMain(QString savefile_name,HANDLE& linkdevice);
	void recvStopSignal(bool stop_flag);
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

	bool display_flag = false;
	bool save_flag = false;

	QString savefile_name;

	bool ctn_stop_flag = false;

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
	QThread* displayThread;
	

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
	void startReadThread(QString line,HANDLE& linkdevice);
	void startDisplayThread(QString line, HANDLE& linkdevice);
	void stopReadThread(bool stop_flag);
	void stopDisplayThread(bool stop_flag);

private slots:
	//信号显示窗口槽函数
	void on_disp_btn();

	void on_earse_btn();
	void on_stop_btn();
	

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

	
private slots:
	void timerUpdate();
	void displayPlotZoomeIncrease();
	void displayPlotZoomeDecrease();

private:
	int display_max_voltage = 1000;
	int display_min_voltage = -1000;

private:
    Ui::HectoBioWindowClass ui;

	QLineEdit* text;
	void timerEvent(QTimerEvent*);
	QTimer* displayUpdateTimer= new QTimer(this);
	bool display_stop_flag = false;

	double time[50000];
	double val[50000] = { 0 };
	double* display_time = new double[500];
	double* display_val = new double[500];
	QVector<double> xs;
	QVector<double> ys;
	

	PUSHORT inBuffer = new USHORT[read_data_length];
	/*double display_time[1024*100];
	double display_val[1024*100];*/
	
	USHORT* ad_data;

	QwtPlotCurve* curve = new QwtPlotCurve("signal");

	int timer = 0;
	int dispay_timer = 0;
	int time_dur = 50;

	double* savetestSignal;


private:
	//电路驱动部分
	//void init_circuit(Circuit* circuit_set);
	void init_circuit();

	//保存QVector类型数据
	FILE* save_file_handle;
	void saveQVectordataAsStream(QString& save_name, QVector<double> savedata,char* mode);

	void saveDataAsStream(QString& save_name, double* savedata);
	void saveDataAsText(QString& save_name, double* savedata);
	void saveDataAsHdf5(const char* save_name, double* savedata);
};

