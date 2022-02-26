/*--------------------------------------------------------------*/
/*							ͷ�ļ�����							*/
/*==============================================================*/
#include "spi_master.h"
#include "zf_systick.h"
#include "string.h"
/*--------------------------------------------------------------*/
/* 							 �������� 							*/
/*==============================================================*/
void master_init(void);
void wifi_set_info(void);
/*--------------------------------------------------------------*/
/* 							 �������� 							*/
/*==============================================================*/
char read_rg[2] = {READ_REGISTER, PLACEHOLDER};//��ȡ�ӻ��Ĵ�����ֵ
char rg_val[2];//�ӻ�״̬�Ĵ���ֵ�洢
char spi_sd_ori[ORI_ARRAY_LEN] = {SEND_DATA_ONLY, SEND_DATA_REGISTER};//ԭ����
spi_m master = {
	.init = master_init,
	.spi_sd = &spi_sd_ori[DATA_START + DATA_POSITION],
	.instruction = &spi_sd_ori[DATA_START + INSTRUCTION_POSITION],
	.number = &spi_sd_ori[DATA_START + NUMBER_POSITION],
};
wifi_info wifi = {
	.name = "��������",//WiFi����
	.password = "",//WiFi����
	.ip = "",//ָ������IP
	.send_port = "",//���Ͷ˿�
	.receive_port = "",//���ն˿�
};
/*--------------------------------------------------------------*/
/* 							 �������� 							*/
/*==============================================================*/
/*------------------------------*/
/*		    SPI������ʼ��		*/
/*==============================*/
void master_init(void){
	systick_timing_ms(20);//��ʱ����ֹSPIͨ�ų���
	spi_init(SPI_N, SPI_SCLK, SPI_MOSI, SPI_MISO, SPI_CS, SPI_MODE, SPI_BAUD);
	wifi_set_info();//����WiFi��Ϣ
//	��ʼ��WiFi
	while(1){
		*master.instruction = WIFI_INITIALIZATION;
		*master.number = 0x00;
		spi_send_data(spi_sd_ori, INFO_LEN);
		spi_sar_data(read_rg, rg_val, 2);
		if(rg_val[0] == WIFI_INIT_SUCCESS || rg_val[1] == WIFI_INIT_SUCCESS) break;
	}
	
}
/*------------------------------*/
/*		    WiFi��Ϣ����		*/
/*==============================*/
void wifi_set_info(void){
//	����WiFi����
	if(strlen(wifi.name) != 0){
		*master.instruction = WIFI_SET_NAME;
		*master.number = 0x00;
		memcpy(master.spi_sd, wifi.name, strlen(wifi.name));
		spi_send_data(spi_sd_ori, INFO_LEN+strlen(wifi.name));//spi_send_data�Ǻ궨�庯������spi_master.h�ļ��п����ҵ�
	}
//	����WiFi����
	if(strlen(wifi.password) != 0){
		*master.instruction = WIFI_SET_PASSWORD;
		*master.number = 0x00;
		memcpy(master.spi_sd, wifi.password, strlen(wifi.password));
		spi_send_data(spi_sd_ori, INFO_LEN+strlen(wifi.password));
	}
//	����ָ������IP
	if(strlen(wifi.ip) != 0){
		*master.instruction = WIFI_SET_IP;
		*master.number = 0x00;
		memcpy(master.spi_sd, wifi.ip, strlen(wifi.ip));
		spi_send_data(spi_sd_ori, INFO_LEN+strlen(wifi.ip));
	}
//	���÷��Ͷ˿�
	if(strlen(wifi.send_port) != 0){
		*master.instruction = WIFI_SET_SENDPORT;
		*master.number = 0x00;
		memcpy(master.spi_sd, wifi.send_port, strlen(wifi.send_port));
		spi_send_data(spi_sd_ori, INFO_LEN+strlen(wifi.send_port));
	}
//	���ý��ն˿�
	if(strlen(wifi.receive_port) != 0){
		*master.instruction = WIFI_SET_RECEIVEPORT;
		*master.number = 0x00;
		memcpy(master.spi_sd, wifi.receive_port, strlen(wifi.receive_port));
		spi_send_data(spi_sd_ori, INFO_LEN+strlen(wifi.receive_port));
	}
}
/*------------------------------*/
/*		    ���ݷ���ģ��		*/
/*==============================*/
void spi_send(const char *data, int len){
//	��������
	int send_times = 0;
	master.instruction = SPI_COMN_START;
//	��������
	while(1){
		master.number = send_times;	
		if(len <= MAX_DATA_LEN){
		//	������Ϣ
			master.instruction = SPI_COMN_END;
			memcpy(master.spi_sd, data+send_times*MAX_DATA_LEN, len);
			spi_send_data(spi_sd_ori, INFO_LEN+len);
			break;
		}
		memcpy(master.spi_sd, data+send_times*MAX_DATA_LEN, MAX_DATA_LEN);
		spi_send_data(spi_sd_ori, INFO_LEN + MAX_DATA_LEN);
		len -= 30;
		master.instruction = SPI_COMN_UNDERWAY;
		send_times++;
	}
}
