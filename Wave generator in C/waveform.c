
#include <avr/io.h>
#include <stdlib.h>
#define F_CPU 1000000
#include <util/delay.h>
#include <time.h>

#include "waveform.h"
#include "uart.h"

#define _CMD_START_CNT 1
#define _CMD_END_CNT   1
#define _CMD_WAVE_CNT  1
#define _CMD_AMP_CNT   3
#define _CMD_FRQ_CNT   3

#define FULL_CMD_CNT (_CMD_START_CNT +  _CMD_WAVE_CNT + _CMD_AMP_CNT + _CMD_FRQ_CNT + _CMD_END_CNT)
#define WAVE_OFFSET  (_CMD_START_CNT)
#define AMP_OFFSET   (_CMD_START_CNT +  _CMD_WAVE_CNT)
#define FREQ_OFFSET  (_CMD_START_CNT +  _CMD_WAVE_CNT + _CMD_AMP_CNT)
#define MARKER_END   (_CMD_START_CNT +  _CMD_WAVE_CNT + _CMD_AMP_CNT + _CMD_FRQ_CNT)
#define MARKER_START (0)

#define WAVEFORM_NUM 5

#define DAC_DDR  DDRB
#define DAC_PORT PORTB

typedef enum {GENERATE_WAVE, UPDATE_WAVE} states_t;

static uint8_t cmd_buffer[FULL_CMD_CNT];
static void (*waveform[WAVEFORM_NUM])(uint8_t amp, uint8_t freq);
static states_t currentState = GENERATE_WAVE;
static uint8_t amp_value = 0;
static uint8_t freq_value = 0;
static uint8_t waveform_index = WAVEFORM_NUM;
unsigned int sine_value[13] = {128,192,238,255,238,192,128,64,1};


void delay(float time){
	for (int j =0;j<time;j++)
	{
	}
}

void squareWave(uint8_t amp, uint8_t time)
{
    DAC_DDR = 255;
    DAC_PORT = amp;
	delay(time*2);
	DAC_PORT = 0;
	delay(time/2);
}

void staircaseWave(uint8_t amp, uint8_t time)
{
    // Refresh DAC DDR to be output.
    DAC_DDR = 255;

    // Generate waveform.
    DAC_PORT = 0x00;
    delay(time);
    DAC_PORT = (amp*4)/5;
    delay(time);
    DAC_PORT = ((float)amp*3)/5;
    delay(time);
    DAC_PORT = ((float)amp*2)/5;
    delay(time);
    DAC_PORT = (amp*1)/5;
    delay(time);
    DAC_PORT = amp;
    delay(time);
}

void triangleWave(uint8_t amp, uint8_t time)
{
    //DAC_DDR = 255;
   /* 
	delay(time);
	for (int i = 0;i<9;i++)
    {
	    DAC_PORT=sine_value[i];
    }*/
   DDRA = 255;
   PORTA =time ;
   
   DAC_DDR = 255;
   DAC_PORT  = 0 ;
   
   int coff =(2*amp) / time ;
   int i ;
	for (i=1;i<255;i++)
	{
		DAC_PORT = i;
		//_delay_us(50);
	}
	for(i=255;i>=0;i--)
	{
		DAC_PORT =i;
		//_delay_us(50);
	}
	
	}

void sineWave(uint8_t amp, uint8_t freq)
{
    DAC_DDR = 255;
    for (int i = 0;i<amp;i+10)
    {
	    DAC_PORT=i;
	    
    }
    for (int i=amp;i>0;i-10)
    {
	    DAC_PORT= i;
    }
	
}
void sawtoothWave(uint8_t amp, uint8_t freq)
{
	DAC_DDR = 255;
	
	DAC_PORT = 0b11111111;
	_delay_us(10);
	/*DAC_PORT = 240;
	/ *_delay_us(10);* /
	DAC_PORT = 230;
	
	DAC_PORT = 210;
	//_delay_us(10);
	DAC_PORT = 200;
	//_delay_us(10);
	DAC_PORT = 180;
	//_delay_us(10);
	DAC_PORT = 160;
	//_delay_us(10);
	DAC_PORT = 140;
	//_delay_us(10);
	DAC_PORT = 120;*/
	//_delay_us(10);
	/*DAC_PORT = 100;*/
	//_delay_us(10);
// 	DAC_PORT = 90;
// 	//_delay_us(10);
// 	DAC_PORT = 70;
// 	//_delay_us(10);
// 	DAC_PORT = 50;
// 	//_delay_us(10);
// 	DAC_PORT = 30;
// 	//_delay_us(10);
// 	DAC_PORT = 10;
// 	//_delay_us(10);
// 	DAC_PORT = 0;
	//_delay_us(10);
	//DAC_PORT = 255;
	
	
	//delay(freq);
	
	
}

