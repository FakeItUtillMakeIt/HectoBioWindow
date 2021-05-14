#include "TimeSerialData.h"
#include "Hdf5Read.h"

TimeSerialData::TimeSerialData() {
	count = 0;
	maxsize = 100;
	maxValue = -100000;
	minValue = 100000;

	hdf5file = new Hdf5Read();
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

void TimeSerialData::insert() {
	this->ms.insert(0,1.0);
}

const double* TimeSerialData::getPlotY() const {
	return value.data();
}


void TimeSerialData::timerEnvent() {
	double rand_data = qrand() % maxsize;
	this->append(count,rand_data);
	//如果计算器大于最大可装载数量/按下暂停+保存
	if (count>maxsize)
	{
		hdf5file->save_file((double*)getPlotY(), 0, count, "", "dataset");
	}
}


void TimeSerialData::append(quint64 ms, double value) {
	dataMutex.lock();
	count++;
	//尾部更新数据
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

	//如果存储数据的数目超过预设最大数目，弹出头，
	if (count>maxsize)
	{
		this->ms.pop_front();
		this->value.pop_front();
	}

	dataMutex.unlock();
}
