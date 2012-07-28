/*
 * Carla Backend
 * Copyright (C) 2011-2012 Filipe Coelho <falktx@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the COPYING file
 */

#ifdef BUILD_BRIDGE
#error Should not use fluidsynth for bridges!
#endif

#include "carla_plugin.h"

#ifdef WANT_FLUIDSYNTH
#include "carla_fluidsynth.h"
#else
#warning fluidsynth not available (no SF2 support)
#endif

CARLA_BACKEND_START_NAMESPACE

#if 0
} /* adjust editor indent */
#endif

#ifdef WANT_FLUIDSYNTH

/*!
 * @defgroup CarlaBackendFluidSynthPlugin Carla Backend FluidSynth Plugin
 *
 * The Carla Backend FluidSynth Plugin.\n
 * http://www.fluidsynth.org/
 * @{
 */

class FluidSynthPlugin : public CarlaPlugin
{
public:
    FluidSynthPlugin(CarlaEngine* const engine, unsigned short id) : CarlaPlugin(engine, id)
    {
        qDebug("FluidSynthPlugin::FluidSynthPlugin()");

        m_type  = PLUGIN_SF2;
        m_label = nullptr;

        // create settings
        f_settings = new_fluid_settings();

        // define settings
        fluid_settings_setnum(f_settings, "synth.sample-rate", x_engine->getSampleRate());
        fluid_settings_setint(f_settings, "synth.threadsafe-api ", 0);

        // create synth
        f_synth = new_fluid_synth(f_settings);

#ifdef FLUIDSYNTH_VERSION_NEW_API
        fluid_synth_set_sample_rate(f_synth, x_engine->getSampleRate());
#endif

        // set default values
        fluid_synth_set_reverb_on(f_synth, 0);
        fluid_synth_set_reverb(f_synth, FLUID_REVERB_DEFAULT_ROOMSIZE, FLUID_REVERB_DEFAULT_DAMP, FLUID_REVERB_DEFAULT_WIDTH, FLUID_REVERB_DEFAULT_LEVEL);

        fluid_synth_set_chorus_on(f_synth, 0);
        fluid_synth_set_chorus(f_synth, FLUID_CHORUS_DEFAULT_N, FLUID_CHORUS_DEFAULT_LEVEL, FLUID_CHORUS_DEFAULT_SPEED, FLUID_CHORUS_DEFAULT_DEPTH, FLUID_CHORUS_DEFAULT_TYPE);

        fluid_synth_set_polyphony(f_synth, 64);

        for (int i=0; i < 16; i++)
            fluid_synth_set_interp_method(f_synth, i, FLUID_INTERP_DEFAULT);
    }

    ~FluidSynthPlugin()
    {
        qDebug("FluidSynthPlugin::~FluidSynthPlugin()");

        if (m_label)
            free((void*)m_label);

        delete_fluid_synth(f_synth);
        delete_fluid_settings(f_settings);
    }

    // -------------------------------------------------------------------
    // Information (base)

    PluginCategory category()
    {
        return PLUGIN_CATEGORY_SYNTH;
    }

    // -------------------------------------------------------------------
    // Information (count)

    uint32_t parameterScalePointCount(uint32_t parameterId)
    {
        assert(parameterId < param.count);

        switch (parameterId)
        {
        case FluidSynthChorusType:
            return 2;
        case FluidSynthInterpolation:
            return 4;
        default:
            return 0;
        }
    }

    // -------------------------------------------------------------------
    // Information (per-plugin data)

    double getParameterValue(uint32_t parameterId)
    {
        assert(parameterId < param.count);
        return param_buffers[parameterId];
    }

    double getParameterScalePointValue(uint32_t parameterId, uint32_t scalePointId)
    {
        assert(parameterId < param.count);
        assert(scalePointId < parameterScalePointCount(parameterId));

        switch (parameterId)
        {
        case FluidSynthChorusType:
            switch (scalePointId)
            {
            case 0:
                return FLUID_CHORUS_MOD_SINE;
            case 1:
                return FLUID_CHORUS_MOD_TRIANGLE;
            default:
                return FLUID_CHORUS_DEFAULT_TYPE;
            }
        case FluidSynthInterpolation:
            switch (scalePointId)
            {
            case 0:
                return FLUID_INTERP_NONE;
            case 1:
                return FLUID_INTERP_LINEAR;
            case 2:
                return FLUID_INTERP_4THORDER;
            case 3:
                return FLUID_INTERP_7THORDER;
            default:
                return FLUID_INTERP_DEFAULT;
            }
        default:
            return 0.0;
        }
    }

    void getLabel(char* const strBuf)
    {
        strncpy(strBuf, m_label, STR_MAX);
    }

    void getMaker(char* const strBuf)
    {
        strncpy(strBuf, "FluidSynth SF2 engine", STR_MAX);
    }

    void getCopyright(char* const strBuf)
    {
        strncpy(strBuf, "GNU GPL v2+", STR_MAX);
    }

    void getRealName(char* const strBuf)
    {
        getLabel(strBuf);
    }

