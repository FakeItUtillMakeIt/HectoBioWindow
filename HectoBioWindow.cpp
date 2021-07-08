/************************************************/
/*����ʱ��2021��5��27��*/
/*��Ҫ���£�1.QT���̣߳�2.����������Ӧ*/
/************************************************/
/*------------using code utf-8---------------*/


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
#include <qDebug>

#include <QThread>




using namespace std;

HectoBioWindow::HectoBioWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	


	//��ʼ������
	this->init_window();
	/*int newWidth = this->width();
	int newHight = this->height();

	float scale = winDpiScale();
	newWidth = newWidth * scale;
	newHight = newHight * scale;
	this->setFixedWidth(newWidth);
	this->setFixedHeight(newHight);*/

	//��ʼ����·����
	circuit_set = new Circuit();
	this->init_circuit(circuit_set);

	

	//��ʼ�����ݱ�����
	hdf5_op = new Hdf5Read();

	//��ʼ������������
	curve->setPen(Qt::blue, 2);
	//���³�ʼ���� �ӵ�·����������     ��·��֮��
	//updateAD_data();

	ui.signalPlot->setTitle(QwtText("SIGNAL PLOT"));
	ui.signalPlot->setAxisTitle(QwtPlot::yLeft, "current / mA");
	ui.signalPlot->setAxisTitle(QwtPlot::xBottom, "time / mS");


	//menubar�Ӷ�����ΪSignal
	QAction* act = new QAction(("Compare widgets"), ui.menuSignal_compare_widget);
	act->setCheckable(true);
	act->setChecked(false);
	ui.menuSignal_compare_widget->addAction(act);

	QAction* act1 = new QAction(("signal select widgets"), ui.menu_signalView_widget);
	act1->setCheckable(true);
	act1->setChecked(false);
	ui.menu_signalView_widget->addAction(act1);

	/*****************************************************************/
	//�Ƚϴ���
	/*****************************************************************/
	//�ź����
	/*****************************************************************/
	//connect(ui.menuSignal_compare_widget, &QMenu::hovered, this, &HectoBioWindow::on_compare_btn);
	connect(ui.menuSignal_compare_widget, &QMenu::triggered, this, &HectoBioWindow::on_compare_btn);
	//��ѡ�źŴ���
	//connect(ui.menu_signalView_widget, &QMenu::hovered, this, &HectoBioWindow::on_signalView_btn);
	connect(ui.menu_signalView_widget, &QMenu::triggered, this, &HectoBioWindow::on_signalView_btn);

	//������ 
	//ͨ��  ȫѡ��ť
	connect(ui.chnselectAll, &QPushButton::clicked, this, &HectoBioWindow::on_selectAll_btn);
	//ȡ��ȫѡ��ť
	connect(ui.chncancelAll, &QPushButton::clicked, this, &HectoBioWindow::on_cancelAll_btn);
	//ȷ��
	connect(ui.selectchnOK, &QPushButton::clicked, this, &HectoBioWindow::on_selectChnOK_btn);
	//�ɼ�����  ȷ��
	connect(ui.acqparaBtnOK, &QPushButton::clicked, this, &HectoBioWindow::on_acqparaOK_btn);
	//�����ɼ�  ����
	connect(ui.ctnscqStart, &QPushButton::clicked, this, &HectoBioWindow::on_startCtnAcq_btn);
	//����
	connect(ui.ctnscqEnd, &QPushButton::clicked, this, &HectoBioWindow::on_stopCtnAcq_btn);

	//��ʾ

	//���βɼ� ����
	connect(ui.singleacqEnd, &QPushButton::clicked, this, &HectoBioWindow::on_singleAcq_btn);
	//DA0 DA0
	connect(ui.btnDA0, &QPushButton::clicked, this, &HectoBioWindow::on_DA0_btn);
	//DA1  DA1
	connect(ui.btnDA1, &QPushButton::clicked, this, &HectoBioWindow::on_DA1_btn);
	//DA2	DA2
	connect(ui.btnDA2, &QPushButton::clicked, this, &HectoBioWindow::on_DA2_btn);
	//DA3	DA3
	connect(ui.btnDA3, &QPushButton::clicked, this, &HectoBioWindow::on_DA3_btn);

	//����
	//���߶�ȡ
	connect(ui.baselineReadBtn, &QPushButton::clicked, this, &HectoBioWindow::on_baselineRd_btn);
	//����У��
	connect(ui.baselineRecoverBtn, &QPushButton::clicked, this, &HectoBioWindow::on_baselineRcv_btn);

	//�����Ϣ
	//���������Ϣ
	connect(ui.saveTextbtn, &QPushButton::clicked, this, &HectoBioWindow::on_saveoutinfo_btn);
	//���
	connect(ui.clearTextbtn, &QPushButton::clicked, this, &HectoBioWindow::on_clearinfo_btn);


	//�ź���ʾ����  ��ʾ��ť
	connect(ui.signaldispDisp, &QPushButton::clicked, this, &HectoBioWindow::on_disp_btn);
	//�����ť
	connect(ui.signalearseDisp, &QPushButton::clicked, this, &HectoBioWindow::on_earse_btn);
	//��ͣ��ť
	connect(ui.signalstopDisp, &QPushButton::clicked, this, &HectoBioWindow::on_stop_btn);
	//���ٰ�ť
	connect(ui.signaldispHurry, &QPushButton::clicked, this, &HectoBioWindow::on_hurry_btn);
	//���ٰ�ť 
	connect(ui.signaldispSlow, &QPushButton::clicked, this, &HectoBioWindow::on_slow_btn);


	//����ʹ��
	//���水ť
	connect(ui.savetestSignalBtn, &QPushButton::clicked, this, &HectoBioWindow::save_testSignal_btn);

	//read_thread = new ReadADDataThread();
	////�߳��ź�
	//connect(read_thread, &ReadADDataThread::isReadDone, this, &HectoBioWindow::dealReadDone);
}


