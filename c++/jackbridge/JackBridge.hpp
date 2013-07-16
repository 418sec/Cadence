/*
 * JackBridge
 * Copyright (C) 2013 Filipe Coelho <falktx@falktx.com>
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

#ifndef __JACKBRIDGE_HPP__
#define __JACKBRIDGE_HPP__

#include "CarlaDefines.hpp"

#ifndef JACKBRIDGE_EXPORT
# undef CARLA_EXPORT
# define CARLA_EXPORT
#endif

#ifdef JACKBRIDGE_DIRECT
# include <jack/jack.h>
# include <jack/midiport.h>
# include <jack/transport.h>
# include <jack/custom.h>
#else

#include <cstddef>

#ifdef CARLA_PROPER_CPP11_SUPPORT
# include <cstdint>
#else
# include <stdint.h>
#endif

#ifndef POST_PACKED_STRUCTURE
# ifdef __GNUC__
  /* POST_PACKED_STRUCTURE needs to be a macro which
      expands into a compiler directive. The directive must
      tell the compiler to arrange the preceding structure
      declaration so that it is packed on byte-boundaries rather
      than use the natural alignment of the processor and/or
      compiler.
  */
  #define PRE_PACKED_STRUCTURE
  #define POST_PACKED_STRUCTURE __attribute__((__packed__))
# else
  #ifdef _MSC_VER
    #define PRE_PACKED_STRUCTURE1 __pragma(pack(push,1))
    #define PRE_PACKED_STRUCTURE    PRE_PACKED_STRUCTURE1
    /* PRE_PACKED_STRUCTURE needs to be a macro which
    expands into a compiler directive. The directive must
    tell the compiler to arrange the following structure
    declaration so that it is packed on byte-boundaries rather
    than use the natural alignment of the processor and/or
    compiler.
    */
    #define POST_PACKED_STRUCTURE ;__pragma(pack(pop))
    /* and POST_PACKED_STRUCTURE needs to be a macro which
    restores the packing to its previous setting */
  #else
    #define PRE_PACKED_STRUCTURE
    #define POST_PACKED_STRUCTURE
  #endif // _MSC_VER
# endif // __GNUC__
#endif

#define JACK_DEFAULT_AUDIO_TYPE "32 bit float mono audio"
#define JACK_DEFAULT_MIDI_TYPE  "8 bit raw midi"

#define JACK_HAS_CUSTOM_DATA_API 1
#define JACK_HAS_PORT_IS_CONTROL_VOLTAGE_FLAG 1

#define JackOpenOptions (JackSessionID|JackServerName|JackNoStartServer|JackUseExactName)
#define JackLoadOptions (JackLoadInit|JackLoadName|JackUseExactName)
#define JACK_POSITION_MASK (JackPositionBBT|JackPositionTimecode)
#define EXTENDED_TIME_INFO

enum JackOptions {
    JackNullOption    = 0x00,
    JackNoStartServer = 0x01,
    JackUseExactName  = 0x02,
    JackServerName    = 0x04,
    JackLoadName      = 0x08,
    JackLoadInit      = 0x10,
    JackSessionID     = 0x20
};

enum JackStatus {
    JackFailure       = 0x01,
    JackInvalidOption = 0x02,
    JackNameNotUnique = 0x04,
    JackServerStarted = 0x08,
    JackServerFailed  = 0x10,
    JackServerError   = 0x20,
    JackNoSuchClient  = 0x40,
    JackLoadFailure   = 0x80,
    JackInitFailure   = 0x100,
    JackShmFailure    = 0x200,
    JackVersionError  = 0x400,
    JackBackendError  = 0x800,
    JackClientZombie  = 0x1000
};

enum JackLatencyCallbackMode {
    JackCaptureLatency,
    JackPlaybackLatency
};

enum JackPortFlags {
    JackPortIsInput    = 0x1,
    JackPortIsOutput   = 0x2,
    JackPortIsPhysical = 0x4,
    JackPortCanMonitor = 0x8,
    JackPortIsTerminal = 0x10,
    JackPortIsControlVoltage = 0x100
};

enum JackTransportState {
    JackTransportStopped     = 0,
    JackTransportRolling     = 1,
    JackTransportLooping     = 2,
    JackTransportStarting    = 3,
    JackTransportNetStarting = 4
};

