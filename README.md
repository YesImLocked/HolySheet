# HolySheet

A VST3 plugin that renders MIDI as sheet music notation in real time — right inside your DAW.

Built for FL Studio. No more exporting from MuseScore. Write music where you make music.

## What it does

- **Load it as an instrument** in FL Studio (or any VST3-compatible DAW)
- **Play notes** from the piano roll — they appear as notation on a treble clef staff
- **Sharps** are displayed in orange next to the note
- **Note names** (C4, F#5, etc.) shown below the staff
- **Ledger lines** drawn automatically for notes above or below the staff
- **Resizable** plugin window

## Building from source

### Prerequisites

- **Windows 10/11**
- **Visual Studio 2022** (Community is fine) with the **"Desktop development with C++"** workload
- **CMake** 3.22+

### Clone

```bash
git clone --recurse-submodules https://github.com/YesImLocked/HolySheet.git
cd HolySheet
```

If you already cloned without `--recurse-submodules`:
```bash
git submodule update --init
```

### Build

```bash
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

The VST3 bundle will be at:
```
build/HolySheet_artefacts/Release/VST3/HolySheet.vst3
```

### Install

Copy the `HolySheet.vst3` folder to:
```
C:\Program Files\Common Files\VST3\
```

Then rescan plugins in your DAW.

## Project structure

```
HolySheet/
├── CMakeLists.txt             # Build configuration
├── JUCE/                      # JUCE framework (git submodule)
└── Source/
    ├── NoteMapper.h           # MIDI note number → staff position + accidental
    ├── PluginProcessor.h/cpp  # MIDI input handling, note tracking
    ├── PluginEditor.h/cpp     # Plugin window, repaint scheduling
    └── StaffRenderer.h/cpp    # Staff drawing, clef, noteheads, ledger lines
```

## Roadmap

- [ ] Note editing (click staff to place notes, push to piano roll)
- [ ] Bass clef / grand staff
- [ ] Key signature support
- [ ] Rhythm and duration display
- [ ] Playback cursor synced to DAW transport
- [ ] Beam grouping and stem direction
- [ ] Dynamic markings

## License

MIT
