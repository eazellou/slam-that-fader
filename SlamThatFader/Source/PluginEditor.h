/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SlamThatFaderAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SlamThatFaderAudioProcessorEditor (SlamThatFaderAudioProcessor& processor,
                                       juce::AudioProcessorValueTreeState& vts);
    ~SlamThatFaderAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    juce::AudioProcessorValueTreeState& m_valueTreeState;
    
    juce::Slider m_gainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_gainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SlamThatFaderAudioProcessorEditor)
};
