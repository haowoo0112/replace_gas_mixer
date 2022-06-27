/*
 * gas_source.hpp
 *
 *  Created on: Nov 29, 2021
 *      Author: JustusJiang-cBPS
 */

#ifndef INC_GAS_SOURCE_HPP_
#define INC_GAS_SOURCE_HPP_
#define SHT31_L_ADDR (0x44 << 1)
#define STC31_L_ADDR (0x29 << 1)

#include "stm32f4xx_hal.h"


extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim1;
extern int flag_p;
extern int flag_v;
extern float pressure;
extern float target;

// TODO: tmp
float P = 0;
float V = 0;
double RH_L = 0.0;
float H_L = 0.0;
float T_L = 0.0;
int get_data_state = 1;
float slope = 1.0;
float offset = 0.0;
float CO2_L = 0.0;
int counter = 0;
int valve_on=40; //7.5: 50
int flag=0;
int err=0;
int flag_co2 = 0;
int flag_pump = 0;

float get_pressure(){
	int OUTPUT_MIN = 1638;
	int OUTPUT_MAX = 14746;
	float PRESSURE_MIN = -15.0;
	float PRESSURE_MAX = +15.0;

	uint8_t S = 5; // pressure gauge: status
	float T = -28.1; // pressure gauge: temperature
	uint8_t pData[4];
	// info needed
	float P = -28.0; // pressure gauge: pressure (mBar)

	// pressure gauge
	HAL_I2C_Master_Receive(&hi2c1, 0x28 << 1, pData, 4, 2);
	S = pData[0] >> 6;
	uint16_t tmp = (((uint16_t)pData[0] & 0x3f) << 8) | pData[1];
	P = (tmp - OUTPUT_MIN) * (PRESSURE_MAX - PRESSURE_MIN) / (OUTPUT_MAX - OUTPUT_MIN) + (PRESSURE_MIN);
	tmp = (((uint16_t)pData[2] << 8) + (pData[3] & 0xe0)) >> 5;
	T = (tmp * 0.0977) - 50;
	P *= 68.94757;

	UNUSED(S);
	UNUSED(T);

	return P;
}

float get_vacuum(){
	HAL_StatusTypeDef ret;
	uint8_t S;
	float T;
	float P = -123.0;
	uint16_t tmp;
	uint8_t buf[8] = "";
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);
	ret = HAL_SPI_Receive(&hspi1, buf, 4, 2);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(1);
	if (ret != HAL_OK) {
//		CDC_Transmit_FS((uint8_t*)"---\r\n", strlen("---\r\n"));
	} else {

		S = (uint8_t)buf[0] >> 6;
		tmp = (((uint16_t)buf[0] & 0x3f) << 8) | buf[1];
		P = (tmp - 1638) * (15.0 - (-15.0)) / (14745 - 1638) + (-15.0);
		P *= 68.9475729;
		tmp = (((uint16_t)buf[2] << 8) + (buf[3] & 0xe0)) >> 5;
		T = (tmp * 0.0977) - 50;
	}

	UNUSED(S);
	UNUSED(T);

	return P;
}

void set_pressure_pump(bool en){
	if(en){
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
	}
}

void set_vacuum_pump(bool en){
	int i;
	if(en){
//		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,499);

	} else {
//		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,0);
//		HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);

	}
}

