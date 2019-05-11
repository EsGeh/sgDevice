#include "Constants_ver2.h"

/*
	NOTES:
		Messages from MIDI-IN are directly sent to the pc.
		Messages from PC are directly sent to MIDI-OUT.
		Control changes are transmitted to the pc via SysEx Messages
		
				0xf0: //start sysex Message
				<control id>
				<value>
				0xf7 //end sysex message

		format for the triggers:
				0xf0: //start sysex Message
				<control id>
				0xf7 //end sysex message
*/

//#define DEBUG_PRINT

// maximum value at analog pins:
#define ANALOG_PIN_RESOULUTION 1023


class Analog
{
	private:
		float analog[ANALOG_COUNT];
	public:

		Analog()
		{
			for(int i=0; i<ANALOG_COUNT; i++)
			{
				//analog[i] = 0;
				analog[i]= ((float )analogRead(i)) / ((float ) ANALOG_PIN_RESOULUTION);
			}
		};

		void setup()
		{
		}

		// send all current values:
		void sendAll()
		{
			for(int i=0; i<ANALOG_COUNT; i++)
			{
				toPC(i);
			}
		}

		void update()
		{
			for( int i=0; i<ANALOG_COUNT; i++)
			{
				float newVal = ((float )analogRead(i)) / ANALOG_PIN_RESOULUTION;
				if (abs(newVal-analog[i]) >= (1.0 /ANALOG_STEPS) )
				{
					analog[i]=newVal;
					toPC(i);
				}
			}
		}

		void toPC(int i)
		{
			#ifdef DEBUG_PRINT
				//Serial.println( analog[i] );
				Serial.print( "Analog value [" );
				Serial.print( i );
				Serial.print( "]: " );
				Serial.println( analog[i] );
			#else
				#ifndef ANALOG_DOUBLE_PRECISION
					Serial.write( 0xB0 );
					Serial.write( ANALOG_CONTROL_ID + i );
					Serial.write( int( analog[i] * ANALOG_MIDI_RES ) );
				#else
					byte lsb = int( analog[i] * ANALOG_MIDI_RES ) & 0b01111111;
					byte msb = int( analog[i] * ANALOG_MIDI_RES ) >> 7;
					Serial.write( 0xB0 );
					Serial.write( ANALOG_CONTROL_ID + i );
					Serial.write( msb);
					Serial.write( 0xB0 );
					Serial.write( 32 + ANALOG_CONTROL_ID + i );
					Serial.write( lsb );
				#endif
			#endif
		}

};

class Midi
{
	private:
	public:
		Midi()
		{
		}

		void setup()
		{
		}

		// MIDI-In -> PC
		void update_midi_in()
		{
			int input = -1;
			if( Serial1.available() > 0 ) {
				input = Serial1.read();
				// filter out "timing" and "Active Sensing" input:
				if( input != 0b11111000 && input != 0b11111110 ) {
					#ifdef DEBUG_PRINT
						//Serial.println( analog[i] );
						Serial.print( "MIDI->PC: " );
						Serial.println( input, HEX);
					#else
						Serial.write( (byte )input );
						Serial.flush();
					#endif
				}
			}
		}

		// PC -> MIDI-Out
		void update_midi_out()
		{
			int input = -1;
			if( Serial.available() > 0 ) {
				input = Serial.read();
				Serial1.write( input );
			}
		}

};

Analog analog;
Midi midi;

void setup() {
	Serial.begin(PC_RATE);
	Serial1.begin(MIDI_RATE);

	//sleep(2);

	analog.setup();
	midi.setup();

	#ifdef DEBUG_PRINT
	Serial.println("initializing");
	#endif
}

//Message

void loop() {
	//analog.update();
	midi.update_midi_in();
	midi.update_midi_out();
}