float HectoBioWindow::winDpiScale() {
	HDC screen = GetDC(NULL);
	FLOAT dpiX = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSX));
	ReleaseDC(0, screen);
	return dpiX / DEFAULT_DPI;
}

//���ݽ����ʼ����·����
void HectoBioWindow::init_circuit(Circuit* circuit_set) {
	//��ʼ����·���Ӳ���
	//����޵�·���ӣ������������Ϊ���ɱ༭����ʱ���Խ׶�����ƣ�
	/*************************************************
		ͨ���������������ݽ���ѡ����и��º�����
	***************************************************/

	//�ȼ���Ƿ������豸
	device_flag = circuit_set->judge_link_device();
	if (device_flag)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("�������豸,�豸��Ϊ��"));
		ui.textBrowser->append(QString::number(DEFAULT_DEVICE_NUM));
	}
	else
	{
		ui.textBrowser->append(QString::fromLocal8Bit("δ��⵽�����豸,�������豸"));
		return;
	}

	for (int i = 0; i < 24; i++)
	{
		circuit_set->channel[i] = selectedChn[i];
	}
	//�ɼ�������ȷ������Ҫ�Ȱ��ɼ�����ģ���ȷ����ť
	circuit_set->trig_mode = trig_mode;
	circuit_set->trig_source = trig_source;
	circuit_set->sample_freq = sample_freq;
	circuit_set->trig_level = trig_level;
	circuit_set->trig_length = trig_length;
	circuit_set->delay = trig_delay;

	/*bool flag = circuit_set->init_start_AD_device();

	char* ret_info = circuit_set->ret_flag;
	ui.textBrowser->append(QString::fromLocal8Bit(ret_info));*/
	
	ReadIndex = 0;
	return;
}


//��ʼ�����溯��
void HectoBioWindow::init_window() {
	//����ֱ���
	int newWidth = this->width();
	int newHight = this->height();

	float scale = winDpiScale();
	newWidth = newWidth * scale;
	newHight = newHight * scale;
	/*this->setFixedWidth(newWidth);
	this->setFixedHeight(newHight);*/
	this->setMinimumWidth(newWidth);
	this->setMinimumHeight(newHight);


	//ͨ����ʼ����ȫ����ʼ��Ϊδѡ��
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


	//�ɼ�����
	//����ģʽ  ����ģʽ��ʼ��Ϊ��������
	ui.trigModel->setEditable(false);

	ui.trigModel->addItem(QString::fromLocal8Bit("����"));
	ui.trigModel->addItem(QString::fromLocal8Bit("�󴥷�"));
	ui.trigModel->addItem(QString::fromLocal8Bit("��ʱ"));
	//����Դ  ����Դ��ʼ��Ϊ�����ش���
	ui.trigSource->setEditable(false);

	ui.trigSource->addItem(QString::fromLocal8Bit("�����ش���"));
	ui.trigSource->addItem(QString::fromLocal8Bit("�⸺�ش���"));
	ui.trigSource->addItem(QString::fromLocal8Bit("���ش���"));
	//������  �����ʳ�ʼ��Ϊ0
	ui.sampleFreq->setText(QString::number(0));
	//������ƽ ������ƽ��ʼ��Ϊ0
	ui.trigLevel->setText(QString::number(0));
	//��������
	ui.trigLength->setText(QString::number(0));
	//��ʱ

	ui.delayTime->setText(QString::number(0));

	//�����ɼ�
	//��ʾ
	ui.ctnscqDisplay->setCheckable(true);
	ui.ctnscqDisplay->setCheckState(Qt::Unchecked);
	//����ģʽ
	ui.ctnacqMutliSelcet->setMaxVisibleItems(10);
	ui.ctnacqMutliSelcet->addItem(QString::fromLocal8Bit("�����ش���"));
	ui.ctnacqMutliSelcet->addItem(QString::fromLocal8Bit("�⸺�ش���"));
	ui.ctnacqMutliSelcet->addItem(QString::fromLocal8Bit("���ش���"));

	ui.ctnscqStart->setEnabled(true);
	ui.ctnscqEnd->setEnabled(false);

	//���βɼ�
	//�ɼ�ʱ��
	ui.singleacqTime->setText(QString::number(0));
	ui.singleacqEnd->setEnabled(true);

	/********************************
	//DA������ʱ�������������ʲô�ģ�
	*********************************/
	//DA0

	ui.da0MutliSelect->setMaxVisibleItems(10);
	ui.da0MutliSelect->addItem(QString::fromLocal8Bit("���Ҳ�"));
	ui.da0MutliSelect->addItem(QString::fromLocal8Bit("����"));

	ui.da0freq->setText(QString::number(0));
	ui.da0samplepoint->setText(QString::number(0));
	ui.da0cycle->setText(QString::number(0));
	ui.da0start->setCheckable(true);
	ui.da0start->setCheckState(Qt::Unchecked);

	//DA1

	ui.da1MutliSelect->setMaxVisibleItems(10);
	ui.da1MutliSelect->addItem(QString::fromLocal8Bit("���Ҳ�"));
	ui.da1MutliSelect->addItem(QString::fromLocal8Bit("����"));

	ui.da1freq->setText(QString::number(0));
	ui.da1samplepoint->setText(QString::number(0));
	ui.da1cycle->setText(QString::number(0));
	ui.da1start->setCheckable(true);
	ui.da1start->setCheckState(Qt::Unchecked);
	//DA2

	ui.da2MutliSelect->setMaxVisibleItems(10);
	ui.da2MutliSelect->addItem(QString::fromLocal8Bit("���Ҳ�"));
	ui.da2MutliSelect->addItem(QString::fromLocal8Bit("����"));

	ui.da2freq->setText(QString::number(0));
	ui.da2samplepoint->setText(QString::number(0));
	ui.da2cycle->setText(QString::number(0));
	ui.da2start->setCheckable(true);
	ui.da2start->setCheckState(Qt::Unchecked);
	//DA3

	ui.da3MutliSelect->setMaxVisibleItems(10);
	ui.da3MutliSelect->addItem(QString::fromLocal8Bit("���Ҳ�"));
	ui.da3MutliSelect->addItem(QString::fromLocal8Bit("����"));

	ui.da3freq->setText(QString::number(0));
	ui.da3samplepoint->setText(QString::number(0));
	ui.da3cycle->setText(QString::number(0));
	ui.da3start->setCheckable(true);
	ui.da3start->setCheckState(Qt::Unchecked);


	ui.baseline_input->setText(QString::fromLocal8Bit("����У��ƫ��"));

	return;
}

