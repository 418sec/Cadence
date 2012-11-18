/*
 * DISTRHO 3BandEQ Plugin, based on 3BandEQ by Michael Gruhn
 * Copyright (C) 2012 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * For a full copy of the license see the LGPL.txt file
 */

#ifndef __DISTRHO_UI_3BANDEQ_H__
#define __DISTRHO_UI_3BANDEQ_H__

#include "DistrhoUIOpenGLExt.h"

#include "DistrhoArtwork3BandEQ.h"
#include "DistrhoPlugin3BandEQ.h"

START_NAMESPACE_DISTRHO

// -------------------------------------------------

class DistrhoUI3BandEQ : public OpenGLExtUI
{
public:
    DistrhoUI3BandEQ();
    ~DistrhoUI3BandEQ();

    // ---------------------------------------------
protected:

    // Information
    unsigned int d_width()
    {
        return DistrhoArtwork3BandEQ::backgroundWidth;
    }

    unsigned int d_height()
    {
        return DistrhoArtwork3BandEQ::backgroundHeight;
    }

    // DSP Callbacks
    void d_parameterChanged(uint32_t index, float value);
    void d_programChanged(uint32_t index);

    // Extended Callbacks
    void imageButtonClicked(ImageButton* button);
    void imageKnobDragStarted(ImageKnob* knob);
    void imageKnobDragFinished(ImageKnob* knob);
    void imageKnobValueChanged(ImageKnob* knob, float value);
    void imageSliderDragStarted(ImageSlider* slider);
    void imageSliderDragFinished(ImageSlider* slider);
    void imageSliderValueChanged(ImageSlider* slider, float value);

private:
    ImageSlider* sliderLow;
    ImageSlider* sliderMid;
    ImageSlider* sliderHigh;
    ImageSlider* sliderMaster;
    ImageKnob* knobLowMid;
    ImageKnob* knobMidHigh;
    ImageButton* buttonAbout;
};

// -------------------------------------------------

END_NAMESPACE_DISTRHO

#endif // __DISTRHO_UI_3BANDEQ_H__
