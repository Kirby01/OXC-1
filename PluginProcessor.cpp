#include "PluginProcessor.h"
#include "PluginEditor.h"

OXC1AudioProcessor::OXC1AudioProcessor()
    : AudioProcessor (BusesProperties()
        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout OXC1AudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "input", "Input", juce::NormalisableRange<float> (0.0f, 4.0f, 0.01f), 1.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "output", "Output", juce::NormalisableRange<float> (0.0f, 4.0f, 0.01f), 1.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "driveLog", "Drive", juce::NormalisableRange<float> (0.0f, 1.0f, 0.0001f), 0.5f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "aTiming", "A Timing", juce::NormalisableRange<float> (0.0001f, 0.1f, 0.0001f), 0.01f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "bTiming", "B Timing", juce::NormalisableRange<float> (0.0001f, 0.1f, 0.0001f), 0.01f));

    params.push_back (std::make_unique<juce::AudioParameterBool>(
        "delta", "Delta", false));

    return { params.begin(), params.end() };
}

void OXC1AudioProcessor::prepareToPlay (double, int)
{
    a = 1.0f;
    b = 1.0f;
}

void OXC1AudioProcessor::releaseResources()
{
}

bool OXC1AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void OXC1AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples  = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    auto* inputParam   = apvts.getRawParameterValue ("input");
    auto* outputParam  = apvts.getRawParameterValue ("output");
    auto* driveParam   = apvts.getRawParameterValue ("driveLog");
    auto* aTimeParam   = apvts.getRawParameterValue ("aTiming");
    auto* bTimeParam   = apvts.getRawParameterValue ("bTiming");
    auto* deltaParam   = apvts.getRawParameterValue ("delta");

    const float input  = inputParam->load();
    const float output = outputParam->load();

    const float driveLog = driveParam->load();
    const float drive = 0.0001f * std::pow (80.0f / 0.0001f, driveLog);

    const float ga = aTimeParam->load();
    const float gb = bTimeParam->load();

    const bool delta = deltaParam->load() > 0.5f;

    constexpr float eps = 1.0e-12f;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float l = buffer.getSample (0, sample) * input;
        float r = numChannels > 1 ? buffer.getSample (1, sample) * input : l;

        const float mid = (l + r) * 0.5f;
        const float det = std::abs (mid * mid) * drive;

        const float aa = juce::jmax (a, eps);
        const float bb = juce::jmax (b, eps);

        const float springPow = std::abs ((1.0f / std::sqrt (aa)) * (1.0f / std::sqrt (bb)));

        const float aOld = a;
        const float bOld = b;

        a = (1.0f - ga) * bOld
          + ga * std::pow (std::abs (bOld - aOld), springPow)
          + det / aa;

        a = juce::jmax (a, eps);

        const float den  = a + b + eps;
        const float rel  = std::abs (a - b) / den;
        const float expo = std::abs (1.0f - rel);

        b = (1.0f - gb) * (a + std::abs (a - b))
          + gb * std::pow (juce::jmax (b, eps), expo);

        b = juce::jlimit (eps, 1000000.0f, b);
        a = juce::jlimit (eps, 1000000.0f, a);

        const float outL = l / b;
        const float outR = r / b;

        if (! delta)
        {
            buffer.setSample (0, sample, outL * output);
            if (numChannels > 1)
                buffer.setSample (1, sample, outR * output);
        }
        else
        {
            buffer.setSample (0, sample, (l - outL) * output);
            if (numChannels > 1)
                buffer.setSample (1, sample, (r - outR) * output);
        }
    }

    for (int ch = 2; ch < numChannels; ++ch)
        buffer.clear (ch, 0, numSamples);
}

void OXC1AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void OXC1AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OXC1AudioProcessor();
}
juce::AudioProcessorEditor* OXC1AudioProcessor::createEditor()
{
    return new OXC1AudioProcessorEditor (*this);
}