# Sonant 1.0 - Comprehensive Demo System

## Overview

This demo system showcases **all Sonant features** without requiring external audio assets. It uses Unreal Engine's built-in sounds as placeholders that you can replace with your own audio.

---

## What's Included

### New C++ Classes

| Class | Purpose | Location |
|-------|---------|----------|
| `ASonantFeatureDemo` | Main demo environment builder | Source/Sonant/Private/ |
| `ASonantDemoCharacter` | Demo player with test inputs | Source/Sonant/Private/ |

### Documentation

| File | Content |
|------|---------|
| `QuickStart_Demo.md` | 5-minute setup guide |
| `DemoSetupGuide.md` | Complete configuration manual |
| `DemoREADME.md` | This file - overview and reference |
| `DemoInput.ini` | Input bindings for demo controls |

### Existing Demo Actors (Already Available)

| Class | Feature Showcased |
|-------|-------------------|
| `ASonantDemoGenerator` | Procedural surface platforms |
| `ASonantAutoShowcase` | Meteor impact demonstration |
| `ASonantGodRayShowcase` | Volumetric atmosphere demo |
| `ASonantSpectacle` | Infinite runner style showcase |

---

## Demo Features Matrix

| Feature | Demo Component | How to Test |
|---------|---------------|-------------|
| **Surface Detection** | Surface Showcase Platforms | Walk on colored platforms |
| **Keyword Matching** | Material naming | Rename materials, hear changes |
| **Physics Impact** | Projectile Targets | Press Q/E for different forces |
| **Auto Reverb** | 4 Sized Rooms | Jump in different rooms |
| **Atmosphere Zones** | Colored Volumes | Walk into zones |
| **Material Caching** | Repeated footsteps | Fast, consistent playback |
| **Per-Face Materials** | Complex meshes | Trace different mesh sections |

---

## File Structure

```
Plugins/Sonant/
├── Source/Sonant/
│   ├── Public/
│   │   ├── SonantFeatureDemo.h      (NEW)
│   │   ├── SonantDemoCharacter.h    (NEW)
│   │   └── ... existing headers
│   └── Private/
│       ├── SonantFeatureDemo.cpp    (NEW)
│       ├── SonantDemoCharacter.cpp  (NEW)
│       └── ... existing implementations
├── Resources/
│   ├── Docs/
│   │   ├── Sonant_1.0_Manual.html   (Updated with Demo section)
│   │   ├── QuickStart_Demo.md       (NEW)
│   │   ├── DemoSetupGuide.md        (NEW)
│   │   └── DemoREADME.md            (This file)
│   └── DemoInput.ini                (NEW)
└── Sonant.uplugin
```

---

## Quick Commands

### Build the Project
```bash
# From your project root
"C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" SonantDevEditor Win64 Development "SonantDev.uproject"
```

### Run the Demo
1. Open `L_Sonant_Demo` (or create new level)
2. Drag `ASonantFeatureDemo` into level
3. Set mesh properties
4. Press Play

---

## Configuration Summary

### Minimum Required Setup

**1. SonantConfig Data Asset:**
```
Keyword Map:
  - Key: "Grass" → Sound: (any USoundBase)
  - Key: "Metal" → Sound: (any USoundBase)
  - Key: "Stone" → Sound: (any USoundBase)
  - Key: "Wood" → Sound: (any USoundBase)
  - Key: "Glass" → Sound: (any USoundBase)
```

**2. Project Settings:**
```
Sonant Audio > Main Config: (Your SonantConfig asset)
```

**3. Gameplay Tags:**
```
Sonant.Event.Footstep
Sonant.Event.Impact.Light
Sonant.Event.Impact.Heavy
```

That's it! The demo will work with these settings.

---

## Audio Asset Strategy

### Phase 1: Verification (No Custom Audio)
Use engine sounds to verify:
- ✅ Surface detection works
- ✅ Impacts trigger correctly
- ✅ Reverb changes with room size
- ✅ Atmospheres transition properly

### Phase 2: Prototyping (Free Sounds)
Download from:
- Freesound.org
- Sonnis GDC bundles
- Epic Marketplace free monthly

### Phase 3: Production (Custom/Paid)
- Record foley
- Purchase professional packs
- Work with audio designers

---

## Demo Controls Reference

### Keyboard
| Key | Function | Context |
|-----|----------|---------|
| 1-5 | Test surface type | Global |
| Q | Light impact (500 force) | Global |
| E | Heavy impact (3000 force) | Global |
| R | Cycle surfaces | Global |
| T | Print debug info | Global |
| F | Manual footstep | Global |
| 0 | Toggle auto-demo | Global |
| 9 | Rebuild environment | Global |
| WASD | Move | In-character |
| Space | Jump | In-character |

### Console
```
show collision              # Visualize traces
stat game                   # Performance stats
slomo 0.5                   # Slow motion
showdebug sonant            # Debug display (if implemented)
```

---

## Troubleshooting Quick Reference

| Problem | Solution |
|---------|----------|
| No sounds | Check AudioModulation plugin, verify config assigned |
| Same sound everywhere | Check material names contain keywords |
| No impacts | Enable physics simulation on projectiles |
| No reverb | Check visibility collision on room meshes |
| Atmospheres not working | Verify GameplayTags registered |

---

## Extending the Demo

### Add New Surface Type
1. Add entry to `SurfaceStations` array in `ASonantFeatureDemo`
2. Add Keyword Map entry in SonantConfig
3. Rebuild demo (press 9)

### Add New Atmosphere
1. Add `FAtmosphereDemoZone` to `AtmosphereZones` array
2. Configure in SonantConfig > Atmospheres
3. Set priority and Control Bus Mix

### Custom Impact Forces
Modify in `ASonantDemoCharacter::SpawnTestProjectile()`:
```cpp
TArray<float> Forces = {100.0f, 1000.0f, 3000.0f}; // Your forces here
```

---

## Performance Notes

The demo is designed to be lightweight:
- **Material Cache:** O(1) lookup after first hit
- **Reverb Updates:** 2Hz (every 0.5 seconds)
- **Projectile Cleanup:** Auto-destroy after 5 seconds
- **Spawned Actors:** Tracked for easy cleanup

For profiling:
```cpp
// In ASonantFeatureDemo::Tick()
SCOPE_CYCLE_COUNTER(STAT_SonantDemo);
```

---

## Next Steps After Demo

1. **Integrate with Your Character**
   - Add footstep traces to Animation Notifies
   - Bind impact events to physics objects
   - Add atmosphere volumes to level

2. **Customize SonantConfig**
   - Add more surface types
   - Fine-tune volume/pitch per surface
   - Set up reverb presets

3. **Production Audio**
   - Replace placeholder sounds
   - Add variation with Sound Cues
   - Mix and master levels

4. **Optimization**
   - Profile with Unreal Insights
   - Adjust raycast frequency if needed
   - Optimize material naming conventions

---

## Support

- **Documentation:** See `Sonant_1.0_Manual.html`
- **Quick Start:** See `QuickStart_Demo.md`
- **Full Setup:** See `DemoSetupGuide.md`
- **Website:** https://www.gregorigin.com

---

## License

The demo system is part of the Sonant plugin and follows the same licensing terms.

**Enjoy your contextual audio! 🔊**
