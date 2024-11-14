#include <stm32f031x6.h>
#include "display.h"
#include "sound.h"
#include "musical_notes.h"
#include <stdbool.h> // For boolean values
#include <stdlib.h>   // For rand() and srand()
#include <time.h> // For time
#include "serial.h"

// Function signatures
void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void chest_box_show();
void chest_box_clear();
void refresh_chest_if_needed();
void initialize_game();
void game(uint32_t *score);
void mainMenu(void);

// Turn LEDs on and off
void RedOn(void);
void RedOff(void);
void YellowOn(void);
void YellowOff(void);
void GreenOn(void);
void GreenOff(void);

void flashGreenLED(void);
void flashRedLED(void);
void flashYellowLED(void);

int _write(int file, char *ptr, int len);

// Global variables
volatile uint32_t milliseconds;
volatile uint32_t gameTime = 60000; // 30 seconds in milliseconds
volatile uint32_t score = 0;
volatile int wolf_toggle = 0; // Toggles between open and closed wolf image
uint16_t wolf1X = 20, wolf1Y = 40;
uint16_t wolf2X = 80, wolf2Y = 100;
int wolf1_dirX = 1, wolf1_dirY = 1; // Direction for wolf movement
int wolf2_dirX = -1, wolf2_dirY = -1;
int chest_display_start_time = 0;
bool chest_visible = false;
bool chest_touched = false;
bool chest_first_show = false;
uint16_t chestX = 0, chestY = 0; // Initial chest position 30  50


// Images used in game
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

