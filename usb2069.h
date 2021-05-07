//AD初始化参数
#include <StdAfx.h>
#include <stdio.h>
#include <tchar.h>


#ifndef _USB2069_PARA_INIT
typedef struct _USB2069_PARA_INIT    
{
	LONG    lSelADClk;           //时钟选择，预留。
	LONG    ADFREQ;              //采样率
	LONG	TriggerMode;         //触发模式
	LONG	TriggerSource;	     //触发源 
	LONG    TriggerDelay;        //触发延时
	LONG    TriggerLength;       //触发长度
	LONG    TriggerLevel;        //模拟触发电平
	LONG    lEnCh[24];           //通道使能
} USB2069_PARA_INIT,*PUSB2069_PARA_INIT;
#endif

//时钟选择，预留
typedef enum EmADClkSel
{
	ADCLK_INT        = 0, //板上时钟
	ADCLK_EXT        = 1, //板外时钟
} ADCLK_SEL;

//触发模式
typedef enum EmTriggerMode
{
	TRIG_MODE_CONTINUE        = 0, //连续采集
	TRIG_MODE_POST            = 1, //后触发		
	TRIG_MODE_DELAY           = 2, //延时触发
	TRIG_MODE_PRE			  = 3, //前触发，USB2069不支持		
	TRIG_MODE_MIDDLE          = 4, //中触发，USB2069不支持		
} TRIGGER_MODE;

//触发源
typedef enum EmTriggerSource
{
	TRIG_SRC_EXT_RISING      = 0,  //外正沿触发
	TRIG_SRC_EXT_FALLING     = 1,  //外负沿触发	
	TRIG_SRC_SOFT            = 2,  //软件触发
} TRIGGER_SOURCE;

//选择DA 
typedef enum EmSelDA
{
	SEL_DA0       = 0, //选择DA0 
	SEL_DA1       = 1, //选择DA1
	SEL_DA2       = 2, //选择DA2
	SEL_DA3       = 3, //选择DA3
} SEL_DA;

//触发长度单位
#define   TRIG_UNIT   8
//单次采集读取长度
#define READ_MAX_LEN   1572864L  

//DA停止设置
#define DA_END_NO_PRD 1 //收到DA停止命令后,立刻停止输出
#define DA_END_YES_PRD 0 //收到DA停止命令后，输出当前周期后再停止

//读/写零偏
#define WRITEOFFSET 0 //写零偏
#define READOFFSET  1 //读零偏

//测频基准时钟
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

   	//######################## 常规通用函数 #################################
	//判断是否为高速USB设备
	DEVAPI BOOL FAR PASCAL USB2069_IsHighDevice(HANDLE hDevice,PUCHAR pDat);
	//根据设备号打开设备
	DEVAPI HANDLE FAR PASCAL USB2069_Link(UCHAR DeviceNO);
	//断开设备
	DEVAPI BOOL FAR PASCAL USB2069_UnLink(HANDLE hDevice);
	//初始化参数并开始采集
	DEVAPI BOOL FAR PASCAL USB2069_InitAD(HANDLE hDevice, PUSB2069_PARA_INIT para_init);
	//读取AD数据
	DEVAPI BOOL FAR PASCAL USB2069_ReadAD(HANDLE hDevice,PUSHORT pBuf, ULONG nCount);
	//结束采集
	DEVAPI BOOL FAR PASCAL USB2069_StopAD(HANDLE hDevice, UCHAR devNum);
	//设置DO
	DEVAPI BOOL FAR PASCAL USB2069_SetDO(HANDLE hDevice, LONG lDO);
	//读取DI
	DEVAPI BOOL FAR PASCAL USB2069_GetDI(HANDLE hDevice, PLONG pDI);
	//设置DA
	DEVAPI BOOL FAR PASCAL USB2069_SetDA(HANDLE hDevice, UCHAR bySelDA, BOOL bEnable, PLONG pFreq,
		BOOL bLoop, LONG lCycleCnt,LONG bEndPrd,PUSHORT pDABuf, ULONG nCount);
	//设置AD零偏
	DEVAPI BOOL FAR PASCAL USB2069_ADoffset(HANDLE hDevice, BOOL bWtRd,LONG lSelReg,PLONG plADoffset);
	//读取硬件缓存溢出位
	DEVAPI BOOL FAR PASCAL USB2069_GetBufOver(HANDLE hDevice, PLONG pBufOver);
	//读取测频
	DEVAPI BOOL FAR PASCAL USB2069_GetFreq(HANDLE hDevice, PLONG plDat);
	//软件触发
	DEVAPI BOOL FAR PASCAL USB2069_ExeSoftTrig(HANDLE hDevice);
	
#ifdef __cplusplus
}
#endif