    void getParameterName(uint32_t parameterId, char* const strBuf)
    {
        assert(parameterId < param.count);

        switch (parameterId)
        {
        case FluidSynthReverbOnOff:
            strncpy(strBuf, "Reverb On/Off", STR_MAX);
            break;
        case FluidSynthReverbRoomSize:
            strncpy(strBuf, "Reverb Room Size", STR_MAX);
            break;
        case FluidSynthReverbDamp:
            strncpy(strBuf, "Reverb Damp", STR_MAX);
            break;
        case FluidSynthReverbLevel:
            strncpy(strBuf, "Reverb Level", STR_MAX);
            break;
        case FluidSynthReverbWidth:
            strncpy(strBuf, "Reverb Width", STR_MAX);
            break;
        case FluidSynthChorusOnOff:
            strncpy(strBuf, "Chorus On/Off", STR_MAX);
            break;
        case FluidSynthChorusNr:
            strncpy(strBuf, "Chorus Voice Count", STR_MAX);
            break;
        case FluidSynthChorusLevel:
            strncpy(strBuf, "Chorus Level", STR_MAX);
            break;
        case FluidSynthChorusSpeedHz:
            strncpy(strBuf, "Chorus Speed", STR_MAX);
            break;
        case FluidSynthChorusDepthMs:
            strncpy(strBuf, "Chorus Depth", STR_MAX);
            break;
        case FluidSynthChorusType:
            strncpy(strBuf, "Chorus Type", STR_MAX);
            break;
        case FluidSynthPolyphony:
            strncpy(strBuf, "Polyphony", STR_MAX);
            break;
        case FluidSynthInterpolation:
            strncpy(strBuf, "Interpolation", STR_MAX);
            break;
        case FluidSynthVoiceCount:
            strncpy(strBuf, "Voice Count", STR_MAX);
            break;
        default:
            *strBuf = 0;
            break;
        }
    }

    void getParameterUnit(uint32_t parameterId, char* const strBuf)
    {
        assert(parameterId < param.count);

        switch (parameterId)
        {
        case FluidSynthChorusSpeedHz:
            strncpy(strBuf, "Hz", STR_MAX);
            break;
        case FluidSynthChorusDepthMs:
            strncpy(strBuf, "ms", STR_MAX);
            break;
        default:
            *strBuf = 0;
            break;
        }
    }

    void getParameterScalePointLabel(uint32_t parameterId, uint32_t scalePointId, char* const strBuf)
    {
        assert(parameterId < param.count);
        assert(scalePointId < parameterScalePointCount(parameterId));

        switch (parameterId)
        {
        case FluidSynthChorusType:
            switch (scalePointId)
            {
            case 0:
                strncpy(strBuf, "Sine wave", STR_MAX);
                return;
            case 1:
                strncpy(strBuf, "Triangle wave", STR_MAX);
                return;
            }
        case FluidSynthInterpolation:
            switch (scalePointId)
            {
            case 0:
                strncpy(strBuf, "None", STR_MAX);
                return;
            case 1:
                strncpy(strBuf, "Straight-line", STR_MAX);
                return;
            case 2:
                strncpy(strBuf, "Fourth-order", STR_MAX);
                return;
            case 3:
                strncpy(strBuf, "Seventh-order", STR_MAX);
                return;
            }
        }

        *strBuf = 0;
    }

    // -------------------------------------------------------------------
    // Set data (plugin-specific stuff)

    void setParameterValue(uint32_t parameterId, double value, bool sendGui, bool sendOsc, bool sendCallback)
    {
        assert(parameterId < param.count);
        param_buffers[parameterId] = fixParameterValue(value, param.ranges[parameterId]);

        switch (parameterId)
        {
        case FluidSynthReverbOnOff:
            value = value > 0.5 ? 1 : 0;
            fluid_synth_set_reverb_on(f_synth, value);
            break;

        case FluidSynthReverbRoomSize:
        case FluidSynthReverbDamp:
        case FluidSynthReverbLevel:
        case FluidSynthReverbWidth:
            fluid_synth_set_reverb(f_synth, param_buffers[FluidSynthReverbRoomSize], param_buffers[FluidSynthReverbDamp], param_buffers[FluidSynthReverbWidth], param_buffers[FluidSynthReverbLevel]);
            break;

        case FluidSynthChorusOnOff:
        {
            const CarlaPluginScopedDisabler m(this, ! x_engine->isOffline());
            value = value > 0.5 ? 1 : 0;
            fluid_synth_set_chorus_on(f_synth, value);
            break;
        }

        case FluidSynthChorusNr:
        case FluidSynthChorusLevel:
        case FluidSynthChorusSpeedHz:
        case FluidSynthChorusDepthMs:
        case FluidSynthChorusType:
        {
            const CarlaPluginScopedDisabler m(this, ! x_engine->isOffline());
            fluid_synth_set_chorus(f_synth, rint(param_buffers[FluidSynthChorusNr]), param_buffers[FluidSynthChorusLevel], param_buffers[FluidSynthChorusSpeedHz], param_buffers[FluidSynthChorusDepthMs], rint(param_buffers[FluidSynthChorusType]));
            break;
        }

        case FluidSynthPolyphony:
        {
            const CarlaPluginScopedDisabler m(this, ! x_engine->isOffline());
            fluid_synth_set_polyphony(f_synth, rint(value));
            break;
        }

        case FluidSynthInterpolation:
        {
            const CarlaPluginScopedDisabler m(this, ! x_engine->isOffline());
            for (int i=0; i < 16; i++)
                fluid_synth_set_interp_method(f_synth, i, rint(value));
            break;
        }

        default:
            break;
        }

        CarlaPlugin::setParameterValue(parameterId, value, sendGui, sendOsc, sendCallback);
    }

    void setMidiProgram(int32_t index, bool sendGui, bool sendOsc, bool sendCallback, bool block)
    {
        assert(index < (int32_t)midiprog.count);

        if (cin_channel < 0 || cin_channel > 15)
            return;

        if (index >= 0)
        {
            if (x_engine->isOffline())
            {
                if (block) engineProcessLock();
                fluid_synth_program_select(f_synth, cin_channel, f_id, midiprog.data[index].bank, midiprog.data[index].program);
                if (block) engineProcessUnlock();
            }
            else
            {
                const CarlaPluginScopedDisabler m(this, block);
                fluid_synth_program_select(f_synth, cin_channel, f_id, midiprog.data[index].bank, midiprog.data[index].program);
            }
        }

        CarlaPlugin::setMidiProgram(index, sendGui, sendOsc, sendCallback, block);
    }