/*************************************************
   ����ʦ����ıȽϹ���
***************************************************/
//�ȽϽ���ۺ���

void HectoBioWindow::on_compare_btn() {
	CompareWidgets* cmpwindow = new CompareWidgets();
	cmpwindow->setWindowTitle(QString::fromLocal8Bit("���źűȽϽ���"));
	
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
   ����������Ĵ�ֹ���
***************************************************/
//��ֽ���ۺ���
void HectoBioWindow::on_signalView_btn() {
	signaView* signalviewWindow = new signaView();
	signalviewWindow->setWindowTitle(QString::fromLocal8Bit("�źŴ�֣����ڲ�ʵ����Ա������"));

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

/*************************************************
   ͨ��ȫ��ʹ��
***************************************************/
//ͨ���ӹ��ܲۺ���
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
   ͨ��ȫ��ȡ��ѡ��
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

	return;
}

/*************************************************
   ѡ��ͨ���ۺ���
***************************************************/
void HectoBioWindow::on_selectChnOK_btn() {
	//
	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("δ��⵽�����豸,�������豸"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("δ��⵽�����豸,�������豸"));
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
		ui.textBrowser->append(QString::fromLocal8Bit("�������һ��"));
	}
	else
	{
		ui.textBrowser->append(QString::fromLocal8Bit("��ѡ��ͨ��,Ȼ�������һ��"));
	}

	return;
}

/*************************************************
   //�ɼ��������ܲۺ���
***************************************************/
void HectoBioWindow::on_acqparaOK_btn() {

	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("δ��⵽�����豸,�������豸"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("δ��⵽�����豸,�������豸"));
		return;
	}

	trig_mode = ui.trigModel->currentIndex();
	trig_source = ui.trigSource->currentIndex();
	sample_freq = ui.sampleFreq->text().toInt();
	trig_level = ui.trigLevel->text().toInt();
	trig_length = ui.trigLength->text().toInt();
	trig_delay = ui.delayTime->text().toInt();

	QMessageBox::warning(this, QString::fromLocal8Bit("��ȷ�ϲɼ���������"), QString::fromLocal8Bit("��ȷ�ϲɼ���������"), QMessageBox::Yes);

	ui.textBrowser->append(QString::fromLocal8Bit("���²ɼ�����ȷ����ť\n����ģʽ��"));
	ui.textBrowser->append((ui.trigModel->itemText(trig_mode)));
	ui.textBrowser->append(QString::fromLocal8Bit("����Դ��"));
	ui.textBrowser->append((ui.trigSource->itemText(trig_source)));
	ui.textBrowser->append(QString::fromLocal8Bit("����Ƶ�ʣ�"));
	ui.textBrowser->append(QString::number(sample_freq));
	ui.textBrowser->append(QString::fromLocal8Bit("������ƽ��"));
	ui.textBrowser->append(QString::number(trig_level));
	ui.textBrowser->append(QString::fromLocal8Bit("�������ȣ�"));
	ui.textBrowser->append(QString::number(trig_length));
	ui.textBrowser->append(QString::fromLocal8Bit("��ʱ��"));
	ui.textBrowser->append(QString::number(trig_delay));

	return;
}


