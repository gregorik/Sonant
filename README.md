# Sonant 0.1
A high-performance Gameplay Audio Manager that treats audio as Logic, not just Content. It runs entirely as a native Game Instance Subsystem.

CORE FEATURES:

1. INTELLIGENT SURFACE DETECTION (The "Zero-Config" Engine)
Sonant "reads" your specific context. Instead of relying solely on Physical Materials, Sonant scans the distinct name of the Material Interface (e.g., M_Temple_Stone_Wet) and matches it against a keyword registry you define.

    The Workflow: Map the keyword "Stone" to your Stone Sound Pack once. Now, every material in your project with "Stone" in the name works instantly.

    The Performance: Surface resolution is cached at runtime. The string search runs only once per material asset. Every subsequent footstep is a generic

    O(1) pointer lookup.

2. PRIORITY-BASED ATMOSPHERES (The "Wwise-Lite" Mixer)
Solved: The "Cave inside a Forest" problem.
Sonant replaces standard Sound Mix Modifiers with a Weighted Priority Stack.

    Tag your volumes (e.g., Indoor, Underwater, Combat).

    Assign a Priority Integer to each tag in the Config.

    Sonant automatically calculates the highest-priority active tag and blends the correct Audio Modulation Control Bus instantly. No visual scripting required.

3. NATIVE C++ PERFORMANCE
Your game thread is precious. Sonant adds zero components to your Character. It contains no ticking actors. All logic is event-driven via the C++ Subsystem.

4. METASOUND READY
Fully compatible with UE 5.7+ MetaSounds. Drive procedural footsteps (walk vs. run) by plugging a MetaSoundSource directly into the Sonant registry.
