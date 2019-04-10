#include "sgDevice.h"

#include "Global.h"

#include "m_pd.h"
#include <fcntl.h> // working with file descriptors (open,close, ...)
#include <unistd.h>
#include <termios.h> // usb specific stuff
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#define IS_TRIGGER(ind) IN_RANGE(ind,TRIGGERS_CONTROL_ID,TOG_COUNT)
#define IS_SWITCH(ind) IN_RANGE(ind,SWITCHES_CONTROL_ID,TOG_COUNT)
#define IS_ANALOG(ind) IN_RANGE(ind,ANALOG_CONTROL_ID,ANALOG_COUNT)
#define IS_META(ind) IN_RANGE(ind,META_CONTROL_ID,META_COUNT)

#define INDEX_TRIGGER(pos) (pos-TRIGGERS)
#define INDEX_SWITCH(pos) (pos-SWITCHES)
#define INDEX_ANALOG(pos) (pos-ANALOG)
#define INDEX_META(pos) (pos-META)

#define IN_RANGE(x, min,count) (x>=min && x<min+count)

#define MAX_LENGTH_MESSAGE 8

// make shure this can be used as a boolean value:
#ifdef ANALOG_DOUBLE_PRECISION
#define ANALOG_DOUBLE_PRECISION 1
#else
#define ANALOG_DOUBLE_PRECISION 0
#endif


static t_class* sgDevice_class;
t_class* register_sgDevice_class( t_symbol* class_name );

void sgDevice_setup()
{
	sgDevice_class = register_sgDevice_class( gensym("sgDevice") );
}


//////////////////////////////////////////////
// sgDevice obj
//////////////////////////////////////////////

typedef struct _sgDevice {
	//internal obj information:
	t_object obj;
	t_int bufferLength; // number of symbols received yet
	t_int buffer[MAX_LENGTH_MESSAGE];
	// most/least significant bit for control messages:
	t_int msb;
	t_int lsb;
	// outlets:
	t_outlet* outlet;
} t_sgDevice;


// the constructor
void* sgDevice_init( );
void sgDevice_exit( t_sgDevice* pThis );
void sgDeviceOnInput(t_sgDevice* pThis, t_floatarg f);

void output(
		t_sgDevice* pThis,
		t_symbol* package_name,
		unsigned int index,
		t_float val
);

/* output format
	sgDevice.<descr>
	descr ::=
		trigger <0..5>
		switch <0..5> <bool>
		analog <0..5> <analogVal>
		meta <0..2> <bool>
*/


t_class* register_sgDevice_class( t_symbol* class_name )
{
	t_class* class = class_new(
		class_name, 	                  // name
		(t_newmethod )sgDevice_init, 	  // newmethod
		(t_method ) sgDevice_exit,      // freemethod
		sizeof(t_sgDevice), 		        // size
		CLASS_DEFAULT, 			            // object type
		0 				                      // creation arguments
	);
	class_addfloat(
			class,
			sgDeviceOnInput
	);
	return class;
}


// the constructor
void* sgDevice_init( )
{
	t_sgDevice* x = (t_sgDevice* )pd_new(sgDevice_class);
	x -> bufferLength = 0;
	x -> outlet = outlet_new(& x->obj, &s_list);

	return x;
}

// the constructor
void sgDevice_exit( t_sgDevice* pThis )
{
}

void sgDeviceOnInput(t_sgDevice* pThis, t_floatarg f)
{
	DB_PRINT( "buffer len.: %i, %x", pThis->bufferLength, (int )f);
	if(
		(f>=0x80)
	)
	// got statusbyte:
	{
		DB_PRINT("status byte");
		if(
			(((int )f) >> 4) == 0xB
		)
		// this is a control change message:
		{
			if(
				// already read some control messages MSB
				pThis->bufferLength == 3
				&& IS_ANALOG( pThis->buffer[1] )
				&& IN_RANGE(pThis->buffer[1], 0, 32)
				&& ANALOG_DOUBLE_PRECISION
			)
			// MSB
			{
				DB_PRINT("continue...");
				pThis->buffer [pThis->bufferLength] = f;
				pThis->bufferLength ++;
			}
			else
			{
				DB_PRINT("start new");
				pThis->buffer [0] = f;
				pThis->bufferLength=1;
			}
		}
	}
	else if( pThis->bufferLength > 0 )
	// got data byte:
	{
		DB_PRINT("argument");
		pThis->buffer [pThis->bufferLength] = f;
		pThis->bufferLength ++;

		// check if we have we buffered enough to execute it:
		if( 
			pThis->bufferLength == 3
			||
			pThis->bufferLength == 6
		)
		{
			t_int control_id = pThis->buffer[1];
			DB_PRINT("control_id %i", control_id);
			if(
				IS_TRIGGER( control_id )
				&& pThis->bufferLength == 3
			)
			{
				output(
						pThis,
						gensym("trigger"),
						control_id-TRIGGERS_CONTROL_ID,
						1
				);
			}
			
			else if(
				IS_SWITCH( control_id )
				&& pThis->bufferLength == 3
			)
			{
				t_int value = pThis->buffer[2];
				output(
						pThis,
						gensym("switch"),
						control_id-SWITCHES_CONTROL_ID,
						value
				);
			}

			else if(
				IS_META( control_id )
				&& pThis->bufferLength == 3
			)
			{
				output(
						pThis,
						gensym("meta"),
						control_id-META_CONTROL_ID,
						1
				);
			}

			#ifndef ANALOG_DOUBLE_PRECISION
			else if(
				IS_ANALOG(control_id)
				&& pThis->bufferLength == 3
			)
			{
				t_int value = pThis->buffer[2];
				output(
						pThis,
						gensym("analog"),
						control_id-ANALOG_CONTROL_ID,
						((float )value)/ANALOG_MIDI_RES
				);
				pThis->bufferLength=0;
			}
			#else
			else if(
				IS_ANALOG(control_id)
				&& pThis->bufferLength == 6
			)
			{
				t_int value = (pThis->buffer[2] << 7) + pThis->buffer[5];
				output(
						pThis,
						gensym("analog"),
						control_id-ANALOG_CONTROL_ID,
						((float )value)/ANALOG_MIDI_RES
				);
				pThis->bufferLength=0;
			}
			#endif
		}
	}
	else
	{
		//DB_PRINT("unknown message!");
	}
}

void output(
		t_sgDevice* pThis,
		t_symbol* package_name,
		unsigned int index,
		t_float val
)
{
	const int count = 3;
	t_atom list[count];
	SETSYMBOL(&list[0], package_name);
	SETFLOAT(&list[1], index);
	SETFLOAT(&list[2], val);
	outlet_list(
		pThis->outlet,
		&s_list,
		count,
		list
	);

	/*
	//sgDevice.analog ( index ( i ) value ( val ) )
	t_atom list[8];
	SETSYMBOL(&list[0], package_name);
	SETFLOAT(&list[1], 6);
	SETSYMBOL(&list[2], gensym("index"));
	SETFLOAT(&list[3], 1);
	SETFLOAT(&list[4], index);
	SETSYMBOL(&list[5], gensym("value"));
	SETFLOAT(&list[6], 1);
	SETFLOAT(&list[7], val );

	outlet_list(
		pThis->outlet,
		&s_list,
		8,
		list
	);
	*/
}
