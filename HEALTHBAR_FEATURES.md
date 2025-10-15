# Health Bar UI System

## Overview
A cyberpunk-styled health bar system with visual feedback and regeneration mechanics.

## Features Implemented

### 1. **Player Health System**
- Added to `Player` class:
  - `health` - Current health (starts at 100.0)
  - `maxHealth` - Maximum health (100.0)
  - `lastDamageTime` - Timestamp of last damage taken
  - `damageFlashTimer` - Controls damage flash effect duration

### 2. **Health Bar UI** (`ui.cpp`)
- **Cyberpunk Design:**
  - Segmented bar with 10 sections
  - Grid pattern background for tech aesthetic
  - Double-line cyan border with glow effect
  - Dynamic health color:
    - Green (>60% health) - Safe
    - Yellow (30-60% health) - Caution
    - Red (<30% health) - Critical
  
- **Visual Effects:**
  - Glow effects on edges and top
  - Smooth gradient on health bar
  - Low health warning pulse (when <25%)
  - Health value displayed with shadow
  - "HEALTH" label with cyan glow
  
- **Dimensions:**
  - Width: 300px
  - Height: 30px
  - Position: Bottom-left (30px from edges)

### 3. **Damage System**
- `takeDamage(float damage)` - Reduces health and triggers visual feedback
- Prevents health from going below 0
- Triggers damage flash overlay

### 4. **Health Regeneration**
- Automatic regeneration after 3 seconds of no damage
- Regenerates 10 HP per second
- Stops at max health

### 5. **Visual Feedback**
- **Damage Flash Overlay:**
  - Red screen flash when taking damage
  - Fades out over 0.3 seconds
  - Red vignette effect around screen edges
  - 8-step gradient for smooth vignette

### 6. **Testing**
- Press **T** key to take 15 damage (for testing)
- Watch health regenerate after 3 seconds
- Observe color changes and effects

## Technical Details

### Health Bar Rendering Order
1. Outer frame with glow
2. Background with grid pattern
3. Health fill with gradient
4. Segmented overlays
5. Border effects
6. Text labels

### Color Scheme
- Frame: Cyan (#00FFFF)
- Background: Dark blue-grey (#141E1E)
- Health colors:
  - Safe: Bright green (#00FF96)
  - Warning: Yellow-orange (#FFC800)
  - Critical: Red (#FF3232)

### Performance
- All effects use efficient rectangle/line drawing
- No texture loading required
- Minimal performance impact

## Integration
The health system is fully integrated with:
- Player class (health tracking)
- Main game loop (regeneration updates)
- UI rendering system (HUD display)
- Damage feedback (screen overlays)

## Future Enhancements
- Shield/armor system
- Health pickups
- Different damage types
- Health bar animations
- Death/respawn system
- Damage direction indicators
