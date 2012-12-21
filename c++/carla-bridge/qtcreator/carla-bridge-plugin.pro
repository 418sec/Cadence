# QtCreator project file

QT = core gui

CONFIG    = debug link_pkgconfig qt warn_on
PKGCONFIG = jack liblo

TARGET   = carla-bridge-qtcreator
TEMPLATE = app
VERSION  = 0.5.0

SOURCES  = \
    ../carla_bridge_osc.cpp \
    ../carla_bridge_plugin.cpp

SOURCES += \
    ../../carla-engine/carla_engine.cpp \
    ../../carla-engine/carla_engine_osc.cpp \
    ../../carla-engine/carla_engine_thread.cpp \
    ../../carla-engine/jack.cpp

SOURCES += \
    ../../carla-plugin/carla_plugin.cpp \
    ../../carla-plugin/carla_plugin_thread.cpp \
    ../../carla-plugin/ladspa.cpp \
    ../../carla-plugin/dssi.cpp \
    ../../carla-plugin/lv2.cpp \
    ../../carla-plugin/vst.cpp

HEADERS  = \
    ../carla_bridge.hpp \
    ../carla_bridge_client.hpp \
    ../carla_bridge_osc.hpp \
    ../carla_bridge_toolkit.hpp \

HEADERS += \
    ../../carla-includes/carla_defines.hpp \
    ../../carla-includes/carla_midi.h \
    ../../carla-includes/ladspa_rdf.hpp \
    ../../carla-includes/lv2_rdf.hpp

HEADERS += \
    ../../carla-backend/carla_backend.hpp \
    ../../carla-backend/carla_backend_utils.hpp

HEADERS += \
    ../../carla-engine/carla_engine.hpp \
    ../../carla-engine/carla_engine_osc.hpp \
    ../../carla-engine/carla_engine_thread.hpp \

HEADERS += \
    ../../carla-plugin/carla_plugin.hpp \
    ../../carla-plugin/carla_plugin_thread.hpp

HEADERS += \
    ../../carla-utils/carla_lib_utils.hpp \
    ../../carla-utils/carla_osc_utils.hpp \
    ../../carla-utils/carla_ladspa_utils.hpp \
    ../../carla-utils/carla_lv2_utils.hpp \
    ../../carla-utils/carla_vst_utils.hpp

INCLUDEPATH = .. \
    ../../carla-backend \
    ../../carla-engine \
    ../../carla-includes \
    ../../carla-jackbridge \
    ../../carla-plugin \
    ../../carla-utils

DEFINES  = QTCREATOR_TEST
DEFINES += DEBUG
#DEFINES += VESTIGE_HEADER
DEFINES += BUILD_BRIDGE BUILD_BRIDGE_PLUGIN
DEFINES += CARLA_ENGINE_JACK
DEFINES += WANT_LADSPA WANT_DSSI WANT_LV2 WANT_VST

LIBS     = -ldl \
    ../../carla-lilv/carla_lilv.a \
    ../../carla-rtmempool/carla_rtmempool.a

QMAKE_CXXFLAGS *= -std=c++0x
