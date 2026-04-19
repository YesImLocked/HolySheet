#include "PluginProcessor.h"
#include "PluginEditor.h"

HolySheetProcessor::HolySheetProcessor()
    : AudioProcessor(BusesProperties()
          .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

void HolySheetProcessor::prepareToPlay(double, int)
{
}

void HolySheetProcessor::releaseResources()
{
}

void HolySheetProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer& midiMessages)
{
    // Silence the audio output — we're a visual-only plugin
    buffer.clear();

    // Process incoming MIDI to track which notes are held
    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();

        if (msg.isNoteOn())
        {
            std::lock_guard<std::mutex> lock(notesMutex);
            // Avoid duplicates
            bool found = false;
            for (auto& n : heldNotes)
            {
                if (n.midiNote == msg.getNoteNumber())
                {
                    n.velocity = msg.getVelocity();
                    found = true;
                    break;
                }
            }
            if (!found)
                heldNotes.push_back({ msg.getNoteNumber(), msg.getVelocity() });
        }
        else if (msg.isNoteOff())
        {
            std::lock_guard<std::mutex> lock(notesMutex);
            heldNotes.erase(
                std::remove_if(heldNotes.begin(), heldNotes.end(),
                    [&](const HeldNote& n) { return n.midiNote == msg.getNoteNumber(); }),
                heldNotes.end());
        }
        else if (msg.isAllNotesOff() || msg.isAllSoundOff())
        {
            std::lock_guard<std::mutex> lock(notesMutex);
            heldNotes.clear();
        }
    }

    // Trigger a repaint on the editor if it exists
    if (auto* editor = dynamic_cast<HolySheetEditor*>(getActiveEditor()))
        editor->triggerAsyncUpdate();
}

std::vector<HeldNote> HolySheetProcessor::getHeldNotes() const
{
    std::lock_guard<std::mutex> lock(notesMutex);
    return heldNotes;
}

// This creates the plugin instance
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HolySheetProcessor();
}
