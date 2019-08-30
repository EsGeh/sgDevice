#include "sgDevice.h"

#include "Global.h"

#include "m_pd.h"
#include <fcntl.h> // working with file descriptors (open,close, ...)
#include <unistd.h>
#include <termios.h> // usb specific stuff
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#ifndef WITH_PREFIX
	#error "please define WITH_PREFIX first!"
#endif

#define IS_TRIGGER(ind) IN_RANGE(ind,TRIGGERS_CONTROL_ID,TRIGGERS_COUNT)
#define IS_SWITCH(ind) IN_RANGE(ind,SWITCHES_CONTROL_ID,SWITCHES_COUNT)
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
#undef ANALOG_DOUBLE_PRECISION
#define ANALOG_DOUBLE_PRECISION 1
#else
#define ANALOG_DOUBLE_PRECISION 0
#endif


//////////////////////////////////////////////
// sgDevice obj
//////////////////////////////////////////////

typedef struct _sgDevice {
	//internal obj information:
	t_object obj;
	ByteDynA buffer;
	//t_int bufferLength; // number of symbols received yet
	//t_int buffer[MAX_LENGTH_MESSAGE];
	// most/least significant bit for control messages:
	// outlets:
	t_outlet* outlet;
} t_sgDevice;


// the constructor
void* WITH_PREFIX(init)( );
void WITH_PREFIX(exit)( t_sgDevice* pThis );
void WITH_PREFIX(on_input)(t_sgDevice* pThis, t_floatarg f);

void WITH_PREFIX(on_device_output_ctrl)(
		t_sgDevice* pThis,
		const unsigned char control_id,
		t_int value
);

void WITH_PREFIX(output)(
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

t_class* WITH_PREFIX(register_class)( t_symbol* class_name )
{
	t_class* class = class_new(
		class_name, 	                  // name
		(t_newmethod )WITH_PREFIX(init), 	  // newmethod
		(t_method ) WITH_PREFIX(exit),      // freemethod
		sizeof(t_sgDevice), 		        // size
		CLASS_DEFAULT, 			            // object type
		0 				                      // creation arguments
	);
	class_addfloat(
			class,
			WITH_PREFIX(on_input)
	);
	return class;
}


// the constructor
void* WITH_PREFIX(init)( )
{
	t_sgDevice* x = (t_sgDevice* )pd_new(WITH_PREFIX(class));
	//x -> bufferLength = 0;
	ByteDynA_init( &x->buffer );
	x -> outlet = outlet_new(& x->obj, &s_list);

	return x;
}

// the constructor
void WITH_PREFIX(exit)( t_sgDevice* pThis )
{
	ByteDynA_exit( &pThis->buffer );
}

void WITH_PREFIX(on_input)(t_sgDevice* pThis, t_floatarg f)
{
	DB_PRINT( "buffer len.: %i, %x", ByteDynA_get_size( &pThis->buffer ), (int )f);
	unsigned char current_byte = (unsigned char )f;
	if(
		(current_byte >= 0x80)
	)
	// status byte:
	{
		unsigned char
			msg_type = current_byte >> 4,
			channel = (current_byte & 0b00001111 )
			;
		DB_PRINT( "status byte. msg: %x, channel: %i", msg_type, channel);
		// only consider control change events:
		if(
			channel == 0
			&& msg_type == 0xB
		)
		{
			// continue
			if(
					// we already have a msb,
					// and we are waiting for a lsb
					ByteDynA_get_size( &pThis->buffer ) >= 2
					&&
					ByteDynA_get_array( &pThis->buffer )[1] < 32
			)
			{
				if( ByteDynA_get_size( &pThis->buffer ) == 3 )
				{
					ByteDynA_append( &pThis->buffer, current_byte);
				}
				else
				{
					ByteDynA_clear( &pThis->buffer );
				}
			}
			// start new message
			else
			{
				ByteDynA_clear( &pThis->buffer );
				ByteDynA_append( &pThis->buffer, current_byte);
			}
		}
		else if( 
				ByteDynA_get_size( &pThis->buffer ) > 0
		)
		{
			ByteDynA_clear( &pThis->buffer );
		}
	}
	else
	// data byte
	{
		if(
				ByteDynA_get_size( &pThis->buffer ) > 0
		)
		// we have a status byte already,
		// and we are waiting for data bytes
		{
			if(
					ByteDynA_get_size( &pThis->buffer ) < 6
			)
			{
				ByteDynA_append( &pThis->buffer, current_byte );
			}
		}

		// maybe output s.t. and clear the buffer
		if(
				ByteDynA_get_size( &pThis->buffer ) == 3
				&& 
				// lsb, so we are done!
				ByteDynA_get_array( &pThis->buffer )[1] >= 32
		)
		{
			// dump lsb
			WITH_PREFIX(on_device_output_ctrl)(
					pThis,
					ByteDynA_get_array( &pThis->buffer )[1],
					ByteDynA_get_array( &pThis->buffer )[2]
			);
			ByteDynA_clear( &pThis->buffer );
		}
		if(
				// we have a msb in the first triple:
				ByteDynA_get_size( &pThis->buffer ) == 6
				//ByteDynA_get_array( &pThis->buffer )[1] < 32
		)
		{
			if(
					// we have the corresponding lsb
					ByteDynA_get_array( &pThis->buffer )[4]
					==
					ByteDynA_get_array( &pThis->buffer )[1] + 32
			)
			{
				// dump msb+lsb
				//DB_PRINT( "msb+lsb found!");
				WITH_PREFIX(on_device_output_ctrl)(
						pThis,
						ByteDynA_get_array( &pThis->buffer )[4],
						(((unsigned int )ByteDynA_get_array( &pThis->buffer )[2]) << 7)
						+
						(((unsigned int )ByteDynA_get_array( &pThis->buffer )[5]) << 0)
				);
			}
			else
			{
				if(
					ByteDynA_get_array( &pThis->buffer )[4] >= 32
				)
				{
					// dump lsb
					WITH_PREFIX(on_device_output_ctrl)(
							pThis,
							ByteDynA_get_array( &pThis->buffer )[4],
							ByteDynA_get_array( &pThis->buffer )[5]
					);
				}
			}
			ByteDynA_clear( &pThis->buffer );
		}
	}
}

void WITH_PREFIX(on_device_output_ctrl)(
		t_sgDevice* pThis,
		const unsigned char control_id,
		t_int value
)
{

	if(
		IS_TRIGGER( control_id )
	)
	{
		WITH_PREFIX(output)(
				pThis,
				gensym("trigger"),
				control_id-TRIGGERS_CONTROL_ID,
				value
		);
	}
	
	else if(
		IS_SWITCH( control_id )
	)
	{
		WITH_PREFIX(output)(
				pThis,
				gensym("switch"),
				control_id-SWITCHES_CONTROL_ID,
				value
		);
	}

	else if(
		IS_META( control_id )
	)
	{
		WITH_PREFIX(output)(
				pThis,
				gensym("meta"),
				control_id-META_CONTROL_ID,
				value
		);
	}

	else if(
		IS_ANALOG(control_id)
	)
	{
		WITH_PREFIX(output)(
				pThis,
				gensym("analog"),
				control_id-ANALOG_CONTROL_ID,
				((float )value)/ANALOG_MIDI_RES
		);
	}
}

void WITH_PREFIX(output)(
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
