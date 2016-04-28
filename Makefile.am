AUTOMAKE_OPTIONS = foreign

ROOTFLAGS = $(shell root-config --cflags)
CXXFLAGS = -I.  $(ROOTFLAGS) -I$(ONLINE_MAIN)/include -I$(OFFLINE_MAIN)/include -g

LDFLAGS = -L$(ONLINE_MAIN)/lib -L$(OFFLINE_MAIN)/lib -lpmonitor -fPIC

INCLUDES = -I$(includedir) -I$(ONLINE_MAIN)/include -I$(OFFLINE_MAIN)/include -I$(ROOTSYS)/include

CXXFLAGS += -O2 -g

lib_LTLIBRARIES = \
  libcyclops.la 

noinst_HEADERS = \
  cyclopsLinkDef.h 

include_HEADERS = \
  APad.h \
  AZig.h \
  MyFavoriteMartin.h \
  FillRawHist.h \
  FillCalHist.h \
  FillMaxCharge.h \
  FillMaxTPCCharge.h \
  cyclops.h \
  caen_correction.h \
  prdfoStream.h \
  ATrace.h

libcyclops_la_SOURCES = \
  APad.C \
  AZig.C \
  cyclops.C \
  MyFavoriteMartin.C \
  FillRawHist.C \
  FillCalHist.C \
  FillMaxCharge.C \
  FillMaxTPCCharge.C \
  ATrace.C \
  caen_correction.cc \
  prdfoStream.cc \
  cyclops_Dict.C

libcyclops_la_LDFLAGS =  -L$(ONLINE_MAIN)/lib -L$(OFFLINE_MAIN)/lib -lpmonitor -fPIC

libcyclops_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L/usr/local/lib/root \
-lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lm -ldl

# Rule for generating CINT dictionaries from class headers.
cyclops_Dict.C: \
  APad.h \
  AZig.h \
  ATrace.h \
  cyclops.h \
  FillRawHist.h \
  FillCalHist.h \
  FillMaxCharge.h \
  FillMaxTPCCharge.h \
  MyFavoriteMartin.h \
  caen_correction.h \
  cyclopsLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(INCLUDES) $^

clean-local:
	rm -f *Dict*