    // -------------------------------------------------------------------
    // Plugin state

    void reload()
    {
        qDebug("FluidSynthPlugin::reload() - start");

        // Safely disable plugin for reload
        const CarlaPluginScopedDisabler m(this);

        if (x_client->isActive())
            x_client->deactivate();

        // Remove client ports
        removeClientPorts();

        // Delete old data
        deleteBuffers();

        uint32_t aouts, params, j;
        aouts  = 2;
        params = FluidSynthParametersMax;

        aout.ports    = new CarlaEngineAudioPort*[aouts];
        aout.rindexes = new uint32_t[aouts];

        param.data    = new ParameterData[params];
        param.ranges  = new ParameterRanges[params];

        //const int portNameSize = CarlaEngine::maxPortNameSize() - 1;
        char portName[STR_MAX];

        // ---------------------------------------
        // Audio Outputs

#ifndef BUILD_BRIDGE
        if (carlaOptions.process_mode != PROCESS_MODE_MULTIPLE_CLIENTS)
        {
            strcpy(portName, m_name);
            strcat(portName, ":out-left");
        }
        else
#endif
            strcpy(portName, "out-left");

        aout.ports[0]    = (CarlaEngineAudioPort*)x_client->addPort(CarlaEnginePortTypeAudio, portName, false);
        aout.rindexes[0] = 0;

#ifndef BUILD_BRIDGE
        if (carlaOptions.process_mode != PROCESS_MODE_MULTIPLE_CLIENTS)
        {
            strcpy(portName, m_name);
            strcat(portName, ":out-right");
        }
        else
#endif
            strcpy(portName, "out-right");

        aout.ports[1]    = (CarlaEngineAudioPort*)x_client->addPort(CarlaEnginePortTypeAudio, portName, false);
        aout.rindexes[1] = 1;

        // ---------------------------------------
        // MIDI Input

#ifndef BUILD_BRIDGE
        if (carlaOptions.process_mode != PROCESS_MODE_MULTIPLE_CLIENTS)
        {
            strcpy(portName, m_name);
            strcat(portName, ":midi-in");
        }
        else
#endif
            strcpy(portName, "midi-in");

        midi.portMin = (CarlaEngineMidiPort*)x_client->addPort(CarlaEnginePortTypeMIDI, portName, true);

        // ---------------------------------------
        // Parameters

#ifndef BUILD_BRIDGE
        if (carlaOptions.process_mode != PROCESS_MODE_MULTIPLE_CLIENTS)
        {
            strcpy(portName, m_name);
            strcat(portName, ":control-in");
        }
        else
#endif
            strcpy(portName, "control-in");

        param.portCin = (CarlaEngineControlPort*)x_client->addPort(CarlaEnginePortTypeControl, portName, true);

#ifndef BUILD_BRIDGE
        if (carlaOptions.process_mode != PROCESS_MODE_MULTIPLE_CLIENTS)
        {
            strcpy(portName, m_name);
            strcat(portName, ":control-out");
        }
        else
#endif
            strcpy(portName, "control-out");

        param.portCout = (CarlaEngineControlPort*)x_client->addPort(CarlaEnginePortTypeControl, portName, false);

        // ----------------------
        j = FluidSynthReverbOnOff;
        param.data[j].index  = j;
        param.data[j].rindex = j;
        param.data[j].type   = PARAMETER_INPUT;
        param.data[j].hints  = PARAMETER_IS_ENABLED | PARAMETER_IS_AUTOMABLE | PARAMETER_IS_BOOLEAN;
        param.data[j].midiChannel = 0;
        param.data[j].midiCC = -1;
        param.ranges[j].min = 0.0;
        param.ranges[j].max = 1.0;
        param.ranges[j].def = 0.0;
        param.ranges[j].step = 1.0;
        param.ranges[j].stepSmall = 1.0;
        param.ranges[j].stepLarge = 1.0;
        param_buffers[j] = param.ranges[j].def;

        // ----------------------
        j = FluidSynthReverbRoomSize;
        param.data[j].index  = j;
        param.data[j].rindex = j;
        param.data[j].type   = PARAMETER_INPUT;
        param.data[j].hints  = PARAMETER_IS_ENABLED | PARAMETER_IS_AUTOMABLE;
        param.data[j].midiChannel = 0;
        param.data[j].midiCC = -1;
        param.ranges[j].min = 0.0;
        param.ranges[j].max = 1.2;
        param.ranges[j].def = FLUID_REVERB_DEFAULT_ROOMSIZE;
        param.ranges[j].step = 0.01;
        param.ranges[j].stepSmall = 0.0001;
        param.ranges[j].stepLarge = 0.1;
        param_buffers[j] = param.ranges[j].def;

        // ----------------------
        j = FluidSynthReverbDamp;
        param.data[j].index  = j;
        param.data[j].rindex = j;
        param.data[j].type   = PARAMETER_INPUT;
        param.data[j].hints  = PARAMETER_IS_ENABLED | PARAMETER_IS_AUTOMABLE;
        param.data[j].midiChannel = 0;
        param.data[j].midiCC = -1;
        param.ranges[j].min = 0.0;
        param.ranges[j].max = 1.0;
        param.ranges[j].def = FLUID_REVERB_DEFAULT_DAMP;
        param.ranges[j].step = 0.01;
        param.ranges[j].stepSmall = 0.0001;
        param.ranges[j].stepLarge = 0.1;
        param_buffers[j] = param.ranges[j].def;

        // ----------------------
        j = FluidSynthReverbLevel;
        param.data[j].index  = j;
        param.data[j].rindex = j;
        param.data[j].type   = PARAMETER_INPUT;
        param.data[j].hints  = PARAMETER_IS_ENABLED | PARAMETER_IS_AUTOMABLE;
        param.data[j].midiChannel = 0;
        param.data[j].midiCC = MIDI_CONTROL_REVERB_SEND_LEVEL;
        param.ranges[j].min = 0.0;
        param.ranges[j].max = 1.0;
        param.ranges[j].def = FLUID_REVERB_DEFAULT_LEVEL;
        param.ranges[j].step = 0.01;
        param.ranges[j].stepSmall = 0.0001;
        param.ranges[j].stepLarge = 0.1;
        param_buffers[j] = param.ranges[j].def;

        // ----------------------
        j = FluidSynthReverbWidth;
        param.data[j].index  = j;
        param.data[j].rindex = j;
        param.data[j].type   = PARAMETER_INPUT;
        param.data[j].hints  = PARAMETER_IS_ENABLED | PARAMETER_IS_AUTOMABLE;
        param.data[j].midiChannel = 0;
        param.data[j].midiCC = -1;
        param.ranges[j].min = 0.0;
        param.ranges[j].max = 10.0; // should be 100, but that sounds too much
        param.ranges[j].def = FLUID_REVERB_DEFAULT_WIDTH;
        param.ranges[j].step = 0.01;
        param.ranges[j].stepSmall = 0.0001;
        param.ranges[j].stepLarge = 0.1;
        param_buffers[j] = param.ranges[j].def;

        // ----------------------
        j = FluidSynthChorusOnOff;
        param.data[j].index  = j;
        param.data[j].rindex = j;
        param.data[j].type   = PARAMETER_INPUT;
        param.data[j].hints  = PARAMETER_IS_ENABLED | PARAMETER_IS_BOOLEAN;
        param.data[j].midiChannel = 0;
        param.data[j].midiCC = -1;
        param.ranges[j].min = 0.0;
        param.ranges[j].max = 1.0;
        param.ranges[j].def = 0.0;
        param.ranges[j].step = 1.0;
        param.ranges[j].stepSmall = 1.0;
        param.ranges[j].stepLarge = 1.0;
        param_buffers[j] = param.ranges[j].def;

        // ----------------------
        j = FluidSynthChorusNr;
        param.data[j].index  = j;
        param.data[j].rindex = j;
        param.data[j].type   = PARAMETER_INPUT;
        param.data[j].hints  = PARAMETER_IS_ENABLED | PARAMETER_IS_INTEGER;
        param.data[j].midiChannel = 0;
        param.data[j].midiCC = -1;
        param.ranges[j].min = 0.0;
        param.ranges[j].max = 99.0;
        param.ranges[j].def = FLUID_CHORUS_DEFAULT_N;
        param.ranges[j].step = 1.0;
        param.ranges[j].stepSmall = 1.0;
        param.ranges[j].stepLarge = 10.0;
        param_buffers[j] = param.ranges[j].def;

        // ----------------------
        j = FluidSynthChorusLevel;
        param.data[j].index  = j;
        param.data[j].rindex = j;
        param.data[j].type   = PARAMETER_INPUT;
        param.data[j].hints  = PARAMETER_IS_ENABLED;
        param.data[j].midiChannel = 0;
        param.data[j].midiCC = 0; //MIDI_CONTROL_CHORUS_SEND_LEVEL;
        param.ranges[j].min = 0.0;
        param.ranges[j].max = 10.0;
        param.ranges[j].def = FLUID_CHORUS_DEFAULT_LEVEL;
        param.ranges[j].step = 0.01;
        param.ranges[j].stepSmall = 0.0001;
        param.ranges[j].stepLarge = 0.1;
        param_buffers[j] = param.ranges[j].def;

        // ----------------------
        j = FluidSynthChorusSpeedHz;
        param.data[j].index  = j;
        param.data[j].rindex = j;
        param.data[j].type   = PARAMETER_INPUT;
        param.data[j].hints  = PARAMETER_IS_ENABLED;
        param.data[j].midiChannel = 0;
        param.data[j].midiCC = -1;
        param.ranges[j].min = 0.29;
        param.ranges[j].max = 5.0;
        param.ranges[j].def = FLUID_CHORUS_DEFAULT_SPEED;
        param.ranges[j].step = 0.01;
        param.ranges[j].stepSmall = 0.0001;
        param.ranges[j].stepLarge = 0.1;
        param_buffers[j] = param.ranges[j].def;

        // ----------------------
        j = FluidSynthChorusDepthMs;
        param.data[j].index  = j;
        param.data[j].rindex = j;
        param.data[j].type   = PARAMETER_INPUT;
        param.data[j].hints  = PARAMETER_IS_ENABLED;
        param.data[j].midiChannel = 0;
        param.data[j].midiCC = -1;
        param.ranges[j].min = 0.0;
        param.ranges[j].max = 2048000.0 / x_engine->getSampleRate();
        param.ranges[j].def = FLUID_CHORUS_DEFAULT_DEPTH;
        param.ranges[j].step = 0.01;
        param.ranges[j].stepSmall = 0.0001;
        param.ranges[j].stepLarge = 0.1;
        param_buffers[j] = param.ranges[j].def;

        // ----------------------
        j = FluidSynthChorusType;
        param.data[j].index  = j;
        param.data[j].rindex = j;
        param.data[j].type   = PARAMETER_INPUT;
        param.data[j].hints  = PARAMETER_IS_ENABLED | PARAMETER_IS_INTEGER | PARAMETER_USES_SCALEPOINTS;
        param.data[j].midiChannel = 0;
        param.data[j].midiCC = -1;
        param.ranges[j].min = FLUID_CHORUS_MOD_SINE;
        param.ranges[j].max = FLUID_CHORUS_MOD_TRIANGLE;
        param.ranges[j].def = FLUID_CHORUS_DEFAULT_TYPE;
        param.ranges[j].step = 1;
        param.ranges[j].stepSmall = 1;
        param.ranges[j].stepLarge = 1;
        param_buffers[j] = param.ranges[j].def;

        // ----------------------
        j = FluidSynthPolyphony;
        param.data[j].index  = j;
        param.data[j].rindex = j;
        param.data[j].type   = PARAMETER_INPUT;
        param.data[j].hints  = PARAMETER_IS_ENABLED | PARAMETER_IS_INTEGER;
        param.data[j].midiChannel = 0;
        param.data[j].midiCC = -1;
        param.ranges[j].min = 1;
        param.ranges[j].max = 512; // max theoric is 65535
        param.ranges[j].def = fluid_synth_get_polyphony(f_synth);
        param.ranges[j].step = 1;
        param.ranges[j].stepSmall = 1;
        param.ranges[j].stepLarge = 10;
        param_buffers[j] = param.ranges[j].def;

        // ----------------------
        j = FluidSynthInterpolation;
        param.data[j].index  = j;
        param.data[j].rindex = j;
        param.data[j].type   = PARAMETER_INPUT;
        param.data[j].hints  = PARAMETER_IS_ENABLED | PARAMETER_IS_INTEGER | PARAMETER_USES_SCALEPOINTS;
        param.data[j].midiChannel = 0;
        param.data[j].midiCC = -1;
        param.ranges[j].min = FLUID_INTERP_NONE;
        param.ranges[j].max = FLUID_INTERP_HIGHEST;
        param.ranges[j].def = FLUID_INTERP_DEFAULT;
        param.ranges[j].step = 1;
        param.ranges[j].stepSmall = 1;
        param.ranges[j].stepLarge = 1;
        param_buffers[j] = param.ranges[j].def;

        // ----------------------
        j = FluidSynthVoiceCount;
        param.data[j].index  = j;
        param.data[j].rindex = j;
        param.data[j].type   = PARAMETER_OUTPUT;
        param.data[j].hints  = PARAMETER_IS_ENABLED | PARAMETER_IS_AUTOMABLE | PARAMETER_IS_INTEGER;
        param.data[j].midiChannel = 0;
        param.data[j].midiCC = -1;
        param.ranges[j].min = 0;
        param.ranges[j].max = 65535;
        param.ranges[j].def = 0;
        param.ranges[j].step = 1;
        param.ranges[j].stepSmall = 1;
        param.ranges[j].stepLarge = 1;
        param_buffers[j] = param.ranges[j].def;

        // ---------------------------------------

        aout.count  = aouts;
        param.count = params;

        // plugin checks
        m_hints &= ~(PLUGIN_IS_SYNTH | PLUGIN_USES_CHUNKS | PLUGIN_CAN_DRYWET | PLUGIN_CAN_VOLUME | PLUGIN_CAN_BALANCE);

        m_hints |= PLUGIN_IS_SYNTH;
        m_hints |= PLUGIN_CAN_VOLUME;
        m_hints |= PLUGIN_CAN_BALANCE;

        reloadPrograms(true);

        x_client->activate();

        qDebug("FluidSynthPlugin::reload() - end");
    }

