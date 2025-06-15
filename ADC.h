# ifndef ADC_H
# define ADC_H
#include "ADC.h"
#include <stdint.h>


# define vref 2.5
# define sample_no 16

extern volatile int display_volt;
extern volatile int task_A , task_B; 




void int_to_chars(uint16_t number, char* n);
void converted(uint16_t ADCMEM);
void display_voltage();




# endif
