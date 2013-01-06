#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# LADSPA RDF python support
# Copyright (C) 2011-2012 Filipe Coelho <falktx@falktx.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# For a full copy of the GNU General Public License see the COPYING file

# -------------------------------------------------------------------------------
#  C types

# Imports (Global)
from ctypes import *
from copy import deepcopy

# Null type
c_nullptr = None

# Base Types
LADSPA_Data = c_float
LADSPA_Property = c_int
LADSPA_PluginType = c_ulonglong

# Unit Types
LADSPA_UNIT_DB               = 0x01
LADSPA_UNIT_COEF             = 0x02
LADSPA_UNIT_HZ               = 0x04
LADSPA_UNIT_S                = 0x08
LADSPA_UNIT_MS               = 0x10
LADSPA_UNIT_MIN              = 0x20

LADSPA_UNIT_CLASS_AMPLITUDE  = LADSPA_UNIT_DB|LADSPA_UNIT_COEF
LADSPA_UNIT_CLASS_FREQUENCY  = LADSPA_UNIT_HZ
LADSPA_UNIT_CLASS_TIME       = LADSPA_UNIT_S|LADSPA_UNIT_MS|LADSPA_UNIT_MIN

# Port Types (Official API)
LADSPA_PORT_INPUT            = 0x1
LADSPA_PORT_OUTPUT           = 0x2
LADSPA_PORT_CONTROL          = 0x4
LADSPA_PORT_AUDIO            = 0x8

# Port Hints
LADSPA_PORT_UNIT             = 0x1
LADSPA_PORT_DEFAULT          = 0x2
LADSPA_PORT_LABEL            = 0x4

# Plugin Types
LADSPA_CLASS_UTILITY         = 0x000000001
LADSPA_CLASS_GENERATOR       = 0x000000002
LADSPA_CLASS_SIMULATOR       = 0x000000004
LADSPA_CLASS_OSCILLATOR      = 0x000000008
LADSPA_CLASS_TIME            = 0x000000010
LADSPA_CLASS_DELAY           = 0x000000020
LADSPA_CLASS_PHASER          = 0x000000040
LADSPA_CLASS_FLANGER         = 0x000000080
LADSPA_CLASS_CHORUS          = 0x000000100
LADSPA_CLASS_REVERB          = 0x000000200
LADSPA_CLASS_FREQUENCY       = 0x000000400
LADSPA_CLASS_FREQUENCY_METER = 0x000000800
LADSPA_CLASS_FILTER          = 0x000001000
LADSPA_CLASS_LOWPASS         = 0x000002000
LADSPA_CLASS_HIGHPASS        = 0x000004000
LADSPA_CLASS_BANDPASS        = 0x000008000
LADSPA_CLASS_COMB            = 0x000010000
LADSPA_CLASS_ALLPASS         = 0x000020000
LADSPA_CLASS_EQ              = 0x000040000
LADSPA_CLASS_PARAEQ          = 0x000080000
LADSPA_CLASS_MULTIEQ         = 0x000100000
LADSPA_CLASS_AMPLITUDE       = 0x000200000
LADSPA_CLASS_PITCH           = 0x000400000
LADSPA_CLASS_AMPLIFIER       = 0x000800000
LADSPA_CLASS_WAVESHAPER      = 0x001000000
LADSPA_CLASS_MODULATOR       = 0x002000000
LADSPA_CLASS_DISTORTION      = 0x004000000
LADSPA_CLASS_DYNAMICS        = 0x008000000
LADSPA_CLASS_COMPRESSOR      = 0x010000000
LADSPA_CLASS_EXPANDER        = 0x020000000
LADSPA_CLASS_LIMITER         = 0x040000000
LADSPA_CLASS_GATE            = 0x080000000
LADSPA_CLASS_SPECTRAL        = 0x100000000
LADSPA_CLASS_NOTCH           = 0x200000000