    void reloadPrograms(bool init)
    {
        qDebug("FluidSynthPlugin::reloadPrograms(%s)", bool2str(init));

        // Delete old programs
        if (midiprog.count > 0)
        {
            for (uint32_t i=0; i < midiprog.count; i++)
                free((void*)midiprog.data[i].name);

            delete[] midiprog.data;
        }

        midiprog.count = 0;
        midiprog.data  = nullptr;

        // Query new programs
        fluid_sfont_t* f_sfont;
        fluid_preset_t f_preset;

        f_sfont = fluid_synth_get_sfont_by_id(f_synth, f_id);

        // initial check to know how much midi-programs we have
        f_sfont->iteration_start(f_sfont);
        while (f_sfont->iteration_next(f_sfont, &f_preset))
            midiprog.count += 1;

        if (midiprog.count > 0)
            midiprog.data = new midi_program_t [midiprog.count];

        // Update data
        uint32_t i = 0;
        f_sfont->iteration_start(f_sfont);
        while (f_sfont->iteration_next(f_sfont, &f_preset))
        {
            assert(i < midiprog.count);
            midiprog.data[i].bank    = f_preset.get_banknum(&f_preset);
            midiprog.data[i].program = f_preset.get_num(&f_preset);
            midiprog.data[i].name    = strdup(f_preset.get_name(&f_preset));
            i++;
        }

        //f_sfont->free(f_sfont);

#ifndef BUILD_BRIDGE
        // Update OSC Names
        //osc_global_send_set_midi_program_count(m_id, midiprog.count);

        //for (i=0; i < midiprog.count; i++)
        //    osc_global_send_set_midi_program_data(m_id, i, midiprog.data[i].bank, midiprog.data[i].program, midiprog.data[i].name);

        x_engine->callback(CALLBACK_RELOAD_PROGRAMS, m_id, 0, 0, 0.0);
#endif

        if (init)
        {
            fluid_synth_program_reset(f_synth);

            if (midiprog.count > 0)
            {
                for (i=0; i < 16 && i != 9; i++)
                {
                    fluid_synth_program_select(f_synth, i, f_id, midiprog.data[0].bank, midiprog.data[0].program);
#ifdef FLUIDSYNTH_VERSION_NEW_API
                    fluid_synth_set_channel_type(f_synth, i, CHANNEL_TYPE_MELODIC);
#endif
                }

                fluid_synth_program_select(f_synth, 9, f_id, 128, 0);
#ifdef FLUIDSYNTH_VERSION_NEW_API
                fluid_synth_set_channel_type(f_synth, 9, CHANNEL_TYPE_DRUM);
#endif

                setMidiProgram(0, false, false, false, true);
            }
        }
    }

