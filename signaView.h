#pragma once

#include <QWidget>
#include "ui_signaView.h"
#include "QVector"
#include "string"
#include "QString"
#include "qwt_plot_curve.h"
#include "QFileDialog"
#include "QMessageBox"
#include <iostream>
#include <windows.h>
#include <string>
#include <atlstr.h>
#include <QDir>
#include "direct.h"
#include "Hdf5Read.h"
#include "qwt_plot_zoomer.h"

class signaView : public QWidget
{
	Q_OBJECT

public:
	signaView(QWidget *parent = Q_NULLPTR);
	~signaView();

private:
	Ui::signaView uivw;

	QVector <QString> allfile;
	QString new_good_dir;
	QString new_bad_dir;

	QString selectfilename;

	int total_row = 0;
	int current_row = 0;
	int have_op_count = 0;

	QwtPlotCurve* curve;

	void readDataFromONT(const char* read_file, QwtPlotCurve* curve);


private slots:
	void selectFileBtn();

	void goodSignalBtn();
	void badSignalBtn();

	void displayBtn();

};
