/*
 * DISTHRO Plugin Toolkit (DPT)
 * Copyright (C) 2012 Filipe Coelho <falktx@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * For a full copy of the license see the GPL.txt file
 */

#include "carla_native.hpp"

#include <QtGui/QDialog>

#include "DistrhoPluginMain.cpp"

#if DISTRHO_PLUGIN_HAS_UI
# include "DistrhoUIMain.cpp"
#endif

#ifdef QTCREATOR_TEST
# define DISTRHO_PLUGIN_HAS_UI 1
# define DISTRHO_PLUGIN_IS_SYNTH 1
# define DISTRHO_PLUGIN_WANT_PROGRAMS 1
# define DISTRHO_PLUGIN_WANT_STATE    1
#endif

// -------------------------------------------------

START_NAMESPACE_DISTRHO

#if DISTRHO_PLUGIN_HAS_UI
// -----------------------------------------------------------------------
// Carla UI

class UICarla : public QDialog
{
public:
    UICarla(const HostDescriptor* const host, PluginInternal* const plugin)
        : QDialog(nullptr),
          m_host(host),
          m_plugin(plugin),
          ui(this, realWinId(), setParameterCallback, setStateCallback, uiEditParameterCallback, uiSendNoteCallback, uiResizeCallback)
    {
        setFixedSize(ui.getWidth(), ui.getHeight());
        setWindowTitle("TEST GUI");
    }

    ~UICarla()
    {
    }

    intptr_t realWinId() const
    {
        WId wId = winId();
#if DISTRHO_OS_WINDOWS
        return (intptr_t)static_cast<HWND>(wId);
#else
        return wId;
#endif
    }

    // ---------------------------------------------

    void carla_show(const bool yesNo)
    {
        setVisible(yesNo);
    }

    void carla_idle()
    {
        ui.idle();
    }

    void carla_setParameterValue(const uint32_t index, const float value)
    {
        ui.parameterChanged(index, value);
    }

# if DISTRHO_PLUGIN_WANT_PROGRAMS
    void carla_setMidiProgram(const uint32_t realProgram)
    {
        ui.programChanged(realProgram);
    }
# endif

# if DISTRHO_PLUGIN_WANT_STATE
    void carla_setCustomData(const char* const key, const char* const value)
    {
        ui.stateChanged(key, value);
    }
# endif

    // ---------------------------------------------

protected:
    void setParameterValue(uint32_t index, float value)
    {
        m_host->ui_parameter_changed(m_host->handle, index, value);
    }

# if DISTRHO_PLUGIN_WANT_STATE
    void setState(const char* key, const char* value)
    {
        m_host->ui_custom_data_changed(m_host->handle, key, value);
    }
# endif

    void uiEditParameter(uint32_t, bool)
    {
        // TODO
    }

# if DISTRHO_PLUGIN_IS_SYNTH
    void uiSendNote(bool, uint8_t, uint8_t, uint8_t)
    {
        // TODO
    }
# endif

    void uiResize(unsigned int width, unsigned int height)
    {
        setFixedSize(width, height);
    }

    void closeEvent(QCloseEvent* event)
    {
        m_host->ui_closed(m_host->handle);
        QDialog::closeEvent(event);
    }

private:
    // Plugin stuff
    const HostDescriptor* const m_host;
    PluginInternal* const m_plugin;

    // UI
    UIInternal ui;

    // ---------------------------------------------
    // Callbacks

    static void setParameterCallback(void* ptr, uint32_t rindex, float value)
    {
        UICarla* _this_ = (UICarla*)ptr;
        CARLA_ASSERT(_this_);

        _this_->setParameterValue(rindex, value);
    }

    static void setStateCallback(void* ptr, const char* key, const char* value)
    {
# if DISTRHO_PLUGIN_WANT_STATE
        UICarla* _this_ = (UICarla*)ptr;
        CARLA_ASSERT(_this_);

        _this_->setState(key, value);
# else
        Q_UNUSED(ptr);
        Q_UNUSED(key);
        Q_UNUSED(value);
# endif
    }

