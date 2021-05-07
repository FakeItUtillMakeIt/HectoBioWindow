#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_HectoBioWindow.h"

#include <QLineEdit>
#include <qwt_plot.h>
#include <QWT/qwt.h>
#include <qwt_plot_curve.h>
#include <time.h>
#include "TimeSerialData.h"
#include "Hdf5Read.h"
#include "Circuit.h"

#define CONTINUE_ACQ 1
#define SINGLE_ACQ 2

class HectoBioWindow : public QMainWindow
{
    Q_OBJECT

public:
    HectoBioWindow(QWidget *parent = Q_NULLPTR);

	//Ĭ����������һ���豸
	bool device_flag;
	int selected_chn_sum = 0;
	int acq_flag = CONTINUE_ACQ;

	bool disp_show_flag = false;
	bool disp_stop_flag = false;
	bool disp_up_flag = false;
	bool disp_down_flag = false;

	const float DEFAULT_DPI = 192.0;
	float winDpiScale();

	void init_window();
	bool selectedChn[24];
	int trig_mode = 0;
	int trig_source = 0;
	int sample_freq = 0;
	int trig_level = 0;
	int trig_length = 0;
	int trig_delay = 0;

	//DA
	INT DA0 = 0;
	INT DA1 = 1;
	INT DA2 = 2;
	INT DA3 = 3;

	//��·������
	Circuit* circuit_set;

	//���ݱ�����
	Hdf5Read* hdf5_op;
	const char* db_name = "Dataset";

private slots:
	//�ź���ʾ���ڲۺ���
	void on_disp_btn();

	void on_earse_btn();
	void on_stop_btn();
	void on_hurry_btn();
	void on_slow_btn();


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
	void init_circuit(Circuit* circuit_set);
	//������ʾ��AD����
	bool updateAD_data(PUSHORT data_buf);

	void saveDataAsStream(QString& filename, double* savedata);
	void saveDataAsText(QString& filename, double* savedata);
	void saveDataAsHdf5(const char* filename, double* savedata);
};
