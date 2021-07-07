#include "Circuit.h"


void Circuit::init_circuit_para() {
	//ͨ��Ĭ������ͨ��
	for (int i=0;i<24;i++)
	{
		para_init.lEnCh[i] = true;
	}

	//����Դ
	para_init.TriggerSource = TRIG_SRC_EXT_RISING;
	//�ɼ�ģʽ
	para_init.TriggerMode = TRIG_MODE_CONTINUE;
	//������
	para_init.ADFREQ = 240000;
	//��������
	para_init.TriggerLength = 10;
	para_init.TriggerDelay = 0;
	para_init.TriggerLevel = 2048;
	

	return;
}

bool Circuit::init_usb2069_AD() {
	return 0;
}

bool Circuit::judge_high_usbdevice(PUCHAR high_device) {
	//UCHAR hign_device = 1;
	bool is_highdevice = USB2069_IsHighDevice(link_device, high_device);
	if (is_highdevice)
	{
		return true;
	}
	return false;
}


int Circuit::judge_link_device() {
	
	link_device = USB2069_Link(DEFAULT_DEVICE_NUM);
	if (link_device !=INVALID_HANDLE_VALUE)
	{
		device_flag = true;
		return true;
	}
	else
	{
		return false;
	}
	
}
bool Circuit::judge_unlink_device() {
	
	//link_device = USB2069_Link(DEFAULT_DEVICE_NUM);
	BOOL unlink_flag = USB2069_UnLink(link_device);
	if (!unlink_flag)
	{
		//���豸ʧ��
		return true;
	}
}

//��ʼ��AD�ɼ�����������AD�ɼ�
//����AD�ɼ�֮����������ܻ�ȡ����
bool Circuit::init_start_AD_device() {

	//device_flag = judge_link_device();
	if (!device_flag)
	{
		ret_flag = "��ȡ�����豸ʧ��";
		return false;
	}

	//����ѡ��ͨ��
	for (int i=0;i<24;i++)
	{
		para_init.lEnCh[i] = LONG(channel[i]);
		
	}

	//���´���ģʽ
	switch (trig_mode)
	{
	case 0:
		para_init.TriggerMode = TRIG_MODE_CONTINUE;
		break;
	case 1:
		para_init.TriggerMode = TRIG_MODE_POST;
		break;
	case 2:
		para_init.TriggerMode = TRIG_MODE_DELAY;
		break;
	default:
		para_init.TriggerMode=TRIG_MODE_CONTINUE;
	}

	//���´���Դ
	switch (trig_source)
	{
	case 0:
		para_init.TriggerSource = TRIG_SRC_EXT_RISING;
		break;
	case 1:
		para_init.TriggerSource = TRIG_SRC_EXT_FALLING;
		break;
	case 2:
		para_init.TriggerSource = TRIG_SRC_SOFT;
		break;
	default:
		para_init.TriggerSource = TRIG_SRC_EXT_RISING;
	}
	//���²���Ƶ��
	para_init.ADFREQ = sample_freq;
	//���´�������
	para_init.TriggerLength = trig_length;
	//���´�����ƽ
	para_init.TriggerLevel = trig_level;
	//���´�����ʱ������ʱ����ģʽ��Ч
	para_init.TriggerDelay = delay;
	//����ÿ�ζ�ȡ��������
	read_data_length = 1024 * 100;

	init_AD_flag= USB2069_InitAD(link_device,&para_init);
	if (!init_AD_flag)
	{
		ret_flag = "��ʼ��ADʧ�ܣ������豸";
		return false;
	}
	else
	{
		ret_flag = "��ʼ��AD�ɹ�����ʼ�ɼ�����";
		return true;
	}

	return 0;
}


bool Circuit::stop_AD_device() {
	USB2069_StopAD(link_device,DEFAULT_DEVICE_NUM);
	return 0;
}

//�������
bool Circuit::set_DO(LONG pDO) {
	bool set_DOflag = USB2069_SetDO(link_device,pDO);
	if (set_DOflag)
	{
		return true;
	}
	return false;
}

