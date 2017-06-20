/** @file HL_sys_main.c 
*   @brief Application main file
*   @date 05-Oct-2016
*   @version 04.06.00
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2016 Texas Instruments Incorporated - www.ti.com  
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
#define COMPRESSOR = 0;
#define CONDENSER_FAN = 1;
#define EVAPORATOR_FAN = 2;
#define DEFROSTER = 3;

#define OFF = 0;
#define ON = 1;
#define NA = 0;

/* USER CODE END */

/* Include Files */

#include "HL_sys_common.h"

/* USER CODE BEGIN (1) */
#include "HL_system.h"
#include "HL_sys_core.h"
#include "HL_adc.h"
#include "HL_het.h"
#include "HL_rti.h"
#include "HL_esm.h"
#include "HL_gio.h"
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */

	//UIInit();
	sensorInit();

/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */

void SafetyInit()(void) {
	/*
	 * This function will:
	 * Manage the anti-reset timer.
	 * Check for brownouts.
	 */
}

void sensorInit(void) {
//The analog to digital conversion code for all three sensors is here.

	adcData_t adc_data; //ADC Data Structure
	adcData_t *adc_data_ptr = &adc_data; //ADC Data Pointer
	unsigned int value;
	float resistance;
	float degperohm = 0.003267974;
	unsigned int temperature;

	rtiInit();
	gioInit();
	hetInit();
	adcInit();

	rtiEnableNotification(rtiREG1, rtiNOTIFICATION_COMPARE0);
	_enable_IRQ_interrupt_();
	rtiStartCounter(rtiREG1, rtiCOUNTER_BLOCK0);

//Take temperature readings.
	while(1) {
		adcStartConversion(adcREG1, 1U); //Start ADC conversion
		while(!adcIsConversionComplete(adcREG1, 1U)); //Wait for ADC conversion
			adcGetData(adcREG1, 1U, &adc_data); //Store conversion into ADC pointer
			value = (unsigned int)adc_data_ptr->value;

			resistance = 1000 * (4096 / value);
			temperature = degperohm * resistance;

/* Utilize temperature readings.
 * Condenser Temperature Manager/Condenser Fan Controller
 * The following conditional statements select whichever of the 9 condenser fan speeds
 * is best, depending on the temperature.
 * The fan speed follows the condenser temperature to minimize noise levels and power consumption.
 */
		pwmStart(hetRAM1, pwm0);

			if (temperature < 40) {
				pwmSetDuty(hetRAM1, pwm0, 0U);
			} else if (temperature >= 40 && temperature <= 42) {
				pwmSetDuty(hetRAM1, pwm0, 20U); //Speed 1
			} else if (temperature >= 42 && temperature <= 44) {
				pwmSetDuty(hetRAM1, pwm0, 30U); //Speed 2
			} else if (temperature >= 44 && temperature <= 46) {
				pwmSetDuty(hetRAM1, pwm0, 40U); //Speed 3
			} else if (temperature >= 46 && temperature <= 48) {
				pwmSetDuty(hetRAM1, pwm0, 50U); //Speed 4
			} else if (temperature >= 48 && temperature <= 50) {
				pwmSetDuty(hetRAM1, pwm0, 60U); //Speed 5
			} else if (temperature >= 50 && temperature <= 52) {
				pwmSetDuty(hetRAM1, pwm0, 70U); //Speed 6
			} else if (temperature >= 52 && temperature <= 54) {
				pwmSetDuty(hetRAM1, pwm0, 80U); //Speed 7
			} else if (temperature >= 54 && temperature <= 56) {
				pwmSetDuty(hetRAM1, pwm0, 90U); //Speed 8
			} else if (temperature >= 56 && temperature <= 58) {
				pwmSetDuty(hetRAM1, pwm0, 100U); //Speed 9 - The fan's maximum speed (100%).
			} else if (temperature > 65) {
				//Display a temperature warning.
			} else if (temperature > 70) {
				//Shut the compressor off, because the condenser is overheating.
			}

	}//End of the first ADC conversion while loop.

}

void Power(short int devnumber, short int state, short int dutycycle) {
/*
 * Power([Device Number, Device State (On or Off), Duty Cycle (if applicable));
 * Example Usage: Power(COMPRESSOR, ON, NA);
 * NA means 'Not applicable', as the compressor for this project is single speed.
 *
 */
}

/* USER CODE END */
