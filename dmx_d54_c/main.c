/*
 * dmx_d54_c.c
 *
 * Created: 28/04/2016 18:41:01
 * Author : Enrico
 */ 
#include <string.h>

#include "main.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define LED_DMX_OK_SET() LED_S_PORT |= LED_S_PIN
#define LED_DMX_OK_CLR() LED_S_PORT &= ~LED_S_PIN
#define LED_STATUS_OK_SET() LED_S_PORT |= LED_STATUSOK
#define LED_STATUS_OK_CLR() LED_S_PORT &= ~LED_STATUSOK

#if D54_SYNC_LOGIC_ZERO == 0
    #define D54_SYNC_SET() D54_SYNC_PORT |= D54_SYNC_PIN
    #define D54_SYNC_CLR() D54_SYNC_PORT &= ~D54_SYNC_PIN
#else
    #define D54_SYNC_SET() D54_SYNC_PORT &= ~D54_SYNC_PIN
    #define D54_SYNC_CLR() D54_SYNC_PORT |= D54_SYNC_PIN
#endif

volatile unsigned char D54_State,
					   D54_cTS,
					   D54_cChn;

unsigned char Buffer[DMX_BUFFER_SIZE];
//unsigned char* D54Buffer = DMXBuffer;
volatile unsigned short dmxcnt = 0;
volatile unsigned short d54cnt = 0;

void dmx_init()
{
	// Clear UDR0
	char x = UDR0;

	// Clear DMX buffer
	memset(Buffer, 0, DMX_BUFFER_SIZE);
	// Baud rate: 250kbaud @ 16MHz
	UBRR0 = DMX_UBBER_VALUE;
	// Clear UCSR0A
	UCSR0A = 0;
	// RX Complete Interrupt Enable, Receiver Enable.
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0);//|(1<<TXEN0);
	// Set frame format: Data bits = 8, Stop bits = 2, Parity = 0..
	UCSR0C = (3<<UCSZ00)|(1<<USBS0);

	// DMX Receive timeout timer
	OCR1A = TIMER1_CTC_VALUE;
	// Enable CTC interrupt..
	TIMSK1 |= (1<<OCIE1A);
	// Prescaled /1024
	TCCR1B = 0b101;

}

void d54_init()
{
	// Initialize data port for D54 data out..
	D54_DATA_DDR = 0xFF;
	D54_DATA_PORT = 0;

	// Initialize sync pin for D54 out..
	// SYNC signal is reverse logic 1 = Sync OFF, 0 = Sync ON..
	D54_SYNC_DDR |= D54_SYNC_DDR_BIT; // (1) Set pin dir as out.

	// Initialize Timer0 CTC value, to be fired every D54_TIME_SAMPLE..
	//OCR0A = D54_TIMER_CTC_VAL;
	OCR0A = D54_TIMER_CTC_VAL * D54_T_START;
	// Enable CTC interrupt..
	TIMSK0 |= (1<<OCIE0A);
	// Clear Timer on Compare Match (CTC)
	TCCR0A = (1<<WGM01);
	// No prescaling
	//TCCR0B = 1;
	// Prescaled /8
	TCCR0B = 2;

    D54_SYNC_CLR();
	D54_State = D54_S_START;
}


inline void change_state(unsigned char state, unsigned char tval)
{
	D54_State = state;
	OCR0A = D54_TIMER_CTC_VAL * tval;
}

inline void d54_sync_out()
{
	D54_DATA_PORT = 0;
    D54_SYNC_SET();
}

inline void d54_data_out(unsigned char data)
{
    D54_SYNC_CLR();
	D54_DATA_PORT = data;
}

ISR(TIMER0_COMPA_vect)
{
	switch (D54_State)
	{
		case D54_S_START:
		{
			d54cnt = 0;
			d54_sync_out();
			change_state(D54_S_SYNC, D54_T_SYNC);

			break;
		}
		

		case D54_S_SYNC:
		{
			d54_data_out(Buffer[d54cnt++]);
			change_state(D54_S_BYTE, D54_T_BYTE);

			break;
		}

		case D54_S_BYTE:
		{
			d54_sync_out();

			if (d54cnt < D54_CH_MAX)
				change_state(D54_S_SYNC, D54_T_SYNC);
			else
				change_state(D54_S_EOF, D54_T_EOF);

			break;
		}

		case D54_S_EOF:
		{
			d54_data_out(0);
			change_state(D54_S_START, D54_T_START);

			break;
		}

		default:
		{
			d54_sync_out();
			break;
		}
	}
}

ISR(TIMER1_COMPA_vect) {
    LED_DMX_OK_CLR();
	dmxcnt = 0;
}

ISR(USART0_RX_vect)
{
	//unsigned char dreg = UCSR0A;
	unsigned char data = UDR0;

	// Framing error
	if (UCSR0A & (1<<FE0)) {
        LED_DMX_OK_CLR();
		dmxcnt = 0;
	}
	else {
		Buffer[dmxcnt] = data;
		if (++dmxcnt >= DMX_BUFFER_SIZE)
			dmxcnt = 0;
        LED_DMX_OK_SET();
		OCR1A = TCNT1 + TIMER1_CTC_VALUE;
	}

}

int main(void)
{
	dmx_init();
	d54_init();

	LED_S_DDR_REG = 0xFF;
    LED_DMX_OK_SET();
    LED_STATUS_OK_SET();

	// Enable global interrupts
	sei();

	for (;;) {
        LED_STATUS_OK_SET();
		_delay_ms(750);
        LED_STATUS_OK_CLR();
        _delay_ms(750);
	}
}
