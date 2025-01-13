#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include <iostream>

const float minX = -230.0f;
const float maxX = 230.0f;
const float minZ = -230.0f;
const float maxZ = 230.0f;

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
};

namespace drone
{
    class Drone
    {
    public:
        Drone()
        {
            position = glm::vec3(0, 5, 5);
            forward = glm::vec3(0, 0, -1);
            right = glm::vec3(1, 0, 0);
            up = glm::vec3(0, 1, 0);
            distanceToTarget = 1;
            y_rotation = 1.0f;
            x_rotation = 1.0f;
            z_rotation = 1.0f;
            radius;
            isDroneInsidePickUpCircle = false;
            isDroneInsideDeliveryCircle = false;
            parcel;
            packetsDelivered = 0;
            fuel = 0.05;
            propellerRotation = 10;
        }

        Drone(const glm::vec3 &position)
        {
            Set(position, up);
        }

        ~Drone()
        {
        }

        void Set(const glm::vec3 &position, const glm::vec3 &up)
        {
            this->position = position;
            this->up = up;
        }

        void TranslateForward(float distance)
        {
            position += glm::normalize(forward) * distance;
        }

        void TranslateRight(float distance)
        {
            position += glm::normalize(right) * distance;
        }

        void TranslateUpward(float distance)
        {
            position += glm::normalize(up) * distance;
        }

        void resetNeutralPosition()
        {
            if (!isMovingForward && x_rotation > 0.0f)
            {
                x_rotation -= resetPositionSpeed;
            }

            if (!isMovingBackward && x_rotation < 0.0f)
            {
                x_rotation += resetPositionSpeed;
            }

            if (!isMovingRight && z_rotation > 0.0f)
            {
                z_rotation -= resetPositionSpeed;
            }

            if (!isMovingLeft && z_rotation < 0.0f)
            {
                z_rotation += resetPositionSpeed;
            }
        }

        bool CheckAABBCollision(const AABB &a, const AABB &b)
        {
            return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
                   (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
                   (a.min.z <= b.max.z && a.max.z >= b.min.z);
        }

        bool isDroneInsideCircle(const glm::vec2 &circleCenter, float circleRadius)
        {
            glm::vec2 dronePos2D(position.x, position.z);
            float distance = glm::distance(dronePos2D, circleCenter);
            return distance <= circleRadius;
        }

        void checkCollision(std::vector<glm::vec3> pillarPositions, std::vector<float> pillarsRadius)
        {
            AABB droneAABB;
            droneAABB.min = position - glm::vec3(radius);
            droneAABB.max = position + glm::vec3(radius);
            for (size_t i = 0; i < pillarPositions.size(); i++)
            {
                if (i >= pillarsRadius.size())
                {
                    // safety check to prevent out-of-bounds access
                    continue;
                }

                // pillar bounding box + position/size rectification for it
                AABB pillarAABB;
                glm::vec3 scaledRadius = glm::vec3(pillarsRadius[i] * 0.25f, pillarsRadius[i], pillarsRadius[i] * 0.25f);
                glm::vec3 meshOffset = glm::vec3(1.0f, 0.0f, 0.0f);
                pillarAABB.min = (pillarPositions[i] + meshOffset) - scaledRadius;
                pillarAABB.max = (pillarPositions[i] + meshOffset) + scaledRadius;

                if (CheckAABBCollision(droneAABB, pillarAABB))
                {
                    float deltaX = position.x - pillarPositions[i].x;
                    float deltaZ = position.z - pillarPositions[i].z;
                    float deltaY = position.y - pillarPositions[i].y;
                    float threshold = 0.1f;
                    // determine collision side
                    if (std::abs(deltaX) > std::abs(deltaZ))
                    {
                        if (deltaX > 0)
                        {
                            position.x += 0.25f;
                            isMovingRight = false;
                        }
                        else
                        {
                            position.x -= 0.25f;
                            isMovingLeft = false;
                        }
                    }
                    else
                    {
                        if (deltaZ > 0)
                        {
                            position.z += 0.25f;
                            isMovingForward = false;
                        }
                        else
                        {
                            position.z -= 0.25f;
                            isMovingBackward = false;
                        }
                    }
                }
            }

            glm::vec3 bottomCenter = glm::vec3(position.x, position.y - 1.0f, position.z);
            glm::vec3 cornerBottomRightBack = glm::vec3(position.x, position.y - 1.0f, position.z - 1.0f);

            if (cornerBottomRightBack.y <= 0.30f)
            {
                position.y = 1.50f;
            }
            if (bottomCenter.y <= 0.0f)
            {
                position.y = 1.30f;
            }
        }

        void alignWithCamera(implemented::Camera &camera, glm::vec3 cameraOffset)
        {
            // Create a rotation matrix based on the drone's Y rotation
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(y_rotation), glm::vec3(0, 1, 0));

            // Rotate the offset vector
            glm::vec3 rotatedOffset = glm::vec3(rotationMatrix * glm::vec4(cameraOffset, 1.0f));

            // Update the camera's position
            camera.position = position + rotatedOffset;

            // Make the camera look at the drone
            camera.forward = glm::normalize(position - camera.position);

            // Recalculate the camera's right and up vectors
            camera.right = glm::normalize(glm::cross(camera.forward, glm::vec3(0, 1, 0)));
            camera.up = glm::cross(camera.right, camera.forward);
        }