enum JackPositionBits {
    JackPositionBBT      = 0x010,
    JackPositionTimecode = 0x020,
    JackBBTFrameOffset   = 0x040,
    JackAudioVideoRatio  = 0x080,
    JackVideoFrameOffset = 0x100
};

enum JackSessionEventType {
    JackSessionSave         = 1,
    JackSessionSaveAndQuit  = 2,
    JackSessionSaveTemplate = 3
};

enum JackSessionFlags {
    JackSessionSaveError    = 0x01,
    JackSessionNeedTerminal = 0x02
};

enum JackCustomChange {
    JackCustomRemoved,
    JackCustomAdded,
    JackCustomReplaced
};

typedef uint32_t jack_nframes_t;
typedef uint32_t jack_port_id_t;
typedef uint64_t jack_time_t;
typedef uint64_t jack_unique_t;
typedef unsigned char jack_midi_data_t;
typedef float jack_default_audio_sample_t;

typedef enum JackOptions jack_options_t;
typedef enum JackStatus jack_status_t;
typedef enum JackLatencyCallbackMode jack_latency_callback_mode_t;
typedef enum JackTransportState jack_transport_state_t;
typedef enum JackPositionBits jack_position_bits_t;
typedef enum JackSessionEventType jack_session_event_type_t;
typedef enum JackSessionFlags jack_session_flags_t;
typedef enum JackCustomChange jack_custom_change_t;

struct _jack_midi_event {
    jack_nframes_t    time;
    size_t            size;
    jack_midi_data_t* buffer;
};

PRE_PACKED_STRUCTURE
struct _jack_latency_range {
    jack_nframes_t min;
    jack_nframes_t max;
} POST_PACKED_STRUCTURE;

PRE_PACKED_STRUCTURE
struct _jack_position {
    jack_unique_t  unique_1;
    jack_time_t    usecs;
    jack_nframes_t frame_rate;
    jack_nframes_t frame;
    jack_position_bits_t valid;
    int32_t bar;
    int32_t beat;
    int32_t tick;
    double  bar_start_tick;
    float   beats_per_bar;
    float   beat_type;
    double  ticks_per_beat;
    double  beats_per_minute;
    double  frame_time;
    double  next_time;
    jack_nframes_t bbt_offset;
    float          audio_frames_per_video_frame;
    jack_nframes_t video_offset;
    int32_t        padding[7];
    jack_unique_t  unique_2;
} POST_PACKED_STRUCTURE;

struct _jack_session_event {
    jack_session_event_type_t type;
    const char* session_dir;
    const char* client_uuid;
    char*       command_line;
    jack_session_flags_t flags;
    uint32_t future;
};

struct _jack_session_command_t {
    const char* uuid;
    const char* client_name;
    const char* command;
    jack_session_flags_t flags;
};

typedef struct _jack_port jack_port_t;
typedef struct _jack_client jack_client_t;
typedef struct _jack_midi_event jack_midi_event_t;
typedef struct _jack_latency_range jack_latency_range_t;
typedef struct _jack_position jack_position_t;
typedef struct _jack_session_event jack_session_event_t;
typedef struct _jack_session_command_t jack_session_command_t;

typedef void (*JackLatencyCallback)(jack_latency_callback_mode_t mode, void* arg);
typedef int  (*JackProcessCallback)(jack_nframes_t nframes, void* arg);
typedef void (*JackThreadInitCallback)(void *arg);
typedef int  (*JackGraphOrderCallback)(void *arg);
typedef int  (*JackXRunCallback)(void* arg);
typedef int  (*JackBufferSizeCallback)(jack_nframes_t nframes, void* arg);
typedef int  (*JackSampleRateCallback)(jack_nframes_t nframes, void* arg);
typedef void (*JackPortRegistrationCallback)(jack_port_id_t port, int register_, void* arg);
typedef void (*JackClientRegistrationCallback)(const char* name, int register_, void* arg);
typedef int  (*JackClientRenameCallback)(const char* old_name, const char* new_name, void* arg);
typedef void (*JackPortConnectCallback)(jack_port_id_t a, jack_port_id_t b, int connect, void* arg);
typedef int  (*JackPortRenameCallback)(jack_port_id_t port, const char* old_name, const char* new_name, void* arg);
typedef void (*JackFreewheelCallback)(int starting, void* arg);
typedef void (*JackShutdownCallback)(void* arg);
typedef void (*JackInfoShutdownCallback)(jack_status_t code, const char* reason, void* arg);
typedef int  (*JackSyncCallback)(jack_transport_state_t state, jack_position_t* pos, void* arg);
typedef void (*JackTimebaseCallback)(jack_transport_state_t state, jack_nframes_t nframes, jack_position_t* pos, int new_pos, void* arg);
typedef void (*JackSessionCallback)(jack_session_event_t* event, void* arg);
typedef void (*JackCustomDataAppearanceCallback)(const char* client_name, const char* key, jack_custom_change_t change, void* arg);

