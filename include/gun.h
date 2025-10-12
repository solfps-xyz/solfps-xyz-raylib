#ifndef GUN_H
#define GUN_H

#include <raylib.h>

class Gun {
public:
    Model model;
    Vector3 position;
    Vector3 rotation;
    float bobOffset;
    float bobSpeed;
    float recoilAngle;
    bool isRecoiling;
    
    Gun();
    ~Gun();
    void update(float deltaTime, bool isMoving, bool isShooting);
    void draw(Camera3D camera);
    void drawSimple(Camera3D camera); // Simple gun for now without model
    void applyRecoil();
};

#endif // GUN_H