        void controls(implemented::Camera &camera, WindowObject *window, float deltaTime)
        {
            if (fuel > 0)
            {
                float cameraSpeed = 6.0f;
                float distance = cameraSpeed * deltaTime;
                float rotationSpeed = 10.0f;
                float movementSpeed = 20.0f; // Adjust as needed

                float yawRadians = glm::radians(y_rotation);
                glm::vec3 forward = glm::vec3(glm::sin(yawRadians), 0, glm::cos(yawRadians));
                glm::vec3 right_calibration = glm::vec3(glm::cos(yawRadians), 0, -glm::sin(yawRadians));
                if (window->KeyHold(GLFW_KEY_W) && position.y >= 1.50f)
                {
                    isMovingForward = true;
                    camera.TranslateForward(distance);
                    position += forward * movementSpeed * deltaTime;
                    if (x_rotation <= 30.0f)
                        x_rotation += distance * rotationSpeed;
                }

                if (window->KeyHold(GLFW_KEY_D) && position.y >= 1.50f)
                {
                    isMovingRight = true;
                    camera.TranslateRight(-distance);
                    position -= right_calibration * movementSpeed * deltaTime;
                    if (z_rotation <= 30.0f)
                        z_rotation += distance * rotationSpeed;
                }

                if (window->KeyHold(GLFW_KEY_S) && position.y >= 1.50f)
                {
                    isMovingBackward = true;
                    camera.TranslateForward(-distance);
                    position -= forward * movementSpeed * deltaTime;
                    if (x_rotation >= -30.0f)
                        x_rotation -= distance * rotationSpeed;
                }

                if (window->KeyHold(GLFW_KEY_A) && position.y >= 1.50f)
                {
                    isMovingLeft = true;
                    camera.TranslateRight(distance);
                    position += right_calibration * movementSpeed * deltaTime;
                    if (z_rotation >= -30.0f)
                        z_rotation -= distance * rotationSpeed;
                }

                position.x = glm::clamp(position.x, minX, maxX);
                position.z = glm::clamp(position.z, minZ, maxZ);

                if (window->KeyHold(GLFW_KEY_SPACE))
                {
                    camera.TranslateUpward(distance);
                    TranslateUpward(distance);
                }

                if (window->KeyHold(GLFW_KEY_M))
                {
                    camera.TranslateUpward(-distance);
                    TranslateUpward(-distance);
                }
                float sensivityOX = 0.001f;
                float sensivityOY = 0.001f;
                if (window->KeyHold(GLFW_KEY_LEFT))
                {
                    y_rotation += distance * rotationSpeed;
                }

                if (window->KeyHold(GLFW_KEY_RIGHT))
                {
                    y_rotation -= distance * rotationSpeed;
                }
            }
            else
            {
                if (position.y > 0.1f)
                {
                    position.y -= deltaTime;
                }
                else
                {
                    position.y = 0.0f;
                }
            }
        }

        public:
            float distanceToTarget;
            float radius;
            float y_rotation;
            float x_rotation;
            float z_rotation;
            float resetPositionSpeed = 0.75f;
            bool isMovingForward = false;
            bool isMovingBackward = false;
            bool isMovingRight = false;
            float fuel = 0.05;
            bool isMovingLeft = false;
            bool isDroneInsidePickUpCircle;
            bool isDroneInsideDeliveryCircle;
            int packetsDelivered = 0;
            parcel::Parcel *parcel;
            float propellerRotation;
            glm::vec3 position;
            glm::vec3 forward;
            glm::vec3 right;
            glm::vec3 up;
        };
    }