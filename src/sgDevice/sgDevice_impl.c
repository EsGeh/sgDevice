#include "sgDevice_impl.h"

#include "Constants.h"

#include "m_pd.h"

static t_class* sgDevice_class;

t_class* register_sgDevice_class( t_symbol* class_name );

#define WITH_PREFIX(TOKEN) sgDevice ## _ ## TOKEN

#include "sgDevice_template.h"

void sgDevice_impl_setup()
{
	sgDevice_class = sgDevice_register_class( gensym("sgDevice") );
}
