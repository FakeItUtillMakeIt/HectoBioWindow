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

//�������߳�
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


//��ʾ�߳�
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

	//Ĭ����������һ���豸
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

	//��·������
	//Circuit* circuit_set;
	long read_data_length = 1024 * 100;
	bool softTrig = false;
	USHORT* dataBuff[MAX_SEGMENT];
	BOOL NewSegmentData[MAX_SEGMENT];
	int ReadIndex;//���ݲɼ��̵߳�ǰ������������
	int CurrentIndex;//���ݴ����̵߳�ǰ������������
	BOOL ADRun;//���ڲɼ���־
	HANDLE hEvent;//�¼�
	int display_chn;//��ʾ��ͨ��
	ULONG discnt;//��ʾ�ĵ���
	ULONG samcnt;

	//��ʾ��ȡ�߳�
	ReadThread* m_readThread;
	DisplayThread* m_displayThread;

	

	//��ȡ����ʾ����
	bool ctn_display = false;
	void readAD_data();
	void displayAD_data();
	//������ʾ��AD����
	bool updateAD_data(PUSHORT data_buf);

	BOOL readUSB(PUSHORT pBuf, int bufsize);
	void setTrigPara(bool set);


	//���ݱ�����
	Hdf5Read* hdf5_op;
	const char* db_name = "Dataset";

signals:
	void startReadThread();
	void startDisplayThread();

private slots:
	//�ź���ʾ���ڲۺ���
	void on_disp_btn();

	void on_earse_btn();
	void on_stop_btn();
	void on_hurry_btn();
	void on_slow_btn();

	//test
	void test_readthread(QString line);
	void test_displaythread(QString line);

private slots:
	//menu�Ӵ��ڲۺ���
	void on_compare_btn();
	void on_signalView_btn();

private slots:
	//ͨ���ӹ��ܲۺ���
	void on_selectAll_btn();
	void on_cancelAll_btn();
	void on_selectChnOK_btn();


	//�ɼ��������ܲۺ���
	void on_acqparaOK_btn();

	//�����ɼ����ܲۺ���
	void on_startCtnAcq_btn();
	void on_stopCtnAcq_btn();

	//���βɼ��ۺ���
	void on_singleAcq_btn();

	//DA0-DA3���ܲۺ���
	void on_DA0_btn();
	void on_DA1_btn();
	void on_DA2_btn();
	void on_DA3_btn();

	void on_baselineRcv_btn();
	void on_baselineRd_btn();

	//�����Ϣ
	void on_saveoutinfo_btn();
	void on_clearinfo_btn();

	//����ʹ��
	//����
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
	//��·��������
	//void init_circuit(Circuit* circuit_set);
	void init_circuit();

	

	void saveDataAsStream(QString& save_name, double* savedata);
	void saveDataAsText(QString& save_name, double* savedata);
	void saveDataAsHdf5(const char* save_name, double* savedata);
};

