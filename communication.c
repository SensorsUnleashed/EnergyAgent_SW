#include "communication.h"
#include "./driverlib/driverlib.h"


/* Circular TX buffer */
#define TXBUF_LEN 	500
static uint8_t tx_buf[TXBUF_LEN];
static uint8_t* tx_readptr = &tx_buf[0];		//Where new data should be send from
static uint8_t* tx_writeptr = &tx_buf[0];		//Where new data should be added
static uint8_t* tx_end = &tx_buf[TXBUF_LEN-1];
static uint8_t* tx_start = &tx_buf[0];

void fifo_add(uint8_t byte){
	*tx_writeptr++ = byte;
	if(tx_writeptr > tx_end) tx_writeptr = tx_start;
}

void tx(){
	if(tx_writeptr != tx_readptr){
        //Poll for transmit interrupt flag
        if(!(UCA0IFG & UCTXIFG))
        {
        	return;
        }
		UCA0TXBUF = *tx_readptr++;
		if(tx_readptr > tx_end) tx_readptr = tx_start;
	}
}
/**
 * puts() is used by printf() to display or send a string.. This function
 *     determines where printf prints to. For this case it sends a string
 *     out over UART, another option could be to display the string on an
 *     LCD display.
 **/
void puts(char *s) {
	uint8_t c;
	while ((c = *s++)) {
		fifo_add(c);
	}
}
/**
 * puts() is used by printf() to display or send a character. This function
 *     determines where printf prints to. For this case it sends a character
 *     out over UART.
 **/
void putc(uint8_t b) {
	fifo_add(b);
}

__attribute__((interrupt(USCI_A0_VECTOR))) void USCI_A0_ISR(void) {
	uint8_t byte;
	switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
	{
	case USCI_NONE: break;
	case USCI_UART_UCRXIFG:

		// Leave LPM so next byte can be sent
//		__bic_SR_register_on_exit(LPM0_bits);

//		char r = UCA0RXBUF;					// Get the received character
//		//if (r == 't')						// 'u' received?
//		{
//			FLAGS |= TX;					// Set flag to transmit data
//			__bic_SR_register_on_exit(LPM3_bits);	// Wake-up CPU
//		}
		UCA0TXBUF = UCA0RXBUF;	//Echo what comes in
		break;
	case USCI_UART_UCTXIFG:
//		if(fifo_get(&byte)){
//			UCA0TXBUF = byte;
//		}
//		else{	//Nothing more to send
//            UCA0IE &= ~UCTXIE;
//		}
		break;
	case USCI_UART_UCSTTIFG: break;
	case USCI_UART_UCTXCPTIFG: break;
	default: break;
	}
}
