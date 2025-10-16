#include "mobile_controls.h"
#include <raylib.h>
#include <raymath.h>
#include <cmath>

// Define max touch points if not already defined
#ifndef MAX_TOUCH_POINTS
#define MAX_TOUCH_POINTS 10
#endif

MobileControls::MobileControls() {
    joystickRadius = 120.0f; // Increased from 80.0f for larger touch area
    joystickDeadzone = 0.15f;
    joystickActive = false;
    joystickTouchId = -1;
    joystickCenter = (Vector2){ joystickRadius + 40.0f, 720.0f - joystickRadius - 40.0f }; // Default position
    joystickCurrent = joystickCenter;
    
    lookTouchActive = false;
    lookTouchId = -1;
    lookSensitivity = 0.8f; // Increased from 0.3f for faster mobile look
    
    shootPressed = false;
    jumpPressed = false;
    reloadPressed = false;
    crouchPressed = false;
    sprintPressed = false;
    
    shootTouchId = -1;
    jumpTouchId = -1;
    reloadTouchId = -1;
    crouchTouchId = -1;
    sprintTouchId = -1;
}

void MobileControls::update(int screenWidth, int screenHeight) {
    // Update joystick position based on screen size
    joystickCenter = (Vector2){ joystickRadius + 40.0f, screenHeight - joystickRadius - 40.0f };
    
    // Update button positions (right side of screen)
    float buttonSize = 70.0f;
    float buttonSpacing = 15.0f;
    float rightMargin = screenWidth - buttonSize - 30.0f;
    
    shootButton = (Rectangle){ rightMargin, screenHeight - buttonSize - 30.0f, buttonSize, buttonSize };
    jumpButton = (Rectangle){ rightMargin - buttonSize - buttonSpacing, screenHeight - buttonSize - 30.0f, buttonSize, buttonSize };
    reloadButton = (Rectangle){ rightMargin, screenHeight - buttonSize * 2 - buttonSpacing - 30.0f, buttonSize, buttonSize };
    crouchButton = (Rectangle){ rightMargin - buttonSize - buttonSpacing, screenHeight - buttonSize * 2 - buttonSpacing - 30.0f, buttonSize, buttonSize };
    sprintButton = (Rectangle){ joystickCenter.x - buttonSize/2, joystickCenter.y - joystickRadius - buttonSize - 20.0f, buttonSize, buttonSize };
    
    updateJoystick();
    updateLookTouch(screenWidth);
    updateButtons();
}

void MobileControls::updateJoystick() {
    int touchCount = GetTouchPointCount();
    
    // Reset if no touches
    if (touchCount == 0) {
        joystickActive = false;
        joystickTouchId = -1;
        return;
    }
    
    // If joystick is active, find the touch in the left half that's closest to the last position
    if (joystickActive && joystickTouchId >= 0) {
        bool touchFound = false;
        float closestDistance = 100.0f; // Max distance to consider same touch
        int closestIndex = -1;
        
        for (int i = 0; i < touchCount && i < MAX_TOUCH_POINTS; i++) {
            Vector2 touchPos = GetTouchPosition(i);
            
            // Only consider touches in the left half
            if (touchPos.x < GetScreenWidth() / 2.0f) {
                float distance = Vector2Distance(touchPos, joystickCurrent);
                
                if (distance < closestDistance) {
                    closestDistance = distance;
                    closestIndex = i;
                    touchFound = true;
                }
            }
        }
        
        if (touchFound && closestIndex >= 0) {
            joystickTouchId = closestIndex;
            joystickCurrent = GetTouchPosition(closestIndex);
        } else {
            // No touch found in left half, deactivate joystick
            joystickActive = false;
            joystickTouchId = -1;
        }
    }
    
    // Check for new joystick touch
    if (!joystickActive) {
        for (int i = 0; i < touchCount && i < MAX_TOUCH_POINTS; i++) {
            Vector2 touchPos = GetTouchPosition(i);
            
            // Check if touch is in left half of screen (joystick area)
            if (touchPos.x < GetScreenWidth() / 2.0f) {
                // Make sure this isn't on a button
                bool onButton = isTouchInRect(touchPos, sprintButton);
                
                if (!onButton) {
                    joystickTouchId = i;
                    joystickActive = true;
                    joystickCurrent = touchPos;
                    break;
                }
            }
        }
    }
}

