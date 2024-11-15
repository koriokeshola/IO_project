/* Program Description: This is a 2-player game is called Carrot Chase.
It features a bunny that collects carrots and chests while avoiding 2 wolves.
Compete to win!
Authors: Bouthayna Metarfi, Kori Okeshola and Seema Alazhari */

#include <stm32f031x6.h>
#include "display.h" // For display
#include "sound.h" // For sound
#include "musical_notes.h" // For sound frequencies
#include <stdbool.h> // For boolean values
#include <stdlib.h>   // For rand() and srand()
#include <time.h> // For time
#include "serial.h" // For serial communication

// Function signatures
void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void chest_box_show(); // Manage display of chest
void chest_box_clear();
void refresh_chest_if_needed();
void initialise_game();
void game(uint32_t *score, int round); // Main game logic
void mainMenu(void); 
void determineWinner(int player1_score, int player2_score); // Function to calculate and display winner

// Functions for LEDs
void RedOn(void);
void RedOff(void);
void YellowOn(void);
void YellowOff(void);
void GreenOn(void);
void GreenOff(void);
void flashGreenLED(void);
void flashRedLED(void);
void flashYellowLED(void);

int _write(int file, char *ptr, int len); // Function for serial communications

// Global variables
volatile uint32_t milliseconds;
volatile uint32_t gameTime = 60000; // 60 seconds in milliseconds
volatile uint32_t score = 0;
volatile int wolf_toggle = 0; // Toggles between open and closed wolf image
uint16_t wolf1X = 20, wolf1Y = 40; // Wolf starting positions
uint16_t wolf2X = 80, wolf2Y = 100;
int wolf1_dirX = 1, wolf1_dirY = 1; // Direction for wolf movement
int wolf2_dirX = -1, wolf2_dirY = -1;
int chest_display_start_time = 0;
bool chest_visible = false;
bool chest_touched = false;
bool chest_first_show = false;
uint16_t chestX = 0, chestY = 0; // Initial chest position: 30,  50


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

// Main function
int main()
{
	int player1_score = 0;
	int player2_score = 0;
	int high_score = 0;
	int current_high_score;
	int round;

	// Initialise game subsytems:
	initSerial();  // Serial communication
	initClock(); // System processor clock
	initSysTick(); // Timer for e.g. interrupts and delays
	setupIO(); // Pin and peripheral configuration
	initTimer(); // Control game timing and delays
	initSound(); // Audio output system

	// Display main menu screen
	mainMenu();

	while(1) // Run in loop to play as many times as desired
	{
		player1_score = 0;
		player2_score = 0;

		for(round = 1; round <= 2; round++) // Multiplayer implementation
		{
			if(round == 1) // Only wait for Up button press for round 1, so round 2 runs automatically
			{
				// Define co-ordinates & direction variables for animation of bunny moving across screen in main menu
				int x = 10;
				int y = 130;
				int oldx = x;
				int direction = 1;
				int righttoggle = 0;

				// Delay ie. pause until up button is pressed
				while( (GPIOA->IDR & (1 << 8)) != 0)
				{
					// Erase the old bunny position
					fillRectangle(oldx, y, 16, 16, 0);

					// Update bunny's x-position to m
					x += direction;

					// If bunny hits the screen edge, reverse direction
					if (x >= 110 || x <= 10)
					{
						direction = -direction;
					}

					// Draw the bunny based on the current frame
					if (direction == 1) // Moving right
					{
						if (righttoggle)
							putImage(x, y, 16, 16, leftbunny1, 0, 0);
						else
							putImage(x, y, 16, 16, leftbunny2, 0, 0);
					}
					else // Moving left
					{
						if (righttoggle)
							putImage(x, y, 16, 16, leftbunny1, 1, 0);
						else
							putImage(x, y, 16, 16, leftbunny2, 1, 0);
					}

					// Toggle the frame for animation
					righttoggle ^= 1;

					// Update oldx for the next iteration
					oldx = x;
					delay(100);
				}
			}

			clear();

			// Print start screen for player 1 or player 2
			if(round == 1)
			{
				printText("Player 1", 37, 60, RGBToWord(0, 255, 10), 0);
				printTextX2("Start!", 30, 75, RGBToWord(0, 255, 10), 0);
				flashYellowLED();
				delay(2000);
				clear();
				game(&player1_score, round);
				clear();

			}
			else if(round == 2)
			{
				printText("Player 2", 37, 60, RGBToWord(255, 0, 10), 0);
				printTextX2("Start!", 30, 75, RGBToWord(255, 0, 10), 0);
				flashYellowLED();
				delay(2000);
				clear();
				game(&player2_score, round);
				clear();
			}
			// Controls game speed
			delay(50);

		} // End for loop

		// Calculate highest score and store for future rounds
		if(player1_score > high_score)
		{
			high_score = player1_score;
		}

		if(player2_score > high_score)
		{
			high_score = player2_score;
		}

		determineWinner(player1_score, player2_score); // Use function to compare scores and determine winner

		printText("Press Up to", 26, 70, RGBToWord(0, 50, 255), 0);
		printText("play again!", 27, 80, RGBToWord(0, 50, 255), 0);

		current_high_score = high_score;
		printText("High Score:", 20, 110, RGBToWord(0, 50, 255), 0);
		printNumber(current_high_score, 90, 120, RGBToWord(0, 50, 255), 0);
		delay(5000);

		
		// Delay ie. pause until up button is pressed
		while( (GPIOA->IDR & (1 << 8)) != 0)
		{
			delay(100);
		}
		clear();

	} // End while loop

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

// Function used in order to get random numbers to use as seed for srand
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
	// Checks to see if point px,py is within the rectange defined by x,y,w,h
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
	// Set pins for button inputs
	pinMode(GPIOB,4,0);
	pinMode(GPIOB,5,0);
	pinMode(GPIOA,8,0);
	pinMode(GPIOA,11,0);
	// Enable internal input pin pull-up resistors
	enablePullUp(GPIOB,4);
	enablePullUp(GPIOB,5);
	enablePullUp(GPIOA,11);
	enablePullUp(GPIOA,8);
	// Set pins for LED outputs
	pinMode(GPIOA,9,1);
	pinMode(GPIOA,10,1);
	pinMode(GPIOA,12,1);
}