const uint32_t deadbunny[]={ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,0,0,0,0,0,0,0,0,0,0,57293,57293,57293,14168,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,14168,57293,57293,57293,0,0,0,0,0,0,0,0,57293,57293,14168,14168,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,14168,57293,57293,57293,57293,0,0,0,0,0,0,0,57293,57293,14168,14168,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,14168,14168,57293,57293,57293,57293,0,0,0,0,0,0,57293,57293,14168,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,14168,14168,57293,57293,57293,0,0,0,0,0,0,57293,14168,14168,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,14168,14168,57293,57293,57293,0,0,0,0,57293,57293,14168,14168,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,14168,14168,57293,57293,57293,0,0,0,0,57293,57293,14168,14168,57293,0,57293,57293,57293,0,57293,57293,57293,57293,57293,0,57293,57293,57293,0,57293,57293,57293,14168,14168,57293,57293,57293,0,0,0,57293,57293,14168,14168,14168,57293,57293,0,57293,0,57293,57293,57293,57293,57293,57293,57293,0,57293,0,57293,57293,57293,57293,14168,14168,57293,57293,57293,0,0,0,57293,57293,14168,14168,14168,57293,57293,57293,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,0,57293,57293,57293,57293,57293,14168,14168,14168,57293,57293,57293,0,0,57293,57293,14168,14168,14168,57293,57293,0,57293,0,57293,57293,57293,57293,57293,57293,57293,0,57293,0,57293,57293,57293,57293,14168,14168,14168,57293,57293,57293,0,0,57293,57293,14168,14168,14168,57293,0,57293,57293,57293,57293,57293,57293,14168,57293,57293,0,57293,57293,57293,0,57293,57293,0,14168,14168,14168,57293,57293,57293,0,0,57293,57293,14168,14168,14168,0,57293,57293,57293,57293,57293,57293,14168,57293,14168,57293,57293,57293,57293,57293,57293,57293,0,0,0,14168,14168,57293,57293,57293,57293,0,57293,57293,14168,14168,14168,0,0,0,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,0,0,0,0,14168,14168,14168,57293,57293,57293,0,57293,57293,14168,14168,14168,0,0,0,0,57293,57293,14168,14168,14168,14168,14168,14168,14168,57293,57293,57293,57293,0,0,0,0,14168,14168,57293,57293,57293,57293,57293,57293,14168,14168,14168,0,0,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,0,0,0,14168,14168,57293,57293,57293,57293,57293,57293,14168,14168,0,0,0,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,57293,14168,57293,57293,57293,57293,0,0,14168,14168,57293,57293,57293,57293,57293,57293,14168,14168,0,0,0,57293,57293,57293,14168,57293,57293,57293,57293,57293,57293,57293,57293,14168,57293,57293,57293,57293,0,0,14168,14168,57293,57293,57293,57293,57293,14168,14168,14168,0,0,57293,57293,57293,57293,14168,57293,57293,57293,57293,57293,57293,57293,57293,14168,57293,57293,57293,57293,0,0,14168,14168,14168,57293,57293,57293,57293,14168,14168,14168,0,0,57293,57293,14168,14168,57293,57293,57293,57293,57293,57293,57293,57293,57293,14168,57293,57293,57293,57293,57293,0,14168,14168,14168,57293,57293,57293,14168,14168,14168,0,0,0,14168,14168,14168,14168,57293,57293,14168,57293,57293,57293,57293,57293,57293,14168,14168,57293,57293,57293,57293,0,14168,14168,14168,14168,57293,57293,14168,14168,14168,0,0,0,14168,57293,57293,57293,14168,14168,14168,57293,57293,57293,57293,14168,14168,14168,14168,14168,57293,57293,57293,0,14168,14168,14168,14168,57293,14168,14168,14168,0,0,0,0,57293,57293,57293,57293,14168,57293,57293,57293,57293,14168,57293,14168,14168,14168,14168,14168,0,0,0,0,0,14168,14168,14168,57293,14168,14168,14168,0,0,0,0,57293,57293,57293,14168,14168,57293,57293,57293,14168,14168,14168,14168,14168,14168,14168,0,0,0,0,0,0,14168,14168,14168,57293,14168,14168,0,0,0,0,0,57293,57293,57293,14168,14168,57293,57293,57293,14168,14168,14168,0,0,0,0,0,0,0,0,0,0,0,14168,14168,57293,14168,14168,0,0,0,0,0,57293,57293,14168,14168,0,57293,57293,14168,14168,14168,0,0,0,0,0,0,0,0,0,0,0,0,0,14168,14168,14168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const uint16_t chest_box[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,0,0,0,65535,65287,28994,28994,28994,28994,28994,28994,28994,28994,28994,65535,65535,0,0,0,65535,65287,5178,5178,5178,5178,5178,5178,5178,5178,5178,65535,65287,0,0,0,65535,65287,28994,28994,28994,28994,28994,28994,28994,28994,28994,65535,65287,0,0,0,65535,65287,5178,5178,5178,5178,5178,5178,5178,5178,5178,65287,65287,0,0,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,0,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,0,0,65287,65287,5178,5178,65287,65287,22355,65287,65287,5178,5178,65287,65287,0,0,0,65287,65287,28994,28994,28994,65287,65287,65287,28994,28994,28994,65287,65287,0,0,0,65287,65287,5178,5178,5178,5178,5178,5178,5178,5178,5178,65287,65287,0,0,0,65287,65287,28994,28994,28994,28994,28994,28994,28994,28994,28994,65287,65287,0,0,0,65287,65287,5178,5178,5178,5178,5178,5178,5178,5178,5178,65287,65287,0,0,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,0,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,65287,0
};

const uint16_t play_button[] = { 12800,12800,13073,12800,45568,47211,48879,65535,65535,65535,65535,65535,65535,65535,65535,65535,12800,12800,48879,47211,45568,12800,13073,55948,24575,65535,65535,65535,65535,65535,65535,65535,12800,12800,65535,65535,7638,22082,12800,12800,46113,15541,65535,65535,65535,65535,65535,65535,12800,12800,65535,65535,65535,65535,15541,46113,12800,12800,22082,7638,65535,65535,65535,65535,12800,12800,65535,65535,65535,65535,65535,24575,64148,13073,12800,45568,47211,48879,65535,65535,12800,12800,65535,65535,65535,65535,65535,65535,65535,48879,47211,45568,12800,13073,55948,24575,12800,12800,65535,65535,65535,65535,65535,65535,65535,65535,65535,7638,30274,12800,12800,39300,12800,12800,65535,65535,65535,65535,65535,65535,65535,65535,65535,7638,22082,12800,12800,39300,12800,12800,65535,65535,65535,65535,65535,65535,65535,48879,47211,45568,12800,13073,64148,24575,12800,12800,65535,65535,65535,65535,65535,24575,55948,13073,12800,45568,47211,48879,65535,65535,12800,12800,65535,65535,65535,65535,15541,46113,12800,12800,30274,7638,65535,65535,65535,65535,12800,12800,65535,65535,7638,22082,12800,12800,46113,15541,65535,65535,65535,65535,65535,65535,12800,12800,48879,47211,37376,12800,13073,64148,24575,65535,65535,65535,65535,65535,65535,65535,12800,12800,13073,12800,45568,47211,48879,65535,65535,65535,65535,65535,65535,65535,65535,65535,12800,12800,12800,30274,7638,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,23733,22082,23733,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535
};



int main()
{
    // if round == 1, score1 = score, else score2 = score
	int player1_score = 0;
	int player2_score = 0;
	int round;

	initSerial();
	initClock();
	initSysTick();
	setupIO();
	initTimer();
	flashGreenLED();
	initSound();

	mainMenu();
	
	delay(3000);
	clear();


	// while(round)
	//{
		for(round = 1; round <= 2; round++) // Multiplayer implementation
		{
			// Print welcome for player 1 or player 2
			if(round == 1)
			{
				printText("Player 1 Start!", 15, 60, RGBToWord(0xff, 128, 255), 0);    // Adjust position if needed
				flashYellowLED();
				delay(2000);
				clear();
				game(&player1_score);
				clear();

			}
			else if(round == 2)
			{
				printText("Player 2 Start!", 15, 60, RGBToWord(0xff, 127, 255), 0);    // Adjust position if needed
				flashYellowLED();
				delay(2000);
				clear();
				game(&player2_score);
				clear();
			}
			// Controls game speed
			delay(50);


		} //end for loop


		// Display congratulatory message based on final scores
		if(player1_score > player2_score)
		{
			flashYellowLED();
			printText("Player 1 Wins!", 30, 60, RGBToWord(0xff, 0, 0), 0);    // Adjust position if needed
			printText("Good Job!", 20, 80, RGBToWord(0xff, 0, 0), 0);
			delay(3000);
			clear();
		}
		else if(player2_score > player1_score)
		{
			flashYellowLED();
			printText("Player 2 Wins!", 30, 60, RGBToWord(0xff, 0, 0), 0);
			printText("Good Job!", 20, 80, RGBToWord(0xff, 0, 0), 0);
			delay(3000);
			clear();
		}
		else if(player1_score == player2_score)
		{
			flashYellowLED();
			printText("It's a Tie!", 30, 60, RGBToWord(0xff, 0, 0), 0);
			delay(3000);
			clear();
		}

		printText("Play again?", 20, 80, RGBToWord(0xff, 0, 0), 0);
		printText("Press up to restart", 20, 100, RGBToWord(0xff, 0, 0), 0);
		if ((GPIOB->IDR & (1 << 11))==0)
		{
			round = 1;
		}
		else if((GPIOB->IDR & (1 << 8))==0)
		{
			round = 0;
		}
	//}


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

// function used in order to get random numbers to use as seed for srand
void initTimer()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;  // Enable TIM2 clock
    TIM2->PSC = 47999;                   // Set prescaler to slow the timer
    TIM2->CR1 |= TIM_CR1_CEN;            // Start the timer
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
	x2 = x1+w -1;
	y2 = y1+h -1;
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
	pinMode(GPIOA,9,1);
	pinMode(GPIOA,10,1);
	pinMode(GPIOA,12,1);
}


// Function that checks if it's time to show the chest for first time
void chest_box_show()
{
	chest_visible = true; // Mark chest as visible
}


// function that clears the chest box from screen
void chest_box_clear()
{
	fillRectangle(chestX, chestY, 16, 16, 0); // Clear the chest
	chest_visible = false; // Update visibility
}



// Function to refresh the chest if a wolf overlaps it
void refresh_chest_if_needed()
{
    // Only redraw if chest is visible and a wolf is at its position
    if (chest_visible == true &&
			(isInside(wolf1X, wolf1Y, 16, 16, chestX, chestY) || isInside(wolf1X, wolf1Y, 16, 16, chestX + 16, chestY) ||
			isInside(wolf1X, wolf1Y, 16, 16, chestX, chestY + 16) || isInside(wolf1X, wolf1Y, 16, 16, chestX + 16, chestY + 16)

		 	|| isInside(wolf2X, wolf2Y, 16, 16, chestX, chestY) || isInside(wolf2X, wolf2Y, 16, 16, chestX + 16, chestY) ||
			isInside(wolf2X, wolf2Y, 16, 16, chestX, chestY + 16) || isInside(wolf2X, wolf2Y, 16, 16, chestX + 16, chestY + 16)) )
	{
        putImage(chestX, chestY, 16, 16, chest_box, 0, 0);
    }
}


// Game initialization function
void initialize_game()
{
    srand(TIM2->CNT);  // Call srand only once at the start
}


// Function for main game logic
void game(uint32_t *score)
{
	bool game_mode = true;
	int hinverted = 0;
	int vinverted = 0;
	int toggle = 0;
	int righttoggle=0;
	int lefttoggle=0;
	int uptoggle=0;
	int downtoggle=0;
	int hmoved = 0;
	int vmoved = 0;
	uint16_t x = 70;
	uint16_t y = 50;
	uint16_t oldx = x;
	uint16_t oldy = y;
	uint16_t carrotX = 30, carrotY = 80; // Initial carrot position
	uint32_t lastSecond = 10; // Track the last displayed second
	int random_time_start = 0;
	int random_time_end = 0;

	chest_visible = false;
	chest_touched = false;
	chest_first_show = false;
	*score = 0;
	gameTime = 60000;


	while(game_mode)
	{
		// time at which chest will appear in game
		if (gameTime <= 50000 && gameTime >= 44000 && chest_touched == false)
		{
			if (chest_first_show == false)
			{
				initialize_game();

				chestX = (rand() % 100) + 10 ; // Random x position between 10 and 110
				chestY = (rand()  % 90) + 30 ; // Random y position between 20 and 140
				// printNumber(chestY, 60, 80, RGBToWord(0xff, 0xff, 0), 0);

				chest_first_show = true;
				putImage(chestX, chestY, 16, 16, chest_box, 0, 0); // Display the chest
			}
			// if chest_visible is false call chest_box_show() to make it true
			if (chest_visible == false)
			{
				// turns on the chest_visible value
				chest_box_show();

				// random_time_start = TIM2->CNT;
				// printNumber(random_time_start, 60, 80, RGBToWord(0xff, 0xff, 0), 0);
			}
			// Check if the rabbit touches the chest
			if (isInside(chestX, chestY, 16, 16, x, y) ||
				isInside(chestX, chestY, 16, 16, x + 16, y) ||
				isInside(chestX, chestY, 16, 16, x, y + 16) ||
				isInside(chestX, chestY, 16, 16, x + 16, y + 16))
			{
				chest_touched = true;
				*score += 5;
				playNote(C7);
				delay(50);
				playNote(0);
				printNumber(*score, 5, 5, RGBToWord(0xff, 0xff, 0), 0);
				chest_box_clear();  // Clear chest if touched
			}
			else if (chest_visible)
			{
				// Refresh chest if wolves overlap it
				refresh_chest_if_needed();
			}
		}
		// clear chest from screen when cetain time passes
		else if (gameTime < 44000)
		{
			// Clear chest
			chest_box_clear();
		}



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

		// Gives 5 seconds at beginning for bunny to stay alive when hit by wolf to prevent errors in 2nd round
		if(gameTime < 55000)
		{
			// Collision detection with wolves
			if (isInside(wolf1X, wolf1Y, 12, 16, x, y) || isInside(wolf1X, wolf1Y, 12, 16, x + 12, y) ||
				isInside(wolf1X, wolf1Y, 12, 16, x, y + 16) || isInside(wolf1X, wolf1Y, 12, 16, x + 12, y + 16)

				|| isInside(wolf2X, wolf2Y, 12, 16, x, y) || isInside(wolf2X, wolf2Y, 12, 16, x + 12, y) ||
				isInside(wolf2X, wolf2Y, 12, 16, x, y + 16) || isInside(wolf2X, wolf2Y, 12, 16, x + 12, y + 16))
			{

				clear();  // Clear the screen to show the deadbunny image clearly

				// Position the deadbunny image in the center
				uint16_t deadbunnyX = (128 - 32) / 2;  // Adjust 128 to screen width and 32 to deadbunny width
				uint16_t deadbunnyY = (160 - 32) / 2;  // Adjust 160 to screen height and 32 to deadbunny height

				putImage(deadbunnyX, deadbunnyY, 32, 32, deadbunny, 0, 0);  // Display deadbunny in center

				// Display "RIP" text in red on top of the deadbunny image
				printTextX2("RIP", deadbunnyX + 1, deadbunnyY - 20, RGBToWord(0xff, 0, 0), 0);

				delay(3000);  // Pause to show deadbunny image and RIP text before game-over screen

				// Display existing game-over message
				clear();
				printText("Game Over", 30, 50, RGBToWord(0xff, 0, 0), 0);    // Adjust position if needed
				printText("Hit by Wolf :(", 18, 70, RGBToWord(0, 0, 32), 0);  // Adjust position if needed
				printText("Score =", 20, 90, RGBToWord(0xff, 0xff, 0), 0);  // Adjust position if needed
				printNumber(*score, 80, 90, RGBToWord(0xff, 0xff, 0), 0);

				delay(3000);
				break;
			}
		}
		else
		{
			printText("!!", x, y, RGBToWord(102, 0, 204), 0);
		}
		

		// Display game score when time is up
		if (gameTime == 0)
		{
			clear(); // Clear the screen
			printTextX2("Game Over", 15, 60, RGBToWord(0xff, 0, 0), 0);
			printNumberX2(*score, 60, 100, RGBToWord(0xff, 0xff, 0), 0);
			delay(5000); // Delay to show final score
			//break; // End game loop
			game_mode = false;
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
		printNumber(*score, 60, 20, RGBToWord(0xff, 0xff, 0), 0);

		hmoved = vmoved = 0;
		hinverted = vinverted = 0;

		if ((GPIOB->IDR & (1 << 4))==0) // right pressed
		{
			if (x < 110)
			{
				// erasing the old bunny by filling the area with the background color (0=black)
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

				// flip the toggle value

				}
				righttoggle = righttoggle ^ 1;
				// update the oldx and oldy variables to the new position for next movement
				oldx = x;
				oldy = y;

			}


		}
		if ((GPIOB->IDR & (1 << 5))==0) // left pressed
		{

			if (x > 10)
			{
				// erasing the old bunny by filling the area with the background color (0=black)
				fillRectangle(oldx,oldy,16,16,0);
				x = x - 1;
				hmoved = 1;
				hinverted=1;

				// toggle between leftbunny1 and leftbunny2
				if(lefttoggle)
				{
					putImage(x,y,16,16,leftbunny2,1,0); // flipped horizontally
				}
				else
				{
					putImage(x,y,16,16,leftbunny1,1,0); // flipped horizontally
				}
				lefttoggle = lefttoggle ^ 1;
				// update the oldx and oldy variables to the new position for next movement
				oldx= x;
				oldy =y;

			}
		}
		if ( (GPIOA->IDR & (1 << 11)) == 0) // down pressed
		{
			if (y < 140) // move down boundary
			{
				fillRectangle(oldx,oldy,16,16,0); // clear the old position
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
				// update the oldx and oldy variables to the new position for next movement
				oldx = x;
				oldy = y;

			}

		}
		if ( (GPIOA->IDR & (1 << 8)) == 0) // up pressed
		{
			if (y > 16)
			{
				fillRectangle(oldx,oldy,16,16,0); // clear the old position
				y = y - 1;
				vmoved = 1;
				vinverted = 1;

				if(uptoggle)
				{
					putImage(x,y,16,16,upbunny1,1,0);
				}
				else
				{
					putImage(x,y,16,16,upbunny1,1,0); // flipped horizontally
				}

				// update the oldx and oldy variables to the new position for next movement
				oldx = x;
				oldy = y;

			}

		}
		if ((vmoved) || (hmoved))
		{
			// only redraw if there has been some movement (reduces flicker)
			fillRectangle(oldx,oldy,16,16,0);
			// oldx = x;
			// oldy = y;
			if (hmoved && hinverted ==1) // moving right
			{
				if (lefttoggle)
					putImage(x,y,16,16,leftbunny1,hinverted,0);
				else
					putImage(x,y,16,16,leftbunny2,hinverted,0);

				toggle = toggle ^ 1;
				//lefttoggle ^= 1;
			}
			else if(hmoved && hinverted  ==0) // moving left
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


		//if (vmoved || hmoved)

			if (isInside(carrotX, carrotY, 12, 16, x, y) ||
				isInside(carrotX, carrotY, 12, 16, x + 12, y) ||
				isInside(carrotX, carrotY, 12, 16, x, y + 16) ||
				isInside(carrotX, carrotY, 12, 16, x + 12, y + 16))
			{
				(*score)++;
				playNote(E3);
				delay(200);
				playNote(0);
				flashGreenLED();
				printNumber(*score, 5, 5, RGBToWord(0xff, 0xff, 0), 0);
				fillRectangle(carrotX, carrotY, 16, 16, 0); // Erase carrot
				carrotX = (rand() % 100) + 10; // Random x position within screen bounds
				carrotY = (rand() % 120) + 20; // Random y position within screen bounds
			}
			fillRectangle(carrotX, carrotY, 12, 16, 0); // Clear old carrot
			putImage(carrotX, carrotY, 16, 16, carrot, 0, 0); // Draw new carrot

		}
		// use delay to slow down characters
		delay(40);

	} // end while loop
} // end game function

void mainMenu(void)
{	
	printText("Welcome to", 10, 20, RGBToWord(0, 204, 0), 0);
	printText("Carrot Chase!", 10, 30, RGBToWord(255, 128, 0), 0);
	printText("Eat Carrots and", 10, 60, RGBToWord(0, 102, 204), 0);
	printText("Defeat the Wolves", 7, 70, RGBToWord(0, 102, 204), 0);
	printText("Who will win -", 10, 80, RGBToWord(0, 102, 204), 0);
	printText("Player 1 or 2?", 10, 90, RGBToWord(0, 102, 204), 0);
	printText("Press up to start", 10, 100, RGBToWord(0, 102, 204), 0);
	putImage(55, 120, 16, 16, play_button, 0, 0);
	// putImage(chestX, chestY, 16, 16, chest_box, 0, 0); // Display the chest
}

void RedOn(void) // turns on the red LED without changing the others
{
	GPIOA -> ODR |= (1 << 9);
}

void RedOff(void) // turns off the red LED without changing the others
{
	GPIOA -> ODR &= ~(1 << 9);
}

void YellowOn(void) // turns on the yellow LED without changing the others
{
	GPIOA -> ODR |= (1 << 10);
}

void YellowOff(void) // turns on the yellow LED without changing the others
{
	GPIOA -> ODR &= ~(1 << 10);
}

void GreenOn(void) // turns on the green LED without changing the others
{
	GPIOA -> ODR |= (1 << 12);
}

void GreenOff(void) // turns on the green LED without changing the others
{
	GPIOA -> ODR &= ~(1 << 12);
}

// Function for flashing green LED when carrot is eaten
void flashGreenLED(void)
{
	for (int i = 0; i < 3; i++)
	{
		GreenOn();
		delay(10);
		GreenOff();
		delay(10);
	}
}

// Function for flashing red LED when bunny dies
void flashRedLED(void)
{
	for (int i = 0; i < 3; i++)
	{
		RedOn();
		delay(10);
		RedOff();
		delay(10);
	}
}

// Function for flashing yellow LED when game starts
void flashYellowLED(void)
{
	for (int i = 0; i < 3; i++)
	{
		YellowOff();
		delay(10);
		YellowOff();
		delay(10);
	}
}

// Function for serial communication
int _write(int file, char *ptr, int len)
{
    int i;
    for(i = 0; i < len; i++) {
        eputchar(*ptr++);
    }
    return len;
}
