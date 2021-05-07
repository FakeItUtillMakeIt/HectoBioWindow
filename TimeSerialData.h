#pragma once
#ifndef TIMESERIALDATA_H
#define TIMESERIALDATA_H

#include <QVector>
#include <QMutex>

class TimeSerialData
{
public:
	TimeSerialData();
	void append(quint64 ms, double value);
	int getCount() const;
	const double* getPlotX() const;
	const double* getPlotY() const;
	int getPlotCount() const;

	int getmsstart() {
		return ms.at(0);
	}
	int getmsstop() {
		return ms.at(ms.size() - 1);
	}

	QMutex dataMutex;

protected:

private:
	QVector<double> ms;
	QVector<double> value;
	double maxValue;
	double minValue;
	int count;
	int maxsize;
 };

#endif //TIMESERIALDATA_H