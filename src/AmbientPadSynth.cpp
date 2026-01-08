#include "AmbientPadSynth.h"
#include <cmath>

static constexpr float detune[4] = {1.0f, 1.003f, 0.997f, 1.006f};
static constexpr int chord[4]    = {0, 3, 7, 10}; // minor 7

void AmbientPadSynth::prepare(double sampleRate)
{
    sr = sampleRate;
    filter.reset();
    filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    filter.setResonance(0.25f);

    juce::dsp::Reverb::Parameters rp;
    rp.roomSize = 0.6f;
    rp.damping  = 0.4f;
    rp.width    = 1.0f;
    rp.wetLevel = 0.25f;
    rp.dryLevel = 0.75f;
    reverb.setParameters(rp);

    updateChord();
}

void AmbientPadSynth::setParams(const AmbientParams& newParams)
{
    target = newParams;
}

float AmbientPadSynth::osc(int v)
{
    phase[v] += freq[v] / sr;
    if (phase[v] >= 1.0f)
        phase[v] -= 1.0f;

    float s = std::sin(juce::MathConstants<float>::twoPi * phase[v]);
    float t = std::asin(s) * (2.0f / juce::MathConstants<float>::pi);

    return 0.7f * s + 0.3f * t;
}

void AmbientPadSynth::updateChord()
{
    float base = 110.0f; // A2
    for (int i = 0; i < maxVoices; ++i)
    {
        freq[i] = base * std::pow(2.0f, chord[i] / 12.0f) * detune[i];
    }
}


void AmbientPadSynth::process(float* left, float* right, int numSamples)
{
    // ===== Per-sample oscillator/filter =====
    for (int i = 0; i < numSamples; ++i)
    {
        current.activity += (target.activity - current.activity) * 0.0005f;
        current.warmth   += (target.warmth   - current.warmth)   * 0.0005f;
        current.space    += (target.space    - current.space)    * 0.0005f;

        float cutoff = 600.0f + current.warmth * 2400.0f;
        filter.setCutoffFrequency(cutoff);

        float sig = 0.0f;
        for (int v = 0; v < maxVoices; ++v)
            sig += osc(v);
        sig *= 0.25f;

        sig = filter.processSample(0, sig);

        float t = (float)sampleCounter / sr;
        float lfo = std::sin(juce::MathConstants<float>::twoPi * 0.02f * t);

        left[i]  = sig * (0.5f + 0.1f * lfo);
        right[i] = sig * (0.5f - 0.1f * lfo);

        sampleCounter++;
    }

    // ===== Reverb block =====
    int processed = 0;
    constexpr int reverbBlockSize = 64;
    float* channels[2] = { left, right };

    while (processed < numSamples)
    {
        int bs = std::min(reverbBlockSize, numSamples - processed);

        juce::dsp::AudioBlock<float> block(channels, 2, bs);
        juce::dsp::ProcessContextReplacing<float> context(block);
        reverb.process(context);

        // move pointer forward
        channels[0] += bs;
        channels[1] += bs;
        processed += bs;
    }
}
