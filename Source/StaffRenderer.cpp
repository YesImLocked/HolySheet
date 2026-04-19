#include "StaffRenderer.h"
#include "PluginProcessor.h"

StaffRenderer::StaffRenderer()
{
}

void StaffRenderer::draw(juce::Graphics& g, juce::Rectangle<int> bounds,
                          const std::vector<HeldNote>& liveNotes,
                          const std::vector<PlacedNote>& placedNotes,
                          int hoveredLinespace)
{
    g.fillAll(juce::Colour(30, 30, 35));

    float staffTop = getStaffTop(bounds);

    drawStaffLines(g, bounds, staffTop);
    drawClef(g, bounds.getX() + 20.0f, staffTop);

    // Draw hover ghost (semi-transparent note where mouse is)
    if (hoveredLinespace > -999)
        drawHoverGhost(g, staffTop, hoveredLinespace, bounds);

    // Draw placed notes (user-written) in cyan
    float placedStartX = noteAreaStartX;
    float spacing = 35.0f;

    if (!placedNotes.empty())
    {
        auto sorted = placedNotes;
        std::sort(sorted.begin(), sorted.end(),
            [](const PlacedNote& a, const PlacedNote& b) { return a.midiNote < b.midiNote; });

        float totalWidth = sorted.size() * spacing;
        float availableWidth = bounds.getWidth() - noteAreaStartX - 20.0f;
        if (totalWidth > availableWidth && sorted.size() > 1)
            spacing = availableWidth / (float)sorted.size();

        for (size_t i = 0; i < sorted.size(); ++i)
        {
            float x = placedStartX + i * spacing;
            auto staffNote = midiToStaffNote(sorted[i].midiNote);
            drawNote(g, x, staffTop, staffNote, juce::Colour(0, 200, 220));
            drawNoteLabel(g, x, staffTop, staffNote, juce::Colour(0, 200, 220).withAlpha(0.6f));
        }
    }

    // Draw live notes (from DAW) in white, offset below the staff area
    if (!liveNotes.empty())
    {
        auto sorted = liveNotes;
        std::sort(sorted.begin(), sorted.end(),
            [](const HeldNote& a, const HeldNote& b) { return a.midiNote < b.midiNote; });

        // Show live notes as a small indicator row at the bottom
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.setFont(juce::FontOptions(11.0f));

        float liveY = bounds.getBottom() - 30.0f;
        g.drawText("LIVE:", bounds.getX() + 10, (int)liveY, 40, 16,
                   juce::Justification::centredLeft);

        float liveX = bounds.getX() + 55.0f;
        for (size_t i = 0; i < sorted.size(); ++i)
        {
            auto staffNote = midiToStaffNote(sorted[i].midiNote);
            static const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
            int pitchClass = sorted[i].midiNote % 12;
            int octave = (sorted[i].midiNote / 12) - 1;
            juce::String label = juce::String(noteNames[pitchClass]) + juce::String(octave);

            g.setColour(juce::Colours::white.withAlpha(0.7f));
            g.drawText(label, (int)(liveX + i * 40), (int)liveY, 35, 16,
                       juce::Justification::centred);
        }
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
    g.setColour(juce::Colours::white.withAlpha(0.9f));
    g.setFont(juce::FontOptions(staffLineSpacing * 5.5f));

    juce::String clefText = juce::CharPointer_UTF8("\xf0\x9d\x84\x9e");
    auto font = g.getCurrentFont();
    if (font.getStringWidth(clefText) == 0)
        clefText = "G";

    g.drawText(clefText,
               (int)x, (int)(staffTop - staffLineSpacing * 1.5f),
               40, (int)(staffLineSpacing * 7),
               juce::Justification::centredLeft, false);
}

void StaffRenderer::drawNote(juce::Graphics& g, float centerX, float staffTop,
                              const StaffNote& note, juce::Colour colour)
{
    float y = linespaceToY(staffTop, note.linespace);

    drawLedgerLines(g, centerX, staffTop, note.linespace);

    // Note head
    g.setColour(colour);
    g.fillEllipse(centerX - noteRadius, y - noteRadius * 0.8f,
                  noteRadius * 2.0f, noteRadius * 1.6f);

    // Sharp
    if (note.sharp)
    {
        g.setColour(juce::Colour(255, 165, 0));
        g.setFont(juce::FontOptions(staffLineSpacing * 1.5f));
        g.drawText("#",
                   (int)(centerX + sharpOffsetX), (int)(y - staffLineSpacing * 0.75f),
                   14, (int)(staffLineSpacing * 1.5f),
                   juce::Justification::centred, false);
    }
}

void StaffRenderer::drawNoteLabel(juce::Graphics& g, float centerX, float staffTop,
                                   const StaffNote& note, juce::Colour colour)
{
    g.setColour(colour);
    g.setFont(juce::FontOptions(10.0f));

    static const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int step = ((note.linespace % 7) + 7) % 7;
    static const int stepToSemitone[] = { 0, 2, 4, 5, 7, 9, 11 };
    int semitone = stepToSemitone[step] + (note.sharp ? 1 : 0);
    juce::String label = juce::String(noteNames[semitone % 12]) + juce::String(note.octave);

    g.drawText(label,
               (int)(centerX - 15), (int)(staffTop + staffLineSpacing * 5.5f),
               30, 14,
               juce::Justification::centred, false);
}

void StaffRenderer::drawHoverGhost(juce::Graphics& g, float staffTop, int linespace,
                                    juce::Rectangle<int> bounds)
{
    float y = linespaceToY(staffTop, linespace);
    float centerX = bounds.getCentreX();

    // Ghost note head
    g.setColour(juce::Colour(0, 200, 220).withAlpha(0.3f));
    g.fillEllipse(centerX - noteRadius, y - noteRadius * 0.8f,
                  noteRadius * 2.0f, noteRadius * 1.6f);

    // Ghost ledger lines
    g.setColour(juce::Colours::white.withAlpha(0.15f));
    if (linespace <= 1)
    {
        for (int ls = 0; ls >= linespace; ls -= 2)
        {
            float ly = linespaceToY(staffTop, ls);
            g.drawHorizontalLine((int)ly, centerX - noteRadius - 4, centerX + noteRadius + 4);
        }
    }
    if (linespace >= 11)
    {
        for (int ls = 12; ls <= linespace; ls += 2)
        {
            float ly = linespaceToY(staffTop, ls);
            g.drawHorizontalLine((int)ly, centerX - noteRadius - 4, centerX + noteRadius + 4);
        }
    }
}

void StaffRenderer::drawLedgerLines(juce::Graphics& g, float centerX, float staffTop,
                                     int linespace)
{
    g.setColour(juce::Colours::white.withAlpha(0.6f));

    if (linespace <= 1)
    {
        for (int ls = 0; ls >= linespace; ls -= 2)
        {
            float y = linespaceToY(staffTop, ls);
            g.drawHorizontalLine((int)y, centerX - noteRadius - 4, centerX + noteRadius + 4);
        }
    }

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
    float topLineLinespace = 10.0f; // F5
    return staffTop + (topLineLinespace - linespace) * (staffLineSpacing / 2.0f);
}

float StaffRenderer::getStaffTop(juce::Rectangle<int> bounds) const
{
    return bounds.getCentreY() - (2.0f * staffLineSpacing) - 15.0f;
}

int StaffRenderer::yToLinespace(float staffTop, float y) const
{
    float topLineLinespace = 10.0f;
    float raw = topLineLinespace - (y - staffTop) / (staffLineSpacing / 2.0f);
    return (int)std::round(raw);
}
