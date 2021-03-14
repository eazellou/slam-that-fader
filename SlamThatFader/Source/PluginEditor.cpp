/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace {
    constexpr auto SliderY{40};
constexpr auto SliderHeight{300};
    constexpr auto SliderWidth{40};
constexpr auto LabelTopMargin{20};
    constexpr auto LabelHeight{40};
}

//==============================================================================
SlamThatFaderAudioProcessorEditor::SlamThatFaderAudioProcessorEditor (SlamThatFaderAudioProcessor& processor,
                                                                      juce::AudioProcessorValueTreeState& vts)
: AudioProcessorEditor{processor}
, m_valueTreeState{vts}
, m_gainSlider{juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextEntryBoxPosition::TextBoxBelow}
{

    addAndMakeVisible(m_gainSlider);
    m_gainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(m_valueTreeState, "gain", m_gainSlider));
    
    setSize(juce::jmax(SliderWidth, 300), SliderHeight + SliderY + LabelTopMargin + LabelHeight);
}

SlamThatFaderAudioProcessorEditor::~SlamThatFaderAudioProcessorEditor()
{
}

//==============================================================================
void SlamThatFaderAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void SlamThatFaderAudioProcessorEditor::resized()
{
    auto r = getLocalBounds();

    r.removeFromTop(SliderY);
    
    m_gainSlider.setBounds(r.removeFromTop(SliderHeight));
}
