#ifndef INC_USER_MAIN_HPP_
#define INC_USER_MAIN_HPP_


#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f4xx.h"
void user_main(void);
void EXPLOR_read(void);

struct EXPLOR_sensor_data
{
	int mode;
	double CO2_data;
	double compansated_CO2_data;
};


#ifdef __cplusplus
}
#endif


#endif /* INC_USER_MAIN_HPP_ */
