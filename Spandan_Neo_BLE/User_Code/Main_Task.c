#include "main.h"
#include "adc.h"
#include "ipcc.h"
#include "rf.h"
#include "rtc.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h".
#include "usbd_cdc_if.h"
int Health_Cnt = 0;
int Health_Cnt1 = 0;

int ADC_Sample_Cnt = 0;

int Buffer_Cnt = 0;
_Bool System_Initialised = 0;
int i = 0;
int ms_cnt = 0;
unsigned char Data_Form = '0';

unsigned char Data_String[20];
extern unsigned char DUID[];

extern int ADC_Data;

int RX_Cnt = 0;
int TX_Cnt = 0;

uint8_t Data_Queue[Query_len][Query_Size];

uint16_t ECG_1_ADC_BUFFER[Packet_Len];

extern uint8_t UpdateCharData[];

extern unsigned char VCP_STRING[];

uint16_t ADC_ECG_1_Channel_Select(void);

void Packet_Construction_Int_Form(void);
void Packet_Construction_Hex_Form (void);
void Capture_Parameter_2ms(void);
void Packet_Construction(void);
void BLE_Receive_Handler(void);
void Send_Packet(void);
void Main_Init(void);

uint16_t ECG_1_ADC_Data = 0;
int TCnt = 0;
_Bool ADC_DR = 0;
_Bool Packet_Constructed = 0;

unsigned char BLE_RX_BUF[255];
int Data_Sending_Enable = 0;

int Packet_Cnt = 0;
extern uint16_t SizeMycharnotify;

int String_Length = 0;
uint16_t BP = 0;
uint32_t Fail_cnt = 0;
uint32_t Success_Cnt = 0;
uint16_t Sending_Duration_Cnt = 0;
extern uint8_t Utsav_BLE_Status;

extern _Bool Roll_Back;

void Main_Init(void)
{

//		for(i=0;i<12;i++)
//		{
//			DUID[i] =  *(__IO uint8_t*)(0x1FFF7590+i);
//		}

		HAL_Delay(2000);

		System_Initialised = 1;
		SizeMycharnotify = 240;
		HAL_ADC_Start(&hadc1);
		HAL_Delay(1000);
}

void Main_task(void)
{
	while (1)
	{
	  if(Packet_Constructed)
	  {
		  Send_Packet();
		  Packet_Constructed = 0;
	  }
	}
}

void Packet_Construction_Hex_Form(void)
{
	Health_Cnt1 = 1;
	String_Length = 0;

	if(Utsav_BLE_Status)
	{
		Fail_cnt++;
		BP = Utsav_BLE_Status;
	}

	memset(UpdateCharData,0x00,1024);
	memset(VCP_STRING,0x00,255);

	memcpy(VCP_STRING,&Packet_Cnt,4); 										String_Length += 4;
	memcpy(VCP_STRING+String_Length,&Fail_cnt,4); 							String_Length += 4;
	memcpy(VCP_STRING+String_Length,&BP,2);       							String_Length += 2;
	memcpy(VCP_STRING+String_Length,&ECG_1_ADC_BUFFER,2*Packet_Len); 		String_Length += (2*Packet_Len);

    memcpy(Data_Queue[RX_Cnt],VCP_STRING,String_Length);

	Packet_Cnt++;
    RX_Cnt++;
	if(RX_Cnt >= Query_len)
	{
	  RX_Cnt = 0;
	  Roll_Back = 1;
	}
}

void Packet_Construction_Int_Form(void)
{
		Health_Cnt1 = 1;
		String_Length = 0;

		memset(UpdateCharData,0x00,1024);
		sprintf(VCP_STRING,"%08d,",Packet_Cnt);
		strcat(UpdateCharData,VCP_STRING);

		for(i=0;i<Packet_Len;i++)
		{
			sprintf(VCP_STRING,"%04d,",ECG_1_ADC_BUFFER[i]);strcat(UpdateCharData,VCP_STRING);
		}

		sprintf(VCP_STRING,"Y\r\n");
		strcat(UpdateCharData,VCP_STRING);
		String_Length = strlen(UpdateCharData);

		Packet_Cnt++;
}


void Packet_Construction(void)
{
	if(Data_Form == 'H')
	{
		Packet_Construction_Hex_Form();
	}
	else if(Data_Form == 'I')
	{
		Packet_Construction_Int_Form();
	}
}

void Send_Packet(void)
{
	CDC_Transmit_FS(UpdateCharData,String_Length);
}

void MS_Handler(void)
{
	if(System_Initialised)
	{
		ms_cnt++;
		if(ms_cnt>=Sampling_Interval)
		{
			ms_cnt = 0;

			if(Data_Sending_Enable == 1)
			{
				if(ADC_Sample_Cnt < Packet_Len)
				{

					ECG_1_ADC_Data = ADC_ECG_1_Channel_Select();
					ECG_1_ADC_BUFFER[ADC_Sample_Cnt]    = ECG_1_ADC_Data;

					ADC_Sample_Cnt++;
				}

				if(ADC_Sample_Cnt >= Packet_Len)
				{
					ADC_Sample_Cnt = 0;
					Packet_Construction();
				}

				if(Sending_Duration_Cnt++ >= Sending_Duration_Cnt_Value/2)
				{
					Sending_Duration_Cnt = 0;
					if((TX_Cnt < RX_Cnt) || (Roll_Back == 1))
					{
						Packet_Constructed = 1;
					}
				}
			}
		}
	}
}

void BLE_Receive_Handler(void)
{
    if(BLE_RX_BUF[0] == '1')
    {
    	Data_Sending_Enable = 1;
    	Data_Form = 'I';
    	Packet_Cnt = 0;
    	Fail_cnt = 0;
    	TX_Cnt = 0;
		RX_Cnt = 0;
		Roll_Back = 0;
		ADC_Sample_Cnt = 0;
		Packet_Constructed = 0;
    }
    else if(BLE_RX_BUF[0] == '0')
    {
    	Data_Sending_Enable = 0;
    }
    else if(BLE_RX_BUF[0] == 'c')
    {
    	Data_Sending_Enable = 2;
    }
    else if(BLE_RX_BUF[0] == 'h')
	{
		Data_Sending_Enable = 1;
		Data_Form = 'H';
		Packet_Cnt = 0;
		Fail_cnt = 0;
		TX_Cnt = 0;
		RX_Cnt = 0;
		Roll_Back = 0;
		ADC_Sample_Cnt = 0;
		Packet_Constructed = 0;
	}
}

uint16_t ADC_ECG_1_Channel_Select(void)
{
	uint16_t adcval =  (hadc1.Instance->DR);
	return adcval;
}

