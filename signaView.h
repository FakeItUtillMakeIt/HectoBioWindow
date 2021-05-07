#pragma once

#include <QWidget>
#include "ui_signaView.h"

class signaView : public QWidget
{
	Q_OBJECT

public:
	signaView(QWidget *parent = Q_NULLPTR);
	~signaView();

private:
	Ui::signaView ui;
};
