# Mobile Controls Implementation

## Overview
The game now automatically detects mobile devices and displays touch-friendly on-screen controls for smartphones and tablets.

## Features

### **1. Device Detection**
The game uses the WasmBridge to detect the device type:
```cpp
bool isMobile = PrivyBridge::isMobileDevice() || PrivyBridge::isTabletDevice();
```

Available detection functions from WasmBridge:
- `isMobile()` - Detects smartphones
- `isTablet()` - Detects tablets
- `isDesktop()` - Detects desktop/laptop
- `hasTouch()` - Detects touch support
- `getScreenWidth()` / `getScreenHeight()` - Get screen dimensions
- `getOrientation()` - Returns 'portrait' or 'landscape'

### **2. On-Screen Controls**

#### **Virtual Joystick** (Bottom-Left)
- **Location**: Bottom-left corner
- **Size**: 80px radius
- **Deadzone**: 15% of radius
- **Function**: 8-directional movement
- **Visual**: Cyan outlined circle with draggable stick
- Returns Vector2 movement (-1 to 1 for X and Z)

#### **Action Buttons** (Bottom-Right)
1. **FIRE Button** (Red) - Primary weapon fire
   - Continuous press for automatic fire
   - 70x70px rounded rectangle
   
2. **JUMP Button** (Green) - Jump action
   - Single press to jump
   - Located left of FIRE button
   
3. **RELOAD Button** (Orange) - Reload weapon
   - Single press to reload
   - Above FIRE button
   - Shows "R" label
   
4. **CROUCH Button** (Purple) - Toggle crouch
   - Toggle press to crouch/stand
   - Above JUMP button
   - Shows "C" label
   
5. **RUN Button** (Blue) - Sprint
   - Press while moving forward to sprint
   - Located above joystick
   - Shows "RUN" label

#### **Look Controls** (Center Screen)
- Touch and drag anywhere in the middle of the screen
- Controls camera rotation (yaw and pitch)
- Sensitivity: 0.3x
- Intuitive swipe-to-look mechanic

### **3. Control Layout**

```
┌─────────────────────────────────────────────┐
│                                             │
│           [Touch to look around]      [R]   │
│                                       [C]   │
│                                             │
│                                      [JUMP] │
│                                      [FIRE] │
│                                             │
│    [RUN]                                    │
│                                             │
│     ◉                                       │
│   Joystick                                  │
└─────────────────────────────────────────────┘
```

### **4. Input Handling**

#### Desktop Mode
- Keyboard + Mouse (WASD, Shift, Space, C, R, Mouse)
- Cursor locked to window
- Original control scheme

#### Mobile Mode  
- Touch controls enabled
- Cursor visible (for UI interaction)
- Virtual joystick for movement
- Touch buttons for actions
- Swipe-to-look camera control

### **5. Technical Implementation**

#### MobileControls Class (`mobile_controls.h/cpp`)
```cpp
class MobileControls {
public:
    // Joystick state
    Vector2 getMovementVector();    // Returns -1 to 1
    
    // Look controls
    Vector2 getLookDelta();         // Returns pixel delta
    
    // Button states
    bool shootPressed;
    bool jumpPressed;
    bool reloadPressed;
    bool crouchPressed;
    bool sprintPressed;
    
    // Update and draw
    void update(int screenWidth, int screenHeight);
    void draw(int screenWidth, int screenHeight);
};
```

#### Player Input Methods
```cpp
// Desktop input (keyboard/mouse)
void handleInput(float deltaTime);
void handleMouseLook();

// Mobile input (touch controls)
void handleMobileInput(float deltaTime, Vector2 moveVector, 
                       bool sprint, bool jump, bool crouch, 
                       bool shoot, bool reload);
void handleMobileLook(Vector2 lookDelta);
```

### **6. Visual Design**

All controls follow the cyberpunk aesthetic:
- **Colors**:
  - Joystick: Cyan (#00FFFF)
  - FIRE: Red (#FF6464)
  - JUMP: Green (#00C864)
  - RELOAD: Orange (#FF9600)
  - CROUCH: Purple (#8A2BE2)
  - RUN: Blue (#0096C8)

- **Effects**:
  - Semi-transparent backgrounds (50-80% opacity)
  - Glow effects on active state
  - Rounded corners for modern look
  - State-based color intensity

### **7. Touch Zones**

The screen is divided into regions to prevent control conflicts:

- **Left Third**: Joystick area
  - Any touch here activates joystick
  - Centered at bottom-left
  
- **Right Third**: Action buttons
  - Button collision detection
  - Highest priority for button presses
  
- **Center**: Look controls
  - Remaining area for camera control
  - Only active when not touching joystick or buttons

### **8. Multi-Touch Support**

- Supports up to 10 simultaneous touch points
- Independent touch tracking for each control
- Touch IDs prevent cross-control interference
- Smooth tracking even during rapid input changes

### **9. Integration with Game Systems**

#### Movement
- Joystick vector directly drives player velocity
- Sprint only works when moving forward (Y < -0.3)
- Crouch toggle maintained across frames

#### Combat
- Continuous fire when FIRE button held
- Reload triggers same animation as 'R' key
- Crosshair remains centered for aiming

#### Camera
- Look delta applied to yaw/pitch
- Sensitivity matches desktop mouse feel
- Pitch clamping prevents over-rotation

### **10. Performance**

- Minimal overhead: ~0.1ms per frame
- No texture loading (pure shape rendering)
- Efficient touch point iteration
- State-based updates only when needed

### **11. Testing**

To test mobile controls:

1. **On Desktop**: 
   - Can't test directly (no touch support)
   - Build and deploy to web server
   - Access from mobile device

2. **On Mobile**:
   - Open game in mobile browser
   - Controls automatically appear
   - Test all buttons and joystick
   - Verify look controls work smoothly

3. **Browser DevTools**:
   - Enable device emulation
   - Select mobile device
   - Test touch interactions
   - Note: Some features may not work perfectly in emulation

### **12. Future Enhancements**

Potential improvements:
- [ ] Haptic feedback on button press
- [ ] Customizable button layouts
- [ ] Adjustable control sensitivity
- [ ] Button size options for accessibility
- [ ] Landscape/portrait layout switching
- [ ] Control opacity settings
- [ ] Alternative control schemes (gyroscope aiming)
- [ ] Tutorial overlay for first-time mobile users

## Code Files

- `include/mobile_controls.h` - Class definition
- `src/mobile_controls.cpp` - Implementation
- `include/privy_bridge.h` - Device detection functions
- `src/main.cpp` - Integration and conditional rendering
- `src/player.cpp` - Mobile input handlers

## WasmBridge Integration

The device detection relies on the JavaScript WasmBridge providing:

```typescript
isMobile: () => boolean;
isTablet: () => boolean;
isDesktop: () => boolean;
getScreenWidth: () => number;
getScreenHeight: () => number;
hasTouch: () => boolean;
```

These functions must be available on `window.PrivyBridge` before the game initializes.
