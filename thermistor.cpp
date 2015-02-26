/************************************************
 * Thermistor Library
 * Written by Benjamin Shaya
 *
 * This library uses lookup tables and interpolation to convert
 * Voltage to temperature using a thermistor-fixed resistor divider
 * 
 * The lookup tables are input voltages scaled to 16 bits for maximum precision
 * LUT's are stored in flash, reducing memory consumption 
 * This requires usage of pgm_read_word_near to obtain values
 *
 * Searching the LUT is done via binary search to optimize runtime
 *
 * Adding a thermistor:
 *  - calculate the input voltages using your thermistor + resistor
 *  - Enter these values into a const PROGMEM uint16_t array
 *  - Add a line to calculate the length of your LUT
 *  - Store the "units" of your LUT, the degrees C between each entry
 *  - store the "offset" of your LUT, the temperature in C of the 0th entry
 *  - add a function that calls getTemp using your constants and LUT
 *  - add the function to thermistor.h
 ***************************************************************/

#include <avr/pgmspace.h>
#include "thermistor.h"

/************************************************
 * Macros
 ************************************************/
#define FLASH(X) pgm_read_word_near(X)

/***********************************************
 * Constants
 ***********************************************/

//1k resistor
const uint16_t cMM103J1F_LUT[] PROGMEM = {
  65340, 65327, 65313, 65298, 65282, 65266, 65248, 65229, 65209, 65188, 65166, 
  65143, 65118, 65092, 65065, 65036, 65005, 64973, 64939, 64904, 64866, 64827, 
  64786, 64742, 64696, 64648, 64598, 64545, 64490, 64432, 64371, 64307, 64241, 
  64171, 64098, 64022, 63942, 63859, 63772, 63682, 63587, 63488, 63386, 63279, 
  63167, 63051, 62931, 62805, 62675, 62540, 62399, 62253, 62102, 61945, 61782, 
  61613, 61438, 61258, 61071, 60877, 60678, 60471, 60258, 60038, 59811, 59577, 
  59336, 59087, 58832, 58569, 58299, 58021, 57735, 57442, 57141, 56832, 56516, 
  56192, 55861, 55522, 55175, 54821, 54459, 54087, 53711, 53326, 52934, 52534,
  52127, 51714, 51294, 50867, 50434, 49994, 49546, 49093, 48635, 48170, 47702, 
  47224, 46746, 46260, 45771, 45276, 44776, 44278, 43769, 43259, 42748, 42237, 
  41721, 41199, 40682, 40153, 39631, 39109, 38588, 38057, 37528, 37004, 36472,
  35948, 35431, 34911, 34387, 33875, 33362, 32849, 32332, 31825, 31322, 30819, 
  30323, 29828, 29339, 28853, 28373, 27897, 27424, 26955, 26493, 26034, 25581,
  25136, 24693, 24255, 23824, 23398, 22977, 22561, 22151, 21748, 21350, 20956,
  20571, 20191, 19815, 19445, 19082, 18724, 18373, 18025, 17685, 17351, 17022,
  16697, 16378, 16067, 15759, 15458, 15162, 14870, 14586, 14303, 14029, 13761,
  13494, 13236, 12980, 12731, 12483, 12241, 12006, 11773, 11547, 11324, 11108,
  10895, 10684, 10477, 10282, 10083, 9888, 9698, 9511, 9328, 9148, 8972, 8800,
  8632, 8466, 8305, 8146, 7991, 7839, 7690, 7545, 7402, 7262, 7125, 6991, 6860,
  6731, 6605, 6482, 6362, 6243, 6128, 6015, 5904, 5795, 5689, 5584, 5483, 5383, 
  5285, 5189, 5095, 5004, 4913, 4825, 4739, 4654, 4572, 4491, 4411, 4333, 4257,
  4182, 4109, 4037, 3967, 3899, 3831, 3765, 3700, 3637, 3575, 3514, 3454, 3395,
  3338, 3282, 3227, 3173, 3120, 3068, 3017, 2968, 2919, 2871
};
const uint16_t cMM103J1F_length = sizeof(cMM103J1F_LUT)/sizeof(cMM103J1F_LUT[0]);
const uint16_t cMM103J1F_units = 1;
const uint16_t cMM103J1F_offset = -40;

//use 4K7 resistor
const uint16_t cUSP10982_LUT[] PROGMEM = {
  64632, 64571, 64506, 64438, 64366, 64289, 64209, 64124, 64034, 63939, 63839,
  63734, 63623, 63506, 63384, 63255, 63120, 62977, 62828, 62672, 62508, 62337,
  62157, 61969, 61773, 61568, 61354, 61131, 60898, 60656, 60403, 60141, 59868,
  59584, 59290, 58985, 58668, 58340, 58001, 57650, 57288, 56914, 56528, 56130,
  55720, 55299, 54865, 54420, 53963, 53494, 53014, 52522, 52019, 51505, 50980,
  50445, 49898, 49343, 48777, 48203, 47619, 47026, 46426, 45818, 45204, 44581,
  43953, 43319, 42680, 42036, 41390, 40739, 40083, 39427, 38769, 38109, 37449,
  36788, 36130, 35470, 34813, 34159, 33506, 32854, 32210, 31567, 30930, 30297,
  29669, 29049, 28433, 27825, 27224, 26629, 26040, 25460, 24889, 24323, 23770,
  23219, 22683, 22152, 21633, 21120, 20615, 20125, 19638, 19161, 18695, 18242,
  17795, 17355, 16929, 16503, 16094, 15694, 15304, 14916, 14539, 14173, 13811 
};
const uint16_t cUSP10982_length = sizeof(cUSP10982_LUT)/sizeof(cUSP10982_LUT[0]);
const uint16_t cUSP10982_offset = -40;
const uint16_t cUSP10982_units = 1;

