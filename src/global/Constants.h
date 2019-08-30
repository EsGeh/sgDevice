#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_


/////////////////////////////
// baud rates:
/////////////////////////////
#define MIDI_RATE 31250
//#define PC_RATE 9600
#define PC_RATE 38400

/////////////////////////////
// control id in MIDI messages:
/////////////////////////////

#define TRIGGERS_CONTROL_ID (32+0)
#define SWITCHES_CONTROL_ID (32+10)
#define ANALOG_CONTROL_ID (32+20)
#define META_CONTROL_ID 64

/* midi control numbers 0..31:
 if ANALOG_DOUBLE_PRECISION is defined,
 these may specify the
 most significant bits (MSB) of
 the controls 32..63.
 This way 2 midi messages can be grouped
 for higher precision.
*/

/////////////////////////////
// arduino pins
/////////////////////////////

#define PIN_MIDI_IN 2
#define PIN_MIDI_OUT 3

#define PIN_META_TRIGGER_0 11
#define META_COUNT 2

#define PIN_TRIGGER_OR_SWITCH 10
#define TRIGGERS_COUNT 6
#define SWITCHES_COUNT 6
#define PIN_TOG_0 4

#define TRIGGER_PRESSED 0
#define DEBOUNCE_TIME 200

// which value means
#define TRIGGERS 1


/////////////////////////////
// analog parameters:
/////////////////////////////

#define ANALOG_COUNT 6
#define ANALOG_MIDI_RES 511.0

#define ANALOG_STEPS ANALOG_MIDI_RES

// if defined: use 2 midi messages to use 14 bytes instead of 7 bytes
// define this, if you want more than ANALOG_MIDI_RES > 127.0

#define ANALOG_DOUBLE_PRECISION 1

#endif
