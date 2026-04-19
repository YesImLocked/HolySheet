#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>
#include <mutex>

struct HeldNote
{
    int midiNote;
    int velocity;
};

class HolySheetProcessor : public juce::AudioProcessor
{
public:
    HolySheetProcessor();
    ~HolySheetProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "HolySheet"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}

    // Thread-safe access to held notes for the editor
    std::vector<HeldNote> getHeldNotes() const;

private:
    std::vector<HeldNote> heldNotes;
    mutable std::mutex notesMutex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HolySheetProcessor)
};
