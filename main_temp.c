#include <stm32f031x6.h>
#include <stdint.h>
#include "display.h"
#include "musical_notes.h"
#include "sound.h"
void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);

volatile uint32_t milliseconds;
volatile uint32_t gameTime = 60000; // 10 seconds in milliseconds
volatile uint32_t score = 0;

volatile int wolf_toggle = 0; // Toggles between open and closed wolf image
uint16_t wolf1X = 20, wolf1Y = 40;
uint16_t wolf2X = 80, wolf2Y = 100;
int wolf1_dirX = 1, wolf1_dirY = 1; // Direction for wolf movement
int wolf2_dirX = -1, wolf2_dirY = -1;

uint32_t *background_tune_notes; // beep
uint32_t *background_tune_times;
uint32_t background_tune_note_count;
uint32_t background_repeat_tune;

const uint32_t my_notes[]={A4,C3,B5,D3,F2};
const uint32_t my_note_times[]={200,100,300,200,500};

// Variables for playing beep sound whenever carrot is eaten
uint32_t beepSound = C1;
uint32_t beepDuration = 100;
volatile uint32_t beepTimer = 0;


const uint16_t carrot[]= { 0,0,0,0,0,0,0,0,0,0,54815,54815,54815,54815,0,0,0,0,0,0,0,0,0,0,0,54815,54815,54815,54815,54815,0,0,0,0,0,0,0,0,0,54815,45333,45333,45333,45333,45333,45333,45333,0,0,0,0,0,0,0,0,54815,45333,65315,65315,65315,54815,45333,45333,45333,0,0,0,0,0,0,0,65315,55579,55579,65315,65315,45333,45333,45333,45333,0,0,0,0,0,0,65315,65315,65315,55579,55579,65315,65315,65315,45333,54815,0,0,0,0,0,0,65315,65315,65315,55579,55579,65315,65315,55579,45333,54815,0,0,0,0,0,65315,55579,55579,65315,65315,55579,65315,65315,55579,45333,54815,0,0,0,0,65315,65315,65315,55579,65315,65315,65315,65315,55579,55579,55579,0,0,0,0,0,65315,65315,65315,55579,55579,65315,65315,65315,55579,55579,0,0,0,0,0,55579,65315,65315,65315,65315,55579,65315,65315,65315,55579,0,0,0,0,0,65315,55579,55579,65315,65315,65315,65315,65315,65315,0,0,0,0,0,0,0,65315,65315,55579,65315,65315,65315,0,0,0,0,0,0,0,0,0,0,65315,65315,65315,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

};


const uint16_t rightbunny1[]= { 0,0,0,0,0,0,0,0,57293,57293,20801,0,0,0,0,0,0,0,0,0,0,0,0,57293,57293,20801,20801,0,0,0,0,0,0,0,0,0,0,0,57293,57293,57293,20801,20801,0,0,0,0,0,0,0,0,0,0,57293,57293,57293,20801,20801,0,0,0,0,0,0,0,0,0,0,57293,57293,57293,20801,20801,20801,0,0,0,0,0,0,0,0,0,57293,57293,57293,20801,20801,0,0,0,0,0,0,0,0,0,0,0,57293,57293,20801,20801,20801,0,0,0,0,0,0,0,0,0,57293,57293,57293,57293,20801,57293,57293,57293,57293,57293,57293,57293,0,0,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,0,0,57293,0,65535,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,0,65535,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,20801,20801,57293,57293,20801,20801,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,20801,20801,0,0,0,20801,20801,57293,57293,57293,0,0,0,0,57293,57293,57293,20801,20801,0,20801,57293,57293,57293,0,0,0,0,0,0,0,0,57293,57293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

};