// Function that checks if it's time to show the chest for first time. Chest gives player bonus 5 points
void chest_box_show()
{
	chest_visible = true; // Mark chest as visible
}


// Function that clears the chest box from screen
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
void initialise_game()
{
    srand(TIM2->CNT);  // Call srand only once at the start
}


// Function for main game logic
void game(uint32_t *score, int round)
{
	bool game_mode = true; // Set game mode as true ie. playable
	// Variables for image display and movement
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
	int temp = 0; // Used to store score during serial communications

	chest_visible = false;
	chest_touched = false;
	chest_first_show = false;
	*score = 0; // Use pointer variable to pass score by reference for efficiency
	gameTime = 60000;

	// Display score for player 1 or 2 for clarification
	if(round == 1)
	{
		eputs("Player 1 Score Tracking\n");
	}
	else if(round == 2)
	{
		eputs("Player 2 Score Tracking\n");
	}

	while(game_mode) // Ensure game runs in loop to play as many times as desired
	{
		// Serial communications implementation - display score as it increases
		if (temp != *score)
		{
			char scoreString[10];
			sprintf(scoreString, "Score: %d\n", *score);
			eputs(scoreString);
			temp = *score;
		}
		
		
		// Time at which chest will appear in game
		if (gameTime <= 50000 && gameTime >= 44000 && chest_touched == false)
		{
			if (chest_first_show == false)
			{
				initialise_game();

				chestX = (rand() % 100) + 10 ; // Random x position between 10 and 110
				chestY = (rand()  % 90) + 30 ; // Random y position between 20 and 140

				chest_first_show = true;
				putImage(chestX, chestY, 16, 16, chest_box, 0, 0); // Display the chest
			}
			// If chest_visible is false call chest_box_show() to make it true
			if (chest_visible == false)
			{
				// Turns on the chest_visible value
				chest_box_show();
			}
			// Check if the rabbit touches the chest
			if (isInside(chestX, chestY, 16, 16, x, y) ||
				isInside(chestX, chestY, 16, 16, x + 16, y) ||
				isInside(chestX, chestY, 16, 16, x, y + 16) ||
				isInside(chestX, chestY, 16, 16, x + 16, y + 16))
			{
				// Play tune and flash LED when chest increases score
				chest_touched = true;
				*score += 5;
				flashGreenLED();
				playNote(C7);
				delay(100);
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
		// Clear chest from screen when certain time passes
		else if (gameTime < 44000)
		{
			chest_box_clear();
		}

		// Wolf animation toggle logic
		// Clear previous wolf positions (increase the clearing area slightly to avoid residue)
		fillRectangle(wolf1X - 1, wolf1Y - 1, 18, 18, 0); // Clear a slightly larger area
		fillRectangle(wolf2X - 1, wolf2Y - 1, 18, 18, 0);

		// Logic to display open and closed mouth images for animation
		if (wolf_toggle)
		{
			putImage(wolf1X, wolf1Y, 16, 16, wolfopen, 0, 0);
			putImage(wolf2X, wolf2Y, 16, 16, wolfopen, 0, 0);
		} else
		{
			putImage(wolf1X, wolf1Y, 16, 16, wolfclosed, 0, 0);
			putImage(wolf2X, wolf2Y, 16, 16, wolfclosed, 0, 0);
		}
		wolf_toggle ^= 1; // Alternation logic

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

		// Gives 5 seconds at beginning for bunny to stay alive when hit by wolf - prevents errors in 2nd round
		if(gameTime < 55000)
		{
			// Collision detection with wolves
			if (isInside(wolf1X, wolf1Y, 12, 16, x, y) || isInside(wolf1X, wolf1Y, 12, 16, x + 12, y) ||
				isInside(wolf1X, wolf1Y, 12, 16, x, y + 16) || isInside(wolf1X, wolf1Y, 12, 16, x + 12, y + 16)

				|| isInside(wolf2X, wolf2Y, 12, 16, x, y) || isInside(wolf2X, wolf2Y, 12, 16, x + 12, y) ||
				isInside(wolf2X, wolf2Y, 12, 16, x, y + 16) || isInside(wolf2X, wolf2Y, 12, 16, x + 12, y + 16))
			{
				// Play tune and flash LEDs when wolf hits bunny
				flashRedLED();
				playNote(G6);
				delay(200);

				flashRedLED();
				playNote(G5);
				delay(200);

				flashRedLED();
				playNote(G4);
				delay(200);

				playNote(0);
				
				clear();  // Clear the screen to show the deadbunny image clearly

				// Position the deadbunny image in the center
				uint16_t deadbunnyX = (128 - 32) / 2;  // Adjust 128 to screen width and 32 to deadbunny width
				uint16_t deadbunnyY = (160 - 32) / 2;  // Adjust 160 to screen height and 32 to deadbunny height

				putImage(deadbunnyX, deadbunnyY, 32, 32, deadbunny, 0, 0);  // Display deadbunny in center

				// Display "RIP" text in red on top of the deadbunny image
				printTextX2("RIP", deadbunnyX + 1, deadbunnyY - 20, RGBToWord(0xff, 0, 0), 0);

				delay(3000);  // Pause to show deadbunny image and RIP text before game-over screen

				// Display game-over message
				clear();
				printText("Game Over.", 30, 50, RGBToWord(0xff, 0, 0), 0);
				printText("Bit by Wolf :(", 16, 70, RGBToWord(0, 100, 100), 0);
				printText("Score =", 25, 90, RGBToWord(0xff, 0xff, 0), 0); 
				printNumber(*score, 80, 90, RGBToWord(0xff, 0xff, 0), 0);

				delay(3000);
				break;
			}
		}
		else
		{
			// Tag "!!" along with bunny in first 5 seconds to prevent losing too early
			printText("!!", x, y, RGBToWord(102, 0, 204), 0);
		}
		

		// Display game score when time is up
		if (gameTime == 0)
		{
			clear(); // Clear the screen
			playNote(F4); // Play tune to signify end of round
			delay(200);
			playNote(0);

			printTextX2("Game Over", 15, 60, RGBToWord(0xff, 0, 0), 0);
			printNumberX2(*score, 15, 100, RGBToWord(0xff, 0xff, 0), 0);
			delay(5000); // Delay to show final score
			game_mode = false; // Game mode will be activated again when up is pressed
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

		// Initialise movement variables to 0
		hmoved = vmoved = 0;
		hinverted = vinverted = 0;

		if ((GPIOB->IDR & (1 << 4))==0) // Right button pressed
		{
			if (x < 110) // Move right boundary
			{
				// Erasing the old bunny by filling the area with the background color (0=black)
				fillRectangle(oldx,oldy,16,16,0);
				x = x + 1;
				hmoved = 1;
				hinverted=0;

				// Toggle between rightbunny1 and rightbunny2 (for animation)
				if(righttoggle)
				{
					putImage(x,y,16,16,rightbunny2,0,0);
				}
				else
				{
					putImage(x,y,16,16,rightbunny1,0,0);
				}
				righttoggle = righttoggle ^ 1; // Flip the toggle value
				// Update the oldx and oldy variables to the new position for next movement
				oldx = x;
				oldy = y;

			}


		}
		if ((GPIOB->IDR & (1 << 5))==0) // Left button pressed
		{

			if (x > 10) // Move left boundary
			{
				// Erasing the old bunny by filling the area with the background color (0=black)
				fillRectangle(oldx,oldy,16,16,0);
				x = x - 1;
				hmoved = 1;
				hinverted = 1;

				// Toggle between leftbunny1 and leftbunny2
				if(lefttoggle)
				{
					putImage(x,y,16,16,leftbunny2,1,0); // Flipped horizontally
				}
				else
				{
					putImage(x,y,16,16,leftbunny1,1,0); // Flipped horizontally
				}
				lefttoggle = lefttoggle ^ 1; // Flip the toggle value
				// Update the oldx and oldy variables to the new position for next movement
				oldx = x;
				oldy = y;

			}
		}
		if ( (GPIOA->IDR & (1 << 11)) == 0) // Down button pressed
		{
			if (y < 140) // Move down boundary
			{
				fillRectangle(oldx,oldy,16,16,0); // Clear the old position
				y = y + 1;
				vmoved = 1;
				vinverted = 0;
				
				// Toggle between downbunny1 and downbunny2
				if(downtoggle)
				{
					putImage(x,y,16,16,downbunny1,0,0);
				}
				else
				{
					putImage(x,y,16,16,downbunny1,0,0); // Flipped horizontally
				}
				// Update the oldx and oldy variables to the new position for next movement
				oldx = x;
				oldy = y;

			}

		}
		if ( (GPIOA->IDR & (1 << 8)) == 0) // Up button pressed
		{
			if (y > 16) // Move up boundary
			{
				fillRectangle(oldx,oldy,16,16,0); // Clear the old position
				y = y - 1;
				vmoved = 1;
				vinverted = 1;

				// Toggle between upbunny1 and upbunny2
				if(uptoggle)
				{
					putImage(x,y,16,16,upbunny1,1,0);
				}
				else
				{
					putImage(x,y,16,16,upbunny1,1,0); // Flipped horizontally
				}

				// Update the oldx and oldy variables to the new position for next movement
				oldx = x;
				oldy = y;

			}

		}
		if ((vmoved) || (hmoved)) // If vertical or horizontal movement has occurred
		{
			// Only redraw if there has been some movement (reduces flicker)
			fillRectangle(oldx,oldy,16,16,0);
			if (hmoved && hinverted ==1) // Moving right
			{
				if (lefttoggle)
					putImage(x,y,16,16,leftbunny1,hinverted,0);
				else
					putImage(x,y,16,16,leftbunny2,hinverted,0);

				toggle = toggle ^ 1; // Toggle between leftbunny1 and leftbunny2
			}
			else if(hmoved && hinverted  ==0) // Moving left
			{
				if (righttoggle)
					putImage(x, y, 16, 16, rightbunny2, 1, 0);
				else
					putImage(x, y, 16, 16, rightbunny1, 1, 0);

				toggle = toggle ^ 1; // Toggle between rightbunny1 and rightbunny2


			}
			else if (vmoved && vinverted == 0) // Moving down
			{
				if (downtoggle)
					putImage(x, y, 16, 16, downbunny1, 0, 0);
				else
					putImage(x, y, 16, 16, downbunny1, 0, 0); // Additional down frame

				downtoggle ^= 1; // Toggle between downbunny1 and downbunny2
			}

			else if (vmoved && vinverted == 1) // Moving up
			{
				if (uptoggle)
					putImage(x, y, 16, 16, upbunny1, 1, 0);
				else
					putImage(x, y, 16, 16, upbunny1, 1, 0); // Additional up frame

				uptoggle ^= 1; // Toggle between upbunny1 and upbunny2
			}

			oldx = x;
			oldy = y;

			// Check if bunny's position is overlapping with carrot's position, so score can increment
			if (isInside(carrotX, carrotY, 12, 16, x, y) ||
				isInside(carrotX, carrotY, 12, 16, x + 12, y) ||
				isInside(carrotX, carrotY, 12, 16, x, y + 16) ||
				isInside(carrotX, carrotY, 12, 16, x + 12, y + 16))
			{
				(*score)++;
				// Play tune and flash green LED
				playNote(C7);
				delay(200);
				playNote(0);
				flashGreenLED();
				printNumber(*score, 5, 5, RGBToWord(0xff, 0xff, 0), 0); // Update current score displayed
				fillRectangle(carrotX, carrotY, 16, 16, 0); // Erase carrot
				carrotX = (rand() % 100) + 10; // Random x position within screen bounds
				carrotY = (rand() % 120) + 20; // Random y position within screen bounds
			}
			fillRectangle(carrotX, carrotY, 12, 16, 0); // Clear old carrot
			putImage(carrotX, carrotY, 16, 16, carrot, 0, 0); // Draw new carrot

		}
		// Use delay to slow down characters
		delay(40);

	} // end while loop
} // end game function

void mainMenu(void)
{	
	printText("Welcome to", 10, 20, RGBToWord(0, 204, 0), 0);
	printText("Carrot Chase!", 10, 30, RGBToWord(255, 128, 0), 0);
	printText("Eat Carrots and", 10, 50, RGBToWord(0, 102, 204), 0);
	printText("Beat the Wolves.", 10, 60, RGBToWord(0, 102, 204), 0);
	printText("Who will win -", 10, 70, RGBToWord(0, 102, 204), 0);
	printText("Player 1 or 2?", 10, 80, RGBToWord(0, 102, 204), 0);
	printText("Press up", 10, 100, RGBToWord(255, 20, 255), 0);
	printText("to start...", 10, 110, RGBToWord(255, 20, 255), 0);

	// Flash LEDs and play tune as game starts
	flashRedLED();
	playNote(G4);
	delay(200);

	flashYellowLED();
	playNote(G5);
	delay(200);

	flashGreenLED();
	playNote(G6);
	delay(200);

	playNote(0);
}

determineWinner(int player1_score, int player2_score)
{
	// Display congratulatory message based on final scores, flashing green LEDs and playing tune
	if(player1_score > player2_score)
	{
		printText("Player 1 Wins!", 18, 70, RGBToWord(0, 255, 10), 0);
		printText("Good Job!", 30, 80, RGBToWord(0, 255, 10), 0);

		flashGreenLED();
		playNote(G4);
		delay(200);

		flashGreenLED();
		playNote(G5);
		delay(200);

		flashGreenLED();
		playNote(G6);
		delay(500);

		playNote(0);

		
		delay(3000);
		clear();
	}
	else if(player2_score > player1_score)
	{
		printText("Player 2 Wins!", 18, 70, RGBToWord(255, 0, 10), 0);
		printText("Good Job!", 30, 80, RGBToWord(255, 0, 10), 0);

		flashGreenLED();
		playNote(G4);
		delay(200);

		flashGreenLED();
		playNote(G5);
		delay(200);

		flashGreenLED();
		playNote(G6);
		delay(500);

		playNote(0);

		delay(3000);
		clear();
	}
	else if(player1_score == player2_score)
	{
		printText("It's a Tie!", 27, 75, RGBToWord(255, 255, 255), 0);
		flashGreenLED();

		playNote(G4);
		delay(200);

		flashGreenLED();
		playNote(G5);
		delay(200);

		flashGreenLED();
		playNote(G6);
		delay(500);

		playNote(0);

		delay(3000);
		clear();
	}
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

void YellowOff(void) // turns off the yellow LED without changing the others
{
	GPIOA -> ODR &= ~(1 << 10);
}

void GreenOn(void) // turns on the green LED without changing the others
{
	GPIOA -> ODR |= (1 << 12);
}

void GreenOff(void) // turns off the green LED without changing the others
{
	GPIOA -> ODR &= ~(1 << 12);
}

// Function for flashing green LED
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

// Function for flashing yellow LED
void flashYellowLED(void)
{
	for (int i = 0; i < 3; i++)
	{
		YellowOn();
		delay(10);
		YellowOff();
		delay(10);
	}
}

// Function for flashing red LED
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

// Function for serial communication
int _write(int file, char *ptr, int len)
{
    int i;
    for(i = 0; i < len; i++) {
        eputchar(*ptr++);
    }
    return len;
}