LADSPA_GROUP_DYNAMICS        = LADSPA_CLASS_DYNAMICS | LADSPA_CLASS_COMPRESSOR | LADSPA_CLASS_EXPANDER | LADSPA_CLASS_LIMITER | LADSPA_CLASS_GATE
LADSPA_GROUP_AMPLITUDE       = LADSPA_CLASS_AMPLITUDE | LADSPA_CLASS_AMPLIFIER | LADSPA_CLASS_WAVESHAPER | LADSPA_CLASS_MODULATOR | LADSPA_CLASS_DISTORTION | LADSPA_GROUP_DYNAMICS
LADSPA_GROUP_EQ              = LADSPA_CLASS_EQ | LADSPA_CLASS_PARAEQ | LADSPA_CLASS_MULTIEQ
LADSPA_GROUP_FILTER          = LADSPA_CLASS_FILTER | LADSPA_CLASS_LOWPASS | LADSPA_CLASS_HIGHPASS | LADSPA_CLASS_BANDPASS | LADSPA_CLASS_COMB | LADSPA_CLASS_ALLPASS | LADSPA_CLASS_NOTCH
LADSPA_GROUP_FREQUENCY       = LADSPA_CLASS_FREQUENCY | LADSPA_CLASS_FREQUENCY_METER | LADSPA_GROUP_FILTER | LADSPA_GROUP_EQ | LADSPA_CLASS_PITCH
LADSPA_GROUP_SIMULATOR       = LADSPA_CLASS_SIMULATOR | LADSPA_CLASS_REVERB
LADSPA_GROUP_TIME            = LADSPA_CLASS_TIME | LADSPA_CLASS_DELAY | LADSPA_CLASS_PHASER | LADSPA_CLASS_FLANGER | LADSPA_CLASS_CHORUS | LADSPA_CLASS_REVERB
LADSPA_GROUP_GENERATOR       = LADSPA_CLASS_GENERATOR | LADSPA_CLASS_OSCILLATOR

# Scale Point
class LADSPA_RDF_ScalePoint(Structure):
    _fields_ = [
        ("Value", LADSPA_Data),
        ("Label", c_char_p)
    ]

# Port
class LADSPA_RDF_Port(Structure):
    _fields_ = [
        ("Type", LADSPA_Property),
        ("Hints", LADSPA_Property),
        ("Label", c_char_p),
        ("Default", LADSPA_Data),
        ("Unit", LADSPA_Property),

        ("ScalePointCount", c_ulong),
        ("ScalePoints", POINTER(LADSPA_RDF_ScalePoint))
    ]

# Plugin
class LADSPA_RDF_Descriptor(Structure):
    _fields_ = [
        ("Type", LADSPA_PluginType),
        ("UniqueID", c_ulong),
        ("Title", c_char_p),
        ("Creator", c_char_p),

        ("PortCount", c_ulong),
        ("Ports", POINTER(LADSPA_RDF_Port))
    ]

# -------------------------------------------------------------------------------
#  Python compatible C types

PyLADSPA_RDF_ScalePoint = {
    'Value': 0.0,
    'Label': ""
}

PyLADSPA_RDF_Port = {
    'Type': 0x0,
    'Hints': 0x0,
    'Label': "",
    'Default': 0.0,
    'Unit': 0x0,

    'ScalePointCount': 0,
    'ScalePoints': [],

    # Only here to help, NOT in the API:
    'index': 0
}

PyLADSPA_RDF_Descriptor = {
    'Type': 0x0,
    'UniqueID': 0,
    'Title': "",
    'Creator': "",

    'PortCount': 0,
    'Ports': []
}

# -------------------------------------------------------------------------------
#  RDF data and conversions

# Namespaces
NS_dc     = "http://purl.org/dc/elements/1.1/"
NS_rdf    = "http://www.w3.org/1999/02/22-rdf-syntax-ns#"
NS_rdfs   = "http://www.w3.org/2000/01/rdf-schema#"
NS_ladspa = "http://ladspa.org/ontology#"

