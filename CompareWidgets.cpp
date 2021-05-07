#include "CompareWidgets.h"
#include <QPushButton>
#include "qwt_plot_zoomer.h"
#include "QAction"
#include "QMenu"
#include "Hdf5Read.h"

//#include "hdf5.h"
#include <QAbstractItemView>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <Qvariant.h>
#include "QFileDialog"
#include "qDebug"
#include "qwt_plot.h"


CompareWidgets::CompareWidgets(QWidget *parent)
	: QWidget(parent)
{
	uicw.setupUi(this);

	for (int i = 0; i < 500; i++)
	{
		time[i] = i;
	}


	createActions();
	createContentMenu();
	//this->resize(300, 400);
	//�ԱȰ�ť
	connect(uicw.compareBtn, &QPushButton::clicked, this, &CompareWidgets::on_compare_btn);
	//�����ť
	connect(uicw.eraserAll_btn, &QPushButton::clicked, this, &CompareWidgets::on_eraser_Allplot_btn);

}

CompareWidgets::~CompareWidgets()
{
}



void CompareWidgets::createActions() {

	connect(uicw.selectFileBtn, &QPushButton::clicked, this, &CompareWidgets::on_select_file_btn);

}

void CompareWidgets::createContentMenu() {

}

void CompareWidgets::createMenu() {

}

//ѡ���ļ��ۺ���
void CompareWidgets::selcetFile() {
	QFileDialog* filedialog = new QFileDialog(this);
	filedialog->setWindowTitle("select file");
	filedialog->setDirectory(".");
	//�����ļ�������
	filedialog->setNameFilter(tr("Serials File(*.fast5 *.f5 *.txt)"));
	//����ѡ�����ļ�
	filedialog->setFileMode(QFileDialog::ExistingFiles);
	//������ͼģʽ
	filedialog->setViewMode(QFileDialog::Detail);
	QStringList filenames;
	if (filedialog->exec())
	{
		filenames = filedialog->selectedFiles();
	}
	for (auto tmp : filenames)
	{
		uicw.selectedFile->append(tmp);
	}
	uicw.selectedFile->show();

	this->selectList(filenames);
}

//��ѡ�д�����ʾ
void CompareWidgets::selectList(QStringList selectedFilename) {

	uicw.selectedList->setSelectionMode(QAbstractItemView::ContiguousSelection);
	int num = 0;
	for (auto tmp : selectedFilename)
	{
		uicw.selectedList->insertItem(num, tmp);
		num += 1;
	}

}

//
void CompareWidgets::on_select_file_btn() {
	selcetFile();

}



/********************************************
* �ԱȰ�ť�ۺ���
*****************************************/
//���ܣ���ȡѡ����ļ�
void CompareWidgets::on_compare_btn() {

	//��ȡlistwidget�е�ѡ��
	//����ͼ��
	QwtLegend* legend = new QwtLegend();
	auto selected_file = uicw.selectedList->selectedItems();
	for (auto tmp : selected_file)
	{
		//������ӻ�ȡѡ���ļ�������
		QString endvalue;
		endvalue = tmp->text().split("/").back();
		QwtPlotCurve* curve = new QwtPlotCurve(endvalue);
		QString file_path = tmp->text();
		uicw.selectedFile->append(file_path);

		//readDataAsStream(file_path,curve);
		readDataAsHdf5(file_path.toStdString().c_str(), curve);
		int ranR, ranG, ranB;
		ranR = rand() % 255;
		ranG = rand() % 255;
		ranB = rand() % 255;

		QColor color = QColor(ranR, ranG, ranB);
		curve->setPen(color, 2);
		curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		curve->setLegendAttribute(curve->LegendShowLine);
		legend->setDefaultItemMode(QwtLegendData::Checkable);
		//connect(legend, SIGNAL(Checked(const QVariant &,bool,int)),SLOT(showI));
		uicw.signalCompare->insertLegend(legend);
		curve->attach(uicw.signalCompare);
		//curve->setLegendAttribute(QwtPlotCurve::);
	}



	//uicw.signalCompare->replot();
	uicw.signalCompare->setTitle(QwtText("SIGNAL PLOT"));
	uicw.signalCompare->setAxisTitle(QwtPlot::yLeft, "current / mA");
	uicw.signalCompare->setAxisTitle(QwtPlot::xBottom, "time / mS");



	QwtPlotZoomer* zoomer = new QwtPlotZoomer(uicw.signalCompare->canvas());
	//ѡ����ɫ����Ϊ��ɫ
	zoomer->setRubberBandPen(QColor(Qt::red));

	zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
	zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton);
	zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);

	zoomer->setEnabled(true);

}

void CompareWidgets::on_eraser_Allplot_btn() {
	//uicw.signalCompare->close();
	uicw.signalCompare->detachItems();
	uicw.signalCompare->replot();
}




void CompareWidgets::readDataAsText(QString& read_file, double* time, double* showdata) {
	QFile readfile(read_file);
	readfile.open(QIODevice::ReadOnly);
	QTextStream txtout(&readfile);
	QString linestr;

	for (size_t i = 0; i < 400; i++)
	{
		time[i] = i;
		//linestr = txtout.readLine();
		txtout >> linestr;
		showdata[i] = linestr.toDouble();
	}

}
void CompareWidgets::readDataAsStream(QString& file_path, QwtPlotCurve* curve) {
	FILE* readfile = fopen(file_path.toStdString().c_str(), "r");

	if (readfile == NULL)
	{
		return;
	}
	int count = 0;

	while (!(feof(readfile)) && count < 499)
	{
		double buf[1];
		fread(buf, sizeof(double), 1, readfile);
		/**(showdata + count) = buf[0];
		*(time + count) = count;*/
		val[count] = buf[0];
		time[count] = count;
		count++;
	}

	fclose(readfile);

	curve->setSamples(time, val, 500);

}

void CompareWidgets::readDataAsHdf5(const char* read_file, QwtPlotCurve* curve) {
	Hdf5Read* hdf5read = new Hdf5Read();

	int row;
	int col;
	row = 1;
	col = 500;

	for (int i = 0; i < 500; i++)
	{
		time[i] = i;
	}

	const char* db_name = "Dataset";
	hdf5read->read_file(val, row, col, read_file, db_name);

	qDebug() << val[0] << val[499];

	curve->setSamples(time, val, 499);
}

