#include <stdint.h>

void initSerial(void);
void eputchar(char c);
char egetchar(void);
void eputs(char *String);
void printDecimal(int32_t Value);
int serial_variable(void); // implementation is to fetch a char using serial interface