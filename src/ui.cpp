#include "ui.h"
#include <raylib.h>
#include <string>

void UI::drawCrosshair(int screenWidth, int screenHeight) {
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;
    int size = 8;
    int gap = 6;
    int thickness = 2;
    
    Color crosshairColor = (Color){ 0, 255, 100, 200 };
    
    // Top
    DrawRectangle(centerX - thickness/2, centerY - gap - size, thickness, size, crosshairColor);
    // Bottom
    DrawRectangle(centerX - thickness/2, centerY + gap, thickness, size, crosshairColor);
    // Left
    DrawRectangle(centerX - gap - size, centerY - thickness/2, size, thickness, crosshairColor);
    // Right
    DrawRectangle(centerX + gap, centerY - thickness/2, size, thickness, crosshairColor);
    
    // Center dot
    DrawCircle(centerX, centerY, 1, crosshairColor);
}

void UI::drawReticle(int screenWidth, int screenHeight, bool shooting) {
    if (shooting) {
        // Expand reticle when shooting
        drawCrosshair(screenWidth, screenHeight);
    } else {
        drawCrosshair(screenWidth, screenHeight);
    }
}

void UI::drawGunHUD(int ammo, int maxAmmo, int screenWidth, int screenHeight) {
    int hudX = screenWidth - 150;
    int hudY = screenHeight - 80;
    
    // Ammo display
    DrawRectangle(hudX - 10, hudY - 10, 140, 70, Fade((Color){ 0, 0, 0, 255 }, 0.7f));
    DrawRectangleLines(hudX - 10, hudY - 10, 140, 70, (Color){ 0, 255, 255, 255 });
    
    // Ammo text
    DrawText(TextFormat("%d", ammo), hudX, hudY, 40, 
             ammo > 10 ? (Color){ 0, 255, 100, 255 } : RED);
    DrawText(TextFormat("/ %d", maxAmmo), hudX + 60, hudY + 15, 20, LIGHTGRAY);
    
    // Ammo label
    DrawText("AMMO", hudX + 20, hudY + 45, 10, DARKGRAY);
}

void UI::drawHealthBar(float health, float maxHealth, int screenWidth, int screenHeight) {
    int barWidth = 300;
    int barHeight = 30;
    int barX = 30;
    int barY = screenHeight - 60;
    
    float healthPercent = health / maxHealth;
    
    // Outer frame with cyberpunk glow
    DrawRectangle(barX - 5, barY - 5, barWidth + 10, barHeight + 10, 
                  (Color){ 10, 10, 15, 200 });
    DrawRectangleLines(barX - 5, barY - 5, barWidth + 10, barHeight + 10, 
                      (Color){ 0, 255, 255, 150 });
    DrawRectangleLines(barX - 4, barY - 4, barWidth + 8, barHeight + 8, 
                      (Color){ 0, 255, 255, 80 });
    
    // Background with grid pattern
    DrawRectangle(barX, barY, barWidth, barHeight, (Color){ 20, 20, 30, 230 });
    
    // Draw grid lines for cyberpunk aesthetic
    for (int i = 0; i <= 10; i++) {
        int x = barX + (barWidth * i / 10);
        DrawLine(x, barY, x, barY + barHeight, (Color){ 40, 40, 50, 100 });
    }
    
    // Determine health color with smooth transitions
    Color healthColor;
    Color glowColor;
    if (healthPercent > 0.6f) {
        healthColor = (Color){ 0, 255, 150, 255 };
        glowColor = (Color){ 0, 255, 150, 100 };
    } else if (healthPercent > 0.3f) {
        healthColor = (Color){ 255, 200, 0, 255 };
        glowColor = (Color){ 255, 200, 0, 100 };
    } else {
        healthColor = (Color){ 255, 50, 50, 255 };
        glowColor = (Color){ 255, 50, 50, 100 };
    }
    
    // Calculate filled width
    int filledWidth = (int)(barWidth * healthPercent);
    
    // Draw health bar with gradient effect
    if (filledWidth > 0) {
        // Base health bar
        DrawRectangle(barX, barY, filledWidth, barHeight, healthColor);
        
        // Glow effect on top edge
        DrawRectangle(barX, barY, filledWidth, 3, Fade(WHITE, 0.6f));
        
        // Side glow
        DrawRectangle(barX - 2, barY, 2, barHeight, Fade(glowColor, 0.5f));
        if (filledWidth < barWidth) {
            DrawRectangle(barX + filledWidth, barY, 2, barHeight, Fade(glowColor, 0.8f));
        }
    }
    
    // Segmented overlay (creates sectioned appearance)
    for (int i = 1; i < 10; i++) {
        int segX = barX + (barWidth * i / 10);
        DrawRectangle(segX - 1, barY, 2, barHeight, (Color){ 10, 10, 15, 180 });
    }
    
    // Border with double-line effect
    DrawRectangleLines(barX, barY, barWidth, barHeight, (Color){ 0, 255, 255, 255 });
    DrawRectangleLines(barX + 1, barY + 1, barWidth - 2, barHeight - 2, 
                      (Color){ 0, 200, 255, 150 });
    
    // Health text with shadow
    DrawText(TextFormat("%.0f", health), barX + 8, barY + 6, 20, (Color){ 0, 0, 0, 200 });
    DrawText(TextFormat("%.0f", health), barX + 7, barY + 5, 20, WHITE);
    
    // Max health indicator
    DrawText(TextFormat("/ %.0f", maxHealth), barX + 60, barY + 9, 14, (Color){ 150, 150, 170, 255 });
    
    // "HEALTH" label with glow
    DrawText("HEALTH", barX + barWidth - 65, barY + 9, 12, (Color){ 0, 255, 255, 200 });
    
    // Low health warning pulse
    if (healthPercent < 0.25f) {
        float pulse = (sinf(GetTime() * 8.0f) + 1.0f) * 0.5f; // Pulsing effect
        DrawRectangle(barX - 5, barY - 5, barWidth + 10, barHeight + 10, 
                     Fade((Color){ 255, 0, 0, 255 }, pulse * 0.3f));
    }
}

