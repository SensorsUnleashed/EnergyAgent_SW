/*
 * metrology-background.c
 *
 *  Created on: 26/04/2016
 *      Author: omn
 */
#include "./driverlib/driverlib.h"
#include "communication.h"
#include "types.h"

/* These are the buffers for one set of samples */
static voltage_sample_t adc_v_buffer;
static current_sample_t adc_i_buffer;

void metrology_start(){
	printf("metrology_start \n");

	//Start sampling
	SD24_startConverterConversion(SD24_BASE, SD24_CONVERTER_1);
}

void metrology_print_status(){
	int16_t temp1;
	int32_t temp2;
	if(fifo_getavilable() > 50){
		temp1 = adc_v_buffer;
		temp2 = adc_i_buffer;
		printf("ch1=%i \n", temp1);
		printf("ch2=%l \n", temp2);
		printf("------------ \n");
	}
}

void __attribute__ ((interrupt(SD24_VECTOR))) SD24_ISR(void){
	switch(__even_in_range(SD24IV,SD24IV_SD24MEM3))
	{
	case SD24IV_NONE: break;
	case SD24IV_SD24OVIFG: break;
	case SD24IV_SD24MEM0: break;
	case SD24IV_SD24MEM1:
		adc_v_buffer = SD24_getHighWordResults(SD24_BASE, SD24_CONVERTER_0);	//16bit
		adc_i_buffer = SD24_getResults(SD24_BASE, SD24_CONVERTER_1);			//32bit
		break;
	case SD24IV_SD24MEM2: break;
	case SD24IV_SD24MEM3: break;
	default: break;
	}
}
