# @configure_input@

PATH_GLOBAL = src/global
PATH_CODE = src/sgDevice

PATH_PD_OBJS = pd_objs

AM_CFLAGS = -Wall -fpic -std=c99 -Winline -fgnu89-inline -I$(top_srcdir)/$(PATH_GLOBAL)

# by default, copy all to the prefix dir, no subdirs
libdir = $(prefix)
datadir = $(prefix)

# list of all products:
lib_LTLIBRARIES = sgDevice.la
data_DATA = \
	$(PATH_PD_OBJS)/sgDevice-help.pd \
	$(PATH_PD_OBJS)/sgDeviceEvents.pd \
	$(PATH_PD_OBJS)/sgDevice_obj.pd \
	$(PATH_PD_OBJS)/sgDeviceModes_obj.pd \
	$(PATH_PD_OBJS)/sgDeviceModes_ui.pd \
	$(PATH_PD_OBJS)/sdKeyboardEvents.pd \
	$(PATH_PD_OBJS)/sdKeyboard_obj.pd \
	$(PATH_PD_OBJS)/sdKeyboardModes_obj.pd \
	$(PATH_PD_OBJS)/sdKeyboardModes_ui.pd

sgDevice_la_SOURCES = \
  $(PATH_CODE)/sgDevice.c $(PATH_CODE)/sgDevice.h \
  $(PATH_CODE)/sgDevice_impl.c $(PATH_CODE)/sgDevice_impl.h \
  $(PATH_CODE)/sgDevice2_impl.c $(PATH_CODE)/sgDevice2_impl.h \
  $(PATH_CODE)/sgDevice_template.h \
  $(PATH_GLOBAL)/Global.h \
  $(PATH_GLOBAL)/Constants.h \
  $(PATH_GLOBAL)/Constants_ver2.h

sgDevice_la_LDFLAGS = -module
