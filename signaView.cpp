//信号打分显示窗口

#include "signaView.h"



using namespace std;


signaView::signaView(QWidget *parent)
	: QWidget(parent)
{
	uivw.setupUi(this);

	connect(uivw.selectFileDirBtn, &QPushButton::clicked, this, &signaView::selectFileBtn);
	connect(uivw.goodSignalBtn, &QPushButton::clicked, this, &signaView::goodSignalBtn);
	connect(uivw.badSignalBtn, &QPushButton::clicked, this, &signaView::badSignalBtn);

	connect(uivw.displayBtn, &QPushButton::clicked, this, &signaView::displayBtn);
}

signaView::~signaView()
{
}

void signaView::selectFileBtn() {
	QFileDialog* filedialog =new QFileDialog(this);
	filedialog->setWindowTitle("select file");
	filedialog->setDirectory(".");
	//设置文件过滤器
	filedialog->setNameFilter(tr("Serials File(*.fast5 *.f5)"));
	//可以选择多个文件
	filedialog->setFileMode(QFileDialog::ExistingFiles);
	//设置视图模式
	filedialog->setViewMode(QFileDialog::Detail);
	QStringList filenames;
	if (filedialog->exec())
	{
		filenames = filedialog->selectedFiles();
	}

	for (auto tmp : filenames)
	{
		uivw.selectAllFile->insertItem(total_row++,tmp);
		allfile.append(tmp);
	}

	//获取当前运行目录
	string projectpath = getcwd(nullptr,1000);
	QDir dir;
	dir.cd("./");
	if (!dir.exists("good"))
	{
		dir.mkdir("good");
	}
	if (!dir.exists("bad"))
	{
		dir.mkdir("bad");
	}

	new_good_dir = dir.absolutePath() + "/good/";
	new_bad_dir = dir.absolutePath() + "/bad/";

	//CreateDirectory();
	
}

void signaView::displayBtn() {
	if (total_row == have_op_count)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("文件已处理完"), QMessageBox::Yes);
		//return;
	}
	
	QListWidgetItem * current_item = uivw.selectAllFile->item(current_row);
	selectfilename = current_item->text();
	curve = new QwtPlotCurve(selectfilename);
	readDataFromONT(selectfilename.toStdString().c_str(), curve);

	QColor color = Qt::blue;
	curve->setPen(color, 2);
	curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
	curve->setLegendAttribute(curve->LegendShowLine);

	curve->attach(uivw.judgeSignalPlot);

	
	uivw.judgeSignalPlot->setTitle(QwtText("SIGNAL PLOT"));
	uivw.judgeSignalPlot->setAxisTitle(QwtPlot::yLeft, "current / mA");
	uivw.judgeSignalPlot->setAxisTitle(QwtPlot::xBottom, "time / mS");



	QwtPlotZoomer* zoomer = new QwtPlotZoomer(uivw.judgeSignalPlot->canvas());
	//选框颜色设置为红色
	zoomer->setRubberBandPen(QColor(Qt::red));

	zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
	zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton);
	zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);

	zoomer->setEnabled(true);
}

void signaView::goodSignalBtn() {
	//在选择文件的目录创建两个文件夹
	//放入好的文件夹
	if (total_row>have_op_count++)
	{
		QListWidgetItem* current_item = uivw.selectAllFile->item(current_row);
		
		QString old_path =current_item->text();
		QString new_path = new_good_dir + old_path.split("/").back();
		QFile::copy(old_path, new_path);

		uivw.selectAllFile->takeItem(current_row);

	}
	else
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("文件已处理完"), QMessageBox::Yes);
	}
	
	
	
}


void signaView::badSignalBtn() {
	//在选择文件的目录创建两个文件夹
	//放入坏的文件夹
	if (total_row > have_op_count++)
	{
		QListWidgetItem* current_item = uivw.selectAllFile->item(current_row);

		QString old_path = current_item->text();
		QString new_path = new_bad_dir + old_path.split("/").back();
		QFile::copy(old_path, new_path);

		uivw.selectAllFile->takeItem(current_row);
	}
	else
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("error！！！"), QString::fromLocal8Bit("文件已处理完"), QMessageBox::Yes);
	}


}

void signaView::readDataFromONT(const char* read_file, QwtPlotCurve* curve) {
	Hdf5Read* hdf5read = new Hdf5Read();

	int  dim;


	const char* db_name = "Signal";
	double* data = hdf5read->read_ont_file(dim, read_file, db_name);

	double* x = (double*)malloc(sizeof(double) * dim);
	for (int i = 0; i < dim; i++)
	{
		*(x + i) = (double)i;
	}

	curve->setSamples(x, data, dim);
}