    // -------------------------------------------------------------------
    // Plugin processing

    void process(float**, float** outBuffer, uint32_t frames, uint32_t framesOffset)
    {
        uint32_t i, k;
        uint32_t midiEventCount = 0;

        double aouts_peak_tmp[2] = { 0.0 };

        CARLA_PROCESS_CONTINUE_CHECK;

        // --------------------------------------------------------------------------------------------------------
        // Parameters Input [Automation]

        if (m_active && m_activeBefore)
        {
            bool allNotesOffSent = false;

            const CarlaEngineControlEvent* cinEvent;
            uint32_t time, nEvents = param.portCin->getEventCount();

            unsigned char nextBankIds[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 0, 0, 0, 0, 0, 0 };

            if (midiprog.current >= 0 && midiprog.count > 0 && cin_channel >= 0 && cin_channel < 16)
                nextBankIds[cin_channel] = midiprog.data[midiprog.current].bank;

            for (i=0; i < nEvents; i++)
            {
                cinEvent = param.portCin->getEvent(i);

                if (! cinEvent)
                    continue;

                time = cinEvent->time - framesOffset;

                if (time >= frames)
                    continue;

                // Control change
                switch (cinEvent->type)
                {
                case CarlaEngineEventNull:
                    break;

                case CarlaEngineEventControlChange:
                {
                    double value;

                    // Control backend stuff
                    if (cinEvent->channel == cin_channel)
                    {
                        if (MIDI_IS_CONTROL_BREATH_CONTROLLER(cinEvent->controller) && (m_hints & PLUGIN_CAN_DRYWET) > 0)
                        {
                            value = cinEvent->value;
                            setDryWet(value, false, false);
                            postponeEvent(PluginPostEventParameterChange, PARAMETER_DRYWET, 0, value);
                            continue;
                        }

                        if (MIDI_IS_CONTROL_CHANNEL_VOLUME(cinEvent->controller) && (m_hints & PLUGIN_CAN_VOLUME) > 0)
                        {
                            value = cinEvent->value*127/100;
                            setVolume(value, false, false);
                            postponeEvent(PluginPostEventParameterChange, PARAMETER_VOLUME, 0, value);
                            continue;
                        }

                        if (MIDI_IS_CONTROL_BALANCE(cinEvent->controller) && (m_hints & PLUGIN_CAN_BALANCE) > 0)
                        {
                            double left, right;
                            value = cinEvent->value/0.5 - 1.0;

                            if (value < 0)
                            {
                                left  = -1.0;
                                right = (value*2)+1.0;
                            }
                            else if (value > 0)
                            {
                                left  = (value*2)-1.0;
                                right = 1.0;
                            }
                            else
                            {
                                left  = -1.0;
                                right = 1.0;
                            }

                            setBalanceLeft(left, false, false);
                            setBalanceRight(right, false, false);
                            postponeEvent(PluginPostEventParameterChange, PARAMETER_BALANCE_LEFT, 0, left);
                            postponeEvent(PluginPostEventParameterChange, PARAMETER_BALANCE_RIGHT, 0, right);
                            continue;
                        }
                    }

                    // Control plugin parameters
                    for (k=0; k < param.count; k++)
                    {
                        if (param.data[k].midiChannel != cinEvent->channel)
                            continue;
                        if (param.data[k].midiCC != cinEvent->controller)
                            continue;
                        if (param.data[k].type != PARAMETER_INPUT)
                            continue;

                        if (param.data[k].hints & PARAMETER_IS_AUTOMABLE)
                        {
                            if (param.data[k].hints & PARAMETER_IS_BOOLEAN)
                            {
                                value = cinEvent->value < 0.5 ? param.ranges[k].min : param.ranges[k].max;
                            }
                            else
                            {
                                value = cinEvent->value * (param.ranges[k].max - param.ranges[k].min) + param.ranges[k].min;

                                if (param.data[k].hints & PARAMETER_IS_INTEGER)
                                    value = rint(value);
                            }

                            setParameterValue(k, value, false, false, false);
                            postponeEvent(PluginPostEventParameterChange, k, 0, value);
                        }
                    }

                    break;
                }

                case CarlaEngineEventMidiBankChange:
                    if (cinEvent->channel < 16)
                        nextBankIds[cinEvent->channel] = rint(cinEvent->value);
                    break;

                case CarlaEngineEventMidiProgramChange:
                    if (cinEvent->channel < 16)
                    {
                        uint32_t bankId = nextBankIds[cinEvent->channel];
                        uint32_t progId = rint(cinEvent->value);

                        for (k=0; k < midiprog.count; k++)
                        {
                            if (midiprog.data[k].bank == bankId && midiprog.data[k].program == progId)
                            {
                                if (cinEvent->channel == cin_channel)
                                {
                                    setMidiProgram(k, false, false, false, false);
                                    postponeEvent(PluginPostEventMidiProgramChange, k, 0, 0.0);
                                }
                                else
                                    fluid_synth_program_select(f_synth, cinEvent->channel, f_id, bankId, progId);

                                break;
                            }
                        }
                    }
                    break;

                case CarlaEngineEventAllSoundOff:
                    if (cinEvent->channel == cin_channel)
                    {
                        if (! allNotesOffSent)
                            sendMidiAllNotesOff();

                        allNotesOffSent = true;

#ifdef FLUIDSYNTH_VERSION_NEW_API
                        fluid_synth_all_notes_off(f_synth, cin_channel);
                        fluid_synth_all_sounds_off(f_synth, cin_channel);
                    }
                    else if (cinEvent->channel < 16)
                    {
                        fluid_synth_all_notes_off(f_synth, cinEvent->channel);
                        fluid_synth_all_sounds_off(f_synth, cinEvent->channel);
#endif
                    }
                    break;

                case CarlaEngineEventAllNotesOff:
                    if (cinEvent->channel == cin_channel)
                    {
                        if (! allNotesOffSent)
                            sendMidiAllNotesOff();

                        allNotesOffSent = true;

#ifdef FLUIDSYNTH_VERSION_NEW_API
                        fluid_synth_all_notes_off(f_synth, cin_channel);
                    }
                    else if (cinEvent->channel < 16)
                    {
                        fluid_synth_all_notes_off(f_synth, cinEvent->channel);
#endif
                    }
                    break;
                }
            }
        } // End of Parameters Input

        CARLA_PROCESS_CONTINUE_CHECK;

        // --------------------------------------------------------------------------------------------------------
        // MIDI Input (External)

        if (cin_channel >= 0 && cin_channel < 16 && m_active && m_activeBefore)
        {
            engineMidiLock();

            for (i=0; i < MAX_MIDI_EVENTS && midiEventCount < MAX_MIDI_EVENTS; i++)
            {
                if (extMidiNotes[i].channel < 0)
                    break;

                if (extMidiNotes[i].velo)
                    fluid_synth_noteon(f_synth, cin_channel, extMidiNotes[i].note, extMidiNotes[i].velo);
                else
                    fluid_synth_noteoff(f_synth, cin_channel, extMidiNotes[i].note);

                extMidiNotes[i].channel = -1;
                midiEventCount += 1;
            }

            engineMidiUnlock();

        } // End of MIDI Input (External)

        CARLA_PROCESS_CONTINUE_CHECK;

        // --------------------------------------------------------------------------------------------------------
        // MIDI Input (System)

        if (m_active && m_activeBefore)
        {
            const CarlaEngineMidiEvent* minEvent;
            uint32_t time, nEvents = midi.portMin->getEventCount();

            for (i=0; i < nEvents && midiEventCount < MAX_MIDI_EVENTS; i++)
            {
                minEvent = midi.portMin->getEvent(i);

                if (! minEvent)
                    continue;

                time = minEvent->time - framesOffset;

                if (time >= frames)
                    continue;

                uint8_t status  = minEvent->data[0];
                uint8_t channel = status & 0x0F;

                // Fix bad note-off
                if (MIDI_IS_STATUS_NOTE_ON(status) && minEvent->data[2] == 0)
                    status -= 0x10;

                if (MIDI_IS_STATUS_NOTE_OFF(status))
                {
                    uint8_t note = minEvent->data[1];

                    fluid_synth_noteoff(f_synth, channel, note);

                    if (channel == cin_channel)
                        postponeEvent(PluginPostEventNoteOff, channel, note, 0.0);
                }
                else if (MIDI_IS_STATUS_NOTE_ON(status))
                {
                    uint8_t note = minEvent->data[1];
                    uint8_t velo = minEvent->data[2];

                    fluid_synth_noteon(f_synth, channel, note, velo);

                    if (channel == cin_channel)
                        postponeEvent(PluginPostEventNoteOn, channel, note, velo);
                }
                else if (MIDI_IS_STATUS_AFTERTOUCH(status))
                {
                    uint8_t pressure = minEvent->data[1];

                    fluid_synth_channel_pressure(f_synth, channel, pressure);
                }
                else if (MIDI_IS_STATUS_PITCH_WHEEL_CONTROL(status))
                {
                    uint8_t lsb = minEvent->data[1];
                    uint8_t msb = minEvent->data[2];

                    fluid_synth_pitch_bend(f_synth, channel, (msb << 7) | lsb);
                }
                else
                    continue;

                midiEventCount += 1;
            }
        } // End of MIDI Input (System)

        CARLA_PROCESS_CONTINUE_CHECK;

        // --------------------------------------------------------------------------------------------------------
        // Plugin processing

        if (m_active)
        {
            if (! m_activeBefore)
            {
                if (cin_channel >= 0 && cin_channel < 16)
                {
                    fluid_synth_cc(f_synth, cin_channel, MIDI_CONTROL_ALL_SOUND_OFF, 0);
                    fluid_synth_cc(f_synth, cin_channel, MIDI_CONTROL_ALL_NOTES_OFF, 0);
                }

#ifdef FLUIDSYNTH_VERSION_NEW_API
                for (i=0; i < 16; i++)
                {
                    fluid_synth_all_notes_off(f_synth, i);
                    fluid_synth_all_sounds_off(f_synth, i);
                }
#endif
            }

            fluid_synth_process(f_synth, frames, 0, nullptr, 2, outBuffer);
        }

        CARLA_PROCESS_CONTINUE_CHECK;

        // --------------------------------------------------------------------------------------------------------
        // Post-processing (volume and balance)

        if (m_active)
        {
            bool do_balance = (x_bal_left != -1.0 || x_bal_right != 1.0);

            double bal_rangeL, bal_rangeR;
            float oldBufLeft[do_balance ? frames : 0];

            for (i=0; i < aout.count; i++)
            {
                // Volume, using fluidsynth internals
                fluid_synth_set_gain(f_synth, x_vol);

                // Balance
                if (do_balance)
                {
                    if (i%2 == 0)
                        memcpy(&oldBufLeft, outBuffer[i], sizeof(float)*frames);

                    bal_rangeL = (x_bal_left+1.0)/2;
                    bal_rangeR = (x_bal_right+1.0)/2;

                    for (k=0; k < frames; k++)
                    {
                        if (i%2 == 0)
                        {
                            // left output
                            outBuffer[i][k]  = oldBufLeft[k]*(1.0-bal_rangeL);
                            outBuffer[i][k] += outBuffer[i+1][k]*(1.0-bal_rangeR);
                        }
                        else
                        {
                            // right
                            outBuffer[i][k]  = outBuffer[i][k]*bal_rangeR;
                            outBuffer[i][k] += oldBufLeft[k]*bal_rangeL;
                        }
                    }
                }

                // Output VU
                for (k=0; i < 2 && k < frames; k++)
                {
                    if (abs(outBuffer[i][k]) > aouts_peak_tmp[i])
                        aouts_peak_tmp[i] = abs(outBuffer[i][k]);
                }
            }
        }
        else
        {
            // disable any output sound if not active
            for (i=0; i < aout.count; i++)
                memset(outBuffer[i], 0.0f, sizeof(float)*frames);

            aouts_peak_tmp[0] = 0.0;
            aouts_peak_tmp[1] = 0.0;

        } // End of Post-processing

        CARLA_PROCESS_CONTINUE_CHECK;

        // --------------------------------------------------------------------------------------------------------
        // Control Output

        if (m_active)
        {
            k = FluidSynthVoiceCount;
            param_buffers[k] = fluid_synth_get_active_voice_count(f_synth);
            fixParameterValue(param_buffers[k], param.ranges[k]);

            if (param.data[k].midiCC > 0)
            {
                double value = (param_buffers[k] - param.ranges[k].min) / (param.ranges[k].max - param.ranges[k].min);
                param.portCout->writeEvent(CarlaEngineEventControlChange, framesOffset, param.data[k].midiChannel, param.data[k].midiCC, value);
            }
        } // End of Control Output

        CARLA_PROCESS_CONTINUE_CHECK;

        // --------------------------------------------------------------------------------------------------------
        // Peak Values

        x_engine->setOutputPeak(m_id, 0, aouts_peak_tmp[0]);
        x_engine->setOutputPeak(m_id, 1, aouts_peak_tmp[1]);

        m_activeBefore = m_active;
    }

