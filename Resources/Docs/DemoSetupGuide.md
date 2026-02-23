# Sonant 1.0 - Comprehensive Demo Setup Guide

## Quick Answer: Do You Need .Wav Files?

**No, you don't need custom .wav files to run the demo.** The demo uses Unreal Engine's built-in sounds as placeholders. However, for the best experience, you should add proper footstep sounds per surface type.

---

## Demo Components Overview

The comprehensive demo includes:

1. **Surface Showcase** - Walkway with 5 surface types (Grass, Metal, Stone, Wood, Glass)
2. **Physics Impact Range** - Targets for light/medium/heavy impact testing
3. **Atmosphere Zones** - Outdoor, Cave, and Underwater zones with transitions
4. **Reverb Rooms** - 4 rooms of different sizes (Closet, Room, Hall, Cavern)
5. **Auto-Demo Mode** - Automated cycling through features

---

## Step 1: Create the SonantConfig Data Asset

### Option A: Using Engine Sounds (No Custom Audio Needed)

1. In Content Browser, right-click → **Miscellaneous > Data Asset**
2. Select **SonantConfig** as the class
3. Name it `DA_Sonant_Demo`

### Configure Keyword Map

| Keyword | Event Tag | Sound (Engine Default) |
|---------|-----------|------------------------|
| Grass | Sonant.Event.Footstep | `/Engine/EditorSounds/Notifications/Notification01` |
| Metal | Sonant.Event.Footstep | `/Engine/EditorSounds/Notifications/Notification02` |
| Stone | Sonant.Event.Footstep | `/Engine/EditorSounds/Notifications/Notification03` |
| Wood | Sonant.Event.Footstep | `/Engine/EditorSounds/Notifications/Notification04` |
| Glass | Sonant.Event.Footstep | `/Engine/EditorSounds/Notifications/Notification05` |

### Configure Impact Events

Add these entries to each surface's Sounds map:
- `Sonant.Event.Impact.Light` → `/Engine/EditorSounds/Notifications/Notification01`
- `Sonant.Event.Impact.Heavy` → `/Engine/EditorSounds/Notifications/Notification05`

### Configure Atmospheres

| Atmosphere Tag | Priority | Mix Asset |
|----------------|----------|-----------|
| Sonant.Atmosphere.Outdoor | 0 | None (default) |
| Sonant.Atmosphere.Forest | 10 | Create Bus Mix with subtle reverb |
| Sonant.Atmosphere.Cave | 50 | Create Bus Mix with heavy reverb |
| Sonant.Atmosphere.Underwater | 100 | Create Bus Mix with low-pass filter |

### Configure Reverb Settings

| Max Room Radius | Reverb Mix |
|-----------------|------------|
| 250 | `Reverb_Small` (Small Room preset) |
| 600 | `Reverb_Medium` (Medium Room preset) |
| 1200 | `Reverb_Large` (Large Room preset) |
| 3000 | `Reverb_Cavern` (Huge space preset) |

---

## Step 2: Create Gameplay Tags

1. **Project Settings > Gameplay Tags**
2. Add these tags:

```
Sonant.Event.Footstep
Sonant.Event.Impact.Light
Sonant.Event.Impact.Heavy
Sonant.Atmosphere.Outdoor
Sonant.Atmosphere.Forest
Sonant.Atmosphere.Cave
Sonant.Atmosphere.Underwater
```

---

## Step 3: Setup the Demo Level

### Create a New Level
1. File > New Level > Default
2. Save as `L_Sonant_Demo`

### Add the Feature Demo Actor
1. Place **ASonantFeatureDemo** in the level
2. Position at origin (0, 0, 0)
3. Configure properties:

```cpp
PlatformMesh = /Engine/BasicShapes/Cube.Cube
BaseMaterial = /Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial
ProjectileMesh = /Engine/BasicShapes/Sphere.Sphere
AutoDemoInterval = 5.0
bAutoCycleSurfaces = false (set to true for auto-demo)
```

### Add a Player Character

Use the included `BP_ThirdPersonCharacter` or create a new one with:

1. **Line trace for footsteps** (in Animation Blueprint or Character):
```
Event: Footstep Notify
  -> Line Trace by Channel (Start: FootSocket, End: Down 50cm)
  -> If Hit: Get Game Instance -> Get Subsystem (Sonant)
  -> Play Footstep (Location: Hit.Location, Surface Hit: Hit)
```

2. **Input binding for manual testing**:
```
Key: 1 -> Play Surface: Grass
Key: 2 -> Play Surface: Metal
Key: 3 -> Play Surface: Stone
Key: 4 -> Play Surface: Wood
Key: 5 -> Play Surface: Glass
Key: Space -> Spawn Projectile (Impact Test)
```

---

## Step 4: Register Config in Project Settings

1. **Edit > Project Settings > Plugins > Sonant Audio**
2. Set **Main Config** to `DA_Sonant_Demo`