void gas_control(){
	float p_upper = 580.0;
	float p_lower = 460.0;
	float sec = 0;
	static bool co2_state = false;
	static bool p_on = false;
	static bool force_air_on = false;
	static bool valve_on_ctrl = false;
	static bool force_air_on_once = false;
	static bool force_co2_on = false;

	static int target_change_times = 1;
	static float timer = 0;

//	char ch[30] = {};
	P = get_pressure();
	pressure = P;
	timer++;
	sec = timer/40; 
	if(sec<3)
	{
		target = 5;
		valve_on = 25;
	}
	if( sec > 1000000 * target_change_times)
	{
		target_change_times++;
		target = target+0.5;
		if(target <= 10)//waiting for new equipment to tune variables
		{
			valve_on = 50;
		}
		else{
			valve_on = 100;
		}
	}

	co2_state = CO2_L < target-0.3 ? true : false;
	if(P < p_lower)
	{
		force_air_on_once = false;
		force_co2_on = false;
		if(co2_state && !p_on){
			counter = 0;
			p_on = true;
		}
		else{
			valve_on_ctrl = true;
			flag_pump = 1;
			counter = 0;
		}
	}
	else
	{
		if(co2_state && !p_on && !force_co2_on && P > 540)
		{
			force_co2_on = true;
			counter = valve_on/2;
			p_on = true;
			flag_pump = 0;
			if(CO2_L < target-0.5)
				valve_on++;
		}
		if(CO2_L > target + 0.3 && !force_air_on && !force_air_on_once)
		{
			//todo: make sure pump won't start on repeatly
			force_air_on = true;
			force_air_on_once = true;
			if(valve_on_ctrl == true)
			{
				valve_on--;
				valve_on_ctrl = false;
			}
		}
	}
//	if (P < p_lower && !p_on && CO2_L < target-0.3) {
//		p_on = true;
//		counter = 0;
//		valve_on_ctrl = true;
//	}
//	else if (P < p_lower && !flag_pump && CO2_L > target-0.3) {
//		counter = 0;
//		flag_pump = 1;
//	}
//	else if(P > p_upper && P < p_upper+10 && CO2_L > target + 0.3){// test : force co2 lower
//		force_air_on = true;
//		if(valve_on_ctrl == true){
//			valve_on--;
//			valve_on_ctrl = false;
//		}
//
//	}

	if (P > p_upper){
		flag_pump = 0;
	}
	if(P> p_upper+50 || CO2_L < target){
		force_air_on=false;
	}

	if(p_on){
		if(counter < valve_on){
			counter ++;
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);
		}
		else{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
			if(P < p_lower + 40)
				flag_pump = 1;
			p_on = false;
		}
	}
	else{
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
	}

	if(flag_pump || force_air_on)
		set_vacuum_pump(true);
	else
		set_vacuum_pump(false);




//
//	if(p_on)
//	{
//		if(flag_co2)
//		{
//			set_vacuum_pump(true);
//		}
//		if(counter < valve_on && flag_aaa == 0)
//		{
//			counter ++;
//			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);
//		}
//		else
//		{
//			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
//			flag_co2 = 1;
//		}
//		flag_p = 1;
//		flag = 0;
//	}
//
//
//	if (P < p_lower && !p_on) {
//
//		p_on = true;
//
//	}
//	else if (P > p_upper) {
//		set_vacuum_pump(false);
//		flag_co2 = 0;
//		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
//		flag_p = 2;
//		p_on = false;
//		counter = 0;
//		if(CO2_L > 8+0.2 && flag == 0)
//		{
//			valve_on = 40;//5:30
//			flag = 1;
//			flag_aaa = 1;
//		}
//		else if( CO2_L >7.5 && CO2_L < 8-0.2 && flag == 0)
//		{
//			valve_on++;
//			flag = 1;
//			flag_aaa = 0;
//		}
//	}



//	sprintf(ch, "P: %.1f, V: %.1f\r\n", P, V);
//	CDC_Transmit_FS((uint8_t*)ch, strlen(ch));
}

