#pragma once

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"

#include "components/simple_scene.h"
class Tank {
    public:
        Tank(float startX, float startY);
        ~Tank();
        void Init();
        void setX(float newX);
        void setY(float newY);
        float getX();
        float getY();
        float getRotation();
        float setRotation(float x);
        float turretEndX;
        float turretEndY;
        const std::unordered_map<std::string, Mesh*>& getMeshes() const;
        void setMeshes(const std::unordered_map<std::string, Mesh*>& newMeshes);

    private:
    float x;
    float y;
    float rotation;

    std::unordered_map<std::string, Mesh*> meshes;

    float bottomWidth;
    float topWidth;
    float height;
    float circleRadius;

    glm::vec3 baseColor;
    glm::vec3 turretColor;

    float speed;
};