# Prefixes (sorted alphabetically and by type)
rdf_prefix = {
    # Base types
    'dc:creator': NS_dc + "creator",
    'dc:rights':  NS_dc + "rights",
    'dc:title':   NS_dc + "title",
    'rdf:value':  NS_rdf + "value",
    'rdf:type':   NS_rdf + "type",

    # LADSPA Stuff
    'ladspa:forPort':      NS_ladspa + "forPort",
    'ladspa:hasLabel':     NS_ladspa + "hasLabel",
    'ladspa:hasPoint':     NS_ladspa + "hasPoint",
    'ladspa:hasPort':      NS_ladspa + "hasPort",
    'ladspa:hasPortValue': NS_ladspa + "hasPortValue",
    'ladspa:hasScale':     NS_ladspa + "hasScale",
    'ladspa:hasSetting':   NS_ladspa + "hasSetting",
    'ladspa:hasUnit':      NS_ladspa + "hasUnit",

    # LADSPA Extensions
    'ladspa:NotchPlugin':    NS_ladspa + "NotchPlugin",
    'ladspa:SpectralPlugin': NS_ladspa + "SpectralPlugin"
}

def get_c_plugin_class(value):
    valueStr = value.replace(NS_ladspa, "")

    if valueStr == "Plugin":
        return 0x0
    elif valueStr == "UtilityPlugin":
        return LADSPA_CLASS_UTILITY
    elif valueStr == "GeneratorPlugin":
        return LADSPA_CLASS_GENERATOR
    elif valueStr == "SimulatorPlugin":
        return LADSPA_CLASS_SIMULATOR
    elif valueStr == "OscillatorPlugin":
        return LADSPA_CLASS_OSCILLATOR
    elif valueStr == "TimePlugin":
        return LADSPA_CLASS_TIME
    elif valueStr == "DelayPlugin":
        return LADSPA_CLASS_DELAY
    elif valueStr == "PhaserPlugin":
        return LADSPA_CLASS_PHASER
    elif valueStr == "FlangerPlugin":
        return LADSPA_CLASS_FLANGER
    elif valueStr == "ChorusPlugin":
        return LADSPA_CLASS_CHORUS
    elif valueStr == "ReverbPlugin":
        return LADSPA_CLASS_REVERB
    elif valueStr == "FrequencyPlugin":
        return LADSPA_CLASS_FREQUENCY
    elif valueStr == "FrequencyMeterPlugin":
        return LADSPA_CLASS_FREQUENCY_METER
    elif valueStr == "FilterPlugin":
        return LADSPA_CLASS_FILTER
    elif valueStr == "LowpassPlugin":
        return LADSPA_CLASS_LOWPASS
    elif valueStr == "HighpassPlugin":
        return LADSPA_CLASS_HIGHPASS
    elif valueStr == "BandpassPlugin":
        return LADSPA_CLASS_BANDPASS
    elif valueStr == "CombPlugin":
        return LADSPA_CLASS_COMB
    elif valueStr == "AllpassPlugin":
        return LADSPA_CLASS_ALLPASS
    elif valueStr == "EQPlugin":
        return LADSPA_CLASS_EQ
    elif valueStr == "ParaEQPlugin":
        return LADSPA_CLASS_PARAEQ
    elif valueStr == "MultiEQPlugin":
        return LADSPA_CLASS_MULTIEQ
    elif valueStr == "AmplitudePlugin":
        return LADSPA_CLASS_AMPLITUDE
    elif valueStr == "PitchPlugin":
        return LADSPA_CLASS_PITCH
    elif valueStr == "AmplifierPlugin":
        return LADSPA_CLASS_AMPLIFIER
    elif valueStr == "WaveshaperPlugin":
        return LADSPA_CLASS_WAVESHAPER
    elif valueStr == "ModulatorPlugin":
        return LADSPA_CLASS_MODULATOR
    elif valueStr == "DistortionPlugin":
        return LADSPA_CLASS_DISTORTION
    elif valueStr == "DynamicsPlugin":
        return LADSPA_CLASS_DYNAMICS
    elif valueStr == "CompressorPlugin":
        return LADSPA_CLASS_COMPRESSOR
    elif valueStr == "ExpanderPlugin":
        return LADSPA_CLASS_EXPANDER
    elif valueStr == "LimiterPlugin":
        return LADSPA_CLASS_LIMITER
    elif valueStr == "GatePlugin":
        return LADSPA_CLASS_GATE
    elif valueStr == "SpectralPlugin":
        return LADSPA_CLASS_SPECTRAL
    elif valueStr == "NotchPlugin":
        return LADSPA_CLASS_NOTCH
    elif valueStr == "MixerPlugin":
        return LADSPA_CLASS_EQ
    else:
        print("LADSPA_RDF - Got an unknown plugin type '%s'" % valueStr)
        return 0x0