//��������
bool Circuit::get_DI(PLONG pDI	) {
	bool get_DIfalg = USB2069_GetDI(link_device, pDI);
	if (get_DIfalg)
	{
		return true;
	}
	return false;
}

//У����ƫ
// ��ƫ��ȡ WtRd_flag=0 :д 1:��
// Chn:0-23 ��ʾͨ��1-24
//ȡֵ��Χ-32768-32767
bool Circuit::recover_ADoffset(LONG AD_offset,bool WtRdflag,int Chn) {
	if (AD_offset<-32768 )
	{
		AD_offset = -32768;
	}
	else if (AD_offset>32767)
	{
		AD_offset = 32767;
	}
	bool recover_ADoffsetfalg = USB2069_ADoffset(link_device,WtRdflag,Chn,&AD_offset);
	if (recover_ADoffsetfalg)
	{
		return true;
	}
	return false;
}


//��ȡӲ���������λ
//�������pBufOVer ����1�����򷵻�0
bool Circuit::get_bufover(PLONG pBufOVer) {
	bool get_bufoverfalg = USB2069_GetBufOver(link_device, pBufOVer);
	if (get_bufoverfalg)
	{
		return true;
	}
	return false;
}

//��Ƶ��
//freq[0]���ز�Ƶͨ��1ֵ��freq[1]���ز�Ƶͨ��2ֵ��
//freq[2]���ز�Ƶͨ��3ֵ��freq[3]���ز�Ƶͨ��4ֵ��
//Ƶ����ֵ=FRQ_BASE_CLOCK/freq[n]
bool Circuit::get_freq(double* freq) {
	LONG pData[4];
	bool freqflag = USB2069_GetFreq(link_device, pData);
	if (freqflag)
	{
		char output[100];
		if (pData[0]>0)
		{
			*freq = (double)FRQ_BASE_CLOCK/(double)pData[0];
			
		}
		else
		{
			*freq = 0;
		}
		if (pData[1]>0)
		{
			*freq = (double)FRQ_BASE_CLOCK / (double)pData[1];
		}
		else 
		{
			*freq = 0;
		}
		if (pData[2] > 0)
		{
			*freq = (double)FRQ_BASE_CLOCK / (double)pData[2];
		}
		else
		{
			*freq = 0;
		}
		if (pData[3] > 0)
		{
			*freq = (double)FRQ_BASE_CLOCK / (double)pData[3];
		}
		else
		{
			*freq = 0;
		}

		return true;
	}
	return false;
}

//��AD����  ����data_buffer
//����FCFR_USB_2069˵����˵��
//AD����Ӧ��תΪʵ������
//ת����ʽ��v_value=10.0*AD_data/0xFFFF-5.0
bool Circuit::read_AD(PUSHORT data_buffer,ULONG data_length) {
	if (init_AD_flag)
	{
		bool read_data_flag = USB2069_ReadAD(link_device, data_buffer, data_length);

		if (!read_data_flag)
		{
			ret_flag = "��ȡADʧ��";
			return false;
		}
		else {
			ret_flag = "��ȡAD�ɹ�";
		}
		
		stop_AD_device();
		return true;
	}
	
}

//����DA ����תģ��  24λ
bool Circuit::set_DA(int DA_num, bool DA_enable, long DA_freq, bool DA_loop, int DA_cycleCnt, bool DA_stopflag,PUSHORT DA_buf, int DA_cyclePoint) {
	//����USB_DA����
	
	/*bool set_DA_flag=USB2069_SetDA(link_device, DA_num, DA_enable, &DA_freq,
		DA_loop, DA_cycleCnt, DA_stopflag, DA_buf, DA_cyclePoint);*/
	bool set_DA_flag = USB2069_SetDA(link_device, DA_num, DA_enable, &DA_freq,
		DA_loop, DA_cycleCnt, DA_stopflag, DA_buf,DA_cyclePoint);
	
	return set_DA_flag;
}

