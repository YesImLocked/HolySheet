#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>
#include <mutex>

struct HeldNote
{
    int midiNote;
    int velocity;
};

// A note placed by the user on the staff
struct PlacedNote
{
    int midiNote;
    int velocity = 100;
    bool pendingSend = false;  // Needs to be sent as MIDI output
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
    bool producesMidi() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}

    // Thread-safe access to held notes (from incoming MIDI)
    std::vector<HeldNote> getHeldNotes() const;

    // Thread-safe access to placed notes (user-edited on staff)
    std::vector<PlacedNote> getPlacedNotes() const;

    // Called from the editor when user places/removes a note
    void addPlacedNote(int midiNote);
    void removePlacedNote(int midiNote);
    void clearPlacedNotes();

private:
    std::vector<HeldNote> heldNotes;
    std::vector<PlacedNote> placedNotes;
    std::vector<PlacedNote> notesToSend;  // Queue for MIDI output
    mutable std::mutex notesMutex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HolySheetProcessor)
};