def get_c_port_type(value):
    valueStr = value.replace(NS_ladspa, "")

    if valueStr == "Port":
        return 0x0
    elif valueStr == "ControlPort":
        return LADSPA_PORT_CONTROL
    elif valueStr == "AudioPort":
        return LADSPA_PORT_AUDIO
    elif valueStr == "InputPort":
        return LADSPA_PORT_INPUT
    elif valueStr == "OutputPort":
        return LADSPA_PORT_OUTPUT
    elif valueStr in ("ControlInputPort", "InputControlPort"):
        return LADSPA_PORT_CONTROL|LADSPA_PORT_INPUT
    elif valueStr in ("ControlOutputPort", "OutputControlPort"):
        return LADSPA_PORT_CONTROL|LADSPA_PORT_OUTPUT
    elif valueStr in ("AudioInputPort", "InputAudioPort"):
        return LADSPA_PORT_AUDIO|LADSPA_PORT_INPUT
    elif valueStr in ("AudioOutputPort", "OutputAudioPort"):
        return LADSPA_PORT_AUDIO|LADSPA_PORT_OUTPUT
    else:
        print("LADSPA_RDF - Got an unknown port type '%s'" % valueStr)
        return 0x0

def get_c_unit_type(value):
    valueStr = value.replace(NS_ladspa, "")

    if valueStr in ("Unit", "Units", "AmplitudeUnits", "FrequencyUnits", "TimeUnits"):
        return 0x0
    elif valueStr == "dB":
        return LADSPA_UNIT_DB
    elif valueStr == "coef":
        return LADSPA_UNIT_COEF
    elif valueStr == "Hz":
        return LADSPA_UNIT_HZ
    elif valueStr == "seconds":
        return LADSPA_UNIT_S
    elif valueStr == "milliseconds":
        return LADSPA_UNIT_MS
    elif valueStr == "minutes":
        return LADSPA_UNIT_MIN
    else:
        print("LADSPA_RDF - Got an unknown unit type '%s'" % valueStr)
        return 0x0

# -------------------------------------------------------------------------------
#  Global objects

global LADSPA_RDF_PATH, LADSPA_Plugins
LADSPA_RDF_PATH = ("/usr/share/ladspa/rdf", "/usr/local/share/ladspa/rdf")
LADSPA_Plugins  = []

# Set LADSPA_RDF_PATH variable
def set_rdf_path(PATH):
    global LADSPA_RDF_PATH
    LADSPA_RDF_PATH = PATH

# -------------------------------------------------------------------------------
#  Helper methods

LADSPA_RDF_TYPE_PLUGIN = 1
LADSPA_RDF_TYPE_PORT = 2

# Check RDF Type
def rdf_is_type(subject, compare):
    if isinstance(subject, URIRef) and NS_ladspa in subject:
        if compare == LADSPA_RDF_TYPE_PLUGIN:
            return bool(to_plugin_number(subject).isdigit())
        elif compare == LADSPA_RDF_TYPE_PORT:
            return bool("." in to_plugin_number(subject))
    else:
        return False

def to_float(rdfItem):
    return float(str(rdfItem).replace("f", ""))

# Convert RDF LADSPA subject into a number
def to_plugin_number(subject):
    return str(subject).replace(NS_ladspa, "")