const uint16_t rightbunny2[]= { 0,0,0,0,0,57293,20801,0,0,0,0,0,0,0,0,0,0,0,0,0,57293,57293,20801,0,0,0,0,0,0,0,0,0,0,0,0,57293,57293,57293,20801,0,0,0,0,0,0,0,0,0,0,0,0,57293,57293,57293,20801,0,0,0,0,0,0,0,0,0,0,0,0,57293,57293,20801,20801,0,0,0,0,0,0,0,0,0,0,0,57293,57293,57293,20801,0,0,0,0,0,0,0,0,0,0,0,0,57293,57293,20801,20801,20801,0,0,0,0,0,0,0,0,0,0,57293,57293,57293,57293,20801,57293,57293,57293,57293,57293,57293,57293,0,0,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,0,0,57293,0,65535,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,0,65535,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,20801,20801,20801,20801,57293,57293,57293,20801,57293,57293,57293,20801,20801,57293,57293,20801,20801,20801,0,0,0,57293,57293,20801,0,0,0,20801,57293,57293,57293,0,0,0,0,0,0,0,57293,20801,0,0,0,20801,57293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

};

const uint16_t leftbunny1[]= { 0,0,0,0,0,20801,57293,57293,0,0,0,0,0,0,0,0,0,0,0,0,0,20801,20801,57293,57293,0,0,0,0,0,0,0,0,0,0,0,0,20801,20801,57293,57293,57293,0,0,0,0,0,0,0,0,0,0,0,0,20801,20801,57293,57293,57293,0,0,0,0,0,0,0,0,0,0,0,20801,20801,20801,57293,57293,57293,0,0,0,0,0,0,0,0,0,0,0,0,20801,20801,57293,57293,57293,0,0,0,0,0,0,0,0,0,0,0,20801,20801,20801,57293,57293,0,0,0,0,0,0,57293,57293,57293,57293,57293,57293,57293,20801,57293,57293,57293,57293,0,0,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,65535,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,65535,0,57293,57293,57293,20801,20801,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,0,20801,20801,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,20801,20801,20801,20801,57293,57293,57293,0,0,0,0,57293,57293,57293,20801,20801,0,0,0,57293,57293,0,0,0,0,0,0,0,0,57293,57293,57293,20801,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

};

const uint16_t leftbunny2[]= { 0,0,0,0,0,0,0,0,0,20801,57293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20801,57293,57293,0,0,0,0,0,0,0,0,0,0,0,0,0,20801,57293,57293,57293,0,0,0,0,0,0,0,0,0,0,0,0,20801,57293,57293,57293,0,0,0,0,0,0,0,0,0,0,0,0,20801,20801,57293,57293,0,0,0,0,0,0,0,0,0,0,0,0,0,20801,57293,57293,57293,0,0,0,0,0,0,0,0,0,0,0,20801,20801,20801,57293,57293,0,0,0,0,0,57293,57293,57293,57293,57293,57293,57293,20801,57293,57293,57293,57293,0,0,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,65535,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,65535,0,57293,20801,20801,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,20801,20801,20801,57293,57293,20801,20801,57293,57293,57293,20801,57293,57293,57293,20801,20801,0,0,0,57293,57293,57293,20801,0,0,0,20801,57293,57293,0,0,0,0,0,0,0,0,57293,20801,0,0,0,20801,57293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

};

const uint16_t upbunny1[]= { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,57293,57293,57293,57293,0,0,0,0,0,0,0,0,0,0,0,57293,57293,57293,57293,57293,57293,0,0,0,0,0,0,0,0,0,0,57293,57293,57293,57293,57293,57293,57293,0,0,0,0,0,0,0,0,57293,57293,57293,57293,4170,57293,57293,57293,0,0,0,0,0,0,0,0,4170,57293,57293,57293,4170,57293,57293,57293,4170,0,0,0,0,0,0,0,4170,57293,57293,57293,4170,57293,57293,57293,4170,0,0,0,0,0,0,4170,57293,57293,57293,57293,4170,57293,57293,57293,4170,4170,0,0,0,0,0,57293,57293,57293,57293,4170,4170,57293,57293,57293,57293,57293,0,0,0,0,57293,57293,57293,57293,57293,4170,4170,4170,4170,57293,57293,57293,57293,0,0,0,57293,57293,57293,57293,4170,4170,57293,57293,4170,57293,57293,57293,57293,0,0,0,0,4170,4170,4170,4170,57293,57293,57293,4170,4170,4170,4170,0,0,0,0,0,4170,4170,57293,57293,57293,57293,57293,57293,57293,4170,4170,0,0,0,0,0,0,4170,4170,57293,57293,57293,57293,57293,4170,4170,0,0,0,0,0,0,0,0,0,4170,57293,57293,57293,4170,0,0,0,0,0,0,0,0,0,0,0,4170,4170,4170,4170,0,0,0,0,0,0,

};

