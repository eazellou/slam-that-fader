/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "SlamThatFaderConstants.h"

//==============================================================================
/**
*/
class SlamThatFaderAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SlamThatFaderAudioProcessor();
    ~SlamThatFaderAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    std::array<std::atomic<float>*, SlamThatFader::ParamIndex::NumParams> m_rawParameterValues;
    
    juce::AudioProcessorValueTreeState m_parameters;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    // TODO: look into JUCE's valueChanged() callback
    struct ParamState {
        ParamState()
        : m_paramSet(false)
        , m_value(0.f)
        {}
        
        // returns true if value changed
        bool set(float newVal)
        {
            bool changed = !m_paramSet || m_value != newVal;
            m_paramSet = true;
            m_value = newVal;
            return changed;
        }
        
        bool m_paramSet;
        float m_value;
    };
    std::array<ParamState, SlamThatFader::ParamIndex::NumParams> m_paramStates;
    
    // set these parameters and check if they changed
    bool didChange(std::vector<int> paramIDs);
    bool paramsAllSet();

    // amplitude
    float m_prevGain{1.f};
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SlamThatFaderAudioProcessor)
};