/*************************************************
  //�����ɼ����ܲۺ���  �����ɼ��͵��βɼ�����
***************************************************/
void HectoBioWindow::on_startCtnAcq_btn() {
	//��ʼ�����ɼ�֮ǰ���ж��豸�Ƿ�����
	//if (!device_flag)
	//{
	//	QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("δ��⵽�����豸,�������豸"), QMessageBox::Yes);
	//	ui.textBrowser->append(QString::fromLocal8Bit("δ��⵽�����豸,�������豸"));
	//	return;
	//}
	////�Ƿ�ѡ��ͨ��  ���û��ѡ��ͨ��  ����
	//if (selected_chn_sum == 0)
	//{
	//	QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("���ѡ���ͨ��"), QMessageBox::Yes);
	//	ui.textBrowser->append(QString::fromLocal8Bit("�빴ѡ���ͨ��"));
	//	return;
	//}
	/*****************************************************
	* //���ѡ��ͨ������ȡAD����
	������ź���ʾ���ܲ��֣���ʾ�����ݴ��ݵ�ͨ������ʾ��
	******************************************************/
	
	for (int i=0;i<24;i++)
	{
		circuit_set->channel[i] = selectedChn[i];
	}
	circuit_set->trig_mode = ui.trigModel->currentIndex();
	circuit_set->trig_source = ui.trigSource->currentIndex();
	circuit_set->sample_freq = ui.sampleFreq->text().toInt();
	circuit_set->trig_level = ui.trigLevel->text().toInt();
	circuit_set->trig_length = ui.trigLength->text().toInt();
	circuit_set->delay = ui.delayTime->text().toInt();

	/// <summary>
	/// init_start_AD_device����para ����USB����
	/// </summary>
	//�������
	if (circuit_set->bsoftTrig==TRIG_SRC_SOFT)
	{
		softTrig = true;
		
		samcnt = TRIG_UNIT*READ_DATA_LENGTH;
	}
	else
	{
		softTrig = false;
		samcnt = READ_DATA_LENGTH;
	}

	samcnt *= selected_chn_sum;
	if (samcnt>READ_MAX_LEN)
	{
		samcnt = READ_MAX_LEN;
	}
	//��ʼ���ɼ�
	if (circuit_set->init_start_AD_device())
	{
		ui.textBrowser->append(QString::fromLocal8Bit("��ʼ��AD�ɹ�"));
		ui.singleacqEnd->setEnabled(false);
	}
	else
	{
		ui.textBrowser->append(QString::fromLocal8Bit("��ʼ��ADʧ��"));
		//return;
	}
	//�ȴ�AD��ʼ����ɣ������޷������������
	Sleep(10);


	acq_flag = CONTINUE_ACQ;
	//ѡ����ʾ
	if (ui.ctnscqDisplay->checkState()==Qt::Checked)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("��ȡ����ʾ����"));
		ui.singleacqEnd->setEnabled(false);
		ui.ctnscqEnd->setEnabled(true);
		ctn_saveflag = true;
	}
	else
	{
		ui.textBrowser->append(QString::fromLocal8Bit("��ȡ�ͱ�������"));
		ctn_saveflag = false;
	}

	int i = 0;
	//���仺����
	if (ad_data_buff)
	{
		for (i=0;i<MAX_SEGMENT;i++)
		{
			ad_data_buff[i] = NULL;
		}
	}
	//�໺��
	for (i=0;i<MAX_SEGMENT;i++)
	{
		ad_data_buff[i] = new USHORT[samcnt];//ÿ�����������һ��ͨ������
	}
	//��ʼ���໺���־
	for (i=0;i<MAX_SEGMENT;i++)
	{
		NewSegmentData[i] = false;
	}
	ReadIndex = 0;
	ADRUN = true;//������
	//read_thread = new ReadADDataThread();
	////�������̺߳���ʾ�߳�
	////USB2069FCAD  494��
	//read_timer = new QTimer();
	//read_timer->start(10);

	updateAD_data();

	//USB2069_ADoffset(linkdevice, 0, 0, 0);
	//circuit_set->read_AD(ad_data_buff[0], 8000);
	//for (i=0;i<8000;i++)
	//{
	//	//ת����ƽ
	//	unsigned short tmp = (ad_data_buff[0][i] - 32768);
	//	float step = float(1.0 / 32768.0)*10;
	//	
	//	float value = tmp * step;
	//	printf("%f \n", value);
	//	savetestSignal[i] =double(value);
	//}

	

	//PUSHORT data_buf;
	////data_buf Ϊʵʱ���� 
	////�ӵ�·���ֱ�Ӷ�
	//data_buf = new USHORT[circuit_set->read_data_length];
	//bool circuit_read_flag = circuit_set->read_AD(data_buf, circuit_set->read_data_length);

	////������ʾ���ź�  �ж�AD��ȡ�Ƿ�ɹ�
	//if (!circuit_read_flag)
	//{
	//	ui.textBrowser->append(QString::fromLocal8Bit(circuit_set->ret_flag));
	//	circuit_set->stop_AD_device();
	//	return;
	//}
	
	return;

}

/*************************************************
   //ֹͣ�����ɼ��ۺ���   ֹͣ�ɼ�֮��Ӧ���������̺߳������д���

***************************************************/
void HectoBioWindow::on_stopCtnAcq_btn() {
	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("δ��⵽�����豸,�������豸"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("δ��⵽�����豸,�������豸"));
		return;
	}
	//���û��ѡ��ͨ��  ����
	if (selected_chn_sum == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("���ѡ���ͨ��"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("�빴ѡ���ͨ��"));
		return;
	}
	//�����û��ʼ���͵����������������⣬��pass
	//��������ɼ�ֹ֮ͣ��  �ж��Ƿ�ѡ��ʾ
	//�ݶ��Ƿ�ѡ��ʾ������
	//if (ui.ctnscqDisplay->isChecked()==true || ui.ctnscqDisplay->isChecked()==false)
	//{
	//	//�����߳�
	//	//ctn_read_thread->start();
	//	circuit_set->stop_AD_device();
	//	//connect(ctn_read_thread, &ReadADDataThread::isReadDone, this, &HectoBioWindow::dealReadDone);
	//}
	circuit_set->stop_AD_device();
	ui.textBrowser->append(QString::fromLocal8Bit("�ر������ɼ�"));
	
	return;
}

