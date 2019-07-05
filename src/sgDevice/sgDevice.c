#include "sgDevice.h"

#include "sgDevice_impl.h"
#include "sgDevice2_impl.h"

void sgDevice_setup()
{
	sgDevice_impl_setup();
	sgDevice2_impl_setup();
}