void MobileControls::updateLookTouch(int screenWidth) {
    int touchCount = GetTouchPointCount();
    
    // Reset if no touches
    if (touchCount == 0) {
        lookTouchActive = false;
        lookTouchId = -1;
        return;
    }
    
    // If look is active, find the touch in the right half that's closest to the last position
    if (lookTouchActive && lookTouchId >= 0) {
        bool touchFound = false;
        float closestDistance = 150.0f; // Max distance to consider same touch
        int closestIndex = -1;
        
        for (int i = 0; i < touchCount && i < MAX_TOUCH_POINTS; i++) {
            Vector2 touchPos = GetTouchPosition(i);
            
            // Only consider touches in the right half, not on buttons
            if (touchPos.x > screenWidth / 2.0f &&
                !isTouchInRect(touchPos, shootButton) &&
                !isTouchInRect(touchPos, jumpButton) &&
                !isTouchInRect(touchPos, reloadButton) &&
                !isTouchInRect(touchPos, crouchButton)) {
                
                float distance = Vector2Distance(touchPos, lookTouchCurrent);
                
                if (distance < closestDistance) {
                    closestDistance = distance;
                    closestIndex = i;
                    touchFound = true;
                }
            }
        }
        
        if (touchFound && closestIndex >= 0) {
            lookTouchId = closestIndex;
            lookTouchCurrent = GetTouchPosition(closestIndex);
        } else {
            // No touch found in right half, deactivate look
            lookTouchActive = false;
            lookTouchId = -1;
        }
    }
    
    // Check for new look touch (right half of screen only, not on buttons)
    if (!lookTouchActive) {
        for (int i = 0; i < touchCount && i < MAX_TOUCH_POINTS; i++) {
            Vector2 touchPos = GetTouchPosition(i);
            
            // Right half of screen - not on buttons, not joystick
            if (touchPos.x > screenWidth / 2.0f &&
                !isTouchInRect(touchPos, shootButton) &&
                !isTouchInRect(touchPos, jumpButton) &&
                !isTouchInRect(touchPos, reloadButton) &&
                !isTouchInRect(touchPos, crouchButton)) {
                
                lookTouchId = i;
                lookTouchActive = true;
                lookTouchStart = touchPos;
                lookTouchCurrent = touchPos;
                break;
            }
        }
    }
}

void MobileControls::updateButtons() {
    int touchCount = GetTouchPointCount();
    
    // Reset all button states
    bool newShootPressed = false;
    bool newJumpPressed = false;
    bool newReloadPressed = false;
    bool newCrouchPressed = false;
    bool newSprintPressed = false;
    
    // Check all touches against buttons
    for (int i = 0; i < touchCount && i < MAX_TOUCH_POINTS; i++) {
        Vector2 touchPos = GetTouchPosition(i);
        
        if (isTouchInRect(touchPos, shootButton)) {
            newShootPressed = true;
            shootTouchId = i;
        }
        if (isTouchInRect(touchPos, jumpButton)) {
            newJumpPressed = true;
            jumpTouchId = i;
        }
        if (isTouchInRect(touchPos, reloadButton)) {
            newReloadPressed = true;
            reloadTouchId = i;
        }
        if (isTouchInRect(touchPos, crouchButton)) {
            newCrouchPressed = true;
            crouchTouchId = i;
        }
        if (isTouchInRect(touchPos, sprintButton)) {
            newSprintPressed = true;
            sprintTouchId = i;
        }
    }
    
    // Update button states
    shootPressed = newShootPressed;
    jumpPressed = newJumpPressed;
    reloadPressed = newReloadPressed;
    crouchPressed = newCrouchPressed;
    sprintPressed = newSprintPressed;
    
    // Reset touch IDs if not pressed
    if (!newShootPressed) shootTouchId = -1;
    if (!newJumpPressed) jumpTouchId = -1;
    if (!newReloadPressed) reloadTouchId = -1;
    if (!newCrouchPressed) crouchTouchId = -1;
    if (!newSprintPressed) sprintTouchId = -1;
}