void UI::drawWalletInfo(bool connected, const std::string& address, double balance) {
    int x = 20;
    int y = 20;
    int width = 300;
    int height = connected ? 85 : 60;
    
    DrawRectangle(x, y, width, height, Fade((Color){ 20, 20, 30, 255 }, 0.8f));
    DrawRectangleLines(x, y, width, height, (Color){ 138, 43, 226, 255 });
    
    DrawText("WALLET", x + 10, y + 10, 12, (Color){ 138, 43, 226, 255 });
    
    if (connected) {
        DrawText("Status: CONNECTED", x + 10, y + 30, 10, (Color){ 0, 255, 100, 255 });
        
        if (!address.empty()) {
            std::string truncated = address.length() > 20 
                ? address.substr(0, 8) + "..." + address.substr(address.length() - 6)
                : address;
            DrawText(truncated.c_str(), x + 10, y + 48, 9, LIGHTGRAY);
        }
        
        DrawText(TextFormat("%.4f SOL", balance / 1000000000.0), x + 10, y + 65, 10, 
                (Color){ 0, 200, 255, 255 });
    } else {
        DrawText("Status: NOT CONNECTED", x + 10, y + 30, 10, RED);
        DrawText("Press C to connect", x + 10, y + 45, 9, YELLOW);
    }
}

void UI::drawControls() {
    int x = 20;
    int y = 120;
    
    DrawRectangle(x, y, 200, 150, Fade((Color){ 20, 20, 30, 255 }, 0.7f));
    DrawRectangleLines(x, y, 200, 150, (Color){ 0, 200, 255, 255 });
    
    DrawText("CONTROLS", x + 10, y + 8, 10, (Color){ 0, 200, 255, 255 });
    DrawText("WASD - Move", x + 10, y + 25, 9, LIGHTGRAY);
    DrawText("SHIFT - Sprint", x + 10, y + 40, 9, LIGHTGRAY);
    DrawText("C - Crouch", x + 10, y + 55, 9, LIGHTGRAY);
    DrawText("SPACE - Jump", x + 10, y + 70, 9, LIGHTGRAY);
    DrawText("MOUSE - Look", x + 10, y + 85, 9, LIGHTGRAY);
    DrawText("LEFT CLICK - Shoot", x + 10, y + 100, 9, LIGHTGRAY);
    DrawText("R - Reload", x + 10, y + 115, 9, LIGHTGRAY);
    DrawText("ESC - Unlock Cursor", x + 10, y + 130, 9, LIGHTGRAY);
}