/*************************************************
   //���βɼ��ۺ���
***************************************************/
void HectoBioWindow::on_singleAcq_btn() {
	//////////////////////////////////////////////////////////////////////////
	/*����ʱע���²��֣�ʵ��ʱȡ��ע��*/
	//////////////////////////////////////////////////////////////////////////

	//if (!device_flag)
	//{
	//	QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("δ��⵽�����豸,�������豸"), QMessageBox::Yes);
	//	ui.textBrowser->append(QString::fromLocal8Bit("δ��⵽�����豸,�������豸"));
	//	return;
	//}
	////���û��ѡ��ͨ��  ����
	//if (selected_chn_sum == 0)
	//{
	//	QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("���ѡ���ͨ��"), QMessageBox::Yes);
	//	ui.textBrowser->append(QString::fromLocal8Bit("�빴ѡ���ͨ��"));
	//	return;
	//}
	//�����û��ʼ���͵����������������⣬��pass
	if (circuit_set->bsoftTrig == TRIG_SRC_SOFT)
	{
		softTrig = true;

		samcnt = TRIG_UNIT * READ_DATA_LENGTH;
	}
	else
	{
		softTrig = false;
		samcnt = READ_DATA_LENGTH;
	}

	samcnt *= selected_chn_sum;
	if (samcnt > READ_MAX_LEN)
	{
		samcnt = READ_MAX_LEN;
	}

	//���²ɼ�����
	//if (circuit_set->init_start_AD_device())
	//{
	//	ui.textBrowser->append(QString::fromLocal8Bit("��ʼ��AD�ɹ�"));
	//	ui.singleacqEnd->setEnabled(false);
	//}
	//else
	//{
	//	ui.textBrowser->append(QString::fromLocal8Bit("��ʼ��ADʧ��"));
	//	return;
	//}
	//�ȴ�AD��ʼ����ɣ������޷������������
	Sleep(10);


	int i = 0;
	//���仺����
	if (ad_data_buff)
	{
		for (i = 0; i < MAX_SEGMENT; i++)
		{
			ad_data_buff[i] = NULL;
		}
	}
	//�໺��
	for (i = 0; i < MAX_SEGMENT; i++)
	{
		ad_data_buff[i] = new USHORT[samcnt];//ÿ�����������һ��ͨ������
	}
	//��ʼ���໺���־
	for (i = 0; i < MAX_SEGMENT; i++)
	{
		NewSegmentData[i] = false;
	}
	ReadIndex = 0;
	ADRUN = true;//������

	//�����ɼ��͵��βɼ�����
	acq_flag = SINGLE_ACQ;
	ui.ctnscqStart->setEnabled(false);
	ui.ctnscqEnd->setEnabled(false);

	//�ɼ���������ѡ��ͨ����*�趨Ƶ��*�ɼ�ʱ��
	//������ĵ��βɼ�����ʾ�߳�ʱ����Ҫ����ʾ�ĵ����趨Ϊ��ֵ
	//samcnt = selected_chn_sum * circuit_set->para_init.ADFREQ * ui.singleacqTime->text().toInt();
	//���òɼ��߳�
	read_thread = new ReadADDataThread();
	
	//����һ��TIMER����̬ˢ��signaldisplayplot����
	read_timer = new QTimer(this);
	read_timer->start(10);
	//�����߳��ź�,�߳���֮��Ķ���
	connect(read_timer, &QTimer::timeout, this, &HectoBioWindow::displayReadsignal);
	
	char* filepath = "D:\\Download\\sftpdownload\\5.fast5";

	CompareWidgets com;
	
	savetestSignal=com.readDataFromONT(filepath);

	ui.singleacqEnd->setEnabled(false);

	return;
}

/*************************************************
   //DA0-DA3���ܲۺ���
   4ͨ������DAת������DA������AD�໥����
   DA����������PC�Լ�������ģ�Ⲩ��   AD���������Բɼ���--->PC
***************************************************/

