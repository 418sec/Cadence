#!/usr/bin/make -f
# Makefile for Cadence C++ code #
# ------------------------------------ #
# Created by falkTX
#

AR  ?= ar
CC  ?= gcc
CXX ?= g++
MOC ?= $(shell pkg-config --variable=moc_location QtCore)
RCC ?= $(shell pkg-config --variable=rcc_location QtCore)
UIC ?= $(shell pkg-config --variable=uic_location QtCore)
STRIP ?= strip
WINDRES ?= windres

# --------------------------------------------------------------

DEBUG ?= false

ifeq ($(DEBUG),true)
BASE_FLAGS  = -O0 -g -Wall -Wextra
BASE_FLAGS += -DDEBUG
STRIP       = true # FIXME
else
BASE_FLAGS  = -O2 -ffast-math -mtune=generic -msse -mfpmath=sse -Wall -Wextra
BASE_FLAGS += -DNDEBUG
endif

32BIT_FLAGS = -m32
64BIT_FLAGS = -m64

BUILD_C_FLAGS   = $(BASE_FLAGS) -std=c99 $(CFLAGS)
BUILD_CXX_FLAGS = $(BASE_FLAGS) -std=c++0x $(CXXFLAGS)
LINK_FLAGS      = $(LDFLAGS)

ifneq ($(DEBUG),true)
BUILD_CXX_FLAGS += -DQT_NO_DEBUG -DQT_NO_DEBUG_STREAM -DQT_NO_DEBUG_OUTPUT
endif

# --------------------------------------------------------------
# Modify to enable/disable specific features

# Support for LADSPA, DSSI, LV2 and VST plugins
CARLA_PLUGIN_SUPPORT = true

# Support for GIG, SF2 and SFZ sample banks (through fluidsynth and linuxsampler)
CARLA_SAMPLERS_SUPPORT = true

# Support for Native Audio (ALSA and/or PulseAudio in Linux)
CARLA_RTAUDIO_SUPPORT = true

# Comment this line to not use vestige header
BUILD_CXX_FLAGS += -DVESTIGE_HEADER

# --------------------------------------------------------------

HAVE_JACK         = $(shell pkg-config --exists jack && echo true)
HAVE_JACKSESSION  = $(shell pkg-config --atleast-version=0.121.0 jack && echo true)

ifeq ($(CARLA_PLUGIN_SUPPORT),true)
HAVE_SUIL         = $(shell pkg-config --exists suil-0 && echo true)
endif

ifeq ($(CARLA_SAMPLERS_SUPPORT),true)
HAVE_FLUIDSYNTH   = $(shell pkg-config --exists fluidsynth && echo true)
HAVE_LINUXSAMPLER = $(shell pkg-config --exists linuxsampler && echo true)
endif

ifeq ($(CARLA_RTAUDIO_SUPPORT),true)
HAVE_ALSA         = $(shell pkg-config --exists alsa && echo true)
HAVE_PULSEAUDIO   = $(shell pkg-config --exists libpulse-simple && echo true)
endif

HAVE_ZYN_DEPS     = $(shell pkg-config --exists fftw3 mxml && echo true)
