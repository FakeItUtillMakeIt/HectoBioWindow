#pragma once
//���һ����·������
//���ص�·��������
//��·����
#pragma comment(lib,"USB2069.lib")
#include <usb2069.h>
#include <string>

class Circuit
{
public:
	BOOL bsoftTrig;//���������־
	//�ɼ�������ʼ���ṹ��
	//�������������ò���
	USB2069_PARA_INIT para_init;

	//ѡ�н���Ĳ��� Ĭ��Ϊ��������:0 �󴥷���1 ��ʱ��2
	int trig_mode=0;
	//ͨ����Ĭ��ȫΪfalse
	bool channel[24];
	//����Դ �����أ�0 �⸺�أ�1 ��Ե������2
	int trig_source = 0;
	//����Ƶ��
	int sample_freq = 0;
	//������ƽ
	int trig_level = 0;
	//��������
	int trig_length = 0;
	//��ʱ
	int delay = 0;
	//AD�豸

	//read_data_length
	int read_data_length = 0;
	//�豸��  Ĭ��ֻ����һ�� ����Ϊ0
	UCHAR DeviceNO=0;
	HANDLE link_device =nullptr;
	//��ʼ��AD������Ϣ
	char* ret_flag;
	bool device_flag=false;
	bool init_AD_flag;

	void init_circuit_para();
	bool init_start_AD_device();

public:
	
	//����
	bool init_usb2069_AD();
	bool judge_high_usbdevice(PUCHAR hign_device);
	int judge_link_device();
	bool judge_unlink_device();
	//bool init_start_AD_device();
	bool stop_AD_device();
	//
	bool set_DO(LONG pDO);
	bool get_DI(PLONG pDI);
	//У����ƫ
	bool recover_ADoffset(LONG AD_offset, bool WtRdflag = 0,int Chn=0);
	//��ȡ
	bool get_bufover(PLONG pBufOVer);
	bool get_freq(double freq=0);
	bool read_AD(PUSHORT data_buffer, LONG data_length);

	bool set_DA(int DA_num, bool DA_enable, int DA_freq,
		bool DA_loop, int DA_cycleCnt, bool DA_stopflag, PUSHORT DA_buf,
		int DA_cyclePoint);
};