Vector2 MobileControls::getMovementVector() {
    if (!joystickActive) {
        return (Vector2){ 0.0f, 0.0f };
    }
    
    Vector2 delta = Vector2Subtract(joystickCurrent, joystickCenter);
    float distance = Vector2Length(delta);
    
    // Apply deadzone
    if (distance < joystickRadius * joystickDeadzone) {
        return (Vector2){ 0.0f, 0.0f };
    }
    
    // Normalize and clamp to joystick radius
    if (distance > joystickRadius) {
        distance = joystickRadius;
    }
    
    // Convert to -1 to 1 range
    Vector2 normalized = Vector2Scale(delta, 1.0f / joystickRadius);
    
    return normalized;
}

Vector2 MobileControls::getLookDelta() {
    if (!lookTouchActive) {
        return (Vector2){ 0.0f, 0.0f };
    }
    
    Vector2 delta = Vector2Subtract(lookTouchCurrent, lookTouchStart);
    
    // Update start position for continuous look
    lookTouchStart = lookTouchCurrent;
    
    // Apply sensitivity
    return Vector2Scale(delta, lookSensitivity);
}

void MobileControls::draw(int screenWidth, int screenHeight) {
    // Draw screen divider line (subtle indicator of left/right zones)
    int centerX = screenWidth / 2;
    DrawLine(centerX, 0, centerX, screenHeight, Fade((Color){ 0, 255, 255, 255 }, 0.1f));
    
    // Draw zone labels (semi-transparent)
    DrawText("MOVE", 60, 60, 14, Fade((Color){ 0, 255, 255, 255 }, 0.3f));
    DrawText("LOOK", screenWidth - 100, 60, 14, Fade((Color){ 0, 255, 255, 255 }, 0.3f));
    
    drawJoystick();
    drawButtons();
}

void MobileControls::drawJoystick() {
    // Outer circle (joystick boundary)
    DrawCircle(joystickCenter.x, joystickCenter.y, joystickRadius, 
               Fade((Color){ 20, 20, 30, 255 }, 0.5f));
    DrawCircleLines(joystickCenter.x, joystickCenter.y, joystickRadius, 
                   Fade((Color){ 0, 255, 255, 255 }, joystickActive ? 0.8f : 0.6f));
    
    // Deadzone indicator
    DrawCircleLines(joystickCenter.x, joystickCenter.y, joystickRadius * joystickDeadzone,
                   Fade((Color){ 100, 100, 120, 255 }, 0.3f));
    
    // Inner stick
    if (joystickActive) {
        Vector2 stickPos = joystickCurrent;
        Vector2 delta = Vector2Subtract(stickPos, joystickCenter);
        float distance = Vector2Length(delta);
        
        // Clamp stick to radius
        if (distance > joystickRadius) {
            delta = Vector2Scale(Vector2Normalize(delta), joystickRadius);
            stickPos = Vector2Add(joystickCenter, delta);
        }
        
        // Glow effect when active
        DrawCircle(stickPos.x, stickPos.y, 55.0f, Fade((Color){ 0, 255, 255, 255 }, 0.2f));
        DrawCircle(stickPos.x, stickPos.y, 50.0f, // Increased from 35.0f
                  Fade((Color){ 0, 255, 255, 255 }, 0.7f));
        DrawCircleLines(stickPos.x, stickPos.y, 50.0f, // Increased from 35.0f
                       (Color){ 0, 255, 255, 255 });
    } else {
        DrawCircle(joystickCenter.x, joystickCenter.y, 45.0f, // Increased from 30.0f
                  Fade((Color){ 0, 200, 255, 255 }, 0.4f));
        DrawCircleLines(joystickCenter.x, joystickCenter.y, 45.0f, // Increased from 30.0f
                       Fade((Color){ 0, 255, 255, 255 }, 0.5f));
    }
    
    // Direction indicators
    DrawLine(joystickCenter.x, joystickCenter.y - joystickRadius + 5, 
             joystickCenter.x, joystickCenter.y - joystickRadius - 10,
             Fade((Color){ 0, 255, 100, 255 }, 0.4f)); // Up
    
    // Visual indicator when look touch is active (right side)
    if (lookTouchActive) {
        DrawCircle(lookTouchCurrent.x, lookTouchCurrent.y, 8.0f, 
                  Fade((Color){ 0, 255, 255, 255 }, 0.5f));
        DrawCircleLines(lookTouchCurrent.x, lookTouchCurrent.y, 12.0f, 
                       Fade((Color){ 0, 255, 255, 255 }, 0.7f));
        DrawCircleLines(lookTouchCurrent.x, lookTouchCurrent.y, 20.0f, 
                       Fade((Color){ 0, 255, 255, 255 }, 0.3f));
    }
}

