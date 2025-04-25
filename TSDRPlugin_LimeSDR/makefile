#-------------------------------------------------------------------------------
# Adapted for TSDRPlugin_LimeSDR
#-------------------------------------------------------------------------------

PLUGNAME=TSDRPlugin_LimeSDR

# Dependencies
OBJS=$(PLUGNAME).o timer.o
DEPS=TSDRPlugin.h TSDRCodes.h timer.h osdetect.h

ifeq ($(BUILD_TYPE),Debug)
	DEBUGFLAGS+=-g -O0
else
	CFLAGS+=-O3
endif

HEADLOCATION=../TempestSDR/src/include/

ifeq ($(OS),Windows_NT)
	OSNAME ?= WINDOWS
	ifndef $(ARCHNAME)
		ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
			ARCHNAME = X64
		endif
		ifeq ($(PROCESSOR_ARCHITECTURE),x86)
			ARCHNAME = X86
		endif
	endif
else ifeq ($(shell uname -s),Darwin)
	OSNAME ?= MAC
	INC ?= /System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers
	EXT ?= .so
	ifeq ($(shell uname -m),x86)
		ARCHNAME = X86
	endif
	ifeq ($(shell uname -m),x86_64)
		ARCHNAME = X64
	endif
else
	ifndef $(OSNAME)
		UNAME_S := $(shell uname -s)
		ifeq ($(UNAME_S),Linux)
			OSNAME = LINUX
		endif
	endif
	ifndef $(ARCHNAME)
		UNAME_M := $(shell uname -m)
		ifeq ($(UNAME_M),x86_64)
			ARCHNAME = X64
		endif
		ifneq ($(filter %86,$(UNAME_M)),)
			ARCHNAME = X86
		endif
		ifneq ($(filter arm%,$(UNAME_M)),)
			ARCHNAME = ARM
		endif
	endif
endif

ifeq ($(OSNAME),WINDOWS)
	EXT=.dll
	LIBPREFIX=
endif
ifeq ($(OSNAME),LINUX)
	EXT=.so
	LIBPREFIX=lib
	LDFLAGS+=-L/usr/lib/x86_64-linux-gnu -lLimeSuite
endif

ifeq ($(ARCHNAME),X86)
	COMPILATION_TYPE=-m32
else ifeq ($(ARCHNAME),X64)
	COMPILATION_TYPE=-m64
endif

SOURCEFOLDER=src
OUTPUTFOLDER=bin/$(OSNAME)/$(ARCHNAME)
OBJFOLDER=obj

ifeq ($(OSNAME),LINUX)
	CFLAGS+=-fPIC
	LDFLAGS+=-L/usr/lib/x86_64-linux-gnu -lLimeSuite

else ifeq ($(OSNAME),WINDOWS)
	LDFLAGS+=-Wl,--add-stdcall-alias -llimesuite
endif

_OBJS = $(patsubst %,$(OBJFOLDER)/%,$(OBJS))
_DEPS = $(patsubst %,$(SOURCEFOLDER)/%,$(DEPS))

all : $(OUTPUTFOLDER)/$(LIBPREFIX)$(PLUGNAME)$(EXT)

copyoverheaderfile:
	@cp -f $(HEADLOCATION)/TSDRPlugin.h $(SOURCEFOLDER)/
	@cp -f $(HEADLOCATION)/TSDRCodes.h $(SOURCEFOLDER)/

$(OUTPUTFOLDER)/$(LIBPREFIX)$(PLUGNAME)$(EXT): copyoverheaderfile $(_OBJS)
	$(CC) -Wall -shared $(DEBUGFLAGS) $(COMPILATION_TYPE) -o $@ $(_OBJS) -L/usr/lib/x86_64-linux-gnu -lLimeSuite


$(OBJFOLDER)/%.o : $(SOURCEFOLDER)/%.c $(_DEPS)
	mkdir -p $(OUTPUTFOLDER)
	mkdir -p $(OBJFOLDER)
	$(CC) -Wall $(foreach d, $(INC), -I$d) $(CFLAGS) $(DEBUGFLAGS) $(COMPILATION_TYPE) -c $< -o $@

.PHONY: clean

clean :
	rm -f $(OBJFOLDER)/*.o $(OUTPUTFOLDER)/*.* $(OUTPUTFOLDER)/$(LIBPREFIX)$(PLUGNAME)$(EXT)

