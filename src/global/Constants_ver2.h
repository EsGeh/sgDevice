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
#define ANALOG_CONTROL_ID_FINE 32
/* midi control numbers 32..31:
 if ANALOG_DOUBLE_PRECISION is defined,
 these may specify the
 least significant bits (LSB) of
 the controls 0..31.
*/

#define ANALOG_COUNT 16

#define SWITCHES_CONTROL_ID (32+16)
#define SWITCHES_COUNT 16

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

#define ANALOG_MIDI_RES 512
#define ANALOG_STEPS 512

// maximum value at analog pins:
#define ANALOG_PIN_BITS 10
#define ANALOG_PIN_RESOLUTION (1<<ANALOG_PIN_BITS)

// if defined: use 2 midi messages to use 14 bytes instead of 7 bytes.
// uncomment, if you want more than ANALOG_STEPS > 128:
#define ANALOG_DOUBLE_PRECISION

#endif