const uint16_t downbunny1[]={ 0,0,0,0,0,0,4170,4170,4170,4170,0,0,0,0,0,0,0,0,0,0,0,0,4170,57293,57293,57293,4170,0,0,0,0,0,0,0,0,0,4170,4170,57293,57293,57293,57293,57293,4170,4170,0,0,0,0,0,0,4170,4170,57293,57293,57293,57293,57293,57293,57293,4170,4170,0,0,0,0,0,4170,4170,4170,4170,57293,57293,57293,4170,4170,4170,4170,0,0,0,0,57293,57293,57293,57293,4170,4170,57293,57293,4170,57293,57293,57293,57293,0,0,0,57293,57293,57293,57293,57293,4170,4170,4170,4170,57293,57293,57293,57293,0,0,0,0,57293,57293,57293,57293,4170,4170,57293,57293,57293,57293,57293,0,0,0,0,0,4170,57293,57293,57293,57293,4170,57293,57293,57293,4170,4170,0,0,0,0,0,0,4170,57293,57293,57293,4170,57293,57293,57293,4170,0,0,0,0,0,0,0,4170,57293,57293,57293,4170,57293,57293,57293,4170,0,0,0,0,0,0,0,57293,57293,57293,57293,4170,57293,57293,57293,0,0,0,0,0,0,0,0,0,57293,57293,57293,57293,57293,57293,57293,0,0,0,0,0,0,0,0,0,57293,57293,57293,57293,57293,57293,0,0,0,0,0,0,0,0,0,0,0,57293,57293,57293,57293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

};

const uint16_t wolfopen[]= { 30918,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30918,30918,30918,0,50209,50209,50209,50209,30918,50209,50209,50209,50209,0,0,30918,30918,30918,30918,65535,65535,50209,50209,30918,30918,30918,50209,30918,65535,65535,30918,30918,50209,50209,30918,30918,65535,65535,30918,30918,30918,30918,65535,65535,65535,30918,30918,50209,50209,50209,0,0,30918,30918,30918,30918,30918,30918,30918,30918,0,0,50209,50209,50209,50209,61307,0,0,0,0,30918,30918,61307,0,0,0,61307,61307,50209,0,0,50209,61307,0,0,65535,61307,30918,61307,65535,0,0,61307,61307,50209,0,0,50209,61307,61307,61307,61307,61307,61307,61307,61307,61307,61307,61307,61307,50209,0,0,50209,50209,50209,61307,61307,0,0,50209,0,61307,50209,50209,50209,0,0,0,61307,61307,50209,50209,50209,61307,0,0,61307,50209,50209,61307,61307,0,0,0,61307,61307,50209,65535,50209,50209,50209,50209,50209,65535,50209,61307,61307,0,0,61307,18498,61307,50209,65535,65535,14168,14168,14168,65535,65535,50209,61307,18498,18498,0,18498,18498,18498,61307,50209,65535,14168,14168,14168,65535,50209,50209,18498,18498,18498,0,0,0,18498,18498,50209,50209,65535,14168,65535,50209,50209,0,0,0,0,0,0,0,0,0,0,50209,65535,50209,65535,50209,0,0,0,0,0,0,0,0,0,0,0,0,61307,61307,61307,0,0,0,0,0,0,0,

};

