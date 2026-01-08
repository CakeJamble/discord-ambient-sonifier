#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>

#include "AmbientPadSynth.h"

int main()
{
    constexpr double sampleRate = 48000.0;
    constexpr int durationSeconds = 120;
    constexpr int totalSamples = (int)(sampleRate * durationSeconds);
    constexpr int blockSize = 512;

    AmbientPadSynth synth;
    synth.prepare(sampleRate);

    juce::AudioBuffer<float> buffer(2, totalSamples);
    buffer.clear();

    AmbientParams params;
    params.activity = 0.2f;
    params.warmth   = 0.3f;
    params.space    = 0.4f;

    int written = 0;

    while (written < totalSamples)
    {
        // ===== Fake “activity drift” =====
        float t = (float)written / (float)totalSamples;

        params.activity = 0.2f + 0.3f * std::sin(t * juce::MathConstants<float>::twoPi);
        params.warmth   = 0.4f + 0.2f * std::sin(t * 1.7f);
        params.space    = 0.3f + 0.3f * std::sin(t * 0.6f);

        synth.setParams(params);

        int block = std::min(blockSize, totalSamples - written);

        synth.process(
            buffer.getWritePointer(0, written),
            buffer.getWritePointer(1, written),
            block
        );

        written += block;
    }

    // ===== Write WAV =====
    juce::File outFile = juce::File::getCurrentWorkingDirectory()
        .getChildFile("ambient_test.wav");

    outFile.deleteFile();

    juce::WavAudioFormat format;
    std::unique_ptr<juce::FileOutputStream> stream(outFile.createOutputStream());

    juce::AudioFormatWriter* writer =
        format.createWriterFor(stream.get(), sampleRate, 2, 24, {}, 0);

    if (writer != nullptr)
    {
        stream.release(); // writer owns it now
        writer->writeFromAudioSampleBuffer(buffer, 0, totalSamples);
        delete writer;
    }

    return 0;
}