void WAVE_Init(void)
{
    uint8_t i;
    
    /* Init UART driver. */
    UART_cfg my_uart_cfg;
    
    /* Set USART mode. */
    my_uart_cfg.UBRRL_cfg = (BAUD_RATE_VALUE)&0x00FF;
    my_uart_cfg.UBRRH_cfg = (((BAUD_RATE_VALUE)&0xFF00)>>8);
    
    my_uart_cfg.UCSRA_cfg = 0;
    my_uart_cfg.UCSRB_cfg = (1<<RXEN)  | (1<<TXEN) | (1<<TXCIE) | (1<<RXCIE);
    my_uart_cfg.UCSRC_cfg = (1<<URSEL) | (3<<UCSZ0);
    
    UART_Init(&my_uart_cfg);
    
    
    /* Clear cmd_buffer. */
    for(i = 0; i < FULL_CMD_CNT; i += 1)
    {
        cmd_buffer[i] = 0;
    }
    
    /* Initialize waveform array. */
    waveform[0] = squareWave;
    waveform[1] = staircaseWave;
    waveform[2] = triangleWave;
    waveform[3] = sineWave;
	waveform[4] = sawtoothWave;

    /* Start with getting which wave to generate. */ 
    currentState = UPDATE_WAVE;    
}

void WAVE_MainFunction(void)
{    

    // Main function must have two states,
    // First state is command parsing and waveform selection.
    // second state is waveform executing.
    switch(currentState)
    {
        case UPDATE_WAVE:
        {

            /* Receive the full buffer command. */
            UART_ReceivePayload(cmd_buffer, FULL_CMD_CNT);
            
            /* Pull unitl reception is complete. */
            while(0 == UART_IsRxComplete());

            /* Check if the cmd is valid. */
            if((cmd_buffer[MARKER_START] == '@') && (cmd_buffer[MARKER_END] == ';'))
            {
                // Extract amplitude and freq values before sending them to the waveform generator.
                /* Compute amplitude. */
                {
                    char _buffer[_CMD_AMP_CNT];
                    for(uint8_t i = 0; i < _CMD_AMP_CNT; ++i) { _buffer[i] = cmd_buffer[AMP_OFFSET+i]; }
                    amp_value = atoi(_buffer);
                }

                /* Compute frequency. */
                {
                    char _buffer[_CMD_FRQ_CNT];
                    for(uint8_t i = 0; i < _CMD_FRQ_CNT; ++i) { _buffer[i] = cmd_buffer[FREQ_OFFSET+i]; }
                    freq_value = atoi(_buffer);
                }

                /* Compute waveform. */
                {
                    waveform_index = cmd_buffer[WAVE_OFFSET] - '0';
                }
            } 
            else
            {
               /* Clear cmd_buffer. */
                for(uint8_t i = 0; i < FULL_CMD_CNT; i += 1)
                {
                    cmd_buffer[i] = 0;
                }
            }

            // Trigger a new reception.
            UART_ReceivePayload(cmd_buffer, FULL_CMD_CNT);
            UART_SendPayload((uint8_t *)"\r>", 2);
            while (0 == UART_IsTxComplete());
        }
        case GENERATE_WAVE:
        {
            // Execute waveform..
            if(waveform_index < WAVEFORM_NUM)
            {
				float time = (1/(float)freq_value)*1000;
                waveform[waveform_index](amp_value, time);
            }
            // Keep in generate wave if no command it received.
            currentState = (1 == UART_IsRxComplete()) ? UPDATE_WAVE : GENERATE_WAVE;
            break;
        }
        default: {/* Do nothing.*/}
    }
}

