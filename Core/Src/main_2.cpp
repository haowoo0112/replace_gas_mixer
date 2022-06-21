/*
 * main_2.cpp
 *
 *  Created on: Nov 8, 2021
 *      Author: JustusJiang-cBPS
 */


#include "main_2.hpp"
#include "gas_source.hpp"
#include "user_main.hpp"
//#include "module/do_ph-2.hpp"


#include "usbd_cdc_if.h"
#include <stdio.h>
#include <string.h>
//DOPH doph;
//LinearMotor linear_motor;

extern int flag_p;
extern int flag_v;
extern float r;
extern float target;
/* TODO*/
volatile bool tim7_EN = false; // volatile
uint8_t GM[100] = {};
uint32_t global_count = 0;
uint8_t DO[500] = {};


void main_2_init(void){
	/* modules initialization */
	// gas_mixer.init();
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
	//doph.init();
	//linear_motor.init();

	/* timer 7 */
	HAL_TIM_Base_Start_IT(&htim7);
	EXPLOR_read();
}

void main_2_loop(void){
	static double count_pon = 0;
	static double count_poff = 0;
	static double count_von = 0;
	static double count_voff = 0;	
	static int count = 0 ;
	static double time = 0;
	/* real-time process */
	// gas_mixer.loop();
	//linear_motor.loop_rt();


	/**
	 * CDC: communication by Quency
	 */
	/*
	uint32_t tick = HAL_GetTick();
	mother_routine_poll_4_nests(tick, 250);
	mother_routine_update_to_PC(tick, 250);
	mother_routine_handle_PC();
	*/

	/* periodic process */
	if(tim7_EN){
		gas_control();
		user_main();
		CO2_L = r/100;
		count++;
		if(count%40==0)
		{
			time++;
		}
		//doph.loop();

		if(flag_p == 1)
		{
			count_pon++;
			count_poff = 0;
		}
		else if(flag_p == 2)
		{
			count_poff++;
			count_pon = 0;
		}

		if(flag_v == 1)
		{
			count_von++;
			count_voff=0;
		}
		else if(flag_v == 2)
		{
			count_voff++;
			count_von = 0;
		}
		/**
		 * CDC_RD:
		 * 1. time tick
		 * 2. gas mixer
		 * 3. gas source control
		 */
		char foo[400] = "";
		char bar[100] = "";

	//	sprintf(bar, "--%lu--", HAL_GetTick());
	//	strcat(foo, bar);

	//	strcat(foo, (char*)GM);

		// sprintf(bar, "[P]%.2f,[V]%.2f,p_on:%.2f,p_off:%.2f,v_on:%.2f,v_off:%.2f\r\n", P, V,float(count_pon*25/1000),float(count_poff*25/1000),float(count_von*25/1000),float(count_voff*25/1000));
		// strcat(foo, bar);
		if(count%10==0)
		{
			sprintf(bar,"P:%.2f,CO2:%.2f,valve_on:%d,counter:%d,err:%d,time:%.2f,target:%.2f\r\n",P,CO2_L,valve_on,counter,err,time,target);
			strcat(foo,bar);
			HAL_Delay(1);
			CDC_Transmit_FS((uint8_t*)foo, strlen(foo));
		}


		tim7_EN = false;



		/**
		 * RD: PreSens fake DO/pH data
		 */
		/*
		global_count++;

		if((global_count % 12) == 11){
			uint8_t op[300] = {};
			op[0] = 0x01;
			op[1] = 0xAB;
			op[2] = 0x67;
			op[3] = 0x09;
			op[4] = 0x21;

			op[5] = 0x04;
			for(int i = 0; i < 24; i++){

				uint32_t test = i * 4000;

				op[4 * i + 1 + 5] = (uint8_t)((test & 0xFF000000) >> 24);
				op[4 * i + 2 + 5] = (uint8_t)((test & 0x00FF0000) >> 16);
				op[4 * i + 3 + 5] = (uint8_t)((test & 0x0000FF00) >> 8);
				op[4 * i + 4 + 5] = (uint8_t)((test & 0x000000FF) >> 0);
			}

			op[102] = 0x24;
			UNUSED(op);
//			CDC_Transmit_FS(op, 97 + 6);
			HAL_Delay(2);

			global_count = 0;
		} else if((global_count % 12) == 6){
			uint8_t op[300] = {};
			op[0] = 0x01;
			op[1] = 0xAB;
			op[2] = 0x67;
			op[3] = 0x09;
			op[4] = 0x22;

			op[5] = 0x04;
			for(int i = 0; i < 24; i++){

				uint32_t test = i * 300 + 3400;

				op[4 * i + 1 + 5] = (uint8_t)((test & 0xFF000000) >> 24);
				op[4 * i + 2 + 5] = (uint8_t)((test & 0x00FF0000) >> 16);
				op[4 * i + 3 + 5] = (uint8_t)((test & 0x0000FF00) >> 8);
				op[4 * i + 4 + 5] = (uint8_t)((test & 0x000000FF) >> 0);
			}
			op[20 + 1 + 5] = 0xFF;
			op[20 + 2 + 5] = 0xFF;
			op[20 + 3 + 5] = 0xFF;
			op[20 + 4 + 5] = 0xFF;

			op[102] = 0x24;
			UNUSED(op);
//			CDC_Transmit_FS(op, 97 + 6);
			HAL_Delay(2);
		}
		*/
	}
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if(htim == &htim7){
		tim7_EN = true;
	}
}