void HectoBioWindow::on_DA0_btn() {
	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("δ��⵽�����豸,�������豸"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("δ��⵽�����豸,�������豸"));
		return;
	}
	//���û��ѡ��ͨ��  ����
	if (selected_chn_sum == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("���ѡ���ͨ��"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("�빴ѡ���ͨ��"));
		return;
	}
	//ѡ�����Ҳ��򷽲�
	int da0index=0;
	da0index= ui.da0MutliSelect->currentIndex();
	long da0freq = 0;
	da0freq=ui.da0freq->text().toDouble();
	int da0point = 0;
	da0point= ui.da0samplepoint->text().toInt();
	int da0cycle = 0;
	da0cycle=ui.da0cycle->text().toInt();
	bool da0enable = 0;
	da0enable= ui.da0start->checkState();
	bool da0enable_cycle = false;
	bool stopflag = false;
	PUSHORT data_buf = NULL;
	data_buf=new USHORT[circuit_set->read_data_length];
	//PUSHORT data_buf = (PUSHORT)malloc(sizeof(USHORT) * circuit_set->read_data_length);

	if (da0cycle > 0)
	{
		da0enable_cycle = true;
	}

	bool ret_DA_flag = circuit_set->set_DA(DA0, da0enable, da0freq, da0enable_cycle, da0cycle, stopflag, data_buf, da0point);
	if (ret_DA_flag)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("DA0�������"));
	}
	delete[] data_buf;

	/*bool ret_DA_flag = USB2069_SetDA(circuit_set->link_device, DA0, da0enable, &da0freq, da0enable_cycle, da0cycle, stopflag, data_buf, da0point);
	if (ret_DA_flag)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("DA0�������"));
	}
	delete[] data_buf;
	*/

	return;

}
void HectoBioWindow::on_DA1_btn() {
	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("δ��⵽�����豸,�������豸"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("δ��⵽�����豸,�������豸"));
		return;
	}
	//���û��ѡ��ͨ��  ����
	if (selected_chn_sum == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("���ѡ���ͨ��"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("�빴ѡ���ͨ��"));
		return;
	}
	//ѡ�����Ҳ��򷽲�
	int da1index = 0;
	da1index = ui.da1MutliSelect->currentIndex();
	long da1freq = 0;
	da1freq = ui.da1freq->text().toDouble();
	int da1point = 0;
	da1point = ui.da1samplepoint->text().toInt();
	int da1cycle = 0;
	da1cycle = ui.da1cycle->text().toInt();
	bool da1enable = 0;
	da1enable = ui.da1start->checkState();
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
		ui.textBrowser->append(QString::fromLocal8Bit("DA1�������"));
	}
	delete[] data_buf;

	return;
}
void HectoBioWindow::on_DA2_btn() {
	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("δ��⵽�����豸,�������豸"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("δ��⵽�����豸,�������豸"));
		return;
	}
	//���û��ѡ��ͨ��  ����
	if (selected_chn_sum == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("���ѡ���ͨ��"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("�빴ѡ���ͨ��"));
		return;
	}
	//ѡ�����Ҳ��򷽲�
	int da2index = 0;
	da2index = ui.da2MutliSelect->currentIndex();
	long da2freq = 0;
	da2freq = ui.da2freq->text().toDouble();
	int da2point = 0;
	da2point = ui.da2samplepoint->text().toInt();
	int da2cycle = 0;
	da2cycle = ui.da2cycle->text().toInt();
	bool da2enable = 0;
	da2enable = ui.da2start->checkState();
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
		ui.textBrowser->append(QString::fromLocal8Bit("DA2�������"));
	}
	delete[] data_buf;

	return;
}
void HectoBioWindow::on_DA3_btn() {
	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("δ��⵽�����豸,�������豸"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("δ��⵽�����豸,�������豸"));
		return;
	}
	//���û��ѡ��ͨ��  ����
	if (selected_chn_sum == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("���ѡ���ͨ��"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("�빴ѡ���ͨ��"));
		return;
	}
	//ѡ�����Ҳ��򷽲�
	int da3index = 0;
	da3index = ui.da3MutliSelect->currentIndex();
	long da3freq = 0;
	da3freq = ui.da3freq->text().toDouble();
	int da3point = 0;
	da3point = ui.da3samplepoint->text().toInt();
	int da3cycle = 0;
	da3cycle = ui.da3cycle->text().toInt();
	bool da3enable = 0;
	da3enable = ui.da3start->checkState();
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
		ui.textBrowser->append(QString::fromLocal8Bit("DA3�������"));
	}
	delete[] data_buf;

	return;
}

/*************************************************
   //��ƫ
//���߶�ȡ�ۺ���
***************************************************/
void HectoBioWindow::on_baselineRd_btn() {
	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("δ��⵽�����豸,�������豸"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("δ��⵽�����豸,�������豸"));
		return;
	}
	//���û��ѡ��ͨ��  ����
	if (selected_chn_sum == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("���ѡ���ͨ��"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("�빴ѡ���ͨ��"));
		return;
	}
	//ѡ��ͨ����
	double* freq = new double;
	circuit_set->get_freq(freq);
	ui.textBrowser->append(QString::number(*freq));

	return;
}

/*************************************************
   //����У���ۺ���
***************************************************/

void HectoBioWindow::on_baselineRcv_btn() {
	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("δ��⵽�����豸,�������豸"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("δ��⵽�����豸,�������豸"));
		return;
	}
	//���û��ѡ��ͨ��  ����
	if (selected_chn_sum == 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("���ѡ���ͨ��"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("�빴ѡ���ͨ��"));
		return;
	}
	double recover_offset;
	try
	{
		recover_offset = ui.baseline_input->text().toDouble();
		//ui.textBrowser->append(recover_offset);
		char* input_text = (char*)malloc(sizeof(char) * 100);
		sprintf(input_text, "д����ƫ��%f", recover_offset);
		circuit_set->recover_ADoffset(recover_offset);
		ui.textBrowser->append(QString::fromLocal8Bit(input_text));
	}
	catch (...)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("����У��ƫ�ƣ�double����"));
	}

	return;
}


/*************************************************
  //�����Ϣ
//���������Ϣ
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
   ���textBrower
***************************************************/
void HectoBioWindow::on_clearinfo_btn() {
	ui.textBrowser->clear();
}

