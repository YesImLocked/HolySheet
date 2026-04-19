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

    // Called from the audio thread via AsyncUpdater
    void handleAsyncUpdate() override;

private:
    HolySheetProcessor& processor;
    StaffRenderer staffRenderer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HolySheetEditor)
};
