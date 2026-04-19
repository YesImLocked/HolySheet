#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "NoteMapper.h"
#include <vector>

struct HeldNote;
struct PlacedNote;

class StaffRenderer
{
public:
    StaffRenderer();

    void draw(juce::Graphics& g, juce::Rectangle<int> bounds,
              const std::vector<HeldNote>& liveNotes,
              const std::vector<PlacedNote>& placedNotes,
              int hoveredLinespace);

    // Convert a mouse Y position to a staff linespace
    int yToLinespace(float staffTop, float y) const;

    // Get the staff top Y for a given bounds (so editor can use it)
    float getStaffTop(juce::Rectangle<int> bounds) const;

    static constexpr float staffLineSpacing = 12.0f;

private:
    static constexpr float noteRadius = 5.5f;
    static constexpr float sharpOffsetX = -16.0f;
    static constexpr float noteAreaStartX = 80.0f;

    void drawStaffLines(juce::Graphics& g, juce::Rectangle<int> bounds, float staffTop);
    void drawClef(juce::Graphics& g, float x, float staffTop);
    void drawNote(juce::Graphics& g, float centerX, float staffTop,
                  const StaffNote& note, juce::Colour colour);
    void drawLedgerLines(juce::Graphics& g, float centerX, float staffTop, int linespace);
    void drawHoverGhost(juce::Graphics& g, float staffTop, int linespace,
                        juce::Rectangle<int> bounds);
    void drawNoteLabel(juce::Graphics& g, float centerX, float staffTop,
                       const StaffNote& note, juce::Colour colour);

    float linespaceToY(float staffTop, int linespace) const;
};
