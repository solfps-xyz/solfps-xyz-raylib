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
    int barWidth = 200;
    int barHeight = 20;
    int barX = 20;
    int barY = screenHeight - 40;
    
    float healthPercent = health / maxHealth;
    
    // Background
    DrawRectangle(barX, barY, barWidth, barHeight, (Color){ 40, 40, 40, 200 });
    
    // Health bar
    Color healthColor = healthPercent > 0.5f ? (Color){ 0, 255, 100, 255 } : 
                       healthPercent > 0.25f ? (Color){ 255, 200, 0, 255 } : RED;
    DrawRectangle(barX, barY, (int)(barWidth * healthPercent), barHeight, healthColor);
    
    // Border
    DrawRectangleLines(barX, barY, barWidth, barHeight, (Color){ 0, 255, 255, 255 });
    
    // Text
    DrawText(TextFormat("HP: %.0f", health), barX + 5, barY + 2, 16, WHITE);
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
    
    DrawRectangle(x, y, 200, 135, Fade((Color){ 20, 20, 30, 255 }, 0.7f));
    DrawRectangleLines(x, y, 200, 135, (Color){ 0, 200, 255, 255 });
    
    DrawText("CONTROLS", x + 10, y + 8, 10, (Color){ 0, 200, 255, 255 });
    DrawText("WASD - Move", x + 10, y + 25, 9, LIGHTGRAY);
    DrawText("SHIFT - Sprint", x + 10, y + 40, 9, LIGHTGRAY);
    DrawText("SPACE - Jump", x + 10, y + 55, 9, LIGHTGRAY);
    DrawText("MOUSE - Look", x + 10, y + 70, 9, LIGHTGRAY);
    DrawText("LEFT CLICK - Shoot", x + 10, y + 85, 9, LIGHTGRAY);
    DrawText("R - Reload", x + 10, y + 100, 9, LIGHTGRAY);
    DrawText("ESC - Unlock Cursor", x + 10, y + 115, 9, LIGHTGRAY);
}