    // -------------------------------------------------------------------

    bool init(const char* const filename, const char* const name, const char* const label)
    {
        // ---------------------------------------------------------------
        // open soundfont

        f_id = fluid_synth_sfload(f_synth, filename, 0);

        if (f_id < 0)
        {
            setLastError("Failed to load SoundFont file");
            return false;
        }

        // ---------------------------------------------------------------
        // get info

        m_filename = strdup(filename);
        m_label    = strdup(label);

        if (name)
            m_name = x_engine->getUniqueName(name);
        else
            m_name = x_engine->getUniqueName(label);

        // ---------------------------------------------------------------
        // register client

        x_client = x_engine->addClient(this);

        if (! x_client->isOk())
        {
            setLastError("Failed to register plugin client");
            return false;
        }

        return true;
    }

private:
    enum FluidSynthInputParameters {
        FluidSynthReverbOnOff    = 0,
        FluidSynthReverbRoomSize = 1,
        FluidSynthReverbDamp     = 2,
        FluidSynthReverbLevel    = 3,
        FluidSynthReverbWidth    = 4,
        FluidSynthChorusOnOff    = 5,
        FluidSynthChorusNr       = 6,
        FluidSynthChorusLevel    = 7,
        FluidSynthChorusSpeedHz  = 8,
        FluidSynthChorusDepthMs  = 9,
        FluidSynthChorusType     = 10,
        FluidSynthPolyphony      = 11,
        FluidSynthInterpolation  = 12,
        FluidSynthVoiceCount     = 13,
        FluidSynthParametersMax  = 14
    };

    fluid_settings_t* f_settings;
    fluid_synth_t* f_synth;
    int f_id;

    double param_buffers[FluidSynthParametersMax];
    const char* m_label;
};
#endif // WANT_FLUIDSYNTH

CarlaPlugin* CarlaPlugin::newSF2(const initializer& init)
{
    qDebug("CarlaPlugin::newSF2(%p, \"%s\", \"%s\", \"%s\")", init.engine, init.filename, init.name, init.label);

#ifdef WANT_FLUIDSYNTH
    short id = init.engine->getNewPluginId();

    if (id < 0)
    {
        setLastError("Maximum number of plugins reached");
        return nullptr;
    }

    if (! fluid_is_soundfont(init.filename))
    {
        setLastError("Requested file is not a valid SoundFont");
        return nullptr;
    }

    FluidSynthPlugin* const plugin = new FluidSynthPlugin(init.engine, id);

    if (! plugin->init(init.filename, init.name, init.label))
    {
        delete plugin;
        return nullptr;
    }

    plugin->reload();
    plugin->registerToOsc();

    return plugin;
#else
    setLastError("fluidsynth support not available");
    return nullptr;
#endif
}

/**@}*/

CARLA_BACKEND_END_NAMESPACE