const uint16_t wolfclosed[]={ 30918,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30918,30918,30918,0,50209,50209,50209,50209,30918,50209,50209,50209,50209,0,0,30918,30918,30918,30918,65535,65535,50209,50209,30918,30918,30918,50209,30918,65535,65535,30918,30918,50209,50209,30918,30918,65535,65535,30918,30918,30918,30918,65535,65535,65535,30918,30918,50209,50209,50209,0,0,30918,30918,30918,30918,30918,30918,30918,30918,0,0,50209,50209,50209,50209,61307,0,0,0,30918,30918,30918,61307,30918,0,0,61307,61307,50209,0,0,50209,61307,0,0,0,61307,30918,61307,0,0,0,61307,61307,50209,0,0,50209,61307,61307,61307,65535,61307,61307,61307,65535,61307,61307,61307,61307,50209,0,0,50209,50209,50209,61307,61307,61307,61307,61307,61307,61307,61307,50209,50209,0,0,0,61307,61307,50209,61307,61307,61307,61307,61307,61307,61307,50209,61307,61307,0,0,0,61307,61307,50209,61307,61307,0,0,30918,0,61307,50209,61307,61307,0,0,61307,18498,61307,50209,18498,61307,61307,0,0,61307,18498,18498,61307,18498,18498,0,18498,18498,18498,61307,50209,18498,61307,61307,61307,18498,50209,50209,18498,18498,18498,0,0,0,18498,18498,50209,50209,18498,50209,50209,50209,18498,0,0,0,0,0,0,0,0,0,0,18498,18498,18498,18498,18498,0,0,0,0,0,0,0,0,0,0,0,0,61307,61307,61307,0,0,0,0,0,0,0,

};

const uint32_t deadbunny[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,0,0,0,0,0,0,0,0,0,0,57293,57293,57293,14168,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,14168,57293,57293,57293,0,0,0,0,0,0,0,0,57293,57293,14168,14168,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,14168,57293,57293,57293,57293,0,0,0,0,0,0,0,57293,57293,14168,14168,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,14168,14168,57293,57293,57293,57293,0,0,0,0,0,0,57293,57293,14168,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,14168,14168,57293,57293,57293,0,0,0,0,0,0,57293,14168,14168,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,14168,14168,57293,57293,57293,0,0,0,0,57293,57293,14168,14168,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,14168,14168,57293,57293,57293,0,0,0,0,57293,57293,14168,14168,57293,0,57293,57293,57293,0,57293,57293,57293,57293,57293,0,57293,57293,57293,0,57293,57293,57293,14168,14168,57293,57293,57293,0,0,0,57293,57293,14168,14168,14168,57293,57293,0,57293,0,57293,57293,57293,57293,57293,57293,57293,0,57293,0,57293,57293,57293,57293,14168,14168,57293,57293,57293,0,0,0,57293,57293,14168,14168,14168,57293,57293,57293,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,0,57293,57293,57293,57293,57293,14168,14168,14168,57293,57293,57293,0,0,57293,57293,14168,14168,14168,57293,57293,0,57293,0,57293,57293,57293,57293,57293,57293,57293,0,57293,0,57293,57293,57293,57293,14168,14168,14168,57293,57293,57293,0,0,57293,57293,14168,14168,14168,57293,0,57293,57293,57293,57293,57293,57293,14168,57293,57293,0,57293,57293,57293,0,57293,57293,0,14168,14168,14168,57293,57293,57293,0,0,57293,57293,14168,14168,14168,0,57293,57293,57293,57293,57293,57293,14168,57293,14168,57293,57293,57293,57293,57293,57293,57293,0,0,0,14168,14168,57293,57293,57293,57293,0,57293,57293,14168,14168,14168,0,0,0,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,0,0,0,0,14168,14168,14168,57293,57293,57293,0,57293,57293,14168,14168,14168,0,0,0,0,57293,57293,14168,14168,14168,14168,14168,14168,14168,57293,57293,57293,57293,0,0,0,0,14168,14168,57293,57293,57293,57293,57293,57293,14168,14168,14168,0,0,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,0,0,0,14168,14168,57293,57293,57293,57293,57293,57293,14168,14168,0,0,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,14168,57293,57293,57293,57293,0,0,14168,14168,57293,57293,57293,57293,57293,57293,14168,14168,0,0,0,57293,57293,57293,14168,57293,57293,57293,57293,57293,57293,57293,57293,14168,57293,57293,57293,57293,0,0,14168,14168,57293,57293,57293,57293,57293,14168,14168,14168,0,0,57293,57293,57293,57293,14168,57293,57293,57293,57293,57293,57293,57293,57293,14168,57293,57293,57293,57293,0,0,14168,14168,14168,57293,57293,57293,57293,14168,14168,14168,0,0,57293,57293,14168,14168,57293,57293,57293,57293,57293,57293,57293,57293,57293,14168,57293,57293,57293,57293,57293,0,14168,14168,14168,57293,57293,57293,14168,14168,14168,0,0,0,14168,14168,14168,14168,57293,57293,14168,57293,57293,57293,57293,57293,57293,14168,14168,57293,57293,57293,57293,0,14168,14168,14168,14168,57293,57293,14168,14168,14168,0,0,0,14168,57293,57293,57293,14168,14168,14168,57293,57293,57293,57293,14168,14168,14168,14168,14168,57293,57293,57293,0,14168,14168,14168,14168,57293,14168,14168,14168,0,0,0,0,57293,57293,57293,57293,14168,57293,57293,57293,57293,14168,57293,14168,14168,14168,14168,14168,0,0,0,0,0,14168,14168,14168,57293,14168,14168,14168,0,0,0,0,57293,57293,57293,14168,14168,57293,57293,57293,14168,14168,14168,14168,14168,14168,14168,0,0,0,0,0,0,14168,14168,14168,57293,14168,14168,0,0,0,0,0,57293,57293,57293,14168,14168,57293,57293,57293,14168,14168,14168,0,0,0,0,0,0,0,0,0,0,0,14168,14168,57293,14168,14168,0,0,0,0,0,57293,57293,14168,14168,0,57293,57293,14168,14168,14168,0,0,0,0,0,0,0,0,0,0,0,0,0,14168,14168,14168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

};

