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

	//�Ƚϴ���

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
		ui.textBrowser->append(QString::number(device_flag));
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

	bool flag = circuit_set->init_start_AD_device();

	char* ret_info = circuit_set->ret_flag;
	ui.textBrowser->append(QString::fromLocal8Bit(ret_info));


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

	//���βɼ�
	//�ɼ�ʱ��
	ui.singleacqTime->setText(QString::number(0));

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

	//��������
	//ͨ������
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
   ����ʦ����ıȽϹ���
***************************************************/
//�ȽϽ���ۺ���

void HectoBioWindow::on_compare_btn() {
	CompareWidgets* cmpwindow = new CompareWidgets();
	cmpwindow->setWindowTitle(QString::fromLocal8Bit("���źűȽϽ���"));
	
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
   ����������Ĵ�ֹ���
***************************************************/
//��ֽ���ۺ���
void HectoBioWindow::on_signalView_btn() {
	signaView* signalviewWindow = new signaView();
	signalviewWindow->setWindowTitle(QString::fromLocal8Bit("�źŴ�֣����ڲ�ʵ����Ա������"));
	signalviewWindow->show();
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
}

/*************************************************
   ѡ��ͨ���ۺ���
***************************************************/
void HectoBioWindow::on_selectChnOK_btn() {

	if (!device_flag)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error������"), QString::fromLocal8Bit("δ��⵽�����豸,�������豸"), QMessageBox::Yes);
		ui.textBrowser->append(QString::fromLocal8Bit("δ��⵽�����豸,�������豸"));
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

}


/*************************************************
  //�����ɼ����ܲۺ���
***************************************************/
void HectoBioWindow::on_startCtnAcq_btn() {
	//��ʼ�����ɼ�֮ǰ���ж��豸�Ƿ�����
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
	/*****************************************************
	* //���ѡ��ͨ������ȡAD����
	������ź���ʾ���ܲ��֣���ʾ�����ݴ��ݵ�ͨ������ʾ��
	******************************************************/
	acq_flag = CONTINUE_ACQ;
	PUSHORT data_buf = NULL;
	//data_buf Ϊʵʱ����
	//�ӵ�·���ֱ�Ӷ�
	data_buf = new USHORT[circuit_set->read_data_length];
	bool circuit_read_flag = circuit_set->read_AD(data_buf, circuit_set->read_data_length);

	//������ʾ���ź�  �ж�AD��ȡ�Ƿ�ɹ�
	if (!circuit_read_flag)
	{
		ui.textBrowser->append(QString::fromLocal8Bit(circuit_set->ret_flag));
		circuit_set->stop_AD_device();
		return;
	}
	updateAD_data(data_buf);

}

/*************************************************
   //ֹͣ�����ɼ��ۺ���
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
}

/*************************************************
   //���βɼ��ۺ���
***************************************************/
void HectoBioWindow::on_singleAcq_btn() {
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
	acq_flag = SINGLE_ACQ;

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

	this->killTimer(timer);
	curve->setData(0);
	//ui.signalPlot->detachItems();
	ui.signalPlot->replot();
	ui.textBrowser->append("eraser signal plot");
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
	ui.textBrowser->append("stop signal plot");

	//��ͣ����ԷŴ���С�ȹ���

	QwtPlotZoomer* zoomer = new QwtPlotZoomer(ui.signalPlot->canvas());
	//ѡ����ɫ����Ϊ��ɫ
	zoomer->setRubberBandPen(QColor(Qt::red));

	zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
	zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton);
	zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);

	zoomer->setEnabled(true);
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

	if (save_name == "")
	{
		return;
	}
	//��������
	//��ѡ�����ʽ
	//saveDataAsStream(save_name,savetestSignal);

	saveDataAsHdf5(save_name.toStdString().c_str(), savetestSignal);

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
}