#ifndef MAP_H
#define MAP_H

#include <raylib.h>
#include <vector>

struct Wall {
    Vector3 position;
    Vector3 size;
    Color color;
};

struct Platform {
    Vector3 position;
    Vector3 size;
    Color color;
};

class Map {
public:
    std::vector<Wall> walls;
    std::vector<Platform> platforms;
    Vector3 groundPosition;
    Vector2 groundSize;
    
    Map();
    void loadCyberpunkArena();
    void draw();
    bool checkCollision(Vector3 playerPos, float playerRadius, Vector3& correction);
    float getGroundHeight(Vector3 position);
};

#endif // MAP_H
