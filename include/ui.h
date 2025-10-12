#ifndef UI_H
#define UI_H

#include <raylib.h>
#include <string>

class UI {
public:
    static void drawCrosshair(int screenWidth, int screenHeight);
    static void drawGunHUD(int ammo, int maxAmmo, int screenWidth, int screenHeight);
    static void drawWalletInfo(bool connected, const std::string& address, double balance);
    static void drawHealthBar(float health, float maxHealth, int screenWidth, int screenHeight);
    static void drawControls();
    static void drawReticle(int screenWidth, int screenHeight, bool shooting);
};

#endif // UI_H