/*************************************************
  //��ʱ��
//���������ʼ�źţ������ڵ�·δ��ʱ����
***************************************************/
void HectoBioWindow::timerEvent(QTimerEvent*) {
	static int c = 0;
	savetestSignal = new double[500];

	for (int i = 0; i < 499; i++)
	{
		val[i] = val[i + 1];
	}
	//val[499] = qrand() % 500;
	val[499] = savetestSignal[c];
	curve->setSamples(time, val, 500);
	ui.signalPlot->replot();

	c++;

	return;
}


/*************************************************
   �ź���ʾ��ģ��
   //�ź���ʾ�ۺ���
***************************************************/
void HectoBioWindow::on_disp_btn() {

	if (!device_flag)
	{
		return;
	}
	if (selected_chn_sum==0)
	{
		return;
	}

	disp_show_flag = true;
	//ui.signaldispOpenGL->DrawWindowBackground();
	//QwtPlot* myplot = new QwtPlot("curve display");
	/*curve->setSamples(time, val, 500);
	curve->attach(ui.signalPlot);
	ui.signalPlot->replot();*/
	read_timer->start();

	ui.textBrowser->append("now start draw signal plot");
}
/*************************************************
   //ɾ���źŲۺ���
***************************************************/
void HectoBioWindow::on_earse_btn() {
	if (!disp_show_flag)
	{
		return;
	}

	//this->killTimer(timer);
	//curve->setData(0);
	////ui.signalPlot->detachItems();
	//ui.signalPlot->replot();
	read_timer->stop();
	ui.textBrowser->append("eraser signal plot");

	return;

}
/*************************************************
  //��ͣ�źŲۺ���
***************************************************/
void HectoBioWindow::on_stop_btn() {
	if (!disp_show_flag)
	{
		return;
	}
	if (!disp_up_flag || !disp_down_flag)
	{
		return;
	}
	//������ֹͣ״̬
	disp_stop_flag = disp_stop_flag ^ true;
	//this->killTimer(timer);
	read_timer->stop();
	ui.textBrowser->append("stop signal plot");

	//��ͣ����ԷŴ���С�ȹ���

	QwtPlotZoomer* zoomer = new QwtPlotZoomer(ui.signalPlot->canvas());
	//ѡ����ɫ����Ϊ��ɫ
	zoomer->setRubberBandPen(QColor(Qt::red));

	zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
	zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton);
	zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);

	zoomer->setEnabled(true);

	return;

}

/*************************************************
   //�����źŶ����ۺ���
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

	return;
}

/*************************************************
  �����źŶ����ۺ���
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

	return;
}

/*************************************************
   ����AD���ݣ���·��֮�����
***************************************************/
bool HectoBioWindow::updateAD_data() {

	//ʾ���ź�
	//��������ź�
	//for (int i = 0; i < 500; i++)
	//{
	//	time[i] = i;
	//}

	//ui.signalPlot->setTitle(QwtText("SIGNAL PLOT"));
	//ui.signalPlot->setAxisTitle(QwtPlot::yLeft, "current / mA");
	//ui.signalPlot->setAxisTitle(QwtPlot::xBottom, "time / mS");

	//if (data_buf)
	//{
	//	for (int i = 0; i < 500; i++)
	//	{
	//		time[i] = data_buf[i];
	//	}
	//}

	//��������  FCFR 879
	PUSHORT inBuffer = NULL;
	ULONG status = FALSE;
	inBuffer = new USHORT[samcnt];
	ULONG i = 0;
	LONG bBufOver = 0;
	
	
	memset(inBuffer, 0, samcnt * sizeof(USHORT));
	while (ADRUN)
	{
		if (circuit_set->bsoftTrig)
		{
			USB2069_ExeSoftTrig(linkdevice);
		}
		status = USB2069_ReadAD(linkdevice, inBuffer, samcnt);
		//��ȡ���
		if (!USB2069_GetBufOver(linkdevice, &bBufOver))
		{
			ADUpdate = true;
			//goto exit_read;
		}
		if (bBufOver)
		{
			ui.textBrowser->append(QString::fromLocal8Bit("Error:�������\n"));
			ADUpdate = true;
			//goto exit_read;
		}
		#define DISPLAY_CNT 500
		if (samcnt < DISPLAY_CNT)
		{
			//��ʾ��Ŀ ���� ��ȡ��Ŀ
			discnt = samcnt;
		}
		else
		{
			discnt = DISPLAY_CNT;
		}
		for (ULONG i=0;i<discnt;i++)
		{
			//Ĭ����ʾͨ��
			int showchn = 0;
			ad_data_buff[ReadIndex][i] = inBuffer[selected_chn_sum*i+showchn];
		}
		if (NewSegmentData[ReadIndex] == true)
		{
			ReadIndex = 0;
			CurrentIndex = 0;
			for (int i = 0; i < MAX_SEGMENT; i++)
			{
				NewSegmentData[i] = false;
			}
		}
		NewSegmentData[ReadIndex] = true;
		//�����¼������߻�����ʾ���ڣ����������Ѳɼ�
		//�����߳��ź�,�߳���֮��Ķ���
		//ctn_read_thread = new ReadADDataThread();
		//�������̺߳���ʾ�߳�
		//USB2069FCAD  494��
		/*read_timer = new QTimer();
		read_timer->start(10);
		connect(read_timer, &QTimer::timeout, this, &HectoBioWindow::displayCTNReadsignal);*/
		//�����¼������߻��ƴ����̣߳�������׼����
		emit readisDone();
		
		connect(this, &HectoBioWindow::readisDone, this, &HectoBioWindow::displayCTNReadsignal);
		
		
		ReadIndex++;
		if (ReadIndex==MAX_SEGMENT)
		{
			ReadIndex = 0;
			return true;
		}
		ADUpdate = true;

	}

exit_read:
	delete[] inBuffer;
	ADUpdate = true;
	return true;
}