    static void uiEditParameterCallback(void* ptr, uint32_t index, bool started)
    {
        UICarla* _this_ = (UICarla*)ptr;
        CARLA_ASSERT(_this_);

        _this_->uiEditParameter(index, started);
    }

    static void uiSendNoteCallback(void* ptr, bool onOff, uint8_t channel, uint8_t note, uint8_t velocity)
    {
# if DISTRHO_PLUGIN_IS_SYNTH
        UICarla* _this_ = (UICarla*)ptr;
        CARLA_ASSERT(_this_);

        _this_->uiSendNote(onOff, channel, note, velocity);
# else
        Q_UNUSED(ptr);
        Q_UNUSED(onOff);
        Q_UNUSED(channel);
        Q_UNUSED(note);
        Q_UNUSED(velocity);
# endif
    }

    static void uiResizeCallback(void* ptr, unsigned int width, unsigned int height)
    {
        UICarla* _this_ = (UICarla*)ptr;
        CARLA_ASSERT(_this_);

        _this_->uiResize(width, height);
    }
};
#endif

// -----------------------------------------------------------------------
// Carla Plugin

class PluginCarla : public PluginDescriptorClass
{
public:
    PluginCarla(const HostDescriptor* const host)
        : PluginDescriptorClass(host)
    {
#if DISTRHO_PLUGIN_HAS_UI
        uiPtr = nullptr;
#endif
    }

    ~PluginCarla()
    {
#if DISTRHO_PLUGIN_HAS_UI
        if (uiPtr)
            delete uiPtr;
#endif
    }

protected:
    // -------------------------------------------------------------------
    // Plugin parameter calls

    uint32_t getParameterCount()
    {
        return plugin.parameterCount();
    }

    const ::Parameter* getParameterInfo(const uint32_t index)
    {
        static ::Parameter param;

        // reset
        param.hints = ::PARAMETER_IS_ENABLED;

        {
            const uint32_t paramHints = plugin.parameterHints(index);

            if (paramHints & PARAMETER_IS_AUTOMABLE)
                param.hints |= ::PARAMETER_IS_AUTOMABLE;
            if (paramHints & PARAMETER_IS_BOOLEAN)
                param.hints |= ::PARAMETER_IS_BOOLEAN;
            if (paramHints & PARAMETER_IS_INTEGER)
                param.hints |= ::PARAMETER_IS_INTEGER;
            if (paramHints & PARAMETER_IS_LOGARITHMIC)
                param.hints |= ::PARAMETER_IS_LOGARITHMIC;
            if (paramHints & PARAMETER_IS_OUTPUT)
                param.hints |= ::PARAMETER_IS_OUTPUT;
        }

        param.name  = plugin.parameterName(index);
        param.unit  = plugin.parameterUnit(index);

        {
            const ParameterRanges* const ranges(plugin.parameterRanges(index));

            param.ranges.def = ranges->def;
            param.ranges.min = ranges->min;
            param.ranges.max = ranges->max;
            param.ranges.step = ranges->step;
            param.ranges.stepSmall = ranges->stepSmall;
            param.ranges.stepLarge = ranges->stepLarge;
        }

        param.scalePointCount = 0;
        param.scalePoints = nullptr;

        return &param;
    }

    float getParameterValue(const uint32_t index)
    {
        return plugin.parameterValue(index);
    }

    // -------------------------------------------------------------------
    // Plugin midi-program calls

#if DISTRHO_PLUGIN_WANT_PROGRAMS
    virtual uint32_t getMidiProgramCount()
    {
        return plugin.programCount();
    }

    virtual const ::MidiProgram* getMidiProgramInfo(const uint32_t index)
    {
        static ::MidiProgram midiProgram;
        midiProgram.bank    = index / 128;
        midiProgram.program = index % 128;
        midiProgram.name    = plugin.programName(index);
        return &midiProgram;
    }
#endif

