#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_


/////////////////////////////
// baud rates:
/////////////////////////////
#define MIDI_RATE 31250
#define PC_RATE 31250
// (according to MIDI specification)

/////////////////////////////
// control id in MIDI messages:
/////////////////////////////

#define ANALOG_CONTROL_ID 0
#define ANALOG_COUNT 16

#define SWITCHES_CONTROL_ID 16
#define SWITCHES_COUNT 16

/* midi control numbers 32..63:
 if ANALOG_DOUBLE_PRECISION is defined,
 these may specify the
 least significant bits (LSB) of
 the controls 0..31.
 This way 2 midi messages are grouped
 for higher precision.
*/

#define TRIGGERS_CONTROL_ID 64
#define TRIGGERS_COUNT 8

#define META_CONTROL_ID 96
#define META_COUNT 4

/////////////////////////////
// arduino pins
/////////////////////////////

#define PIN_SWITCH 22
#define PIN_TRIGGERS 38
#define PIN_META 46

/////////////////////////////
// analog parameters:
/////////////////////////////

#define ANALOG_MIDI_RES 127.0

#define ANALOG_STEPS ANALOG_MIDI_RES

// if defined: use 2 midi messages to use 14 bytes instead of 7 bytes.

// uncomment, if you want more than ANALOG_MIDI_RES > 127.0:
//#define ANALOG_DOUBLE_PRECISION

#endif
