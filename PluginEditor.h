#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class OXC1AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    explicit OXC1AudioProcessorEditor (OXC1AudioProcessor&);
    ~OXC1AudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    OXC1AudioProcessor& audioProcessor;

    juce::Slider inputSlider;
    juce::Slider outputSlider;
    juce::Slider driveSlider;
    juce::Slider aTimingSlider;
    juce::Slider bTimingSlider;

    juce::ToggleButton deltaButton;

    juce::Label titleLabel;
    juce::Label subtitleLabel;

    juce::Label inputLabel;
    juce::Label outputLabel;
    juce::Label driveLabel;
    juce::Label aTimingLabel;
    juce::Label bTimingLabel;

    std::unique_ptr<SliderAttachment> inputAttachment;
    std::unique_ptr<SliderAttachment> outputAttachment;
    std::unique_ptr<SliderAttachment> driveAttachment;
    std::unique_ptr<SliderAttachment> aTimingAttachment;
    std::unique_ptr<SliderAttachment> bTimingAttachment;
    std::unique_ptr<ButtonAttachment> deltaAttachment;

    void setupSlider (juce::Slider& slider, juce::Label& label, const juce::String& name);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OXC1AudioProcessorEditor)
};