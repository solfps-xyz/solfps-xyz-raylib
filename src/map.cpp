#include "map.h"
#include <raylib.h>
#include <raymath.h>

Map::Map() {
    groundPosition = (Vector3){ 0.0f, 0.0f, 0.0f };
    groundSize = (Vector2){ 50.0f, 50.0f };
}

void Map::loadCyberpunkArena() {
    walls.clear();
    platforms.clear();
    
    // Outer walls - Create an arena
    float arenaSize = 40.0f;
    float wallHeight = 5.0f;
    float wallThickness = 1.0f;
    
    Color neonPurple = (Color){ 138, 43, 226, 255 };
    Color neonCyan = (Color){ 0, 255, 255, 255 };
    Color darkGray = (Color){ 40, 40, 50, 255 };
    
    // North wall
    walls.push_back({
        (Vector3){ 0.0f, wallHeight/2, -arenaSize/2 },
        (Vector3){ arenaSize, wallHeight, wallThickness },
        neonPurple
    });
    
    // South wall
    walls.push_back({
        (Vector3){ 0.0f, wallHeight/2, arenaSize/2 },
        (Vector3){ arenaSize, wallHeight, wallThickness },
        neonPurple
    });
    
    // East wall
    walls.push_back({
        (Vector3){ arenaSize/2, wallHeight/2, 0.0f },
        (Vector3){ wallThickness, wallHeight, arenaSize },
        neonCyan
    });
    
    // West wall
    walls.push_back({
        (Vector3){ -arenaSize/2, wallHeight/2, 0.0f },
        (Vector3){ wallThickness, wallHeight, arenaSize },
        neonCyan
    });
    
    // Interior obstacles - Create cover spots
    // Center platform
    platforms.push_back({
        (Vector3){ 0.0f, 1.0f, 0.0f },
        (Vector3){ 6.0f, 2.0f, 6.0f },
        (Color){ 70, 70, 90, 255 }
    });
    
    // Corner cover - NorthWest
    walls.push_back({
        (Vector3){ -12.0f, 2.0f, -12.0f },
        (Vector3){ 5.0f, 4.0f, 1.5f },
        darkGray
    });
    
    // Corner cover - NorthEast
    walls.push_back({
        (Vector3){ 12.0f, 2.0f, -12.0f },
        (Vector3){ 5.0f, 4.0f, 1.5f },
        darkGray
    });
    
    // Corner cover - SouthWest
    walls.push_back({
        (Vector3){ -12.0f, 2.0f, 12.0f },
        (Vector3){ 5.0f, 4.0f, 1.5f },
        darkGray
    });
    
    // Corner cover - SouthEast
    walls.push_back({
        (Vector3){ 12.0f, 2.0f, 12.0f },
        (Vector3){ 5.0f, 4.0f, 1.5f },
        darkGray
    });
    
    // Side pillars for cover
    for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {
            walls.push_back({
                (Vector3){ i * 8.0f, 3.0f, j * 8.0f },
                (Vector3){ 2.0f, 6.0f, 2.0f },
                (Color){ 50, 50, 70, 255 }
            });
        }
    }
    
    // Elevated platforms for tactical advantage
    platforms.push_back({
        (Vector3){ -15.0f, 2.5f, 0.0f },
        (Vector3){ 4.0f, 0.5f, 8.0f },
        (Color){ 60, 60, 80, 255 }
    });
    
    platforms.push_back({
        (Vector3){ 15.0f, 2.5f, 0.0f },
        (Vector3){ 4.0f, 0.5f, 8.0f },
        (Color){ 60, 60, 80, 255 }
    });
}

void Map::draw() {
    // Draw ground with grid
    DrawPlane(groundPosition, groundSize, (Color){ 30, 30, 40, 255 });
    
    // Draw neon grid
    for (int i = -25; i <= 25; i += 2) {
        DrawLine3D((Vector3){ (float)i, 0.01f, -25.0f }, (Vector3){ (float)i, 0.01f, 25.0f }, 
                   (Color){ 0, 200, 255, 50 });
        DrawLine3D((Vector3){ -25.0f, 0.01f, (float)i }, (Vector3){ 25.0f, 0.01f, (float)i }, 
                   (Color){ 0, 200, 255, 50 });
    }
    
    // Draw walls
    for (const auto& wall : walls) {
        DrawCube(wall.position, wall.size.x, wall.size.y, wall.size.z, wall.color);
        Color wireColor = {
            (unsigned char)Clamp(wall.color.r + 50, 0, 255),
            (unsigned char)Clamp(wall.color.g + 50, 0, 255),
            (unsigned char)Clamp(wall.color.b + 50, 0, 255),
            255
        };
        DrawCubeWires(wall.position, wall.size.x, wall.size.y, wall.size.z, wireColor);
    }
    
    // Draw platforms
    for (const auto& platform : platforms) {
        DrawCube(platform.position, platform.size.x, platform.size.y, platform.size.z, platform.color);
        DrawCubeWires(platform.position, platform.size.x, platform.size.y, platform.size.z, 
                     (Color){ 100, 255, 255, 255 });
    }
}

bool Map::checkCollision(Vector3 playerPos, float playerRadius, Vector3& correction) {
    bool collided = false;
    correction = (Vector3){ 0.0f, 0.0f, 0.0f };
    
    // Check wall collisions
    for (const auto& wall : walls) {
        // Simple AABB collision
        Vector3 halfSize = Vector3Scale(wall.size, 0.5f);
        Vector3 minBounds = Vector3Subtract(wall.position, halfSize);
        Vector3 maxBounds = Vector3Add(wall.position, halfSize);
        
        // Expand bounds by player radius
        minBounds.x -= playerRadius;
        minBounds.z -= playerRadius;
        maxBounds.x += playerRadius;
        maxBounds.z += playerRadius;
        
        if (playerPos.x >= minBounds.x && playerPos.x <= maxBounds.x &&
            playerPos.z >= minBounds.z && playerPos.z <= maxBounds.z &&
            playerPos.y <= maxBounds.y) {
            
            collided = true;
            
            // Calculate push-out vector
            float overlapX = fmin(maxBounds.x - playerPos.x, playerPos.x - minBounds.x);
            float overlapZ = fmin(maxBounds.z - playerPos.z, playerPos.z - minBounds.z);
            
            if (overlapX < overlapZ) {
                correction.x = (playerPos.x < wall.position.x) ? -overlapX : overlapX;
            } else {
                correction.z = (playerPos.z < wall.position.z) ? -overlapZ : overlapZ;
            }
        }
    }
    
    return collided;
}

float Map::getGroundHeight(Vector3 position) {
    // Check if on a platform
    for (const auto& platform : platforms) {
        Vector3 halfSize = Vector3Scale(platform.size, 0.5f);
        Vector3 minBounds = Vector3Subtract(platform.position, halfSize);
        Vector3 maxBounds = Vector3Add(platform.position, halfSize);
        
        if (position.x >= minBounds.x && position.x <= maxBounds.x &&
            position.z >= minBounds.z && position.z <= maxBounds.z) {
            return maxBounds.y;
        }
    }
    
    return 0.0f; // Ground level
}