# Convert RDF LADSPA subject into a plugin and port number
def to_plugin_and_port_number(subject):
    numbers = str(subject).replace(NS_ladspa, "").split(".")
    return (numbers[0], numbers[1])

# Convert RDF LADSPA subject into a port number
def to_plugin_port(subject):
    return to_plugin_and_port_number(subject)[1]

# -------------------------------------------------------------------------------
#  RDF store/retrieve data methods

def check_and_add_plugin(pluginId):
    global LADSPA_Plugins
    for i in range(len(LADSPA_Plugins)):
        if LADSPA_Plugins[i]['UniqueID'] == pluginId:
            return i
    else:
        plugin = deepcopy(PyLADSPA_RDF_Descriptor)
        plugin['UniqueID'] = pluginId
        LADSPA_Plugins.append(plugin)
        return len(LADSPA_Plugins) - 1

def set_plugin_value(pluginId, key, value):
    global LADSPA_Plugins
    index = check_and_add_plugin(pluginId)
    LADSPA_Plugins[index][key] = value

def add_plugin_value(pluginId, key, value):
    global LADSPA_Plugins
    index = check_and_add_plugin(pluginId)
    LADSPA_Plugins[index][key] += value

def or_plugin_value(pluginId, key, value):
    global LADSPA_Plugins
    index = check_and_add_plugin(pluginId)
    LADSPA_Plugins[index][key] |= value

def append_plugin_value(pluginId, key, value):
    global LADSPA_Plugins
    index = check_and_add_plugin(pluginId)
    LADSPA_Plugins[index][key].append(value)

def check_and_add_port(pluginId, portId):
    global LADSPA_Plugins
    index = check_and_add_plugin(pluginId)
    ports = LADSPA_Plugins[index]['Ports']
    for i in range(len(ports)):
        if ports[i]['index'] == portId:
            return (index, i)
    else:
        portCount = LADSPA_Plugins[index]['PortCount']
        port = deepcopy(PyLADSPA_RDF_Port)
        port['index'] = portId
        ports.append(port)
        LADSPA_Plugins[index]['PortCount'] += 1
        return (index, portCount)

def set_port_value(pluginId, portId, key, value):
    global LADSPA_Plugins
    i, j = check_and_add_port(pluginId, portId)
    LADSPA_Plugins[i]['Ports'][j][key] = value

def add_port_value(pluginId, portId, key, value):
    global LADSPA_Plugins
    i, j = check_and_add_port(pluginId, portId)
    LADSPA_Plugins[i]['Ports'][j][key] += value

def or_port_value(pluginId, portId, key, value):
    global LADSPA_Plugins
    i, j = check_and_add_port(pluginId, portId)
    LADSPA_Plugins[i]['Ports'][j][key] |= value

def append_port_value(pluginId, portId, key, value):
    global LADSPA_Plugins
    i, j = check_and_add_port(pluginId, portId)
    LADSPA_Plugins[i]['Ports'][j][key].append(value)

def add_scalepoint(pluginId, portId, value, label):
    global LADSPA_Plugins
    i, j = check_and_add_port(pluginId, portId)
    port = LADSPA_Plugins[i]['Ports'][j]
    scalePoint = deepcopy(PyLADSPA_RDF_ScalePoint)
    scalePoint['Value'] = value
    scalePoint['Label'] = label
    port['ScalePoints'].append(scalePoint)
    port['ScalePointCount'] += 1

def set_port_default(pluginId, portId, value):
    global LADSPA_Plugins
    i, j = check_and_add_port(pluginId, portId)
    port = LADSPA_Plugins[i]['Ports'][j]
    port['Default'] = value
    port['Hints'] |= LADSPA_PORT_DEFAULT

def get_node_objects(valueNodes, nSubject):
    retNodes = []
    for subject, predicate, object_ in valueNodes:
        if subject == nSubject:
            retNodes.append((predicate, object_))
    return retNodes

