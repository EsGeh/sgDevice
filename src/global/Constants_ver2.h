#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_


/////////////////////////////
// baud rates:
/////////////////////////////
#define MIDI_RATE 31250
#define PC_RATE 9600

/////////////////////////////
// control id in MIDI messages:
/////////////////////////////

#define ANALOG_CONTROL_ID 0

/////////////////////////////
// analog parameters:
/////////////////////////////

#define ANALOG_COUNT 1
#define ANALOG_MIDI_RES 127.0

#define ANALOG_STEPS ANALOG_MIDI_RES

// if defined: use 2 midi messages to use 14 bytes instead of 7 bytes
// define this, if you want more than ANALOG_MIDI_RES > 127.0

#define ANALOG_DOUBLE_PRECISION 0

#endif
