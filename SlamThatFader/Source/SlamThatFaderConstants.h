/*
  ==============================================================================

    SlamThatFaderConstants.h
    Created: 8 Mar 2021 8:47:23pm
    Author:  Emma Azelborn

  ==============================================================================
*/

#pragma once

namespace SlamThatFader {
    enum ParamIndex {
        Gain = 0,
        NumParams
    };

struct SlamThatFaderParam {
    juce::String parameterID;
    juce::String name;
    float minValue;
    float maxValue;
    float initialValue;
};

const std::array<SlamThatFaderParam, ParamIndex::NumParams> SlamThatFader_Parameters =
{{
    {
        "gain",
        "Gain",
        0.f,
        48.f,
        6.f
    },
}};
}