void MobileControls::drawButtons() {
    // Shoot button (largest, primary action)
    Color shootColor = shootPressed ? (Color){ 255, 50, 50, 255 } : (Color){ 255, 100, 100, 255 };
    DrawRectangleRounded(shootButton, 0.3f, 8, Fade(shootColor, shootPressed ? 0.8f : 0.5f));
    DrawRectangleRoundedLines(shootButton, 0.3f, 8, 
                             Fade((Color){ 255, 200, 200, 255 }, shootPressed ? 1.0f : 0.7f));
    DrawText("FIRE", shootButton.x + 12, shootButton.y + 25, 16, WHITE);
    
    // Jump button
    Color jumpColor = jumpPressed ? (Color){ 0, 255, 100, 255 } : (Color){ 0, 200, 100, 255 };
    DrawRectangleRounded(jumpButton, 0.3f, 8, Fade(jumpColor, jumpPressed ? 0.8f : 0.5f));
    DrawRectangleRoundedLines(jumpButton, 0.3f, 8,
                             Fade((Color){ 150, 255, 200, 255 }, jumpPressed ? 1.0f : 0.7f));
    DrawText("JUMP", jumpButton.x + 10, jumpButton.y + 25, 14, WHITE);
    
    // Reload button
    Color reloadColor = reloadPressed ? (Color){ 255, 200, 0, 255 } : (Color){ 255, 150, 0, 255 };
    DrawRectangleRounded(reloadButton, 0.3f, 8, Fade(reloadColor, reloadPressed ? 0.8f : 0.5f));
    DrawRectangleRoundedLines(reloadButton, 0.3f, 8,
                             Fade((Color){ 255, 220, 150, 255 }, reloadPressed ? 1.0f : 0.7f));
    DrawText("R", reloadButton.x + 25, reloadButton.y + 25, 20, WHITE);
    
    // Crouch button
    Color crouchColor = crouchPressed ? (Color){ 138, 43, 226, 255 } : (Color){ 100, 30, 180, 255 };
    DrawRectangleRounded(crouchButton, 0.3f, 8, Fade(crouchColor, crouchPressed ? 0.8f : 0.5f));
    DrawRectangleRoundedLines(crouchButton, 0.3f, 8,
                             Fade((Color){ 200, 150, 255, 255 }, crouchPressed ? 1.0f : 0.7f));
    DrawText("C", crouchButton.x + 25, crouchButton.y + 25, 20, WHITE);
    
    // Sprint button (above joystick)
    Color sprintColor = sprintPressed ? (Color){ 0, 200, 255, 255 } : (Color){ 0, 150, 200, 255 };
    DrawRectangleRounded(sprintButton, 0.3f, 8, Fade(sprintColor, sprintPressed ? 0.8f : 0.5f));
    DrawRectangleRoundedLines(sprintButton, 0.3f, 8,
                             Fade((Color){ 150, 220, 255, 255 }, sprintPressed ? 1.0f : 0.7f));
    DrawText("RUN", sprintButton.x + 15, sprintButton.y + 25, 14, WHITE);
}

bool MobileControls::isTouchInRect(Vector2 touchPos, Rectangle rect) {
    return CheckCollisionPointRec(touchPos, rect);
}

bool MobileControls::isTouchInCircle(Vector2 touchPos, Vector2 center, float radius) {
    return Vector2Distance(touchPos, center) <= radius;
}
