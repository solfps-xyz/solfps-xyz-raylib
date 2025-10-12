#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include <raymath.h>

class Player {
public:
    Camera3D camera;
    float yaw;
    float pitch;
    float moveSpeed;
    float sprintSpeed;
    float mouseSensitivity;
    float height;
    Vector3 velocity;
    bool isGrounded;
    bool isSprinting;
    
    // Gun state
    bool isShooting;
    float shootCooldown;
    int ammo;
    int maxAmmo;
    float recoilOffset;
    
    Player();
    void update(float deltaTime);
    void handleInput(float deltaTime);
    void handleMouseLook();
    void applyGravity(float deltaTime);
    void shoot();
    void reload();
    Vector3 getForward();
    Vector3 getRight();
};

#endif // PLAYER_H
