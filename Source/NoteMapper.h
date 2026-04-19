#pragma once

// Maps MIDI note numbers to staff positions and accidentals.
// Position 0 = middle C (ledger line below treble staff).
// Positive = up, negative = down.

struct StaffNote
{
    int linespace;    // Position on the staff (0 = middle C, 1 = D, 2 = E, ...)
    bool sharp;       // Whether this note needs a sharp symbol
    int octave;       // MIDI octave
};

inline StaffNote midiToStaffNote(int midiNote)
{
    // C  C# D  D# E  F  F# G  G# A  A# B
    // Maps each pitch class to a diatonic step (0-6) and whether it's sharp
    struct PitchMapping { int step; bool sharp; };

    static constexpr PitchMapping table[12] = {
        {0, false}, // C
        {0, true},  // C#
        {1, false}, // D
        {1, true},  // D#
        {2, false}, // E
        {3, false}, // F
        {3, true},  // F#
        {4, false}, // G
        {4, true},  // G#
        {5, false}, // A
        {5, true},  // A#
        {6, false}, // B
    };

    int octave = (midiNote / 12) - 1;       // MIDI octave (-1 to 9)
    int pitchClass = midiNote % 12;
    auto mapping = table[pitchClass];

    // linespace: each octave has 7 diatonic steps
    // Middle C (MIDI 60) = octave 4, step 0 => linespace 0
    int linespace = (octave - 4) * 7 + mapping.step;

    return { linespace, mapping.sharp, octave };
}
