/****************************************************
 * Defines
 ****************************************************/

#ifndef _THERMISTOR
#define _THERMISTOR

#include "Arduino.h"
#define PRECISION 100 //multiplier for C

#define IS_THERMISTOR(type) (((type) == THERMISTOR_NONE) || \
                             ((type) == THERMISTOR_MM103J1F) || \
                             ((type) == THERMISTOR_USP10982) || \
                             ((type) == THERMISTOR_NTCALUG03A103H) || \
                             ((type) == THERMISTOR_NTCLG100E2103JB) || \
                             ((type) == THERMISTOR_NTCLE400E3103H))                          
typedef enum {
  THERMISTOR_NONE = 0,
  THERMISTOR_MM103J1F = 1,
  THERMISTOR_USP10982 = 2,
  THERMISTOR_NTCALUG03A103H = 3,
  THERMISTOR_NTCLG100E2103JB = 4,
  THERMISTOR_NTCLE400E3103H = 5
}thermistor_t;

/****************************************************
 * Function Declarations
 ****************************************************/
int16_t getMM103J1FTemp (uint16_t analog_val);
int16_t getUSP10982Temp (uint16_t analog_val);
int16_t getNTCALUG03A103HTemp (uint16_t analog_val);

int16_t getThermistorTemp (uint16_t analog_val, thermistor_t type);
void thermistor_setInputShift (uint8_t new_shift);

#endif
