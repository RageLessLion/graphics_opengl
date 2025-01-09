#pragma once

#include "components/simple_scene.h"
#include "Tank.h"

struct Projectile {
    glm::vec2 position;
    glm::vec2 velocity;
    float radius;
    bool active;

    Projectile(const glm::vec2& pos, const glm::vec2& vel, float r)
        : position(pos), velocity(vel), radius(r), active(true) {}
};

namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:
        Tema1();
        ~Tema1();
        Tank * tank1;
        Tank * tank2;
        void GenerateTerrain();
        float GetTerrainSlope(float x);
         bool canFire;         
    float fireCooldown;   
    float currentCooldown;
        glm::vec2 CalculateTurretEnd(const glm::vec2& tankPosition, float turretRotation, float turretLength);
        std::vector<Projectile> projectiles;
        std::vector<Projectile> projectiles2;

        float GetTerrainHeight(float x);
        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

     protected:
        float cx, cy;
        glm::mat3 modelMatrix;
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;

        // TODO(student): If you need any other class variables, define them here.

    };
}   // namespace m1
