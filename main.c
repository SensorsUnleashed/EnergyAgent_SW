/*
 * communication.h
 *
 *  Created on: 25/04/2016
 *      Author: omn
 */

#include "./driverlib/driverlib.h"
#include "communication.h"
#include "metrology-background.h"

//static inline
void tlv_setup (void)
{
	// Check the TLV checksum
	if(!TLV_performChecksumCheck()) return;

	uint16_t *data_address;
//	TLV_getInfo(TLV_REF_CALIBRATION_REFCAL1, 0, &data_address);
//	REFCAL1 = *(uint8_t *)data_address;
//	TLV_getInfo(TLV_REF_CALIBRATION_REFCAL0, 0, &data_address);
//	REFCAL0 = *(uint8_t *)data_address;

	/* Taken from the TI */
    REFCAL0 = *((uint8_t *)(TLV_START + TLV_CAL_REFCAL0));
    REFCAL1 = 0x00;
    SD24TRIM = 0x0C;


	TLV_getInfo(TLV_DCO_CALIBRATION_CSIRFCAL, 0, &data_address);
	CSIRFCAL = *(uint8_t *)data_address;
	TLV_getInfo(TLV_DCO_CALIBRATION_CSIRTCAL, 0, &data_address);
	CSIRTCAL = *(uint8_t *)data_address;
	TLV_getInfo(TLV_DCO_CALIBRATION_CSERFCAL, 0, &data_address);
	CSERFCAL = *(uint8_t *)data_address;
	TLV_getInfo(TLV_DCO_CALIBRATION_CSERTCAL, 0, &data_address);
	CSERTCAL = *(uint8_t *)data_address;
	TLV_getInfo(TLV_SD24_CALIBRATION_TAG, 0, &data_address);
	SD24TRIM = *(uint8_t *)data_address;


}

void uart_setup(){
	/* Configure the port with the reset bit held high */
	EUSCI_A_UART_initParam uartConfig = {
			EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
			8,                                        // BRDIV = 8
			14,                                      // UCxBRF = 14
			34,                                      // UCxBRS = 34
			EUSCI_A_UART_NO_PARITY,                  // No Parity
			EUSCI_A_UART_LSB_FIRST,                  // MSB First
			EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
			EUSCI_A_UART_MODE,                       // UART mode
			EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling Baudrate
	};

	UCA0TXBUF = 0;

	// Settings P1.2 and P1.3 as UART pins. P1.4 as LED output
	GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
			GPIO_PIN2 | GPIO_PIN3,
			GPIO_PRIMARY_MODULE_FUNCTION);
	GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN4);
	GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4);

	// Configure and enable the UART peripheral
	EUSCI_A_UART_init(EUSCI_A0_BASE, &uartConfig);
	EUSCI_A_UART_enable(EUSCI_A0_BASE);

	EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE,
			EUSCI_A_UART_RECEIVE_INTERRUPT);
}


void adc_setup(){
    SD24CTL = SD24REFS;	//Internal reference

    SD24CCTL0 &= ~SD24LSBACC;
    //Converter ch 0
    SD24INCTL0 = SD24GAIN_1 | SD24INCH_0;
    SD24CCTL0 = SD24GRP | SD24DF;
    SD24CCTL0 &= ~(SD24OSR_32);	//Clear the previous OSR bits
#if (OSR_FILTER == 256)
    SD24CCTL0 |= SD24OSR_256;
#else
    SD24CCTL0 |= SD24OSR_128;
#endif
    SD24PRE0 = 0;

    //Converter ch 1
    SD24INCTL1 = SD24GAIN_16 | SD24INCH_0;
    SD24CCTL1 = SD24DF | SD24IE;
    SD24CCTL1 &= ~(SD24OSR_32);	//Clear the previous OSR bits
#if (OSR_FILTER == 256)
    SD24CCTL1 |= SD24OSR_256;
#else
    SD24CCTL1 |= SD24OSR_128;
#endif
    SD24PRE1 = 0;

	// Delay ~200us for 1.2V ref to settle
	__delay_cycles(3200);

	//Start conversion
    SD24CCTL1 |= SD24SC;
}

int main(void) {
	// Stop WDT
	WDT_hold(WDT_BASE);
	tlv_setup();

	//Set LED Pin
	// LED for indicating checksum result (Blue)
	GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
	GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
	GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
	GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);


	uart_setup();
	adc_setup();

	_enable_interrupts();
	metrology_start();

	while(1) {
		//volatile unsigned int i;	// volatile to prevent optimization

		metrology_print_status();



//		i = -1;					// SW Delay
//		do i--;
//		while(i != 0);

	};
	return 0;
}
