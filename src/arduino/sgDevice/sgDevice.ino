
#include "Constants.h"

// #include <SoftwareSerial.h>

//typedef SoftwareSerial SoftSerial;

//#define DEBUG_PRINT
 

// analog 
#define ANALOG_PIN_RESOULUTION 1023

	//SoftSerial serialMidi(PIN_MIDI_IN,PIN_MIDI_OUT);
 //unsigned long t0 = 0;
 //boolean bData = false; 

/*
  protocol:
    Messages from MIDI-IN are directly sent to the pc.
    Control changes are transmitted to the pc via SysEx Messages
    
        0xf0: //start sysex Message
        <control id>
        <value>
        0xf7 //end sysex message
        
    format for the triggers:
        0xf0: //start sysex Message
        <control id>
        0xf7 //end sysex message
        
    <control id>:
          0..5 for the triggers 0..5
          10..15: the switches 0..5
          20..25 for the analog controls 0..5
          64..: Meta triggers 0,1,2
*/


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

boolean disableModeSwitch=false;

class ButtonsBase
{
	// values
  private:
    unsigned int pin0;
    unsigned int count;
    boolean enabled;
  public:
    boolean value[TOG_COUNT];
	// methods:
  public:
    ButtonsBase(
			const unsigned int pin0,
			const unsigned int count
		)
      :pin0(pin0),
      count(count),
      enabled(true)
    {
      for (int i=0; i<count ; i++)
      {
        value[i]=false;
      }
    }
		void setup( bool pullup=true )
		{
			for( int i=0; i<count; i++ )
			{
   			pinMode(pin0+i, INPUT);
				// enable pullup:
				if( pullup )
   				digitalWrite(pin0+i, HIGH);
			}
		}
    // overwrite:
    virtual void OnChange(const unsigned int index, boolean newValue) { }
    void update()
    {
      if(enabled)
        for (int i=0; i< count ; i++)
          {
            if(digitalRead(pin0+i)!= value[i])
            {
              value[i]=!value[i];
              OnChange(i,value[i]);
            }
          }
    }
    void enable(boolean b=true)
    {
      enabled=b;
    }
    void disable()
    {
      enable(false);
    }
};

class TriggersOrSwitches: public ButtonsBase
{
  public:
    TriggersOrSwitches(
			const unsigned int pin0
		)
      :ButtonsBase(
				pin0,
				1
			)
    {}
    virtual void OnChange(const unsigned int index, boolean newValue)
    {
      //if(!disableModeSwitch)
      {
        #ifdef DEBUG_PRINT
          if(newValue==TRIGGERS)
            Serial.println("switching to TRIGGERS mode");
          else
            Serial.println("switching to SWITCHES mode");
        #endif
      }
    }
};

class Switches: public ButtonsBase
{
  private:
    boolean zero;
  public:
    Switches(
			const unsigned int pin0,
			const unsigned int count,
			const boolean zero
		)
      :ButtonsBase(
				pin0,
				count
			),
      zero(zero)
    {}
    
    virtual void OnChange(
			const unsigned int index,
			boolean newValue
		)
    {
      toPC(index, newValue!=zero );
    }

    void toPC(
			const int index,
			const boolean value
		)
    {
      #ifdef DEBUG_PRINT
        Serial.println(String("Switch[") + String(index) + String("] = " + String((unsigned int )value)));
      #else
				Serial.write( 0xB0 );
				Serial.write( SWITCHES_CONTROL_ID + index );
        Serial.write((byte ) value);
      #endif
    };
};

class Triggers: public ButtonsBase
{
  private:
    boolean down;
    long debounceTime;
    long lastTime;
  public:
    Triggers(
			const unsigned int pin0,
			const unsigned int count,
			const boolean down,
			const long debounceTime
		)
      :ButtonsBase(
				pin0,
				count
			),
      down(down),
      debounceTime(debounceTime),
      lastTime(0)
    {
    }
    virtual void OnChange(const unsigned int index, boolean newValue)
    {
      if(newValue==down)
      {
        long time= millis();
        if((time- lastTime) > 1000)
          disableModeSwitch=false;
        if((time- lastTime) > debounceTime)
        {
          lastTime=time;
          disableModeSwitch=true;
          toPC(index);
        }
      }
    }
    virtual void toPC(const int index)
    {
      #ifdef DEBUG_PRINT
        Serial.println(String("Trigger[") + String(index) + String("] pressed!"));
      #else
				Serial.write( 0xB0 );
				Serial.write( TRIGGERS_CONTROL_ID + index );
        Serial.write((byte ) 1);
				/*
        Serial.write(0xf0); //start sysex Message
        Serial.write(TRIGGERS_CONTROL_ID + index); // the control id
        Serial.write(0xf7); //end sysex message
				*/
      #endif
    };
};

class MetaTriggers: public Triggers
{
  public:
    MetaTriggers(
			const unsigned int pin0,
			const unsigned int count,
			const boolean down,
			const long debounceTime
		)
      :Triggers(
				pin0,
				count,
				down,
				debounceTime
			)
    {}
    
    void toPC(const int index)
    {
      #ifdef DEBUG_PRINT
        Serial.println(String("Meta[") + String(index) + String("] pressed!"));
      #else
				Serial.write( 0xB0 );
				Serial.write( META_CONTROL_ID + index );
        Serial.write((byte ) 1);
				/*
        Serial.write(0xf0); //start sysex Message
        Serial.write(META_CONTROL_ID + index); // the control id
        Serial.write(0xf7); //end sysex message
				*/
      #endif
    };
};

TriggersOrSwitches triggersOrSwitches = TriggersOrSwitches(
	PIN_TRIGGER_OR_SWITCH
);
Triggers triggers = Triggers(
  PIN_TOG_0,
	TOG_COUNT,
	TRIGGER_PRESSED,DEBOUNCE_TIME
);
Switches switches = Switches(
	PIN_TOG_0,
	TOG_COUNT,
	false
);

MetaTriggers metaTriggers = MetaTriggers(
	PIN_META_TRIGGER_0,
	META_COUNT,
	TRIGGER_PRESSED,
	DEBOUNCE_TIME
);

Analog analog;

void setup() {
	triggersOrSwitches.setup();
	triggers.setup();
	switches.setup();
	metaTriggers.setup();
	
	//serialMidi.begin(MIDI_RATE);
	analog.setup();
	//analog.sendAll();

	Serial.begin(PC_RATE);

	#ifdef DEBUG_PRINT
  Serial.println("initializing");
	#endif
}

//Message

void loop() {
  triggersOrSwitches.update();
  if(triggersOrSwitches.value[0]==TRIGGERS)
    triggers.update();
  else
    switches.update();
  metaTriggers.update();
  analog.update();
}
