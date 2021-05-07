#include "TimeSerialData.h"

TimeSerialData::TimeSerialData() {
	count = 0;
	maxsize = 100;
	maxValue = -100000;
	minValue = 100000;
}

int TimeSerialData::getCount() const{
	return count;
}

int TimeSerialData::getPlotCount() const {
	if (count>maxsize)
	{
		return maxsize;
	}
	return count;
}

const double* TimeSerialData::getPlotX() const {
	return ms.data();
}

const double* TimeSerialData::getPlotY() const {
	return value.data();
}

void TimeSerialData::append(quint64 ms, double value) {
	dataMutex.lock();
	count++;
	//
	this->ms.append(ms);
	this->value.append(value);

	if (minValue>value)
	{
		minValue = value;
	}
	if (maxValue<value)
	{
		maxValue = value;
	}

	if (count>maxsize)
	{
		this->ms.pop_front();
		this->value.pop_front();
	}
	dataMutex.unlock();
}
