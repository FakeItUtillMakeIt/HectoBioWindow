//AD��ʼ������
#include <StdAfx.h>
#include <stdio.h>
#include <tchar.h>


#ifndef _USB2069_PARA_INIT
typedef struct _USB2069_PARA_INIT    
{
	LONG    lSelADClk;           //ʱ��ѡ��Ԥ����
	LONG    ADFREQ;              //������
	LONG	TriggerMode;         //����ģʽ
	LONG	TriggerSource;	     //����Դ 
	LONG    TriggerDelay;        //������ʱ
	LONG    TriggerLength;       //��������
	LONG    TriggerLevel;        //ģ�ⴥ����ƽ
	LONG    lEnCh[24];           //ͨ��ʹ��
} USB2069_PARA_INIT,*PUSB2069_PARA_INIT;
#endif

//ʱ��ѡ��Ԥ��
typedef enum EmADClkSel
{
	ADCLK_INT        = 0, //����ʱ��
	ADCLK_EXT        = 1, //����ʱ��
} ADCLK_SEL;

//����ģʽ
typedef enum EmTriggerMode
{
	TRIG_MODE_CONTINUE        = 0, //�����ɼ�
	TRIG_MODE_POST            = 1, //�󴥷�		
	TRIG_MODE_DELAY           = 2, //��ʱ����
	TRIG_MODE_PRE			  = 3, //ǰ������USB2069��֧��		
	TRIG_MODE_MIDDLE          = 4, //�д�����USB2069��֧��		
} TRIGGER_MODE;

//����Դ
typedef enum EmTriggerSource
{
	TRIG_SRC_EXT_RISING      = 0,  //�����ش���
	TRIG_SRC_EXT_FALLING     = 1,  //�⸺�ش���	
	TRIG_SRC_SOFT            = 2,  //�������
} TRIGGER_SOURCE;

//ѡ��DA 
typedef enum EmSelDA
{
	SEL_DA0       = 0, //ѡ��DA0 
	SEL_DA1       = 1, //ѡ��DA1
	SEL_DA2       = 2, //ѡ��DA2
	SEL_DA3       = 3, //ѡ��DA3
} SEL_DA;

//�������ȵ�λ
#define   TRIG_UNIT   8
//���βɼ���ȡ����
#define READ_MAX_LEN   1572864L  

//DAֹͣ����
#define DA_END_NO_PRD 1 //�յ�DAֹͣ�����,����ֹͣ���
#define DA_END_YES_PRD 0 //�յ�DAֹͣ����������ǰ���ں���ֹͣ

//��/д��ƫ
#define WRITEOFFSET 0 //д��ƫ
#define READOFFSET  1 //����ƫ

//��Ƶ��׼ʱ��
#define  FRQ_BASE_CLOCK  48000000L 

/***********************************************************/
#ifndef DEFINING
#define DEVAPI __declspec(dllimport)
#else
#define DEVAPI __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

   	//######################## ����ͨ�ú��� #################################
	//�ж��Ƿ�Ϊ����USB�豸
	DEVAPI BOOL FAR PASCAL USB2069_IsHighDevice(HANDLE hDevice,PUCHAR pDat);
	//�����豸�Ŵ��豸
	DEVAPI HANDLE FAR PASCAL USB2069_Link(UCHAR DeviceNO);
	//�Ͽ��豸
	DEVAPI BOOL FAR PASCAL USB2069_UnLink(HANDLE hDevice);
	//��ʼ����������ʼ�ɼ�
	DEVAPI BOOL FAR PASCAL USB2069_InitAD(HANDLE hDevice, PUSB2069_PARA_INIT para_init);
	//��ȡAD����
	DEVAPI BOOL FAR PASCAL USB2069_ReadAD(HANDLE hDevice,PUSHORT pBuf, ULONG nCount);
	//�����ɼ�
	DEVAPI BOOL FAR PASCAL USB2069_StopAD(HANDLE hDevice, UCHAR devNum);
	//����DO
	DEVAPI BOOL FAR PASCAL USB2069_SetDO(HANDLE hDevice, LONG lDO);
	//��ȡDI
	DEVAPI BOOL FAR PASCAL USB2069_GetDI(HANDLE hDevice, PLONG pDI);
	//����DA
	DEVAPI BOOL FAR PASCAL USB2069_SetDA(HANDLE hDevice, UCHAR bySelDA, BOOL bEnable, PLONG pFreq,
		BOOL bLoop, LONG lCycleCnt,LONG bEndPrd,PUSHORT pDABuf, ULONG nCount);
	//����AD��ƫ
	DEVAPI BOOL FAR PASCAL USB2069_ADoffset(HANDLE hDevice, BOOL bWtRd,LONG lSelReg,PLONG plADoffset);
	//��ȡӲ���������λ
	DEVAPI BOOL FAR PASCAL USB2069_GetBufOver(HANDLE hDevice, PLONG pBufOver);
	//��ȡ��Ƶ
	DEVAPI BOOL FAR PASCAL USB2069_GetFreq(HANDLE hDevice, PLONG plDat);
	//�������
	DEVAPI BOOL FAR PASCAL USB2069_ExeSoftTrig(HANDLE hDevice);
	
#ifdef __cplusplus
}
#endif
