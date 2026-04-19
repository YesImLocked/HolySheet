cmake_minimum_required(VERSION 3.22)
project(HolySheet VERSION 0.1.0)

# Add JUCE as a subdirectory
add_subdirectory(JUCE)

# Plugin target
juce_add_plugin(HolySheet
    COMPANY_NAME "Fermin"
    IS_SYNTH TRUE                       # Loads as channel in FL Studio to receive MIDI
    NEEDS_MIDI_INPUT TRUE               # Receives MIDI from piano roll
    NEEDS_MIDI_OUTPUT TRUE              # Outputs MIDI from notes placed on the staff
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    PLUGIN_MANUFACTURER_CODE Frmn
    PLUGIN_CODE Holy
    FORMATS VST3                        # VST3 format for FL Studio
    PRODUCT_NAME "HolySheet"
)

# Source files
target_sources(HolySheet
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/StaffRenderer.cpp
)

# JUCE modules we need
target_link_libraries(HolySheet
    PRIVATE
        juce::juce_audio_utils
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Definitions
target_compile_definitions(HolySheet
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)
