/*
 * main_2.hpp
 *
 *  Created on: Nov 8, 2021
 *      Author: JustusJiang-cBPS
 */

#ifndef INC_MAIN_2_HPP_
#define INC_MAIN_2_HPP_


#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f4xx.h"


extern TIM_HandleTypeDef htim7;
extern uint8_t bufGM[100];
extern UART_HandleTypeDef huart2;


void main_2_init(void);
void main_2_loop(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef*);


#ifdef __cplusplus
}
#endif


#endif /* INC_MAIN_2_HPP_ */
