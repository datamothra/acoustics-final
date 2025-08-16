# F-Zero GBA Psychoacoustic Demo

A Game Boy Advance demo exploring psychoacoustic spatial audio techniques inspired by F-Zero Maximum Velocity. This project analyzes the GBA's audio capabilities and limitations through interactive experimentation.

## Project Overview

This demo recreates F-Zero's approach to spatial audio where passing cars use just two samples (front/back) with level and panning adjustments. Additionally, it implements an enhanced ILD (Interaural Level Difference) mode to test if head shadow effects can improve spatial perception on the GBA.

## Features

### Audio Modes

1. **Standard Mode**: Traditional stereo panning with volume attenuation
   - Front/back car samples
   - Linear panning based on horizontal position
   - Distance-based volume attenuation

2. **ILD Mode**: Enhanced spatial audio with head shadow simulation
   - Asymmetric level differences between ears
   - Additional attenuation for sounds behind the listener
   - Simulates natural head shadow effects

### Debug Features

- **Real-time Audio Parameters Display**:
  - Current audio mode (Standard/ILD)
  - Active sample (Front/Back)
  - Pan value (-1 to 1)
  - Volume level (0 to 1)
  - Individual left/right channel levels
  - Distance to nearest car

- **Visual Feedback**:
  - Player car position indicator
  - Opponent car positions with perspective
  - Direction indicators (^ for cars ahead, v for cars behind)

## Controls

- **D-Pad**: Move player car (left/right for position, up/down for speed)
- **A Button**: Toggle between Standard and ILD audio modes
- **B Button**: Toggle debug display on/off
- **Start**: (Reserved for pause functionality)

## Technical Implementation

### Psychoacoustic Principles

1. **Standard Panning**: Uses equal-power panning law
   - `Left = cos(pan_angle) * volume`
   - `Right = sin(pan_angle) * volume`

2. **ILD Implementation**: 
   - Asymmetric attenuation based on sound source position
   - Head shadow factor: 0.6x attenuation for opposite ear
   - Additional 0.7x attenuation for sounds behind listener

### Code Structure

```
src/
├── main.cpp           # Main game loop and demo logic
include/
├── common_info.h      # Common definitions
├── common_variable_8x16_sprite_font.h  # Font definitions
graphics/
├── (sprite assets)
audio/
├── car_front.wav      # Sample for cars ahead
├── car_back.wav       # Sample for cars behind
```

## Building the Project

### Prerequisites

1. **Butano**: C++ GBA development framework
   - Install to `/Users/drewatz/Code/butano/`
   
2. **devkitARM**: GBA toolchain
   - Install via devkitPro installer

### Build Instructions

Due to the space in the directory name, use the provided build script:

```bash
# Make the build script executable (first time only)
chmod +x build.sh

# Build the project
./build.sh
```

The build script creates a temporary symlink to handle the space in "Acoustics Final" and produces `psychoacoustic_demo.gba`.

### Manual Build (Alternative)

If you prefer manual building:

```bash
# Create a symlink without spaces
ln -s "/Users/drewatz/Code/Acoustics Final" /tmp/acoustics_final

# Build from the symlink
cd /tmp/acoustics_final
make clean && make

# Copy the ROM back
cp psychoacoustic_demo.gba "/Users/drewatz/Code/Acoustics Final/"
```

## Testing & Analysis

### Verification Methods

1. **Visual Verification**:
   - Check debug values match expected calculations
   - Verify smooth transitions as cars move
   - Confirm mode switching changes parameters correctly

2. **Audio Testing** (when audio is implemented):
   - Compare perceived position with visual position
   - Test effectiveness of ILD vs standard panning
   - Evaluate head shadow simulation quality

### Expected Results

- **Standard Mode**: Should provide basic left-right localization
- **ILD Mode**: Should provide enhanced spatial perception with:
  - Better front/back discrimination
  - More natural-sounding positioning
  - Improved immersion through head shadow effects

## Psychoacoustic Analysis

### GBA Audio Limitations

1. **Hardware**: 
   - 2 DirectSound channels (8-bit PCM)
   - 4 PSG channels (for effects)
   - Limited processing power for real-time DSP

2. **Psychoacoustic Workarounds**:
   - Using sample switching (front/back) instead of filtering
   - Pre-baked audio characteristics in samples
   - Efficient ILD calculations without complex HRTF

### Findings

The demo demonstrates that even with limited hardware, effective spatial audio can be achieved through:
- Strategic sample selection
- Careful parameter calculation
- Psychoacoustic principles (ILD, level differences)

## Future Enhancements

- [ ] Add actual audio samples (car engine sounds)
- [ ] Implement Doppler effect simulation
- [ ] Add more opponent cars for complex scenarios
- [ ] Create racing track visualization
- [ ] Add performance metrics (CPU usage, frame timing)
- [ ] Implement HRTF-inspired filtering (within GBA limits)

## Mobile Development

This project is configured for mobile agent development. The repository structure and build system allow for:
- Remote development and testing
- Automated builds via GitHub Actions
- Cross-platform compatibility

## Credits

- Inspired by F-Zero Maximum Velocity's audio design
- Built with Butano framework by GValiente
- Psychoacoustic principles based on spatial audio research

## License

[Add your license here]

---

**Note**: This is an educational project exploring the GBA's psychoacoustic capabilities. The visual debugging features allow for self-verification of the audio system's behavior even without actual audio playback.