//use 4K7 resistor
const uint16_t cNTCALUG03A103H_LUT[] PROGMEM = {
  64626, 64283, 63831, 63246, 62499, 61558, 60394, 58977, 57282, 55295, 53012, 
  50444, 47619, 44581, 41388, 38106, 34806, 31557, 28419, 25440, 22658, 20096,
  17763, 15661, 13783, 12116, 10646, 9353, 8220, 7230, 6366, 5612, 4954, 4381,  
};
const uint16_t cNTCALUG03A103H_length = 
  sizeof(cNTCALUG03A103H_LUT)/sizeof(cNTCALUG03A103H_LUT[0]);
const uint16_t cNTCALUG03A103H_offset = -40;
const uint16_t cNTCALUG03A103H_units = 5;

//default to shift a 10 bit analog value to a 16 bit value
uint8_t input_shift = 6;

/**************************************************
 * Internal Functions
 **************************************************/
//interpolation function
uint16_t interpolate (uint16_t x_low16, uint16_t x_hi16, uint16_t y_low16, 
                      uint16_t y_hi16, uint16_t x16) {
  //cast to 32 bits unsigned to avoid overflow
  int32_t x_low = (uint32_t) x_low16;
  int32_t x_hi = (uint32_t) x_hi16;
  int32_t y_low = (uint32_t) y_low16;
  int32_t y_hi = (uint32_t) y_hi16;
  int32_t x = (uint32_t) x16;
  return (int16_t) ( ((x_hi-x) * y_low + (x-x_low) * y_hi) / (x_hi-x_low) );
}

//return degrees C * PRECISION (good to +/- 300C for PRECISION = 100)
int16_t getTemp (uint16_t analog_val, int8_t offset, const uint16_t * LUT, 
                 uint16_t LUT_length, uint16_t units) {
  //scale analog val from 10 bit to 16 bit 
  analog_val = analog_val << input_shift;

  //make sure target is in range
  //todo: better out of range handling?
  if ( analog_val > FLASH(LUT + 0) ) return (offset - 1) * PRECISION;
  if ( analog_val < FLASH(LUT + LUT_length-1) ) {
    return (offset + LUT_length * units + 1) * PRECISION;
  }
    
  //binary search to find value
  uint16_t high = LUT_length-1;
  uint16_t low = 0;
  while (high - low > 1) {
    uint16_t mid = (high+low) >> 1;
    uint16_t midval = FLASH(LUT+mid);
    if ( midval == analog_val ) return (mid*units + offset) * PRECISION;
    //y values are in oposite order of x values
    if ( midval > analog_val ) {
      low = mid;
    } else if ( midval < analog_val ) {
      high = mid;
    }
  }
  
  //interpolate
  uint16_t low_temp = (low*units + offset) * PRECISION;
  uint16_t high_temp = (high*units + offset) * PRECISION;
  return interpolate (FLASH(LUT+low), FLASH(LUT+high), low_temp, high_temp, analog_val);
}

/*******************************************
 * Exported Functions
 *******************************************/
int16_t getMM103J1FTemp (uint16_t analog_val) {
  return getTemp (analog_val, cMM103J1F_offset, cMM103J1F_LUT, cMM103J1F_length,
                  cMM103J1F_units);
}

int16_t getUSP10982Temp (uint16_t analog_val) {
  return getTemp (analog_val, cUSP10982_offset, cUSP10982_LUT, cUSP10982_length,
                  cUSP10982_units);
}

int16_t getNTCALUG03A103HTemp (uint16_t analog_val) {
  return getTemp (analog_val, cNTCALUG03A103H_offset, cNTCALUG03A103H_LUT,
                  cNTCALUG03A103H_length, cNTCALUG03A103H_units);
}

//routes according to the type
int16_t getThermistorTemp (uint16_t analog_val, thermistor_t type) {
  if (type == THERMISTOR_MM103J1F) {
    return getMM103J1FTemp (analog_val);
  } else if (type == THERMISTOR_USP10982) {
    return getUSP10982Temp (analog_val);
  } else if (type == THERMISTOR_NTCALUG03A103H) {
    return getNTCALUG03A103HTemp (analog_val);
  } else {
    return 0x80;
  }
}

void thermistor_setInputShift (uint8_t new_shift) {
  input_shift = new_shift;
}