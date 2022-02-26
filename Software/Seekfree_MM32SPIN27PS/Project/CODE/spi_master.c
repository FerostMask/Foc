/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "spi_master.h"
#include "zf_systick.h"
#include "string.h"
/*--------------------------------------------------------------*/
/* 							 函数声明 							*/
/*==============================================================*/
void master_init(void);
void wifi_set_info(void);
/*--------------------------------------------------------------*/
/* 							 变量定义 							*/
/*==============================================================*/
char read_rg[2] = {READ_REGISTER, PLACEHOLDER};//读取从机寄存器数值
char rg_val[2];//从机状态寄存器值存储
char spi_sd_ori[ORI_ARRAY_LEN] = {SEND_DATA_ONLY, SEND_DATA_REGISTER};//原数组
spi_m master = {
	.init = master_init,
	.spi_sd = &spi_sd_ori[DATA_START + DATA_POSITION],
	.instruction = &spi_sd_ori[DATA_START + INSTRUCTION_POSITION],
	.number = &spi_sd_ori[DATA_START + NUMBER_POSITION],
};
wifi_info wifi = {
	.name = "测试中文",//WiFi名称
	.password = "",//WiFi密码
	.ip = "",//指定发送IP
	.send_port = "",//发送端口
	.receive_port = "",//接收端口
};
/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
/*------------------------------*/
/*		    SPI主机初始化		*/
/*==============================*/
void master_init(void){
	systick_timing_ms(20);//延时，防止SPI通信出错
	spi_init(SPI_N, SPI_SCLK, SPI_MOSI, SPI_MISO, SPI_CS, SPI_MODE, SPI_BAUD);
	wifi_set_info();//设置WiFi信息
//	初始化WiFi
	while(1){
		*master.instruction = WIFI_INITIALIZATION;
		*master.number = 0x00;
		spi_send_data(spi_sd_ori, INFO_LEN);
		spi_sar_data(read_rg, rg_val, 2);
		if(rg_val[0] == WIFI_INIT_SUCCESS || rg_val[1] == WIFI_INIT_SUCCESS) break;
	}
	
}
/*------------------------------*/
/*		    WiFi信息设置		*/
/*==============================*/
void wifi_set_info(void){
//	设置WiFi名称
	if(strlen(wifi.name) != 0){
		*master.instruction = WIFI_SET_NAME;
		*master.number = 0x00;
		memcpy(master.spi_sd, wifi.name, strlen(wifi.name));
		spi_send_data(spi_sd_ori, INFO_LEN+strlen(wifi.name));//spi_send_data是宏定义函数，在spi_master.h文件中可以找到
	}
//	设置WiFi密码
	if(strlen(wifi.password) != 0){
		*master.instruction = WIFI_SET_PASSWORD;
		*master.number = 0x00;
		memcpy(master.spi_sd, wifi.password, strlen(wifi.password));
		spi_send_data(spi_sd_ori, INFO_LEN+strlen(wifi.password));
	}
//	设置指定发送IP
	if(strlen(wifi.ip) != 0){
		*master.instruction = WIFI_SET_IP;
		*master.number = 0x00;
		memcpy(master.spi_sd, wifi.ip, strlen(wifi.ip));
		spi_send_data(spi_sd_ori, INFO_LEN+strlen(wifi.ip));
	}
//	设置发送端口
	if(strlen(wifi.send_port) != 0){
		*master.instruction = WIFI_SET_SENDPORT;
		*master.number = 0x00;
		memcpy(master.spi_sd, wifi.send_port, strlen(wifi.send_port));
		spi_send_data(spi_sd_ori, INFO_LEN+strlen(wifi.send_port));
	}
//	设置接收端口
	if(strlen(wifi.receive_port) != 0){
		*master.instruction = WIFI_SET_RECEIVEPORT;
		*master.number = 0x00;
		memcpy(master.spi_sd, wifi.receive_port, strlen(wifi.receive_port));
		spi_send_data(spi_sd_ori, INFO_LEN+strlen(wifi.receive_port));
	}
}
/*------------------------------*/
/*		    数据发送模块		*/
/*==============================*/
void spi_send(const char *data, int len){
//	变量定义
	int send_times = 0;
	master.instruction = SPI_COMN_START;
//	发送数据
	while(1){
		master.number = send_times;	
		if(len <= MAX_DATA_LEN){
		//	设置信息
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
