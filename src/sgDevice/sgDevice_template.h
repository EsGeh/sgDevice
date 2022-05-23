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
#define IS_ANALOG_FINE(ind) IN_RANGE(ind,ANALOG_CONTROL_ID_FINE,ANALOG_COUNT)
#define IS_META(ind) IN_RANGE(ind,META_CONTROL_ID,META_COUNT)

#define INDEX_TRIGGER(pos) (pos-TRIGGERS_CONTROL_ID)
#define INDEX_SWITCH(pos) (pos-SWITCHES_CONTROL_ID)
#define INDEX_ANALOG(pos) (pos-ANALOG_CONTROL_ID)
#define INDEX_ANALOG_FINE(pos) (pos-ANALOG_CONTROL_ID_FINE)
#define INDEX_META(pos) (pos-META_CONTROL_ID)

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
	unsigned char analog_values[ANALOG_COUNT];
	unsigned char analog_fine[ANALOG_COUNT];
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
	ByteDynA_init( &x->buffer );
	memset( x->analog_values, 0, ANALOG_COUNT );
	memset( x->analog_fine, 0, ANALOG_COUNT );
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
	unsigned char current_byte = (unsigned char )f;

	// STATUS BYTE:
	if(
		(current_byte >= 0x80)
	)
	{
		ByteDynA_clear( &pThis->buffer) ;
		char status = current_byte >> 4;
		char channel = current_byte & 0b00001111;
		DB_PRINT( "status.: 0x%x, channel: %u", status, channel);
		if( 
			status == 0xB
			&& channel == 0
		) {
			ByteDynA_append( &pThis->buffer, current_byte );
		}
	}
	// DATA BYTE
	else if(
		// if we are listening:
		ByteDynA_get_size( &pThis->buffer ) > 0
	)
	{
		// 1. add data to buffer:
		ByteDynA_append( &pThis->buffer, current_byte );
	}
	if(
		ByteDynA_get_size( &pThis->buffer ) == 3
	)
	{
		unsigned char control_id = ByteDynA_get_array( &pThis->buffer )[1];
		unsigned char value =  ByteDynA_get_array( &pThis->buffer )[2];
		if(
				IS_ANALOG( control_id )
				||
				IS_ANALOG_FINE( control_id )
		) {
			if( IS_ANALOG( control_id ) ) {
				pThis->analog_values[INDEX_ANALOG(control_id)] = value;
			}
			else if( IS_ANALOG_FINE( control_id ) ) {
				pThis->analog_fine[INDEX_ANALOG_FINE(control_id)] = value;
				control_id -= 32;
			}
			unsigned int full_value =
				(((unsigned int )pThis->analog_values[INDEX_ANALOG(control_id)]) << 3)
				+
				(((unsigned int )pThis->analog_fine[INDEX_ANALOG(control_id)]) << 0)
			;
			DB_PRINT( "ANALOG: %i, %i", control_id, full_value);
			WITH_PREFIX(on_device_output_ctrl)(
					pThis,
					control_id,
					full_value
			);
		}
		else {
			DB_PRINT( "SOMETHING: %i, %i", control_id, value);
			WITH_PREFIX(on_device_output_ctrl)(
					pThis,
					control_id,
					value
			);
		}
		ByteDynA_set_size( &pThis->buffer, 1 );
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
				value > 63
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
				value > 63
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
				value > 63
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
				((float )value)/((float )(ANALOG_PIN_RESOLUTION-1))
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
}
