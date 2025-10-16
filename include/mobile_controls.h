#ifndef MOBILE_CONTROLS_H
#define MOBILE_CONTROLS_H

#include <raylib.h>
#include <raymath.h>

class MobileControls {
public:
    // Joystick state
    Vector2 joystickCenter;
    Vector2 joystickCurrent;
    float joystickRadius;
    float joystickDeadzone;
    bool joystickActive;
    int joystickTouchId;
    
    // Look controls (right side touch)
    Vector2 lookTouchStart;
    Vector2 lookTouchCurrent;
    bool lookTouchActive;
    int lookTouchId;
    float lookSensitivity;
    
    // Action buttons
    Rectangle shootButton;
    Rectangle jumpButton;
    Rectangle reloadButton;
    Rectangle crouchButton;
    Rectangle sprintButton;
    
    bool shootPressed;
    bool jumpPressed;
    bool reloadPressed;
    bool crouchPressed;
    bool sprintPressed;
    
    int shootTouchId;
    int jumpTouchId;
    int reloadTouchId;
    int crouchTouchId;
    int sprintTouchId;
    
    // Constructor
    MobileControls();
    
    // Update controls based on touch input
    void update(int screenWidth, int screenHeight);
    
    // Get movement vector from joystick (-1 to 1 for x and z)
    Vector2 getMovementVector();
    
    // Get look delta (how much to rotate camera)
    Vector2 getLookDelta();
    
    // Draw the mobile controls UI
    void draw(int screenWidth, int screenHeight);
    
private:
    void updateJoystick();
    void updateLookTouch(int screenWidth);
    void updateButtons();
    void drawJoystick();
    void drawButtons();
    
    // Helper to check if touch is in a specific area
    bool isTouchInRect(Vector2 touchPos, Rectangle rect);
    bool isTouchInCircle(Vector2 touchPos, Vector2 center, float radius);
};

#endif // MOBILE_CONTROLS_H
