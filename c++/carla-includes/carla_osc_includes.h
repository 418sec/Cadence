/*
 * Carla common OSC code
 * Copyright (C) 2012 Filipe Coelho <falktx@gmail.com>
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

#ifndef CARLA_OSC_INCLUDES_H
#define CARLA_OSC_INCLUDES_H

#include "carla_includes.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <lo/lo.h>

struct CarlaOscData {
    const char* path;
    lo_address source;
    lo_address target;
};

static inline
void osc_clear_data(CarlaOscData* const oscData)
{
    Q_ASSERT(oscData);
    qDebug("osc_clear_data(path:\"%s\")", oscData->path);

    if (oscData->path)
        free((void*)oscData->path);

    if (oscData->source)
        lo_address_free(oscData->source);

    if (oscData->target)
        lo_address_free(oscData->target);

    oscData->path   = nullptr;
    oscData->source = nullptr;
    oscData->target = nullptr;
}

static inline
void osc_send_configure(const CarlaOscData* const oscData, const char* const key, const char* const value)
{
    Q_ASSERT(oscData && oscData->path);
    Q_ASSERT(key);
    Q_ASSERT(value);
    qDebug("osc_send_configure(path:\"%s\", \"%s\", \"%s\")", oscData->path, key, value);

    if (oscData->target)
    {
        char targetPath[strlen(oscData->path)+11];
        strcpy(targetPath, oscData->path);
        strcat(targetPath, "/configure");
        lo_send(oscData->target, targetPath, "ss", key, value);
    }
}

static inline
void osc_send_control(const CarlaOscData* const oscData, const int32_t index, const float value)
{
    Q_ASSERT(oscData && oscData->path);
    Q_ASSERT(index != -1);
    qDebug("osc_send_control(path:\"%s\", %i, %f)", oscData->path, index, value);

    if (oscData->target)
    {
        char targetPath[strlen(oscData->path)+9];
        strcpy(targetPath, oscData->path);
        strcat(targetPath, "/control");
        lo_send(oscData->target, targetPath, "if", index, value);
    }
}

static inline
void osc_send_program(const CarlaOscData* const oscData, const int32_t index)
{
    Q_ASSERT(oscData && oscData->path);
    Q_ASSERT(index >= 0);
    qDebug("osc_send_program(path:\"%s\", %i)", oscData->path, index);

    if (oscData->target)
    {
        char targetPath[strlen(oscData->path)+9];
        strcpy(targetPath, oscData->path);
        strcat(targetPath, "/program");
        lo_send(oscData->target, targetPath, "i", index);
    }
}

static inline
void osc_send_program(const CarlaOscData* const oscData, const int32_t bank, const int32_t program)
{
    Q_ASSERT(oscData && oscData->path);
    Q_ASSERT(program >= 0 && program < 128);
    Q_ASSERT(bank >= 0);
    qDebug("osc_send_program(path:\"%s\", %i, %i)", oscData->path, bank, program);

    if (oscData->target)
    {
        char targetPath[strlen(oscData->path)+9];
        strcpy(targetPath, oscData->path);
        strcat(targetPath, "/program");
        lo_send(oscData->target, targetPath, "ii", bank, program);
    }
}

static inline
void osc_send_midi_program(const CarlaOscData* const oscData, const int32_t index)
{
    Q_ASSERT(oscData && oscData->path);
    Q_ASSERT(index >= 0);
    qDebug("osc_send_midi_program(path:\"%s\", %i)", oscData->path, index);

    if (oscData->target)
    {
        char targetPath[strlen(oscData->path)+14];
        strcpy(targetPath, oscData->path);
        strcat(targetPath, "/midi_program");
        lo_send(oscData->target, targetPath, "i", index);
    }
}

static inline
void osc_send_midi(const CarlaOscData* const oscData, const uint8_t buf[4])
{
    Q_ASSERT(oscData && oscData->path);
    Q_ASSERT(buf[0] == 0);
    Q_ASSERT(buf[1] != 0);
    qDebug("osc_send_midi(path:\"%s\", 0x%X, %03u, %03u)", oscData->path, buf[1], buf[2], buf[3]);

    if (oscData->target)
    {
        char targetPath[strlen(oscData->path)+6];
        strcpy(targetPath, oscData->path);
        strcat(targetPath, "/midi");
        lo_send(oscData->target, targetPath, "m", buf);
    }
}

static inline
void osc_send_sample_rate(const CarlaOscData* const oscData, const float sampleRate)
{
    Q_ASSERT(oscData && oscData->path);
    Q_ASSERT(sampleRate > 0.0f);
    qDebug("osc_send_sample_rate(path:\"%s\", %f)", oscData->path, sampleRate);

    if (oscData->target)
    {
        char targetPath[strlen(oscData->path)+12];
        strcpy(targetPath, oscData->path);
        strcat(targetPath, "/sample_rate");
        lo_send(oscData->target, targetPath, "f", sampleRate);
    }
}

#ifdef BUILD_BRIDGE
static inline
void osc_send_update(const CarlaOscData* const oscData, const char* const url)
{
    Q_ASSERT(oscData && oscData->path);
    Q_ASSERT(url);
    qDebug("osc_send_update(path:\"%s\", \"%s\")", oscData->path, url);

    if (oscData->target)
    {
        char targetPath[strlen(oscData->path)+8];
        strcpy(targetPath, oscData->path);
        strcat(targetPath, "/update");
        lo_send(oscData->target, targetPath, "s", url);
    }
}

static inline
void osc_send_exiting(const CarlaOscData* const oscData)
{
    Q_ASSERT(oscData && oscData->path);
    qDebug("osc_send_exiting(path:\"%s\")", oscData->path);

    if (oscData->target)
    {
        char targetPath[strlen(oscData->path)+9];
        strcpy(targetPath, oscData->path);
        strcat(targetPath, "/exiting");
        lo_send(oscData->target, targetPath, "");
    }
}

#else
static inline
void osc_send_show(const CarlaOscData* const oscData)
{
    Q_ASSERT(oscData && oscData->path);
    qDebug("osc_send_show(path:\"%s\")", oscData->path);

    if (oscData->target)
    {
        char targetPath[strlen(oscData->path)+6];
        strcpy(targetPath, oscData->path);
        strcat(targetPath, "/show");
        lo_send(oscData->target, targetPath, "");
    }
}

static inline
void osc_send_hide(const CarlaOscData* const oscData)
{
    Q_ASSERT(oscData && oscData->path);
    qDebug("osc_send_hide(path:\"%s\")", oscData->path);

    if (oscData->target)
    {
        char targetPath[strlen(oscData->path)+6];
        strcpy(targetPath, oscData->path);
        strcat(targetPath, "/hide");
        lo_send(oscData->target, targetPath, "");
    }
}

static inline
void osc_send_quit(const CarlaOscData* const oscData)
{
    Q_ASSERT(oscData && oscData->path);
    qDebug("osc_send_quit(path:\"%s\")", oscData->path);

    if (oscData->target)
    {
        char targetPath[strlen(oscData->path)+6];
        strcpy(targetPath, oscData->path);
        strcat(targetPath, "/quit");
        lo_send(oscData->target, targetPath, "");
    }
}
#endif

#ifdef BUILD_BRIDGE_PLUGIN
static inline
void osc_send_bridge_update(const CarlaOscData* const oscData, const char* const url)
{
    Q_ASSERT(oscData && oscData->path);
    Q_ASSERT(url);
    qDebug("osc_send_bridge_update(path:\"%s\", \"%s\")", oscData->path, url);

    if (oscData->target)
    {
        char targetPath[strlen(oscData->path)+15];
        strcpy(targetPath, oscData->path);
        strcat(targetPath, "/bridge_update");
        lo_send(oscData->target, targetPath, "s", url);
    }
}
#endif

static inline
void osc_send_lv2_transfer_atom(const CarlaOscData* const oscData, const char* const type, const char* const value)
{
    Q_ASSERT(oscData && oscData->path);
    Q_ASSERT(type);
    Q_ASSERT(value);
    qDebug("osc_send_lv2_transfer_atom(path:\"%s\", \"%s\", \"%s\")", oscData->path, type, value);

    if (oscData->target)
    {
        char targetPath[strlen(oscData->path)+19];
        strcpy(targetPath, oscData->path);
        strcat(targetPath, "/lv2_atom_transfer");
        lo_send(oscData->target, targetPath, "ss", type, value);
    }
}

static inline
void osc_send_lv2_transfer_event(const CarlaOscData* const oscData, const char* const type, const char* const value)
{
    Q_ASSERT(oscData && oscData->path);
    Q_ASSERT(type);
    Q_ASSERT(value);
    qDebug("osc_send_lv2_transfer_event(path:\"%s\", \"%s\", \"%s\")", oscData->path, type, value);

    if (oscData->target)
    {
        char targetPath[strlen(oscData->path)+20];
        strcpy(targetPath, oscData->path);
        strcat(targetPath, "/lv2_event_transfer");
        lo_send(oscData->target, targetPath, "ss", type, value);
    }
}

#endif // CARLA_OSC_INCLUDES_H
