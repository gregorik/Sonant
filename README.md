<div align=""center"">

# 🔊 Sonant Core

**Zero-Setup Native Contextual Audio Manager for Unreal Engine 5**

[![Unreal Engine](https://img.shields.io/badge/Unreal_Engine-5.7+-blue.svg?logo=unrealengine&logoColor=white)](https://www.unrealengine.com)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)

</div>

---

![SonantPro1](https://github.com/user-attachments/assets/879a77b2-07c9-425b-bc73-23c1a7cb3b91)


## 🚀 No more Physical Material Grind

Tired of manually creating and assigning UPhysicalMaterial assets to every single mesh, landscape layer, and prop in your game just to get the right footstep sound?

**Sonant Core** reinvents audio workflow by introducing **Keyword-Based Surface Detection**. Instead of relying on brittle physics materials, Sonant intelligently scans the names of the visual materials your game is *already using* to instantly deduce the correct surface type. 

If your character steps on a mesh with a material named M_WetGrass_01, Sonant knows to play grass footsteps. It's that simple.

[Discord support](https://discord.gg/nqYQ5mtmHb)

---

## ✨ Core Features

### 🔍 Keyword Surface Detection
Define simple string keywords like "Grass", "Metal", or "Wood" in a central Data Asset. Sonant will automatically match any material hit by a raycast to your defined surface profiles. No more physics material setup required.

### ⚡ Smart Caching Engine
Material lookups happen once. Sonant caches the resolved surface type for every material it encounters, ensuring that subsequent impacts on the same material cost **0.0ms**.

### 🧩 Blueprint Native & Data-Driven
Everything is driven by standard UDataAsset configurations and FGameplayTag events. Easily call PlayFootstep or PlayImpact from your animation notifies or physics events.

---

## 🛠️ Quick Setup

1. **Create a Configuration:** Right-click in your Content Browser -> Miscellaneous -> Data Asset -> SonantCoreConfig.
2. **Define Surfaces:** Add a new Surface Definition. Set the tag (e.g., Sonant.Surface.Wood), add a keyword ("Wood"), and assign your Sound Cue or MetaSound.
3. **Assign to Settings:** Open **Project Settings -> Sonant Core** and assign your new Data Asset.
4. **Trigger Audio:** In your character's blueprint, use the **Sonant Core Subsystem** node to call PlayFootstep and pass in the Hit Result from a line trace.

---

<br>

<div align="center">
<p><i>Sonant Core is useful, but it's just the doorstep. If interested, Sonant Pro can transform your game into a living, breathing acoustic experience.</i></p>
</div>

| Feature | Sonant Core (Free) | Sonant Pro |
| :--- | :---: | :---: |
| **Keyword Surface Detection** | ✅ | ✅ |
| **Smart Caching** | ✅ | ✅ |
| **Dynamic Raycast Reverb** | ❌ | ✅ *(Real-time room size calculation)* |
| **Atmosphere Stacking** | ❌ | ✅ *(Priority-based overlapping audio zones)* |
| **Multiplayer Ready** | ❌ | ✅ *(Dedicated Network Component & RPCs)* |
| **Velocity-Driven Foley** | ❌ | ✅ *(Auto clothing rustle based on speed)* |
| **Spline & Volumetric Audio** | ❌ | ✅ *(Recommended for rivers and roads)* |
| **Surface-Synced VFX & Decals** | ❌ | ✅ *(Spawn bullet holes/dust with audio)* |

### 🚀 **[Get Sonant Pro on Fab.com](https://www.fab.com/listings/0d59694e-e827-48d0-b0fc-d9a25b3d6968)**
*Stop fighting the engine and start designing your soundscape.*

---

## 📄 License
Sonant Core is released under the MIT License. See \LICENSE\ for details. 
*Copyright (c) 2026 GregOrigin. All Rights Reserved.*
