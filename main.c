/* --COPYRIGHT--,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//******************************************************************************
//  MSP430i20xx SD24 Example 01 - SD24, Continuous Conversion on a Group of 3 Channels
//
//  Description: This program uses the SD24 module to perform continuous
//  conversions on a group of channels (0, 1 and 2). A SD24 interrupt occurs
//  whenever the conversions have completed.
//
//  Test by applying voltages to the 3 input channels and setting a breakpoint
//  at the indicated line. Run program until it reaches the breakpoint, then use
//  the debugger's watch window to view the conversion results.
//
//  Results (upper 16 bits only) are stored in three arrays, one for each channel.
//
//  ACLK = 32kHz, MCLK = SMCLK = Calibrated DCO = 16.384MHz, SD_CLK = 1.024MHz
//  * Ensure low_level_init.c is included when building/running this example *
//
//  Notes: For minimum Vcc required for SD24 module - see datasheet
//         100nF cap btw Vref and AVss is recommended when using 1.2V ref
//
//               MSP430i20xx
//             -----------------
//         /|\|                |
//          | |                |
//          --|RST             |
//            |                |
//   Vin1+ -->|A0.0+      VREF |---+
//   Vin1- -->|A0.0-           |   |
//   Vin2+ -->|A1.0+           |  -+- 100nF
//   Vin2- -->|A1.0-           |  -+-
//   Vin3+ -->|A2.0+           |   |
//   Vin3- -->|A2.0-      AVss |---+
//
//  T. Witt
//  Yang Lu
//  Texas Instruments, Inc
//  June 2014
//  Built with Code Composer Studio v5.5
//******************************************************************************
#include "./driverlib/driverlib.h"
#include "communication.h"

static inline void tlv_setup (void)
{
	// Check the TLV checksum
	if(!TLV_performChecksumCheck()) return

	GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);

	uint16_t *data_address;
	TLV_getInfo(TLV_REF_CALIBRATION_REFCAL0, 0, &data_address);
	REFCAL0 = *(uint8_t *)data_address;
	TLV_getInfo(TLV_REF_CALIBRATION_REFCAL1, 0, &data_address);
	REFCAL1 = *(uint8_t *)data_address;
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
	// Internal ref
	SD24_init(SD24_BASE, SD24_REF_INTERNAL);

	//Group with Channel 0
	SD24_initConverterAdvancedParam param = {0};
	param.converter = SD24_CONVERTER_0;
	param.conversionMode = SD24_CONTINUOUS_MODE;
	param.groupEnable = SD24_GROUPED;
	param.inputChannel = SD24_INPUT_CH_ANALOG;
	param.dataFormat = SD24_DATA_FORMAT_2COMPLEMENT;
	param.interruptDelay = SD24_FIRST_SAMPLE_INTERRUPT;
	param.oversampleRatio = SD24_OVERSAMPLE_128;
	param.gain = SD24_GAIN_1;
	SD24_initConverterAdvanced(SD24_BASE, &param);

	//Group with Channel 1
	param.converter = SD24_CONVERTER_1;
	param.conversionMode = SD24_CONTINUOUS_MODE;
	param.groupEnable = SD24_NOT_GROUPED;
	param.inputChannel = SD24_INPUT_CH_ANALOG;
	param.dataFormat = SD24_DATA_FORMAT_2COMPLEMENT;
	param.interruptDelay = SD24_FIRST_SAMPLE_INTERRUPT;
	param.oversampleRatio = SD24_OVERSAMPLE_128;
	param.gain = SD24_GAIN_1;
	SD24_initConverterAdvanced(SD24_BASE, &param);
	SD24_enableInterrupt(SD24_BASE, SD24_CONVERTER_1, SD24_CONVERTER_INTERRUPT);


//    SD24CCTL0 = 0;
//    SD24CCTL1 = 0;
//
//    SD24CTL = SD24REFS;	//Internal reference
//
//    SD24INCTL0 = SD24GAIN_1 | SD24INCH_0;
//    SD24CCTL0 = SD24OSR_128 | SD24GRP | SD24DF;
//    SD24PRE0 = 0;
//
//    SD24INCTL1 = SD24GAIN_1 | SD24INCH_0;
//    SD24CCTL1 = SD24OSR_128 | SD24DF | SD24IE;
//    SD24PRE1 = 0;
//
//
//    SD24CCTL1 |= SD24SC;

	// Enable interrupt
//	param.converter = SD24_CONVERTER_2;
//	param.conversionMode = SD24_CONTINUOUS_MODE;
//	param.groupEnable = SD24_NOT_GROUPED;
//	param.inputChannel = SD24_INPUT_CH_ANALOG;
//	param.dataFormat = SD24_DATA_FORMAT_2COMPLEMENT;
//	param.interruptDelay = SD24_FIRST_SAMPLE_INTERRUPT;
//	param.oversampleRatio = SD24_OVERSAMPLE_128;
//	param.gain = SD24_GAIN_1;
//	SD24_initConverterAdvanced(SD24_BASE, &param);
//	SD24_enableInterrupt(SD24_BASE, SD24_CONVERTER_2, SD24_CONVERTER_INTERRUPT);

	// Delay ~200us for 1.2V ref to settle
	__delay_cycles(3200);
}

int main(void) {
	// Stop WDT
	WDT_hold(WDT_BASE);

	// LED for indicating checksum result (Blue)
	GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
	GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
	tlv_setup();

	//Set LED Pin
	GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);

	//UART
	uart_setup();

	//adc
	adc_setup();

	_enable_interrupts();

	metrology_start();

	// Start conversion
	//
	//    // Enter LPM0 w/ interrupts
	//    __bis_SR_register(LPM0_bits | GIE);

	// Initialize values to display
//	char *s = "NJC's MSP430 LaunchPad Blog";
//	char c = '!';
//	int i = -12345;
//	unsigned u = 4321;
//	long int l = -123456780;
//	long unsigned n = 1098765432;
//	unsigned x = 0xABCD;
//
//	printf("String         %s\r\n", s);
//	printf("Char           %c\r\n", c);
//	printf("Integer        %i\r\n", i);
//	printf("Unsigned       %u\r\n", u);
//	printf("Long           %l\r\n", l);
//	printf("uNsigned loNg  %n\r\n", n);
//	printf("heX            %x\r\n", x);

	while(1) {
		metrology_print_status();
	};
	return 0;
}
