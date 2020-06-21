
// Common prerequisites..
#define MCU_CLOCK_FREQ 20000000 // MCU clock frequency (Hz).
#define F_CPU MCU_CLOCK_FREQ

//Stack defs.
#define STACK_SIZE 64 // Stack size in bytes.
#define STACK_BASE (SRAM_SIZE + STACK_SIZE) // Stack pointer base address.

// USART prerequisites..
#define USART_BAUD_RATE 250000 // USART baud rate (BPS).

//Status LED..
#define LED_S_PORT PORTC
#define LED_S_DDR_REG DDRC
#define LED_S_DDR_BIT (1<<DDRC0)
#define LED_S_DDR_BIT1 (1<<DDRC1)
#define LED_S_PIN (1<<PINC0)
#define LED_STATUSOK (1<<PINC1)

//General flags (Flags register)..
#define F_DMX_OK 0 //DMX frame recieve OK.
#define F_DMX_TEST 6 //DMX Test mode.
#define F_D54_TEST 7 //D54 Test mode.

// ####################################################################################################### //
// # DMX defs.
// ####################################################################################################### //

//DMX USART UBR value..
//#define DMX_UBBER_VALUE ((MCU_CLOCK_FREQ / (16 * USART_BAUD_RATE)) - 0.5) // UBBER value.
#define DMX_UBBER_VALUE ((MCU_CLOCK_FREQ / (16 * USART_BAUD_RATE)) - 1) // UBBER value.

//DMX data..
#define DMX_CHANNELS 512
#define DMX_BUFFER_SIZE DMX_CHANNELS+1// DMX buffer size.

#define TIMER1_CTC_VALUE 15625

// ####################################################################################################### //
// # D54 defs.
// ####################################################################################################### //

//D54 data..
#define D54_CH_MAX 384 //D54 max. channels (384).
//#define D54_TIME_SAMPLE 4 //D54 time sample = 4 us.
#define D54_TIME_SAMPLE 5 //D54 time sample = 5 us.
#define D54_TIMER_CTC_VAL (D54_TIME_SAMPLE * (MCU_CLOCK_FREQ / 1000000 / 8)) //D54 timer value. (8)

#define D54_SYNC_LOGIC_ZERO 1
#define D54_SYNC_LOGIC_ONE  0

// Time slices.
#define D54_T_START 11 //START signal = '> 43us' , so T = 43us / D54_TIME_SAMPLE = 10.75 use 11..
#define D54_T_SYNC 3 //SYNC signal = '10us' , so T = 10us / D54_TIME_SAMPLE = 2.5.
#define D54_T_BYTE 13 //BYTE signal = '> 50us' , so T = 50us / D54_TIME_SAMPLE = 12.5 use 13..
#define D54_T_EOF 13 //EOF signal = '> 50us' , so T = 50us / D54_TIME_SAMPLE = 12.5 use 13..

// D54 data port..
#define D54_DATA_PORT PORTA
#define D54_DATA_DDR DDRA
#define D54_DATA_PIN PINA

//D54 sync pin..
#define D54_SYNC_PORT PORTC
#define D54_SYNC_DDR DDRC
#define D54_SYNC_DDR_BIT (1<<DDRC7)
#define D54_SYNC_PIN (1<<PINC7)

//D54 state..
#define D54_S_DEFAULT 1 //Idle
#define D54_S_START 2 //Start
#define D54_S_SYNC 3 //Sync pulse.
#define D54_S_BYTE 4 //Channel data.
#define D54_S_EOF 5 //End Of Frame.