int main()
{
	int hinverted = 0;
	int vinverted = 0;
	int toggle = 0;
	int righttoggle=0;
	int lefttoggle=0;
	int uptoggle=0;
	int downtoggle=0;
	int hmoved = 0;
	int vmoved = 0;
	uint16_t x = 50;
	uint16_t y = 50;
	uint16_t oldx = x;
	uint16_t oldy = y;
	uint16_t carrotX = 30, carrotY = 80; // Initial carrot position
	uint32_t lastSecond = 10; // Track the last displayed second
	initClock();
	initSysTick();
	setupIO();
	//putImage(20,80,12,16,dg1,0,0);
	putImage(30,80,16,16,carrot,0,0);
	initSound();
	background_tune_notes=my_notes; // beep
	background_tune_times=my_note_times;
	background_tune_note_count=5;
	background_repeat_tune=1;
	while(1)
	{
			// Wolf animation toggle logic
    	// Clear the previous wolf positions (increase the clearing area slightly to avoid residue)
    	fillRectangle(wolf1X - 1, wolf1Y - 1, 18, 18, 0); // Clear a slightly larger area
    	fillRectangle(wolf2X - 1, wolf2Y - 1, 18, 18, 0); // Clear a slightly larger area

		if (wolf_toggle) 
		{
			putImage(wolf1X, wolf1Y, 16, 16, wolfopen, 0, 0);
			putImage(wolf2X, wolf2Y, 16, 16, wolfopen, 0, 0);
		} else 
		{
			putImage(wolf1X, wolf1Y, 16, 16, wolfclosed, 0, 0);
			putImage(wolf2X, wolf2Y, 16, 16, wolfclosed, 0, 0);
		}
		wolf_toggle ^= 1;

		// Wolf movement logic
		wolf1X += wolf1_dirX;
		wolf1Y += wolf1_dirY;
		wolf2X += wolf2_dirX;
		wolf2Y += wolf2_dirY;

		// Boundary check for wolves
		if (wolf1X <= 0 || wolf1X >= 110) wolf1_dirX *= -1;
		if (wolf1Y <= 0 || wolf1Y >= 140) wolf1_dirY *= -1;
		if (wolf2X <= 0 || wolf2X >= 110) wolf2_dirX *= -1;
		if (wolf2Y <= 0 || wolf2Y >= 140) wolf2_dirY *= -1;

		// Collision detection with wolves
		if (isInside(wolf1X, wolf1Y, 16, 16, x, y) || isInside(wolf2X, wolf2Y, 16, 16, x, y))
		 {
						// Collision detection with wolves
			if (isInside(wolf1X, wolf1Y, 16, 16, x, y) || isInside(wolf2X, wolf2Y, 16, 16, x, y)) {
				clear();  // Clear the screen to show the deadbunny image clearly

				// Position the deadbunny image in the center
				uint16_t deadbunnyX = (128 - 32) / 2;  // Adjust 128 to screen width and 32 to deadbunny width
				uint16_t deadbunnyY = (160 - 32) / 2;  // Adjust 160 to screen height and 32 to deadbunny height

				putImage(deadbunnyX, deadbunnyY, 32, 32, deadbunny, 0, 0);  // Display deadbunny in center

				// Display "RIP" text in red on top of the deadbunny image
				printTextX2("RIP", deadbunnyX + 8, deadbunnyY - 20, RGBToWord(0xff, 0, 0), 0);

				delay(3000);  // Pause to show deadbunny image and RIP text before game-over screen

				// Display existing game-over message
				clear();
					printText("Game Over", 40, 60, RGBToWord(0xff, 0, 0), 0);    // Adjust position if needed
					printText("Hit by Wolf", 40, 80, RGBToWord(0xff, 0, 0), 0);  // Adjust position if needed
				delay(5000);
				break;
			}

    }

		if (gameTime == 0)
    	{
        	clear(); // Clear the screen
        	printTextX2("Game Over", 20, 60, RGBToWord(0xff, 0, 0), 0);
        	printNumberX2(score, 40, 100, RGBToWord(0xff, 0xff, 0), 0);
        	delay(5000); // Delay to show final score
        	break; // End game loop
    	}

				// Display the timer in seconds on the screen
		uint32_t currentSecond = gameTime / 1000;
		if (currentSecond != lastSecond)
		{
			fillRectangle(0, 0, 30, 16, 0); // Clear previous timer
			printNumber(currentSecond, 0, 0, RGBToWord(0xff, 0xff, 0), 0); // Display remaining seconds
			lastSecond = currentSecond;
		}

		// Display the current score near the timer
		fillRectangle(0, 20, 50, 16, 0); // Clear previous score
		printText("Carrots:", 0, 20, RGBToWord(0xff, 0xff, 0), 0);
		printNumber(score, 60, 20, RGBToWord(0xff, 0xff, 0), 0);

		hmoved = vmoved = 0;
		hinverted = vinverted = 0;

		if ((GPIOB->IDR & (1 << 4))==0) // right pressed
		{					
			if (x < 110)
			{
				//erasing the old bunny by filling the area with the background color (0=black)
				fillRectangle(oldx,oldy,16,16,0);
				x = x + 1;
				hmoved = 1;
				hinverted=0;

				if(righttoggle)
				{
				putImage(x,y,16,16,rightbunny2,0,0);
				}
				else
				{
				putImage(x,y,16,16,rightbunny1,0,0);

				//flip the toggle value
				
				}
				righttoggle = righttoggle ^ 1;
				//update the oldx and oldy variables to the new position for next movement
				oldx= x;
				oldy =y;
				
			}	

							
		}
		if ((GPIOB->IDR & (1 << 5))==0) // left pressed
		{			
			
			if (x > 10)
			{
				//erasing the old bunny by filling the area with the background color (0=black)
				fillRectangle(oldx,oldy,16,16,0);
				x = x - 1;
				hmoved = 1;
				hinverted=1;

				//toggle between leftbunny1 and leftbunny2
				if(lefttoggle)
				{
					putImage(x,y,16,16,leftbunny2,1,0); //flipped horizontally
				}
				else
				{
					putImage(x,y,16,16,leftbunny1,1,0); //flipped horizontally
				}
				lefttoggle = lefttoggle ^ 1;
				//update the oldx and oldy variables to the new position for next movement
				oldx= x;
				oldy =y;
			
			}			
		}
		if ( (GPIOA->IDR & (1 << 11)) == 0) // down pressed
		{
			if (y < 140) //move down boundary
			{
				fillRectangle(oldx,oldy,16,16,0); //clear the old position
				y = y + 1;			
				vmoved = 1;
				vinverted = 0;

				if(downtoggle)
				{
				putImage(x,y,16,16,downbunny1,0,0);

				}
				else
				{
					putImage(x,y,16,16,downbunny1,0,0); //flipped horizontally
				}
				//update the oldx and oldy variables to the new position for next movement
				oldx= x;
				oldy =y;

			}

		}
		if ( (GPIOA->IDR & (1 << 8)) == 0) // up pressed
		{			
			if (y > 16)
			{
				fillRectangle(oldx,oldy,16,16,0); //clear the old position
				y = y - 1;
				vmoved = 1;
				vinverted = 1;

				if(uptoggle)
				{
				putImage(x,y,16,16,upbunny1,1,0);
				}
				else
				{
				putImage(x,y,16,16,upbunny1,1,0); //flipped horizontally
				}

				//update the oldx and oldy variables to the new position for next movement
				oldx= x;
				oldy =y;
		
			}

		}
		if ((vmoved) || (hmoved))
		{
			// only redraw if there has been some movement (reduces flicker)
			fillRectangle(oldx,oldy,16,16,0);
			//oldx = x;
			//oldy = y;					
			if (hmoved && hinverted ==1) //moving right
			{
				if (lefttoggle)
					putImage(x,y,16,16,leftbunny1,hinverted,0);
				else
					putImage(x,y,16,16,leftbunny2,hinverted,0);
				
				toggle = toggle ^ 1;
				//lefttoggle ^= 1;
			}
			else if(hmoved && hinverted  ==0)// moving left
			{
       		 
            	if (righttoggle)
                	putImage(x, y, 16, 16, rightbunny2, 1, 0);
            	else
                	putImage(x, y, 16, 16, rightbunny1, 1, 0);
            	
				toggle = toggle ^ 1; // toggle between rightbunny1 and rightbunny2
				
        
			}

			else if (vmoved && vinverted == 0) // Moving down
			{
				if (downtoggle)
					putImage(x, y, 16, 16, downbunny1, 0, 0);
				else
					putImage(x, y, 16, 16, downbunny1, 0, 0); // Additional down frame
				downtoggle ^= 1;
			}

			else if (vmoved && vinverted == 1) // Moving up
			{
				if (uptoggle)
					putImage(x, y, 16, 16, upbunny1, 1, 0);
				else
					putImage(x, y, 16, 16, upbunny1, 1, 0); // Additional up frame
				uptoggle ^= 1;
			}

			oldx = x;
			oldy = y;



/*			// Now check for an overlap by checking to see if ANY of the 4 corners of deco are within the target area
			if (isInside(20,80,12,16,x,y) || isInside(20,80,12,16,x+12,y) || isInside(20,80,12,16,x,y+16) || isInside(20,80,12,16,x+12,y+16) )
			{
				printTextX2("GLUG!", 10, 20, RGBToWord(0xff,0xff,0), 0);
			}
*/
		//if (vmoved || hmoved)
		
			if (isInside(carrotX, carrotY, 12, 16, x, y) ||
				isInside(carrotX, carrotY, 12, 16, x + 12, y) ||
				isInside(carrotX, carrotY, 12, 16, x, y + 16) ||
				isInside(carrotX, carrotY, 12, 16, x + 12, y + 16))
			{
				score++;
				beepTimer = beepDuration;
				playNote(beepSound);
				printNumber(score, 5, 5, RGBToWord(0xff, 0xff, 0), 0);
				fillRectangle(carrotX, carrotY, 16, 16, 0); // Erase carrot
				carrotX = (rand() % 100) + 10; // Random x position within screen bounds
				carrotY = (rand() % 120) + 20; // Random y position within screen bounds
			}
			fillRectangle(carrotX, carrotY, 12, 16, 0); // Clear old carrot
			putImage(carrotX, carrotY, 16, 16, carrot, 0, 0); // Draw new carrot
		


		}		
		delay(50);
	}
	return 0;
}
void initSysTick(void)
{
	SysTick->LOAD = 48000;
	SysTick->CTRL = 7;
	SysTick->VAL = 10;
	__asm(" cpsie i "); // enable interrupts
}
void SysTick_Handler(void)
{
	milliseconds++;
    if (gameTime > 0)
        gameTime--; // Decrease game time every millisecond

	static int index = 0;
	static int current_note_timer; // var name

	// Handle background music here
    if (background_tune_notes != 0)
    {
        if (current_note_timer == 0) // Check if it's time to play the next note
        {
            index++; // Move to the next note
            if (index >= background_tune_note_count)
            {
                // If there are no more notes to play
                if (background_repeat_tune == 0)
                {
                    // Don't repeat the tune
                    background_tune_notes = 0; // Stop the music
                }
                else
                {
                    // Reset to the first note and repeat the tune
                    index = 0;
                }
            }
            else
            {
                current_note_timer = background_tune_times[index]; // Set timer for the next note
                playNote(background_tune_notes[index]); // Play the current note
            }
        }

        // Decrement the current note timer
        if (current_note_timer != 0)
            current_note_timer--;
    }

    // Handle beep sound for when a carrot is eaten
    if (beepTimer > 0)
    {
        beepTimer--; // Decrease the beep timer
        if (beepTimer == 0)
        {
            // Optionally stop the beep or reset the beep status
            // (You could stop the note if it’s currently playing, for example)
            // This could be a place to call a function to stop the beep sound
            // e.g., stopNote(); if you have a function defined to stop a sound
        }
    }
}

