#include "StaffRenderer.h"
#include "../Source/PluginProcessor.h"

StaffRenderer::StaffRenderer()
{
}

void StaffRenderer::draw(juce::Graphics& g, juce::Rectangle<int> bounds,
                          const std::vector<HeldNote>& notes)
{
    g.fillAll(juce::Colour(30, 30, 35));  // Dark background

    float staffTop = bounds.getCentreY() - (2.0f * staffLineSpacing);

    // Draw staff lines
    drawStaffLines(g, bounds, staffTop);

    // Draw treble clef indicator
    drawClef(g, bounds.getX() + 20.0f, staffTop);

    if (notes.empty())
        return;

    // Sort notes by pitch for display
    auto sorted = notes;
    std::sort(sorted.begin(), sorted.end(),
        [](const HeldNote& a, const HeldNote& b) { return a.midiNote < b.midiNote; });

    // Draw each note, spaced horizontally
    float startX = bounds.getX() + 80.0f;
    float spacing = 35.0f;

    // If too many notes, compress spacing
    float totalWidth = sorted.size() * spacing;
    float availableWidth = bounds.getWidth() - 100.0f;
    if (totalWidth > availableWidth && sorted.size() > 1)
        spacing = availableWidth / (float)sorted.size();

    for (size_t i = 0; i < sorted.size(); ++i)
    {
        float x = startX + i * spacing;
        auto staffNote = midiToStaffNote(sorted[i].midiNote);
        drawNote(g, x, staffTop, staffNote);
    }
}

void StaffRenderer::drawStaffLines(juce::Graphics& g, juce::Rectangle<int> bounds, float staffTop)
{
    g.setColour(juce::Colours::white.withAlpha(0.7f));

    for (int i = 0; i < 5; ++i)
    {
        float y = staffTop + i * staffLineSpacing;
        g.drawHorizontalLine((int)y, (float)bounds.getX() + 10.0f,
                             (float)bounds.getRight() - 10.0f);
    }
}

void StaffRenderer::drawClef(juce::Graphics& g, float x, float staffTop)
{
    // Draw a simplified treble clef using text
    g.setColour(juce::Colours::white.withAlpha(0.9f));
    g.setFont(juce::Font(staffLineSpacing * 5.5f));

    // Unicode treble clef: U+1D11E or we use a simpler approach
    // Use the musical symbol if available, otherwise draw "G"
    juce::String clefText = juce::CharPointer_UTF8("\xf0\x9d\x84\x9e");

    // Fallback: check if the font can render it
    auto font = g.getCurrentFont();
    if (font.getStringWidth(clefText) == 0)
        clefText = "G";  // Fallback

    g.drawText(clefText,
               (int)x, (int)(staffTop - staffLineSpacing * 1.5f),
               40, (int)(staffLineSpacing * 7),
               juce::Justification::centredLeft, false);
}

void StaffRenderer::drawNote(juce::Graphics& g, float centerX, float staffTop,
                              const StaffNote& note)
{
    float y = linespaceToY(staffTop, note.linespace);

    // Draw ledger lines if needed
    drawLedgerLines(g, centerX, staffTop, note.linespace);

    // Draw the note head (filled ellipse)
    g.setColour(juce::Colours::white);
    g.fillEllipse(centerX - noteRadius, y - noteRadius * 0.8f,
                  noteRadius * 2.0f, noteRadius * 1.6f);

    // Draw sharp symbol if needed
    if (note.sharp)
    {
        g.setColour(juce::Colours::orange);
        g.setFont(juce::Font(staffLineSpacing * 1.5f));
        g.drawText("#",
                   (int)(centerX + sharpOffsetX), (int)(y - staffLineSpacing * 0.75f),
                   14, (int)(staffLineSpacing * 1.5f),
                   juce::Justification::centred, false);
    }

    // Draw note name below the staff
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.setFont(juce::Font(10.0f));
    static const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int midiNote = 60 + note.linespace; // Approximate - this is for display only
    // Reconstruct from linespace and sharp
    int step = ((note.linespace % 7) + 7) % 7;
    static const int stepToSemitone[] = { 0, 2, 4, 5, 7, 9, 11 };
    int semitone = stepToSemitone[step] + (note.sharp ? 1 : 0);
    juce::String label = juce::String(noteNames[semitone % 12]) + juce::String(note.octave);
    g.drawText(label,
               (int)(centerX - 15), (int)(staffTop + staffLineSpacing * 5.5f),
               30, 14,
               juce::Justification::centred, false);
}

void StaffRenderer::drawLedgerLines(juce::Graphics& g, float centerX, float staffTop,
                                     int linespace)
{
    g.setColour(juce::Colours::white.withAlpha(0.6f));

    // Staff lines correspond to linespaces: E4(2), G4(4), B4(6), D5(8), F5(10)
    // But in our system: line 0 of staff = top line (F5, linespace=10)
    // Actually let's think in terms of Y positions.
    // Staff bottom line = E4 = linespace 2
    // Staff top line = F5 = linespace 10
    // Middle C = linespace 0, needs one ledger line below

    // Ledger lines below staff (linespace <= 1)
    // linespace 0 = middle C, needs ledger at linespace 0
    // linespace -2 = A3, needs ledger lines at 0 and -2
    if (linespace <= 1)
    {
        for (int ls = 0; ls >= linespace; ls -= 2)
        {
            float y = linespaceToY(staffTop, ls);
            g.drawHorizontalLine((int)y, centerX - noteRadius - 4, centerX + noteRadius + 4);
        }
    }

    // Ledger lines above staff (linespace >= 11)
    if (linespace >= 11)
    {
        for (int ls = 12; ls <= linespace; ls += 2)
        {
            float y = linespaceToY(staffTop, ls);
            g.drawHorizontalLine((int)y, centerX - noteRadius - 4, centerX + noteRadius + 4);
        }
    }
}

float StaffRenderer::linespaceToY(float staffTop, int linespace) const
{
    // Staff lines from bottom to top: E4(2), G4(4), B4(6), D5(8), F5(10)
    // staffTop is the Y of the top line (F5, linespace 10)
    // Each linespace step = half a staffLineSpacing in Y
    // Higher linespace = higher pitch = lower Y value

    float topLineLinespace = 10.0f;  // F5
    return staffTop + (topLineLinespace - linespace) * (staffLineSpacing / 2.0f);
}
