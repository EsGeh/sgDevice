# @configure_input@

PATH_GLOBAL = src/global
PATH_CODE = src/sgDevice

AM_CFLAGS = -Wall -fpic -std=c99 -Winline -fgnu89-inline -I$(top_srcdir)/$(PATH_GLOBAL)

# by default, copy all to the prefix dir, no subdirs
libdir = $(prefix)

# list of all products:
lib_LTLIBRARIES = sgDevice.la

sgDevice_la_SOURCES = \
  $(PATH_CODE)/sgDevice.c $(PATH_CODE)/sgDevice.h

sgDevice_la_LDFLAGS = -module
