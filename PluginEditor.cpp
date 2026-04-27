#include "PluginEditor.h"

OXC1AudioProcessorEditor::OXC1AudioProcessorEditor (OXC1AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (620, 330);

    titleLabel.setText ("OXC-1", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    titleLabel.setFont (juce::Font (32.0f, juce::Font::bold));
    addAndMakeVisible (titleLabel);

    subtitleLabel.setText ("Dynamic Leveling Amplifier by Harmonious Records", juce::dontSendNotification);
    subtitleLabel.setJustificationType (juce::Justification::centred);
    subtitleLabel.setColour (juce::Label::textColourId, juce::Colours::silver);
    subtitleLabel.setFont (juce::Font (15.0f));
    addAndMakeVisible (subtitleLabel);

    setupSlider (inputSlider,   inputLabel,   "Input");
    setupSlider (outputSlider,  outputLabel,  "Output");
    setupSlider (driveSlider,   driveLabel,   "Drive");
    setupSlider (aTimingSlider, aTimingLabel, "A Timing");
    setupSlider (bTimingSlider, bTimingLabel, "B Timing");

    deltaButton.setButtonText ("Delta");
    deltaButton.setColour (juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible (deltaButton);

    inputAttachment   = std::make_unique<SliderAttachment> (audioProcessor.apvts, "input",   inputSlider);
    outputAttachment  = std::make_unique<SliderAttachment> (audioProcessor.apvts, "output",  outputSlider);
    driveAttachment   = std::make_unique<SliderAttachment> (audioProcessor.apvts, "driveLog", driveSlider);
    aTimingAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "aTiming", aTimingSlider);
    bTimingAttachment = std::make_unique<SliderAttachment> (audioProcessor.apvts, "bTiming", bTimingSlider);
    deltaAttachment   = std::make_unique<ButtonAttachment> (audioProcessor.apvts, "delta", deltaButton);
}

void OXC1AudioProcessorEditor::setupSlider (juce::Slider& slider, juce::Label& label, const juce::String& name)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 22);
    slider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::silver);
    slider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey);
    slider.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    slider.setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::black);
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::darkgrey);
    addAndMakeVisible (slider);

    label.setText (name, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, juce::Colours::white);
    label.setFont (juce::Font (14.0f, juce::Font::bold));
    addAndMakeVisible (label);
}

void OXC1AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (12, 12, 14));

    auto bounds = getLocalBounds().toFloat().reduced (14.0f);

    g.setColour (juce::Colour (32, 32, 36));
    g.fillRoundedRectangle (bounds, 18.0f);

    g.setColour (juce::Colour (85, 85, 92));
    g.drawRoundedRectangle (bounds, 18.0f, 1.5f);

    g.setColour (juce::Colour (18, 18, 22));
    g.fillRoundedRectangle (bounds.reduced (14.0f).withY (92.0f).withHeight (185.0f), 14.0f);

    g.setColour (juce::Colour (65, 65, 72));
    g.drawRoundedRectangle (bounds.reduced (14.0f).withY (92.0f).withHeight (185.0f), 14.0f, 1.0f);
}

void OXC1AudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (24);

    titleLabel.setBounds (area.removeFromTop (42));
    subtitleLabel.setBounds (area.removeFromTop (26));

    area.removeFromTop (25);

    auto knobArea = area.removeFromTop (150);

    const int knobW = knobArea.getWidth() / 5;

    auto k1 = knobArea.removeFromLeft (knobW);
    auto k2 = knobArea.removeFromLeft (knobW);
    auto k3 = knobArea.removeFromLeft (knobW);
    auto k4 = knobArea.removeFromLeft (knobW);
    auto k5 = knobArea;

    inputLabel.setBounds   (k1.removeFromTop (22));
    outputLabel.setBounds  (k2.removeFromTop (22));
    driveLabel.setBounds   (k3.removeFromTop (22));
    aTimingLabel.setBounds (k4.removeFromTop (22));
    bTimingLabel.setBounds (k5.removeFromTop (22));

    inputSlider.setBounds   (k1.reduced (8));
    outputSlider.setBounds  (k2.reduced (8));
    driveSlider.setBounds   (k3.reduced (8));
    aTimingSlider.setBounds (k4.reduced (8));
    bTimingSlider.setBounds (k5.reduced (8));

    deltaButton.setBounds (getWidth() / 2 - 45, getHeight() - 54, 90, 28);
}