---

## Step 5: Test the Demo

### Surface Testing
1. Walk on each colored platform
2. Listen for different sounds per surface
3. Check that material names are detected correctly

### Impact Testing
1. Press Space to spawn physics projectiles
2. Watch them hit impact targets
3. Listen for light/medium/heavy variations based on force

### Atmosphere Testing
1. Walk into the colored zones
2. Check console for atmosphere push/pop
3. Listen for mix changes (if configured)

### Reverb Testing
1. Enter the different sized rooms
2. Jump or make sounds
3. Notice reverb changes based on room size

---

## Using Your Own Audio (Optional Upgrade)

### Where to Get Sounds

**Free Options:**
- **Freesound.org** - Creative Commons sounds
- **Soniss GameDev Audio Bundle** - Free annual GDC giveaway
- **Epic Games Marketplace** - Free monthly content often includes audio
- **Adobe Audition** - Generate procedural sounds

**Paid Options:**
- **Pro Sound Collection** (Unreal Marketplace)
- **Sonniss** - Affordable bundles
- **Boom Library** - Professional quality

### Recommended Structure

```
/Game/Audio/Sonant/
  /Surfaces/
    /Grass/
      S_Grass_Walk_01.wav
      S_Grass_Walk_02.wav
      S_Grass_Run_01.wav
      S_Grass_Land_01.wav
    /Metal/
      S_Metal_Walk_01.wav
      ...
    /Stone/
    /Wood/
    /Glass/
  /Impacts/
    /Light/
    /Heavy/
  /Reverb/
    (Submix effects presets)
```

### Creating Sound Cues

For each surface type:
1. Create a **Sound Cue** (right-click > Sounds > Sound Cue)
2. Add **Random** node for variations
3. Add **Modulator** for pitch variation
4. Set appropriate attenuation

### Updating the Config

Replace engine sounds with your Sound Cues:
1. Open `DA_Sonant_Demo`
2. Update each surface's Sounds map
3. Point to your Sound Cue assets

---

## Demo Checklist

| Feature | Test Method | Expected Result |
|---------|-------------|-----------------|
| Surface Detection | Walk on colored platforms | Different sound per platform |
| Keyword Matching | Rename material, test again | Sound changes with name |
| Physics Impact | Spawn projectiles | Impact sound scales with force |
| Atmosphere Zones | Walk into colored boxes | Mix changes (if configured) |
| Auto Reverb | Enter different sized rooms | Reverb changes with room size |
| Caching | Walk same platform twice | Consistent sound, no hitch |

---

## Troubleshooting Demo Issues

### "All platforms sound the same"
- Check that material names contain keywords (M_Demo_Grass, etc.)
- Verify SonantConfig is assigned in Project Settings
- Check Output Log for "Failed to find object" warnings

### "No impact sounds"
- Ensure projectiles have Simulate Physics enabled
- Check collision responses are set to Block
- Verify impact force exceeds 50.0 threshold

### "Atmospheres not working"
- Check GameplayTags are registered
- Verify Sound Control Bus Mixes exist and are valid
- Ensure priority values are set correctly

### "Reverb not changing"
- Check that AudioModulation plugin is enabled
- Verify rooms have collision on Visibility channel
- Test with `show collision` console command

---

## Console Commands for Testing

```
show collision                    // Visualize trace rays
stat game                         // Check performance
slomo 0.5                         // Slow motion for debugging
showDebug Sonant                  // (If implemented) Debug visualization
```

---

## Next Steps

1. **Customize Surface Types** - Add snow, sand, gravel, etc.
2. **Add More Events** - Sliding, landing, jumping
3. **Integrate with Animation** - Add notifies to your character
4. **Multiplayer Testing** - Verify client-side prediction
5. **Profile Performance** - Use Unreal Insights

---

## Demo Blueprint Code Examples

### BP_DemoController (Player Controller)

Create a Blueprint extending PlayerController with these input actions:

**Input Mapping:**
- 1-5 keys: Test surface types
- Space: Spawn impact projectile
- T: Toggle auto-demo mode
- R: Rebuild demo environment

**Example Blueprint Graph:**
```
Event: Press 1 Key
  -> Get Game Instance
  -> Get Subsystem (Sonant Subsystem)
  -> Play Sound At Location
      Event Tag: Sonant.Event.Footstep
      Location: Get Actor Location
      Surface Hit: (Construct hit result with Grass material)
```

---

## Summary

You **don't need .wav files** to get started. The demo works with engine sounds as placeholders. Once you're happy with the setup, swap in your own audio assets for production quality.

The demo showcases:
- ✅ 5 surface types with automatic detection
- ✅ 3 impact intensities (light/medium/heavy)
- ✅ 3 atmosphere zones with priority mixing
- ✅ 4 reverb room sizes
- ✅ Performance-optimized caching

Run the demo, verify functionality, then upgrade the audio assets!
