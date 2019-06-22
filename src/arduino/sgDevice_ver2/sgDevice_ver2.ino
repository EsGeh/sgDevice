#include "Constants_ver2.h"

/*
	NOTES:
	Messages from MIDI-IN are directly sent to the pc.
	Messages from PC are directly sent to MIDI-OUT.
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

class Buttons
{
	// values
	private:
		unsigned int pin0;
		unsigned int count;
		unsigned int control_id;
	public:
		boolean* value;
	// methods:
	public:
		Buttons(
			const unsigned int pin0,
			const unsigned int count,
			const unsigned int control_id
		)
			:pin0(pin0),
			count(count),
			control_id(control_id),
			value(NULL)
		{
			value = new boolean[count];
			for (int i=0; i<count ; i++)
			{
				value[i]=false;
			}
		}
		~Buttons() {
			delete[] value;
		}
		void setup()
		{
			for( int i=0; i<count; i++ )
			{
				pinMode(pin0+i, INPUT_PULLUP);
			}
		}
		// overwrite:
		void OnChange(const unsigned int index, boolean newValue) { }
		void update()
		{
			for (int i=0; i< count ; i++)
			{
				if(digitalRead(pin0+i)!= value[i])
				{
					value[i]=!value[i];
					#ifdef DEBUG_PRINT
						Serial.println(String("Buttons: pin") + String(pin0+i) + String("] = " + String((unsigned int )value[i])));
					#else
						Serial.write( 0xB0 );
						Serial.write( control_id + i );
						Serial.write((byte ) value[i]);
					#endif
				}
			}
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
Buttons switches(
	PIN_SWITCH,
	SWITCHES_COUNT,
	SWITCHES_CONTROL_ID
);
Buttons triggers(
	PIN_TRIGGERS,
	TRIGGERS_COUNT,
	TRIGGERS_CONTROL_ID
);
Buttons meta(
	PIN_META,
	META_COUNT,
	META_CONTROL_ID
);

void setup() {
	Serial.begin(PC_RATE);
	Serial1.begin(MIDI_RATE);

	//sleep(2);

	analog.setup();
	midi.setup();
	switches.setup();
	triggers.setup();
	meta.setup();

	#ifdef DEBUG_PRINT
	Serial.println("initializing");
	#endif
}

//Message

void loop() {
	analog.update();
	midi.update_midi_in();
	midi.update_midi_out();
	switches.update();
	triggers.update();
	meta.update();
}
