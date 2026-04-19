#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "StaffRenderer.h"

class HolySheetProcessor;

class HolySheetEditor : public juce::AudioProcessorEditor,
                         public juce::AsyncUpdater
{
public:
    explicit HolySheetEditor(HolySheetProcessor&);
    ~HolySheetEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

    void mouseMove(const juce::MouseEvent&) override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseExit(const juce::MouseEvent&) override;

    void handleAsyncUpdate() override;

private:
    HolySheetProcessor& processor;
    StaffRenderer staffRenderer;
    int hoveredLinespace = -999;  // -999 = no hover

    // Convert linespace to MIDI note (assumes C major, no accidentals from click)
    int linespaceToMidi(int linespace) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HolySheetEditor)
};
