/////////////////////////////
// baud rates:
/////////////////////////////
#define PC_RATE 9600
//#define PC_RATE 38400

/////////////////////////////
// aliases for the arduino pins:
/////////////////////////////

// analog 
#define ANALOG_COUNT 6
#define ANALOG_MAX 1023
// A0, ..., A5


class Analog
{
  private:
    float analog[ANALOG_COUNT];
  public:

    Analog()
    {
      for(int i=0; i<ANALOG_COUNT; i++)
      {
        analog[i] = 0;
      }
    };

		void setup()
		{
      // nothing todo here
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
      Serial.println("update");
      for( int i=0; i<ANALOG_COUNT; i++)
      {
        analog[i] = ((float )analogRead(i)) / (float )ANALOG_MAX;
        this->toPC(i);
      }
    }

    void toPC(int i)
    {
        Serial.print( "Analog value [" );
        Serial.print( i );
        Serial.print( "]: " );
        Serial.println( analog[i] );
    }

};

Analog analog;

void setup() {
  Serial.begin(PC_RATE);
	analog.setup();
}

//Message

void loop() {
  Serial.println("bang");
  analog.update();
  delay( 1000 );
}