#endif // ! JACKBRIDGE_DIRECT

CARLA_EXPORT const char*    jackbridge_get_version_string();
CARLA_EXPORT jack_client_t* jackbridge_client_open(const char* client_name, jack_options_t options, jack_status_t* status, ...);
CARLA_EXPORT const char*    jackbridge_client_rename(jack_client_t* client, const char* new_name);

CARLA_EXPORT bool  jackbridge_client_close(jack_client_t* client);
CARLA_EXPORT int   jackbridge_client_name_size();
CARLA_EXPORT char* jackbridge_get_client_name(jack_client_t* client);

CARLA_EXPORT bool jackbridge_activate(jack_client_t* client);
CARLA_EXPORT bool jackbridge_deactivate(jack_client_t* client);

CARLA_EXPORT int jackbridge_get_client_pid(const char* name);
CARLA_EXPORT int jackbridge_is_realtime(jack_client_t* client);

CARLA_EXPORT bool jackbridge_set_thread_init_callback(jack_client_t* client, JackThreadInitCallback thread_init_callback, void* arg);
CARLA_EXPORT void jackbridge_on_shutdown(jack_client_t* client, JackShutdownCallback shutdown_callback, void* arg);
CARLA_EXPORT void jackbridge_on_info_shutdown(jack_client_t* client, JackInfoShutdownCallback shutdown_callback, void* arg);
CARLA_EXPORT bool jackbridge_set_process_callback(jack_client_t* client, JackProcessCallback process_callback, void* arg);
CARLA_EXPORT bool jackbridge_set_freewheel_callback(jack_client_t* client, JackFreewheelCallback freewheel_callback, void* arg);
CARLA_EXPORT bool jackbridge_set_buffer_size_callback(jack_client_t* client, JackBufferSizeCallback bufsize_callback, void* arg);
CARLA_EXPORT bool jackbridge_set_sample_rate_callback(jack_client_t* client, JackSampleRateCallback srate_callback, void* arg);
CARLA_EXPORT bool jackbridge_set_client_registration_callback(jack_client_t* client, JackClientRegistrationCallback registration_callback, void* arg);
#ifndef JACKBRIDGE_DIRECT
CARLA_EXPORT bool jackbridge_set_client_rename_callback(jack_client_t* client, JackClientRenameCallback registration_callback, void* arg);
#endif
CARLA_EXPORT bool jackbridge_set_port_registration_callback(jack_client_t* client, JackPortRegistrationCallback registration_callback, void* arg);
CARLA_EXPORT bool jackbridge_set_port_connect_callback(jack_client_t* client, JackPortConnectCallback connect_callback, void* arg);
CARLA_EXPORT bool jackbridge_set_port_rename_callback(jack_client_t* client, JackPortRenameCallback rename_callback, void* arg);
CARLA_EXPORT bool jackbridge_set_graph_order_callback(jack_client_t* client, JackGraphOrderCallback graph_callback, void* arg);
CARLA_EXPORT bool jackbridge_set_xrun_callback(jack_client_t* client, JackXRunCallback xrun_callback, void* arg);
CARLA_EXPORT bool jackbridge_set_latency_callback(jack_client_t* client, JackLatencyCallback latency_callback, void* arg);

CARLA_EXPORT bool  jackbridge_set_freewheel(jack_client_t* client, int onoff);
CARLA_EXPORT bool  jackbridge_set_buffer_size(jack_client_t* client, jack_nframes_t nframes);
CARLA_EXPORT bool  jackbridge_engine_takeover_timebase(jack_client_t* client);
CARLA_EXPORT float jackbridge_cpu_load(jack_client_t* client);

CARLA_EXPORT jack_nframes_t jackbridge_get_sample_rate(jack_client_t* client);
CARLA_EXPORT jack_nframes_t jackbridge_get_buffer_size(jack_client_t* client);

