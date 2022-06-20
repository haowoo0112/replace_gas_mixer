#include "user_main.hpp"
#include "usbd_cdc_if.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
extern UART_HandleTypeDef huart3;
int cnt=0;
uint8_t bufdata[200] = {0}, result_bufdata[200] = {0};
uint8_t abufdata;
uint8_t data[] = {0x5A, 0x0D, 0x0A};
extern float P;
extern float r;
void user_main(void){
	
	HAL_UART_Transmit_IT(&huart3, (uint8_t *)data, 3);
//
	HAL_UART_Receive_IT(&huart3, (uint8_t *)&abufdata, 1);

}

void EXPLOR_read(void){


//	HAL_UART_Transmit_IT(&huart3, data, strlen(data));
	HAL_Delay(3000);
//	HAL_UART_Transmit(&huart3, (uint8_t *)&data, 5, 100);
	HAL_UART_Transmit_IT(&huart3, (uint8_t *)data, 3);
//
	HAL_UART_Receive_IT(&huart3, (uint8_t *)&abufdata, 1);

}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	bufdata[cnt++] = abufdata;

	char DEBUG_cmd[300] = "";
	char DEBUG_temp[300] = "";
	float C1 = 0, y=0,result=0;
//	sprintf(DEBUG_cmd, "%d \r\n",cnt);
//	CDC_Transmit_FS((uint8_t*)DEBUG_cmd, strlen(DEBUG_cmd));
	if((bufdata[cnt-1] == 0x0A) && (bufdata[cnt-2] == 0x0D)){
		sprintf(DEBUG_cmd, "%d \r\n", cnt);
		int co2=0;
		for(int i=3;i<cnt-2;i++){
			co2 = co2*10+ (bufdata[i]-48);
		}
		C1 = co2;
		y = (2.811 * pow(10,-38)* pow(C1,6) - 9.817 * pow(10,-32) * pow(C1,5) + 1.304 * pow(10,-25) * pow(C1,4) - 8.126 * pow(10,-20)*pow(C1,3) + 2.311 * pow(10,-14) * pow(C1,2)- 2.195 * pow(10,-9) * C1 - 1.471 * pow(10,-3));
		result = C1/(1+y*(1013-(1013+P)));
		r=result;
//		sprintf(DEBUG_cmd, " %d %d %d\r\n", cnt, bufdata[0],bufdata[1]);
		sprintf(DEBUG_cmd, "%f,%f,%f\r\n ", C1/100,y,result/100);
//		CDC_Transmit_FS((uint8_t*)DEBUG_cmd, strlen(DEBUG_cmd));
//		for(int i=0;i<cnt;i++){
//			result_bufdata[i] = bufdata[i];
//		}

		cnt = 0;
	}



	HAL_UART_Receive_IT(&huart3, (uint8_t *)&abufdata, 1);
}
