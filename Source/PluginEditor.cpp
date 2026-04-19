#include "PluginEditor.h"
#include "PluginProcessor.h"

HolySheetEditor::HolySheetEditor(HolySheetProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(700, 350);
    setResizable(true, true);
    setResizeLimits(500, 250, 1400, 700);
}

void HolySheetEditor::paint(juce::Graphics& g)
{
    auto liveNotes = processor.getHeldNotes();
    auto placedNotes = processor.getPlacedNotes();
    staffRenderer.draw(g, getLocalBounds(), liveNotes, placedNotes, hoveredLinespace);

    // Title and instructions
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("HolySheet v0.2  |  Click staff to place notes  |  Right-click to remove",
               getLocalBounds().removeFromBottom(18),
               juce::Justification::centredRight, false);
}

void HolySheetEditor::resized()
{
}

void HolySheetEditor::mouseMove(const juce::MouseEvent& event)
{
    float staffTop = staffRenderer.getStaffTop(getLocalBounds());
    int ls = staffRenderer.yToLinespace(staffTop, (float)event.y);

    // Clamp to reasonable range (A2 to C7 ish)
    if (ls < -8 || ls > 18)
        ls = -999;

    if (ls != hoveredLinespace)
    {
        hoveredLinespace = ls;
        repaint();
    }
}

void HolySheetEditor::mouseDown(const juce::MouseEvent& event)
{
    if (hoveredLinespace == -999)
        return;

    int midiNote = linespaceToMidi(hoveredLinespace);
    if (midiNote < 0 || midiNote > 127)
        return;

    if (event.mods.isRightButtonDown())
    {
        processor.removePlacedNote(midiNote);
    }
    else
    {
        // Toggle: if note exists, remove it; otherwise add it
        auto placed = processor.getPlacedNotes();
        bool exists = false;
        for (const auto& n : placed)
        {
            if (n.midiNote == midiNote)
            {
                exists = true;
                break;
            }
        }

        if (exists)
            processor.removePlacedNote(midiNote);
        else
            processor.addPlacedNote(midiNote);
    }

    repaint();
}

void HolySheetEditor::mouseExit(const juce::MouseEvent&)
{
    hoveredLinespace = -999;
    repaint();
}

void HolySheetEditor::handleAsyncUpdate()
{
    repaint();
}

int HolySheetEditor::linespaceToMidi(int linespace) const
{
    // linespace 0 = middle C = MIDI 60
    // Each diatonic step maps to a semitone offset
    static const int stepSemitones[] = { 0, 2, 4, 5, 7, 9, 11 }; // C D E F G A B

    int octaveOffset = 0;
    int step = linespace;

    // Normalize to 0-6 range
    if (step >= 0)
    {
        octaveOffset = step / 7;
        step = step % 7;
    }
    else
    {
        // Handle negative linespaces
        octaveOffset = (step - 6) / 7; // Floor division
        step = ((step % 7) + 7) % 7;
    }

    return 60 + octaveOffset * 12 + stepSemitones[step];
}

juce::AudioProcessorEditor* HolySheetProcessor::createEditor()
{
    return new HolySheetEditor(*this);
}
