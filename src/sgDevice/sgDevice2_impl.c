#include "sgDevice2_impl.h"

#include "Constants_ver2.h"

#include "m_pd.h"

static t_class* sgDevice2_class;

t_class* register_sgDevice2_class( t_symbol* class_name );

#define WITH_PREFIX(TOKEN) sgDevice2 ## _ ## TOKEN

#include "sgDevice_template.h"

void sgDevice2_impl_setup()
{
	sgDevice2_class = sgDevice2_register_class( gensym("sgDevice2") );
}