def append_and_sort(value, vlist):
    if len(vlist) == 0:
        vlist.append(value)
    elif value < vlist[0]:
        vlist.insert(0, value)
    elif value > vlist[len(vlist) - 1]:
        vlist.append(value)
    else:
        for i in range(len(vlist)):
            if value < vlist[i]:
                vlist.insert(i, value)
                break
        else:
            print("LADSPA_RDF - CRITICAL ERROR #001")

    return vlist

def get_value_index(value, vlist):
    for i in range(len(vlist)):
        if vlist[i] == value:
            return i
    else:
        print("LADSPA_RDF - CRITICAL ERROR #002")
        return 0

# -------------------------------------------------------------------------------
#  RDF sort data methods

# Sort the plugin's port's ScalePoints by value
def SORT_PyLADSPA_RDF_ScalePoints(oldDictList):
    newDictList = []
    indexesList = []

    for i in range(len(oldDictList)):
        newDictList.append(deepcopy(PyLADSPA_RDF_ScalePoint))
        append_and_sort(oldDictList[i]['Value'], indexesList)

    for i in range(len(oldDictList)):
        index = get_value_index(oldDictList[i]['Value'], indexesList)
        newDictList[index]['Value'] = oldDictList[i]['Value']
        newDictList[index]['Label'] = oldDictList[i]['Label']

    return newDictList

# Sort the plugin's port by index
def SORT_PyLADSPA_RDF_Ports(oldDictList):
    newDictList = []
    maxIndex = -1

    for i in range(len(oldDictList)):
        if oldDictList[i]['index'] > maxIndex:
            maxIndex = oldDictList[i]['index']

    for i in range(maxIndex + 1):
        newDictList.append(deepcopy(PyLADSPA_RDF_Port))

    for i in range(len(oldDictList)):
        index = oldDictList[i]['index']
        newDictList[index]['index']   = oldDictList[i]['index']
        newDictList[index]['Type']    = oldDictList[i]['Type']
        newDictList[index]['Hints']   = oldDictList[i]['Hints']
        newDictList[index]['Unit']    = oldDictList[i]['Unit']
        newDictList[index]['Default'] = oldDictList[i]['Default']
        newDictList[index]['Label']   = oldDictList[i]['Label']
        newDictList[index]['ScalePointCount'] = oldDictList[i]['ScalePointCount']
        newDictList[index]['ScalePoints'] = SORT_PyLADSPA_RDF_ScalePoints(oldDictList[i]['ScalePoints'])

    return newDictList

# -------------------------------------------------------------------------------
#  RDF data parsing

from rdflib import ConjunctiveGraph, URIRef, BNode