CARLA_EXPORT jack_port_t*   jackbridge_port_register(jack_client_t* client, const char* port_name, const char* port_type, unsigned long flags, unsigned long buffer_size);

CARLA_EXPORT bool  jackbridge_port_unregister(jack_client_t* client, jack_port_t* port);
CARLA_EXPORT void* jackbridge_port_get_buffer(jack_port_t* port, jack_nframes_t nframes);

CARLA_EXPORT const char*  jackbridge_port_name(const jack_port_t* port);
CARLA_EXPORT const char*  jackbridge_port_short_name(const jack_port_t* port);
CARLA_EXPORT int          jackbridge_port_flags(const jack_port_t* port);
CARLA_EXPORT const char*  jackbridge_port_type(const jack_port_t* port);
CARLA_EXPORT bool         jackbridge_port_is_mine(const jack_client_t* client, const jack_port_t* port);
CARLA_EXPORT bool         jackbridge_port_connected(const jack_port_t* port);
CARLA_EXPORT bool         jackbridge_port_connected_to(const jack_port_t* port, const char* port_name);
CARLA_EXPORT const char** jackbridge_port_get_connections(const jack_port_t* port);
CARLA_EXPORT const char** jackbridge_port_get_all_connections(const jack_client_t* client, const jack_port_t* port);

CARLA_EXPORT bool jackbridge_port_set_name(jack_port_t* port, const char* port_name);
CARLA_EXPORT bool jackbridge_connect(jack_client_t* client, const char* source_port, const char* destination_port);
CARLA_EXPORT bool jackbridge_disconnect(jack_client_t* client, const char* source_port, const char* destination_port);
CARLA_EXPORT int  jackbridge_port_name_size();
CARLA_EXPORT void jackbridge_port_get_latency_range(jack_port_t* port, jack_latency_callback_mode_t mode, jack_latency_range_t* range);
CARLA_EXPORT void jackbridge_port_set_latency_range(jack_port_t* port, jack_latency_callback_mode_t mode, jack_latency_range_t* range);
CARLA_EXPORT bool jackbridge_recompute_total_latencies(jack_client_t* client);

CARLA_EXPORT const char** jackbridge_get_ports(jack_client_t* client, const char* port_name_pattern, const char* type_name_pattern, unsigned long flags);
CARLA_EXPORT jack_port_t* jackbridge_port_by_name(jack_client_t* client, const char* port_name);
CARLA_EXPORT jack_port_t* jackbridge_port_by_id(jack_client_t* client, jack_port_id_t port_id);

CARLA_EXPORT void jackbridge_free(void* ptr);

CARLA_EXPORT uint32_t jackbridge_midi_get_event_count(void* port_buffer);
CARLA_EXPORT bool     jackbridge_midi_event_get(jack_midi_event_t* event, void* port_buffer, uint32_t event_index);
CARLA_EXPORT void     jackbridge_midi_clear_buffer(void* port_buffer);
CARLA_EXPORT bool     jackbridge_midi_event_write(void* port_buffer, jack_nframes_t time, const jack_midi_data_t* data, size_t data_size);
CARLA_EXPORT jack_midi_data_t* jackbridge_midi_event_reserve(void* port_buffer, jack_nframes_t time, size_t data_size);

CARLA_EXPORT int  jackbridge_transport_locate(jack_client_t* client, jack_nframes_t frame);
CARLA_EXPORT void jackbridge_transport_start(jack_client_t* client);
CARLA_EXPORT void jackbridge_transport_stop(jack_client_t* client);
CARLA_EXPORT jack_transport_state_t jackbridge_transport_query(const jack_client_t* client, jack_position_t* pos);

CARLA_EXPORT bool jackbridge_custom_publish_data(jack_client_t* client, const char* key, const void* data, size_t size);
CARLA_EXPORT bool jackbridge_custom_get_data(jack_client_t* client, const char* client_name, const char* key, void** data, size_t* size);
CARLA_EXPORT bool jackbridge_custom_unpublish_data(jack_client_t* client, const char* key);
CARLA_EXPORT bool jackbridge_custom_set_data_appearance_callback(jack_client_t* client, JackCustomDataAppearanceCallback callback, void* arg);
CARLA_EXPORT const char** jackbridge_custom_get_keys(jack_client_t* client, const char* client_name);

#endif // __JACKBRIDGE_HPP__
