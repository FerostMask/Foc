#ifndef _SPI_MASTER_
#define _SPI_MASTER_
#include "zf_spi.h"
/*------------------------------------------------------*/
/* 					  ö�����Ͷ��� 						*/
/*======================================================*/
//	SPI���
typedef enum{
	SPI_N = SPI_1,//SPIģ��
	SPI_SCLK = SPI1_SCK_A04,//ʱ������
	SPI_MOSI = SPI1_MOSI_A07,//������������
	SPI_MISO = SPI_MISO_NULL,//���մӷ�����
	SPI_CS = SPI1_NSS_A04,//Ƭѡ����
	SPI_MODE = 0,//SPIͨ��ģʽ
	SPI_BAUD = 1 * 1000 * 1000,//ʱ��Ƶ��
	SPI_CTN = 1,//�Ƿ����������Ч״̬��CS�������ͣ�
}SPI_M_enum;
//	ͨ�����
typedef enum{
	INFO_LEN = 4,//������Ϣͷ���ȣ�ָ��λ+��ַ�Ĵ���+ͨ��ָʾ+��ţ�
	DATA_START = 2,//����������ʼλ����ȥָ��λ+��ַ�Ĵ�����
	INSTRUCTION_POSITION = 0,//ͨ��ָʾλ��
	NUMBER_POSITION = 1,//���λ��
	DATA_POSITION = 2,//����λ��
	MAX_DATA_LEN = 30,//һ�η��������
	ORI_ARRAY_LEN = MAX_DATA_LEN + INFO_LEN,
	
	READ_REGISTER = 0x04,//ָ��λ | ��״̬�Ĵ���
	PLACEHOLDER = 0x00,//ռλ
	SEND_DATA_ONLY = 0x02,//ָ��λ | ֻ��������
	SEND_DATA_REGISTER = 0x00,//�Ĵ���
	
	SPI_COMN_START = 0xA0,
	SPI_COMN_UNDERWAY = 0xA1,
	SPI_COMN_END = 0xA3,
	
	WIFI_SET_NAME = 0xC0,
	WIFI_SET_PASSWORD = 0xC1,
	WIFI_SET_IP = 0xC2,
	WIFI_SET_SENDPORT = 0xC3,
	WIFI_SET_RECEIVEPORT = 0xC4,
	
	WIFI_INITIALIZATION = 0xD0,
	WIFI_INIT_FAIL = 0x00,
	WIFI_INIT_SUCCESS = 0x01,
}COMN_enum;
/*------------------------------------------------------*/
/* 					   �ṹ�嶨�� 						*/
/*======================================================*/
//	SPI���
typedef struct spi_m{
	void (*init)(void);//���豸��ʼ������
	char *spi_sd;//���ݷ�������
	char *instruction;
	char *number;
}spi_m;
//	WiFi���
typedef struct wifi_info{
	char *name;
	char *password;
	char *ip;
	char *send_port;
	char *receive_port;
}wifi_info;
/*------------------------------------------------------*/
/* 					   �궨�庯�� 						*/
/*======================================================*/
#define spi_send_data(modata, len) spi_mosi(SPI_N, modata, NULL, len)
#define spi_sar_data(modata, midata, len) spi_mosi(SPI_N, modata, midata, len)
/*------------------------------------------------------*/
/* 					  �ⲿ�������� 						*/
/*======================================================*/

#endif
