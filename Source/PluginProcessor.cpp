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
    buffer.clear();

    // Track incoming MIDI notes for display
    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();

        if (msg.isNoteOn())
        {
            std::lock_guard<std::mutex> lock(notesMutex);
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

    // Inject placed notes into the MIDI output
    {
        std::lock_guard<std::mutex> lock(notesMutex);
        for (auto& note : notesToSend)
        {
            auto noteOn = juce::MidiMessage::noteOn(1, note.midiNote, (juce::uint8)note.velocity);
            midiMessages.addEvent(noteOn, 0);

            // Send a short note (note off after ~100ms worth of samples)
            auto noteOff = juce::MidiMessage::noteOff(1, note.midiNote);
            int offsetSamples = juce::jmin(buffer.getNumSamples() - 1, (int)(getSampleRate() * 0.1));
            midiMessages.addEvent(noteOff, offsetSamples);
        }
        notesToSend.clear();
    }

    // Trigger repaint
    if (auto* editor = dynamic_cast<HolySheetEditor*>(getActiveEditor()))
        editor->triggerAsyncUpdate();
}

std::vector<HeldNote> HolySheetProcessor::getHeldNotes() const
{
    std::lock_guard<std::mutex> lock(notesMutex);
    return heldNotes;
}

std::vector<PlacedNote> HolySheetProcessor::getPlacedNotes() const
{
    std::lock_guard<std::mutex> lock(notesMutex);
    return placedNotes;
}

void HolySheetProcessor::addPlacedNote(int midiNote)
{
    std::lock_guard<std::mutex> lock(notesMutex);

    // Don't add duplicates
    for (const auto& n : placedNotes)
        if (n.midiNote == midiNote)
            return;

    PlacedNote note;
    note.midiNote = midiNote;
    note.velocity = 100;
    note.pendingSend = true;
    placedNotes.push_back(note);

    // Queue it for MIDI output
    notesToSend.push_back(note);
}

void HolySheetProcessor::removePlacedNote(int midiNote)
{
    std::lock_guard<std::mutex> lock(notesMutex);
    placedNotes.erase(
        std::remove_if(placedNotes.begin(), placedNotes.end(),
            [&](const PlacedNote& n) { return n.midiNote == midiNote; }),
        placedNotes.end());
}

void HolySheetProcessor::clearPlacedNotes()
{
    std::lock_guard<std::mutex> lock(notesMutex);
    placedNotes.clear();
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HolySheetProcessor();
}