# Fully parse rdf file
def parse_rdf_file(filename):
    primer = ConjunctiveGraph()

    try:
        primer.parse(filename, format='xml')
        rdfList = [(x, y, z) for x, y, z in primer]
    except:
        rdfList = []

    # For BNodes
    indexNodes = [] # Subject (index), Predicate, Plugin, Port
    valueNodes = [] # Subject (index), Predicate, Object

    # Parse RDF list
    for subject, predicate, object_ in rdfList:
        # Fix broken or old plugins
        if predicate == URIRef("http://ladspa.org/ontology#hasUnits"):
            predicate = URIRef(rdf_prefix['ladspa:hasUnit'])

        # Plugin information
        if rdf_is_type(subject, LADSPA_RDF_TYPE_PLUGIN):
            pluginId = int(to_plugin_number(subject))

            if predicate == URIRef(rdf_prefix['dc:creator']):
                set_plugin_value(pluginId, 'Creator', str(object_))

            elif predicate == URIRef(rdf_prefix['dc:rights']):
                # No useful information here
                pass

            elif predicate == URIRef(rdf_prefix['dc:title']):
                set_plugin_value(pluginId, 'Title', str(object_))

            elif predicate == URIRef(rdf_prefix['rdf:type']):
                c_class = get_c_plugin_class(str(object_))
                or_plugin_value(pluginId, 'Type', c_class)

            elif predicate == URIRef(rdf_prefix['ladspa:hasPort']):
                # No useful information here
                pass

            elif predicate == URIRef(rdf_prefix['ladspa:hasSetting']):
                indexNodes.append((object_, predicate, pluginId, None))

            else:
                print("LADSPA_RDF - Plugin predicate '%s' not handled" % predicate)

        # Port information
        elif rdf_is_type(subject, LADSPA_RDF_TYPE_PORT):
            portInfo = to_plugin_and_port_number(subject)
            pluginId = int(portInfo[0])
            portId   = int(portInfo[1])

            if predicate == URIRef(rdf_prefix['rdf:type']):
                c_class = get_c_port_type(str(object_))
                or_port_value(pluginId, portId, 'Type', c_class)

            elif predicate == URIRef(rdf_prefix['ladspa:hasLabel']):
                set_port_value(pluginId, portId, 'Label', str(object_))
                or_port_value(pluginId, portId, 'Hints', LADSPA_PORT_LABEL)

            elif predicate == URIRef(rdf_prefix['ladspa:hasScale']):
                indexNodes.append((object_, predicate, pluginId, portId))

            elif predicate == URIRef(rdf_prefix['ladspa:hasUnit']):
                c_unit = get_c_unit_type(str(object_))
                set_port_value(pluginId, portId, 'Unit', c_unit)
                or_port_value(pluginId, portId, 'Hints', LADSPA_PORT_UNIT)

            else:
                print("LADSPA_RDF - Port predicate '%s' not handled" % predicate)

        # These "extensions" are already implemented
        elif subject in (URIRef(rdf_prefix['ladspa:NotchPlugin']), URIRef(rdf_prefix['ladspa:SpectralPlugin'])):
            pass

        elif type(subject) == BNode:
            valueNodes.append((subject, predicate, object_))

        else:
            print("LADSPA_RDF - Unknown subject type '%s'" % subject)

    # Parse BNodes, indexes
    bnodesDataDump = []

    for nSubject, nPredicate, pluginId, portId in indexNodes:
        nObjects = get_node_objects(valueNodes, nSubject)

        for subPredicate, subSubject in nObjects:
            subObjects = get_node_objects(valueNodes, subSubject)

            for realPredicate, realObject in subObjects:
                if nPredicate == URIRef(rdf_prefix['ladspa:hasScale']) and subPredicate == URIRef(rdf_prefix['ladspa:hasPoint']):
                    bnodesDataDump.append(("scalepoint", subSubject, pluginId, portId, realPredicate, realObject))
                elif nPredicate == URIRef(rdf_prefix['ladspa:hasSetting']) and subPredicate == URIRef(rdf_prefix['ladspa:hasPortValue']):
                    bnodesDataDump.append(("port_default", subSubject, pluginId, portId, realPredicate, realObject))
                else:
                    print("LADSPA_RDF - Unknown BNode combo - '%s' + '%s'" % (nPredicate, subPredicate))

    # Process BNodes, values
    scalePoints  = [] # subject, plugin, port, value, label
    portDefaults = [] # subject, plugin, port, def-value

    for nType, nSubject, nPlugin, nPort, nPredicate, nObject in bnodesDataDump:
        if nType == "scalepoint":
            for i in range(len(scalePoints)):
                if scalePoints[i][0] == nSubject:
                    index = i
                    break
            else:
                scalePoints.append([nSubject, nPlugin, nPort, None, None])
                index = len(scalePoints) - 1

            if nPredicate == URIRef(rdf_prefix['rdf:value']):
                scalePoints[index][3] = to_float(nObject)
            elif nPredicate == URIRef(rdf_prefix['ladspa:hasLabel']):
                scalePoints[index][4] = str(nObject)

        elif nType == "port_default":
            for i in range(len(portDefaults)):
                if portDefaults[i][0] == nSubject:
                    index = i
                    break
            else:
                portDefaults.append([nSubject, nPlugin, None, None])
                index = len(portDefaults) - 1

            if nPredicate == URIRef(rdf_prefix['ladspa:forPort']):
                portDefaults[index][2] = int(to_plugin_port(nObject))
            elif nPredicate == URIRef(rdf_prefix['rdf:value']):
                portDefaults[index][3] = to_float(nObject)

    # Now add the last information
    for scalePoint in scalePoints:
        index, pluginId, portId, value, label = scalePoint
        add_scalepoint(pluginId, portId, value, label)

    for portDefault in portDefaults:
        index, pluginId, portId, value = portDefault
        set_port_default(pluginId, portId, value)

