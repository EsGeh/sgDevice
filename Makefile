# library name
lib.name = \
	sgDevice

PATH_SRC = src
PATH_GLOBAL = $(PATH_SRC)/global
PATH_CODE = src/sgDevice

PATH_PD_OBJS = pd_objs

make-lib-executable=yes

cflags = -Wall -fpic -std=c99 -Winline -fgnu89-inline -I$(PATH_GLOBAL)

class.sources = \
	$(filter-out $(PATH_CODE)/sgDevice.c, $(wildcard $(PATH_CODE)/*.c))

lib.setup.sources = \
	$(PATH_CODE)/sgDevice.c

datafiles = \
	README.md \
	$(PATH_PD_OBJS)/sgDevice-help.pd \
	$(PATH_PD_OBJS)/sgDevice-test.pd \
	$(PATH_PD_OBJS)/sgDeviceEvents.pd \
	$(PATH_PD_OBJS)/sgDeviceToInterface.pd \
	$(PATH_PD_OBJS)/sgDevice_obj.pd \
	$(PATH_PD_OBJS)/sgDeviceModes_obj.pd \
	$(PATH_PD_OBJS)/sgDeviceModes_ui.pd \
	$(PATH_PD_OBJS)/sdKeyboardEvents.pd \
	$(PATH_PD_OBJS)/sdKeyboard_obj.pd \
	$(PATH_PD_OBJS)/sdPianoEvents.pd \
	$(PATH_PD_OBJS)/sdPiano_obj.pd \
	$(PATH_PD_OBJS)/sdKeyboardModes_obj.pd \
	$(PATH_PD_OBJS)/sdKeyboardModes_ui.pd \
	$(PATH_PD_OBJS)/sdEvent_filterByIndex.pd

# include pd-lib-builder
PDLIBBUILDER_DIR=dependencies/pd-lib-builder
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder
