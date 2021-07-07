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
	//�����������������װ������/������ͣ+����
	if (count>maxsize)
	{
		hdf5file->save_file((double*)getPlotY(), 0, count, "", "dataset");
	}
}


void TimeSerialData::append(quint64 ms, double value) {
	dataMutex.lock();
	count++;
	//β����������
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

	//����洢���ݵ���Ŀ����Ԥ�������Ŀ������ͷ��
	if (count>maxsize)
	{
		this->ms.pop_front();
		this->value.pop_front();
	}

	dataMutex.unlock();
}