# -------------------------------------------------------------------------------
#  LADSPA_RDF main methods

import os

# Main function - check all rdfs for information about ladspa plugins
def recheck_all_plugins(qobject, startValue, percentValue, curValue):
    global LADSPA_RDF_PATH, LADSPA_Plugins

    LADSPA_Plugins = []
    rdfFiles       = []
    rdfExtensions  = (".rdf", ".rdF", ".rDF", ".RDF", ".RDf", "Rdf")

    # Get all RDF files
    for PATH in LADSPA_RDF_PATH:
        for root, dirs, files in os.walk(PATH):
            for filename in [filename for filename in files if filename.endswith(rdfExtensions)]:
                rdfFiles.append(os.path.join(root, filename))

    # Parse all RDF files
    for i in range(len(rdfFiles)):
        rdfFile = rdfFiles[i]

        # Tell GUI we're parsing this bundle
        if qobject:
            percent = (float(i) / len(rdfFiles) ) * percentValue
            qobject.pluginLook(startValue + (percent * curValue), rdfFile)

        # Parse RDF
        parse_rdf_file(rdfFile)

    return LADSPA_Plugins

# Convert PyLADSPA_Plugins into ctype structs
def get_c_ladspa_rdfs(pyPluginList):
    C_LADSPA_Plugins  = []
    c_unicodeErrorStr = "(unicode error)".encode("utf-8")

    for plugin in pyPluginList:
        # Sort the ports by index
        pyLadspaPorts = SORT_PyLADSPA_RDF_Ports(plugin['Ports'])

        # Initial data
        desc = LADSPA_RDF_Descriptor()
        desc.Type = plugin['Type']
        desc.UniqueID = plugin['UniqueID']

        try:
            if plugin['Title']:
                desc.Title = plugin['Title'].encode("utf-8")
            else:
                desc.Title = c_nullptr
        except:
            desc.Title = c_unicodeErrorStr

        try:
            if plugin['Creator']:
                desc.Creator = plugin['Creator'].encode("utf-8")
            else:
                desc.Creator = c_nullptr
        except:
            desc.Creator = c_unicodeErrorStr

        desc.PortCount = plugin['PortCount']

        # Ports
        _PortType  = LADSPA_RDF_Port * desc.PortCount
        desc.Ports = _PortType()

        for i in range(desc.PortCount):
            port   = LADSPA_RDF_Port()
            pyPort = pyLadspaPorts[i]

            port.Type  = pyPort['Type']
            port.Hints = pyPort['Hints']

            try:
                if pyPort['Label']:
                    port.Label = pyPort['Label'].encode("utf-8")
                else:
                    port.Label = c_nullptr
            except:
                port.Label = c_unicodeErrorStr

            port.Default = pyPort['Default']
            port.Unit    = pyPort['Unit']

            # ScalePoints
            port.ScalePointCount = pyPort['ScalePointCount']

            _ScalePointType  = LADSPA_RDF_ScalePoint * port.ScalePointCount
            port.ScalePoints = _ScalePointType()

            for j in range(port.ScalePointCount):
                scalePoint   = LADSPA_RDF_ScalePoint()
                pyScalePoint = pyPort['ScalePoints'][j]

                try:
                    if pyScalePoint['Label']:
                        scalePoint.Label = pyScalePoint['Label'].encode("utf-8")
                    else:
                        scalePoint.Label = c_nullptr
                except:
                    scalePoint.Label = c_unicodeErrorStr

                scalePoint.Value = pyScalePoint['Value']

                port.ScalePoints[j] = scalePoint

            desc.Ports[i] = port

        C_LADSPA_Plugins.append(desc)

    return C_LADSPA_Plugins
