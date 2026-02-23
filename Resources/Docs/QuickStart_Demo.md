# Sonant Demo - 5 Minute Quick Start

## Do You Need .Wav Files?

**No!** The demo uses Unreal Engine's built-in sounds. You can add custom audio later.

---

## Step 1: Enable Plugin (30 seconds)

1. Copy `Sonant` folder to your project's `Plugins/` directory
2. Open your project
3. Edit > Plugins > Audio > **Enable Sonant**
4. Restart when prompted

---

## Step 2: Create Config (1 minute)

1. Content Browser > Right-click > **Miscellaneous > Data Asset**
2. Select `SonantConfig`
3. Name: `DA_Sonant_Demo`

### Quick Config Setup:

**Keyword Map** (add these 5 entries):
| Key | Sounds Map Entry |
|-----|------------------|
| `Grass` | Tag: `Sonant.Event.Footstep` → Sound: `/Engine/EditorSounds/Notifications/Notification01` |
| `Metal` | Tag: `Sonant.Event.Footstep` → Sound: `/Engine/EditorSounds/Notifications/Notification02` |
| `Stone` | Tag: `Sonant.Event.Footstep` → Sound: `/Engine/EditorSounds/Notifications/Notification03` |
| `Wood` | Tag: `Sonant.Event.Footstep` → Sound: `/Engine/EditorSounds/Notifications/Notification04` |
| `Glass` | Tag: `Sonant.Event.Footstep` → Sound: `/Engine/EditorSounds/Notifications/Notification05` |

---

## Step 3: Register Config (30 seconds)

1. Edit > Project Settings > Plugins > **Sonant Audio**
2. Set `Main Config` to `DA_Sonant_Demo`

---

## Step 4: Add Gameplay Tags (1 minute)

1. Project Settings > Gameplay Tags
2. Add these tags:
   - `Sonant.Event.Footstep`
   - `Sonant.Event.Impact.Light`
   - `Sonant.Event.Impact.Heavy`

---

## Step 5: Create Demo Level (2 minutes)

### Create Level
1. File > New Level > Default
2. Save as `L_Sonant_Demo`

### Add Demo Actor
1. Drag `ASonantFeatureDemo` into level
2. Set properties:
   - `PlatformMesh` = `/Engine/BasicShapes/Cube`
   - `BaseMaterial` = `/Engine/BasicShapes/BasicShapeMaterial`
   - `ProjectileMesh` = `/Engine/BasicShapes/Sphere`

### Add Input (Optional)
Copy the contents of `Plugins/Sonant/Resources/DemoInput.ini` into your `Config/DefaultInput.ini`

---

## Step 6: Test! (1 minute)

### Press Play and test:

| Action | How | What to Hear |
|--------|-----|--------------|
| **Surface Types** | Walk on colored platforms | Different notification sound per color |
| **Impacts** | Press Q or E | Projectile impacts with different forces |
| **Atmospheres** | Walk into colored boxes | (If configured) Mix changes |
| **Reverb** | Jump in different rooms | (If configured) Room size changes echo |

---

## Default Controls (if input configured)

| Key | Action |
|-----|--------|
| **1-5** | Test specific surface type |
| **Q** | Light impact test |
| **E** | Heavy impact test |
| **R** | Cycle through surfaces |
| **T** | Print current surface info |
| **0** | Toggle auto-demo mode |

---

## What You See

### Surface Showcase
```
[GREEN] [ORANGE] [GRAY] [BROWN] [CYAN]
 Grass   Metal   Stone   Wood   Glass
  (1)     (2)     (3)    (4)     (5)
```

### Atmosphere Zones
```
[    OUTDOOR    ] [  CAVE  ] [ UNDERWATER ]
     (Normal)      (Echo)      (Muffled)
```

### Reverb Rooms
```
[Closet] [Room] [Hall] [Cavern]
  2.5m    5m    10m     20m
```

---

## Troubleshooting

### "No sounds at all"
- Check `AudioModulation` plugin is enabled
- Verify `DA_Sonant_Demo` is assigned in Project Settings
- Check Output Log for warnings

### "Same sound everywhere"
- Material names must contain keywords (Grass, Metal, etc.)
- Look at the demo actor's spawned platforms - they're named `M_Demo_[SurfaceType]_[Index]`

### "No impact sounds"
- Ensure Q/E keys are bound (check Input settings)
- Projectiles need time to fall and hit

---

## Next: Add Your Own Audio

Once the demo works:

1. Record or download footstep sounds
2. Create Sound Cues in `/Game/Audio/Sonant/`
3. Update `DA_Sonant_Demo` to point to your sounds
4. Adjust volume and pitch randomization

See `DemoSetupGuide.md` for full details.

---

## That's It!

You now have a working contextual audio system. The demo proves:
- ✅ Surface detection works
- ✅ Physics impacts work
- ✅ Atmosphere zones work
- ✅ Auto reverb works

**Total setup time: ~5 minutes** (without custom audio)