    // -------------------------------------------------------------------
    // Plugin state calls

    void setParameterValue(const uint32_t index, const float value)
    {
        plugin.setParameterValue(index, value);
    }

#if DISTRHO_PLUGIN_WANT_PROGRAMS
    void setMidiProgram(const uint32_t bank, const uint32_t program)
    {
        const uint32_t realProgram = bank * 128 + program;

        if (realProgram >= plugin.programCount())
            return;

        plugin.setProgram(realProgram);
    }
#endif

#if DISTRHO_PLUGIN_WANT_STATE
    void setCustomData(const char* const key, const char* const value)
    {
        plugin.setState(key, value);
    }
#endif

    // -------------------------------------------------------------------
    // Plugin UI calls

#if DISTRHO_PLUGIN_HAS_UI
    void uiShow(const bool show)
    {
        if (show)
            createUiIfNeeded();

        if (uiPtr)
            uiPtr->carla_show(show);
    }

    void uiIdle()
    {
        if (uiPtr)
            uiPtr->carla_idle();
    }

    void uiSetParameterValue(const uint32_t index, const float value)
    {
        if (uiPtr)
            uiPtr->carla_setParameterValue(index, value);
    }

# if DISTRHO_PLUGIN_WANT_PROGRAMS
    void uiSetMidiProgram(const uint32_t bank, const uint32_t program)
    {
        uint32_t realProgram = bank * 128 + program;

        if (uiPtr)
            uiPtr->carla_setMidiProgram(realProgram);
    }
# endif

# if DISTRHO_PLUGIN_WANT_STATE
    void uiSetCustomData(const char* const key, const char* const value)
    {
        if (uiPtr)
            uiPtr->carla_setCustomData(key, value);
    }
# endif
#endif

    // -------------------------------------------------------------------
    // Plugin process calls

    void activate()
    {
        plugin.activate();
    }

    void deactivate()
    {
        plugin.deactivate();
    }

#if DISTRHO_PLUGIN_IS_SYNTH
    void process(float** const inBuffer, float** const outBuffer, const uint32_t frames, const uint32_t midiEventCount, const ::MidiEvent* const midiEvents)
    {
        for (uint32_t i=0; i < midiEventCount && i < MAX_MIDI_EVENTS; i++)
        {
            const ::MidiEvent* midiEvent = &midiEvents[i];
            MidiEvent* realEvent = &realMidiEvents[i];

            realEvent->buffer[0] = midiEvent->data[0];
            realEvent->buffer[1] = midiEvent->data[1];
            realEvent->buffer[2] = midiEvent->data[2];
            realEvent->frame     = midiEvent->time;
        }

        plugin.run(inBuffer, outBuffer, frames, midiEventCount, realMidiEvents);
    }
#else
    void process(float** const inBuffer, float** const outBuffer, const uint32_t frames, const uint32_t, const ::MidiEvent* const)
    {
        plugin.run(inBuffer, outBuffer, frames, 0, nullptr);
    }
#endif

    // -------------------------------------------------------------------

private:
    PluginInternal plugin;

#if DISTRHO_PLUGIN_IS_SYNTH
    MidiEvent realMidiEvents[MAX_MIDI_EVENTS];
#endif

#if DISTRHO_PLUGIN_HAS_UI
    // UI
    UICarla* uiPtr;

    void createUiIfNeeded()
    {
        if (! uiPtr)
        {
            setLastUiSampleRate(getSampleRate());
            uiPtr = new UICarla(getHostHandle(), &plugin);
        }
    }
#endif

    // -------------------------------------------------------------------

public:
    static PluginHandle _instantiate(const PluginDescriptor*, HostDescriptor* host)
    {
        d_lastBufferSize = host->get_buffer_size(host->handle);
        d_lastSampleRate = host->get_sample_rate(host->handle);
        return new PluginCarla(host);
    }

    static void _cleanup(PluginHandle handle)
    {
        delete (PluginCarla*)handle;
    }
};

END_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------