/*************************************************
   //�����źŲۺ���
***************************************************/
void HectoBioWindow::save_testSignal_btn() {
	//������ͣʱ��ʾ֮ǰ������
	//�����ʽΪhdf5
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

	//��������
	//��ѡ�����ʽ
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
   //�����źŸ�ʽ��saveDataStream
   ����Ϊ������ģʽ����C++�ļ���ʽд
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
   �����źţ���QTTextStream�ļ���д
***************************************************/
void HectoBioWindow::saveDataAsText(QString& save_name, double* savedata) {
	//�ɱ���Ϊhdf5�ļ�
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
  �����źţ���HDF5�ļ�д
***************************************************/
void HectoBioWindow::saveDataAsHdf5(const char* save_name, double* savedata) {
	//�ɱ���Ϊhdf5�ļ�
	//����Hdf5Read��
	int row, col;
	row = 1;
	col = 500;
	hdf5_op = new Hdf5Read();

	hdf5_op->save_file(savedata, row, col, save_name, db_name);

	return;
}


void HectoBioWindow::dealReadDone() {
	//��ȡ���֮�� �ر�AD �洢�õ����� �ر��߳�

	circuit_set->stop_AD_device();
	//�洢s
	//����
	ui.textBrowser->append(QString::fromLocal8Bit("���̳ɹ�"));
	//�رն�ȡ�߳�
	if (acq_flag==SINGLE_ACQ)
	{
		read_thread->quit();
		read_thread->wait();
		read_timer->destroyed();
		delete[] savetestSignal;
	}
	else if (acq_flag==CONTINUE_ACQ)
	{
		ctn_read_thread->quit();
		ctn_read_thread->wait();
		read_timer->destroyed();
	}
	
	
	ui.singleacqEnd->setEnabled(true);
	ui.ctnscqStart->setEnabled(true);
	ui.ctnscqEnd->setEnabled(true);

	
}

//���βɼ�
//��Read�̴߳��� ʵʱ��ʾ�ź�
void HectoBioWindow::displayReadsignal() {
	

	static int c = 0;

	for (int i = 0; i < 499; i++)
	{
		val[i] = val[i + 1];
	}

	
	//val[499] = qrand() % 500;
	val[499] = savetestSignal[c];
	curve->setSamples(time, val, 500);
	curve->attach(ui.signalPlot);
	ui.signalPlot->replot();

	c++;

	//����Ҫ��Ϊʵ�ʲɼ�����
	if (c>8000)
	{
		c = 0;
		read_timer->stop();
		//���βɼ��Ƚ���֮�����
		read_thread->start();
	/*	ui.singleacqEnd->setEnabled(true);
		read_timer->stop();
		read_timer->destroyed();
		read_thread->destroyed();
		delete[] savetestSignal;*/

		//�ȴ��׳�isReadDone�ź�
		if (acq_flag==SINGLE_ACQ)
		{
			connect(read_thread, &ReadADDataThread::isReadDone, this, &HectoBioWindow::dealReadDone);
		}
		else if (acq_flag==CONTINUE_ACQ)
		{
			connect(ctn_read_thread, &ReadADDataThread::isReadDone, this, &HectoBioWindow::dealReadDone);
		}
		
		return;
	}

}



//�����ɼ�
//��Read�̴߳��� ʵʱ��ʾ�ź�
void HectoBioWindow::displayCTNReadsignal() {
	//�����ݺ�curve�������������߳�
	DrawPlot drawplot;
	drawplot.moveToThread(&thread);
	connect(this, SIGNAL(started()), &drawplot, SLOT(&drawplot::slot));
	thread.start();

}

void HectoBioWindow::run() {
	qDebug() << "thread:" << QThread::currentThreadId();

}



void HectoBioWindow::start() {
	//���������߳�
	
	static int c = 0;

	for (int i = 0; i < 499; i++)
	{
		val[i] = val[i + 1];
	}

	int seg;
	for ( seg=1;seg<MAX_SEGMENT;seg++)
	{
		//val[499] = qrand() % 500;
	//val[499] = savetestSignal[c];
		val[499] = ad_data_buff[seg][c];
		curve->setSamples(time, val, 500);
		curve->attach(ui.signalPlot);
		ui.signalPlot->replot();

		c++;
		if (c>=discnt)
		{
			c = 0;
		}
	}

	//����Ҫ��Ϊʵ�ʲɼ�����
	if (seg == MAX_SEGMENT)
	{
		c = 0;
		
		/*	ui.singleacqEnd->setEnabled(true);
			read_timer->stop();
			read_timer->destroyed();
			read_thread->destroyed();
			delete[] savetestSignal;*/

			//�ȴ��׳�isReadDone�ź�

		return;
	}
	return;
}


void DrawSignalThread::run() {

	qDebug() << "current thread is:" << QThread::currentThreadId();
	emit isDrawDone();
	return;
}