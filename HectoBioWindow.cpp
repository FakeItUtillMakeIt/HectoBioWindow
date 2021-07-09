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

	//��ʼ������
	init_window();

	//��ʼ����·����
	init_circuit();

	//USB2069_InitAD(linkdevice,)

	//��ʼ�����ݱ�����
	hdf5_op = new Hdf5Read();

	//��ʼ������������
	curve->setPen(Qt::blue, 2);
	//���³�ʼ���� �ӵ�·����������     ��·��֮��
	updateAD_data(NULL);

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

}


float HectoBioWindow::winDpiScale() {
	HDC screen = GetDC(NULL);
	FLOAT dpiX = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSX));
	ReleaseDC(0, screen);
	return dpiX / DEFAULT_DPI;
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

	ui.trigModel->setCurrentIndex(0);
	//����Դ  ����Դ��ʼ��Ϊ�����ش���
	ui.trigSource->setEditable(false);

	ui.trigSource->addItem(QString::fromLocal8Bit("�����ش���"));
	ui.trigSource->addItem(QString::fromLocal8Bit("�⸺�ش���"));
	ui.trigSource->addItem(QString::fromLocal8Bit("���ش���"));

	ui.trigSource->setCurrentIndex(0);
	//������  �����ʳ�ʼ��Ϊ0
	ui.sampleFreq->setText(QString::number(sample_freq));
	//������ƽ ������ƽ��ʼ��Ϊ0
	ui.trigLevel->setText(QString::number(trig_level));
	//��������
	ui.trigLength->setText(QString::number(trig_length));
	//��ʱ

	ui.delayTime->setText(QString::number(trig_delay));

	//�����ɼ�
	//��ʾ
	ui.ctnscqDisplay->setCheckable(true);
	ui.ctnscqDisplay->setCheckState(Qt::Unchecked);
	ui.ctnscqStart->setEnabled(true);
	ui.ctnscqEnd->setEnabled(false);

	//����ģʽ
	ui.ctnacqMutliSelcet->setMaxVisibleItems(10);
	ui.ctnacqMutliSelcet->addItem(QString::fromLocal8Bit("�����ش���"));
	ui.ctnacqMutliSelcet->addItem(QString::fromLocal8Bit("�⸺�ش���"));
	ui.ctnacqMutliSelcet->addItem(QString::fromLocal8Bit("���ش���"));

	ui.ctnacqMutliSelcet->setCurrentIndex(0);

	//���βɼ�
	//�ɼ�ʱ��
	ui.singleacqEnd->setEnabled(true);
	ui.singleacqTime->setText(QString::number(0));

	/********************************
	//DA������ʱ�������������ʲô�ģ�
	*********************************/
	//DA0

	ui.da0MutliSelect->setMaxVisibleItems(10);
	ui.da0MutliSelect->addItem(QString::fromLocal8Bit("���Ҳ�"));
	ui.da0MutliSelect->addItem(QString::fromLocal8Bit("����"));

	ui.da0freq->setText(QString::number(da_freq));
	ui.da0samplepoint->setText(QString::number(da_sample));
	ui.da0cycle->setText(QString::number(da_cycle));
	ui.da0start->setCheckable(true);
	ui.da0start->setCheckState(Qt::Unchecked);

	//DA1

	ui.da1MutliSelect->setMaxVisibleItems(10);
	ui.da1MutliSelect->addItem(QString::fromLocal8Bit("���Ҳ�"));
	ui.da1MutliSelect->addItem(QString::fromLocal8Bit("����"));

	ui.da1freq->setText(QString::number(da_freq));
	ui.da1samplepoint->setText(QString::number(da_sample));
	ui.da1cycle->setText(QString::number(da_cycle));
	ui.da1start->setCheckable(true);
	ui.da1start->setCheckState(Qt::Unchecked);
	//DA2

	ui.da2MutliSelect->setMaxVisibleItems(10);
	ui.da2MutliSelect->addItem(QString::fromLocal8Bit("���Ҳ�"));
	ui.da2MutliSelect->addItem(QString::fromLocal8Bit("����"));

	ui.da2freq->setText(QString::number(da_freq));
	ui.da2samplepoint->setText(QString::number(da_sample));
	ui.da2cycle->setText(QString::number(da_cycle));
	ui.da2start->setCheckable(true);
	ui.da2start->setCheckState(Qt::Unchecked);
	//DA3

	ui.da3MutliSelect->setMaxVisibleItems(10);
	ui.da3MutliSelect->addItem(QString::fromLocal8Bit("���Ҳ�"));
	ui.da3MutliSelect->addItem(QString::fromLocal8Bit("����"));

	ui.da3freq->setText(QString::number(da_freq));
	ui.da3samplepoint->setText(QString::number(da_sample));
	ui.da3cycle->setText(QString::number(da_cycle));
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

