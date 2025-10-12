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
    float sprintTilt; // Horizontal tilt when running
    Sound shootSound;
    int currentSoundIndex;
    static const int MAX_SOUND_INSTANCES = 4;
    Sound shootSoundInstances[MAX_SOUND_INSTANCES];
    
    Gun();
    ~Gun();
    void update(float deltaTime, bool isMoving, bool isShooting, Vector3 playerVelocity);
    void draw(Camera3D camera);
    void drawSimple(Camera3D camera); // Simple gun for now without model
    void applyRecoil();
};

#endif // GUN_H