void initClock(void)
{
// This is potentially a dangerous function as it could
// result in a system with an invalid clock signal - result: a stuck system
        // Set the PLL up
        // First ensure PLL is disabled
        RCC->CR &= ~(1u<<24);
        while( (RCC->CR & (1 <<25))); // wait for PLL ready to be cleared
        
// Warning here: if system clock is greater than 24MHz then wait-state(s) need to be
// inserted into Flash memory interface
				
        FLASH->ACR |= (1 << 0);
        FLASH->ACR &=~((1u << 2) | (1u<<1));
        // Turn on FLASH prefetch buffer
        FLASH->ACR |= (1 << 4);
        // set PLL multiplier to 12 (yielding 48MHz)
        RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
        RCC->CFGR |= ((1<<21) | (1<<19) ); 

        // Need to limit ADC clock to below 14MHz so will change ADC prescaler to 4
        RCC->CFGR |= (1<<14);

        // and turn the PLL back on again
        RCC->CR |= (1<<24);        
        // set PLL as system clock source 
        RCC->CFGR |= (1<<1);
}
void delay(volatile uint32_t dly)
{
	uint32_t end_time = dly + milliseconds;
	while(milliseconds != end_time)
		__asm(" wfi "); // sleep
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	/*
	*/
	uint32_t mode_value = Port->MODER;
	Mode = Mode << (2 * BitNumber);
	mode_value = mode_value & ~(3u << (BitNumber * 2));
	mode_value = mode_value | Mode;
	Port->MODER = mode_value;
}
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py)
{
	// checks to see if point px,py is within the rectange defined by x,y,w,h
	uint16_t x2,y2;
	x2 = x1+w;
	y2 = y1+h;
	int rvalue = 0;
	if ( (px >= x1) && (px <= x2))
	{
		// ok, x constraint met
		if ( (py >= y1) && (py <= y2))
			rvalue = 1;
	}
	return rvalue;
}

void setupIO()
{
	RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
	display_begin();
	pinMode(GPIOB,4,0);
	pinMode(GPIOB,5,0);
	pinMode(GPIOA,8,0);
	pinMode(GPIOA,11,0);
	enablePullUp(GPIOB,4);
	enablePullUp(GPIOB,5);
	enablePullUp(GPIOA,11);
	enablePullUp(GPIOA,8);
}