/******************************
��ʼ����·
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

			
			ui.textBrowser->append(QString::fromLocal8Bit("�������豸��:")+QString::number(devNum));
			qDebug() << linkdevice << endl;
			device_flag = true;

			return;
		}
	}
	ui.textBrowser->append(QString::fromLocal8Bit("δ�����豸����������²���豸"));
	device_flag = false;
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
		ui.textBrowser->append(QString::fromLocal8Bit("��ǰѡ��ͨ��Ϊ��"));
		QString selectchn_str = "";
		for (int i=0;i<24;i++)
		{
			if (selectedChn[i])
			{
				selectchn_str += QString::number(i)+" ";
				
			}
		}
		ui.textBrowser->append(selectchn_str);
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
  //�����ɼ����ܲۺ���
***************************************************/
void HectoBioWindow::on_startCtnAcq_btn() {
	//��ʼ�����ɼ�֮ǰ���ж��豸�Ƿ�����
	if (1)
	{
		if (!device_flag)
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("δ��⵽�����豸,�������豸"), QMessageBox::Yes);
			ui.textBrowser->append(QString::fromLocal8Bit("δ��⵽�����豸,�������豸"));
			return;
		}
	//�Ƿ�ѡ��ͨ��  ���û��ѡ��ͨ��  ����
		if (selected_chn_sum == 0)
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("���ѡ���ͨ��"), QMessageBox::Yes);
			ui.textBrowser->append(QString::fromLocal8Bit("�빴ѡ���ͨ��"));
			return;
		}
	}
	
	/*****************************************************
	* //���ѡ��ͨ������ȡAD����
	������ź���ʾ���ܲ��֣���ʾ�����ݴ��ݵ�ͨ������ʾ��
	******************************************************/
	ui.textBrowser->append(QString::fromLocal8Bit("�����ɼ�ģʽ"));
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
		ui.textBrowser->append(QString::fromLocal8Bit("��ʼ��AD�ɹ�"));
	}
	//��ֹ�ɼ�����
	setTrigPara(false);

	//�ǵ�������ݸ��º�ͼ��ʵʱ��ʾ
	//�����߳���
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
	//���·��仺����
	if (dataBuff)
	{
		for (i=0;i<MAX_SEGMENT;i++ )
		{
			//delete[] dataBuff[i];
			dataBuff[i] = NULL;
		}
	}
	//�໺��
	for (i=0;i<MAX_SEGMENT;i++)
	{
		dataBuff[i] = new USHORT[samcnt];//ÿ�����������һ��ͨ��AD����
	}
	//��ʼ���໺���־
	for (i=0;i<MAX_SEGMENT;i++)
	{
		NewSegmentData[i] = FALSE;
	}
	ReadIndex = 0;

	ADRun = TRUE;
	display_flag = ui.ctnscqDisplay->isChecked();
	save_flag = !display_flag;
	//������ȡ�̺߳���ʾ�߳�
	if (ADRun && save_flag)
	{
		m_readThread = new ReadThread(this);
		//�޸Ĵ����ź� ��Ҫ�����豸�ź�ͨ����
		connect(this, SIGNAL(startReadThread(QString,HANDLE&)), m_readThread, SLOT(recvMegFromMain(QString,HANDLE&)));
		//m_readThread->start();
		//
		//connect(this->m_readThread, SIGNAL(finished()), this, SLOT(FinishThread()));
		emit startReadThread(QString("��ã��������߳�"),linkdevice);
		m_readThread->start();
	}
	else if (ADRun && display_flag)
	{
		//��ʼ��signalPlot
		//on_disp_btn();
		//displayThread = new DisplayThread(this);
		displayUpdateTimer = new QTimer(this);
		connect(displayUpdateTimer, &QTimer::timeout, this, &HectoBioWindow::timerUpdate);
		displayUpdateTimer->start(1000);//1s����һ��
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
		emit startDisplayThread(QString("��ã���ʾ���߳�"),linkdevice);
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

	//�Ž��߳�����
	bool stutas = USB2069_ReadAD(linkdevice, dataBuff[0], read_data_length);

	/*const char* savefile = "C:\\Users\\LJ\\Desktop\\�ź��ļ�\\CH1.h5";
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


	//�Ž��߳�����
	bool stutas = USB2069_ReadAD(linkdevice, dataBuff[0], read_data_length);

	/*const char* savefile = "C:\\Users\\LJ\\Desktop\\�ź��ļ�\\CH1.h5";
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
		saveDataAsText(QString::fromLocal8Bit("C:\\Users\\LJ\\Desktop\\�ź��ļ�\\CH1.h5"), data);
		//saveDataAsStream(QString::fromLocal8Bit("C:\\Users\\LJ\\Desktop\\�ź��ļ�\\CH1.h5"), data);
		//saveDataAsHdf5("C:\\Users\\LJ\\Desktop\\�ź��ļ�\\CH1.h5", data);
	}
}




/*************************************************
   //ֹͣ�����ɼ��ۺ���
***************************************************/
void HectoBioWindow::on_stopCtnAcq_btn() {
	if (1)
	{
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
			if (USB2069_StopAD(linkdevice, devNum))
			{
				ui.textBrowser->append(QString::fromLocal8Bit("�ر�AD�ɹ�"));
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
	
	//ʹ�ܲɼ�������ť
	setTrigPara(true);

	return;
}

void HectoBioWindow::setTrigPara(bool set) {
	//ͨ��
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

	//�����ɼ�����
	ui.trigModel->setEnabled(set);
	ui.trigSource->setEnabled(set);
	ui.trigLength->setEnabled(set);
	ui.sampleFreq->setEnabled(set);
	ui.trigLevel->setEnabled(set);
	ui.delayTime->setEnabled(set);
	ui.acqparaBtnOK->setEnabled(set);
	//�����ɼ��͵��βɼ�����
	ui.ctnscqStart->setEnabled(set);
	ui.ctnscqEnd->setEnabled(!set);
	ui.singleacqEnd->setEnabled(set);
	ui.ctnscqDisplay->setEnabled(set);
	ui.ctnacqMutliSelcet->setEnabled(set);
	ui.singleacqTime->setEnabled(set);
}

/*************************************************
   //���βɼ��ۺ���
***************************************************/
void HectoBioWindow::on_singleAcq_btn() {
	
	ui.ctnscqStart->setEnabled(false);
	ui.ctnscqEnd->setEnabled(false);
	ui.singleacqEnd->setEnabled(false);

	if (0)
	{
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
	}
	

	if (0)
	{
		//�����û��ʼ���͵����������������⣬��pass
			acq_flag = SINGLE_ACQ;
			ui.textBrowser->append(QString::fromLocal8Bit("���βɼ�ģʽ��"));

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

			//��ȡ���ݳ��ȣ��������ֻ��ȡһ����������
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
				ui.textBrowser->append(QString::fromLocal8Bit("��ʼ��AD�ɹ�"));
			}

			PUSHORT inBuffer = NULL;
			inBuffer = new USHORT[samcnt];
			readUSB(inBuffer, samcnt);

			LONG bufover=0;
			if (!USB2069_GetBufOver(linkdevice,&bufover))
			{
				ui.textBrowser->append(QString::fromLocal8Bit("��ȡ�������λʧ��"));
			}
			if (bufover==1)
			{
				ui.textBrowser->append(QString::fromLocal8Bit("�������"));
			}


			//�ǵ�������ݸ��º�ͼ��ʵʱ��ʾ
			/*Sleep(ui.singleacqTime->text().toInt());*/

			//�����ɼ�
			if (USB2069_StopAD(linkdevice,devNum))
			{
				ui.textBrowser->append(QString::fromLocal8Bit("�ر�AD�ɹ�"));
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
					//��ȡ����
					for (int j = 0; j < (int)sscnt; j++)
					{
						buf[j] = inBuffer[selected_chn_sum * j + chcnt];
						sst += (USHORT)(buf[j]);
					}

					char filename[100];

					sprintf(filename, "C:\\Users\\LJ\\Desktop\\�ź��ļ�\\CH%d.h5", (k + 1));

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
	//һ�ζ���samcnt�������ݣ�����һ���������ȣ����samcnt����READ_MAX_LEN�����Ϊ��ζ�ȡ
	ULONG DataOver = 0; //���������ָʾ
	ULONG rlen = 0;//ÿ�ζ�ȡ����
	ULONG alen = 0;//�Ѿ���ȡ����
	int rcnt = 0;//�ܹ���Ҫ��ȡ�Ĵ���
	if ((bufsize % READ_MAX_LEN) == 0)//�����ȡ�ĳ��ȣ��պ�����������ȡ���ȵ�������
		rcnt = (bufsize / READ_MAX_LEN);
	else
		rcnt = (bufsize / READ_MAX_LEN) + 1;
	//�ֶ�ζ�ȡ��ÿ�ζ���󳤶���READ_MAX_LEN
	for (int i = 0; i < rcnt; i++)
	{
		if (i == (rcnt - 1))//��������һ��
		{
			if ((bufsize % READ_MAX_LEN) == 0)//����պ���������
				rlen = READ_MAX_LEN;
			else
				rlen = bufsize % READ_MAX_LEN;
		}
		else
		{
			rlen = READ_MAX_LEN;
		}
		//���� 
		if (!USB2069_ReadAD(linkdevice, pBuf + alen, rlen))
		{
			ui.textBrowser->append(QString::fromLocal8Bit("��ȡ����ʧ��"));
			
			break;
			return FALSE;
		}
		//�ж������Ƿ����
		alen += rlen;
	}
	return TRUE;
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
		ui.textBrowser->append(QString::fromLocal8Bit("DA0�������"));
		
	}
	delete[] data_buf;


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
		ui.textBrowser->append(QString::fromLocal8Bit("DA2�������"));
		ui.textBrowser->append(ui.da_offset_voltage->text());
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
	

	QString offset_text;
	
	long val[24] = { 0 };
	//Ϊ24��ͨ����ѡ�е�ͨ����������ƫ
	for (LONG i=0;i<24;i++)
	{
		
		if (selectedChn[i])
		{
			if (!USB2069_ADoffset(linkdevice, READOFFSET, i, &val[i]));
			{
				ui.textBrowser->append(QString::fromLocal8Bit("��ȡ��ƫʧ��"));
				return;
			}
		}
		
	}
	ui.textBrowser->append(QString::fromLocal8Bit("��ȡ��ƫ�ɹ�"));
	
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

	
	
	long recover_offset;
	try
	{
		recover_offset = ui.baseline_input->text().toDouble();
		
		for (LONG i=0;i<24;i++)
		{
			if (selectedChn[i])
			{
				if (!USB2069_ADoffset(linkdevice, WRITEOFFSET, i, &recover_offset)) {
					ui.textBrowser->append(QString::fromLocal8Bit("д����ƫʧ��"));
					return;
				}
			}
			
		}
		ui.textBrowser->append(QString::fromLocal8Bit("д����ƫ�ɹ�\r")+ui.baseline_input->text());
			

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
   �ź���ʾ��ģ��
   //�ź���ʾ�ۺ���
***************************************************/
void HectoBioWindow::on_disp_btn() {
	disp_show_flag = true;
	//ui.signaldispOpenGL->DrawWindowBackground();
	//QwtPlot* myplot = new QwtPlot("curve display");
	curve->setSamples(time, val, 500);
	curve->attach(ui.signalPlot);
	ui.signalPlot->replot();
	ui.textBrowser->append(QString::fromLocal8Bit("��ʼ��ʾ"));
}
/*************************************************
   //ɾ���źŲۺ���
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
	ui.textBrowser->append(QString::fromLocal8Bit("�����ʾ"));

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
	}
	disp_stop_flag = disp_stop_flag ^ true;
	this->killTimer(timer);
	//ui.signalPlot->update();
	//ui.signalPlot->close();

	//ui.signalPlot->replot();
	ui.textBrowser->append(QString::fromLocal8Bit("�ݶ��ź���ʾ"));

	//��ͣ����ԷŴ���С�ȹ���

	QwtPlotZoomer* zoomer = new QwtPlotZoomer(ui.signalPlot->canvas());
	//ѡ����ɫ����Ϊ��ɫ
	zoomer->setRubberBandPen(QColor(Qt::red));

	zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
	zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton);
	zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);

	zoomer->setEnabled(true);

	//delete�ڴ�
	for (int i = 0; i < MAX_SEGMENT; i++)
	{
		delete[] dataBuff[i];
	}

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

	ui.textBrowser->append(QString::fromLocal8Bit("��ǰ��ʾ���ʣ�") + QString::fromStdString(text));

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

	ui.textBrowser->append(QString::fromLocal8Bit("��ǰ��ʾ���ʣ�")+QString::fromStdString(text));

	return;
}

/*************************************************
   ����AD���ݣ���·��֮�����
***************************************************/
bool HectoBioWindow::updateAD_data(PUSHORT data_buf) {

	//ʾ���ź�
	//��������ź�
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

//��ȡ�߳�
void ReadThread::run() {
	//��ȡ�߳���Ҫ������
	//QString readf = QString::fromLocal8Bit("��ȡ������...");
	//��ȡ����
	

	//��ȡ���֮�󣬷�����ȡ����ź�
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


//��ʾ�߳�
DisplayThread::DisplayThread(QObject* obj):
	m_obj(obj){
	//linkdevice = 0;
	/*SignalDisplay* showWindow = new SignalDisplay(nullptr);
	showWindow->show();*/
}

void DisplayThread::run() {
	//��ʾ�߳���Ҫ������   �߳����ݴ�����qwtplotcurve��
	// 
	//QString displayf = QString::fromLocal8Bit("��ʾ������...");
	////��ʾ����
	//

	////��ʾ��ϣ���֮�󣬷�����ʾ����ź�
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
		//�����¼�����������ʾ
		//emit--->��������

	}
}

void DisplayThread::recvMegFromMain(QString line,HANDLE& linkdevice) {
	qDebug() << "display thread has recieve" << line.toStdString().c_str() << endl;
	
}

void DisplayThread::recvStopSignal(bool stop_flag) {
	this->stop_flag = stop_flag;

	qDebug() << "stop display thread" << endl;
}