void CO2_read(void)
{									 // TODO new hardware reset
	uint8_t H_read[] = {0x24, 0x00}; // read RH, T
	uint8_t C_read[] = {0x36, 0x39};

	uint8_t buf_HL[10];
	uint8_t buf_HR[10];
	uint8_t buf_CL[30];
	uint8_t buf_CR[30];

	HAL_StatusTypeDef ret_L, ret_R, ret;

	switch (get_data_state)
	{
	case 1:
	{
		ret_L = HAL_I2C_Master_Transmit(&hi2c1, SHT31_L_ADDR, H_read, 2, 1);
		if (ret_L != HAL_OK)
		{
			// TODO
			err=1;
		}
		get_data_state++;
		break;
	}
	case 2:
	{
		ret_L = HAL_I2C_Master_Receive(&hi2c1, SHT31_L_ADDR, buf_HL, 6, 2);
		if (ret_L != HAL_OK)
		{
			// TODO
			err=2;
		}
		else
		{
			uint16_t a = ((uint16_t)buf_HL[3] << 8) | buf_HL[4];
			H_L = a / (double)0xFFFF * 100;
			RH_L = a / (double)0xFFFF;
			uint16_t b = ((uint16_t)buf_HL[0] << 8) | buf_HL[1];
			T_L = b / (double)0xFFFF * 175 - 45;
		}
		get_data_state++;
		break;
	}
	case 3:
	{
		uint8_t setH[] = {0x36, 0x24, 0x00, 0x00};
		uint8_t setT[] = {0x36, 0x1E, 0x00, 0x00};

		// CO2, compensate H
		uint16_t hSet = (uint16_t)(RH_L * 65535);
		setH[2] = (uint8_t)(hSet >> 8);
		setH[3] = (uint8_t)(hSet & 0xFF);
		ret = HAL_I2C_Master_Transmit(&hi2c1, STC31_L_ADDR, setH, 4, 2);
		if (ret != HAL_OK)
		{
			err=3;
		}
		else
		{
		}
		// CO2, compensate T
		uint16_t tSet = (uint16_t)(T_L * 200);
		setT[2] = (uint8_t)(tSet >> 8);
		setT[3] = (uint8_t)(tSet & 0xFF);
		ret = HAL_I2C_Master_Transmit(&hi2c1, STC31_L_ADDR, setT, 4, 2);
		if (ret != HAL_OK)
		{
			err=4;
		}
		else
		{
		}
		get_data_state++;
		break;
	}
	case 4:
	{
		// read
		ret = HAL_I2C_Master_Transmit(&hi2c1, STC31_L_ADDR, C_read, 2, 1);
		if (ret != HAL_OK)
		{
			err=5;
			// CO2_default();
		}
		get_data_state++;
		break;
	}
	case 5:
	{
		get_data_state++;
		break;
	}
	case 6:
	{
		get_data_state++;
		break;
	}
	case 7:
	{
		ret = HAL_I2C_Master_Receive(&hi2c1, STC31_L_ADDR, buf_CL, 6, 4);
		if (ret == HAL_ERROR)
		{
			err=6;
			// CO2_default();
		}
		else
		{
			uint16_t gc = ((uint16_t)buf_CL[0] << 8) | buf_CL[1];
			CO2_L = (gc - 16384) / (double)32768 * 100;
			CO2_L = slope * CO2_L + offset; // RD FIXME
			int16_t tc = (buf_CL[2] << 8) | buf_CL[3];
			//			TCO2_L = tc / (double)200;
			UNUSED(tc);
		}
		get_data_state = 1;
		break;
	}
	}
}

void CO2_default(void)
{ // XXX choose value
	CO2_L = 77.03;
	H_L = 88.03;
	T_L = 99.03;
}

void CO2_init(void)
{
	// I2C commands
	uint8_t disCRC[] = {0x37, 0x68};
	uint8_t binaryGas[] = {0x36, 0x15, 0x00, 0x03};

	HAL_StatusTypeDef ret_L, ret_R;

	ret_L = HAL_I2C_Master_Transmit(&hi2c1, STC31_L_ADDR, disCRC, 2, 100);
	if (ret_L != HAL_OK)
	{
		// TODO: error!
	}
	if (ret_R != HAL_OK)
	{
		// TODO: error!
	}

	HAL_Delay(50);

	ret_L = HAL_I2C_Master_Transmit(&hi2c1, STC31_L_ADDR, binaryGas, 4, 100);

	HAL_Delay(50);

	if (ret_L != HAL_OK)
	{
		// TODO: error!
	}
	if (ret_R != HAL_OK)
	{
		// TODO: error!
	}

	//	uint32_t previous_time = HAL_GetTick();
	//	UNUSED(previous_time);

	CO2_default();
}


#endif /* INC_GAS_SOURCE_HPP_ */
