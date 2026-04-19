#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "NoteMapper.h"
#include <vector>

struct HeldNote;

class StaffRenderer
{
public:
    StaffRenderer();

    void draw(juce::Graphics& g, juce::Rectangle<int> bounds,
              const std::vector<HeldNote>& notes);

private:
    // Staff layout constants
    static constexpr float staffLineSpacing = 12.0f;  // Pixels between staff lines
    static constexpr float noteRadius = 5.5f;
    static constexpr float sharpOffsetX = -16.0f;

    void drawStaffLines(juce::Graphics& g, juce::Rectangle<int> bounds, float staffTop);
    void drawClef(juce::Graphics& g, float x, float staffTop);
    void drawNote(juce::Graphics& g, float centerX, float staffTop, const StaffNote& note);
    void drawLedgerLines(juce::Graphics& g, float centerX, float staffTop, int linespace);

    float linespaceToY(float staffTop, int linespace) const;
};
