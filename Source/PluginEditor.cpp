#include "PluginEditor.h"
#include "PluginProcessor.h"

HolySheetEditor::HolySheetEditor(HolySheetProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(600, 300);
    setResizable(true, true);
    setResizeLimits(400, 200, 1200, 600);
}

void HolySheetEditor::paint(juce::Graphics& g)
{
    auto notes = processor.getHeldNotes();
    staffRenderer.draw(g, getLocalBounds(), notes);

    // Title
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.setFont(juce::Font(12.0f));
    g.drawText("HolySheet v0.1", getLocalBounds().removeFromBottom(20),
               juce::Justification::centredRight, false);
}

void HolySheetEditor::resized()
{
}

void HolySheetEditor::handleAsyncUpdate()
{
    repaint();
}

// Wire up the editor creation in the processor
juce::AudioProcessorEditor* HolySheetProcessor::createEditor()
{
    return new HolySheetEditor(*this);
}
