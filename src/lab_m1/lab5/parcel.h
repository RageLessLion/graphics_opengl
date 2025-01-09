#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include <iostream>
#include <random>

namespace parcel
{
    class Parcel
    {
    public:
        double getRandomNumber(double min, double max)
        {
            std::random_device rd;  // Seed source
            std::mt19937 gen(rd()); // Mersenne Twister engine
            std::uniform_real_distribution<> dis(min, max);
            return dis(gen);
        }
        Parcel()
        {
            position = glm::vec3(getRandomNumber(-100,100), 1.5f, getRandomNumber(-100,100));
            delivery_position = glm::vec3(position.x + getRandomNumber(-100,100), 1.5f, position.z + getRandomNumber(-100,100));
            latched = false;
            delivered = false;
        }

        Parcel(const glm::vec3 &position)
        {
            Set(position);
        }

        ~Parcel()
        {
        }

        void Set(const glm::vec3 &position)
        {
            this->position = position;
        }

    public:
        glm::vec3 position;
        glm::vec3 delivery_position;
        bool latched;
        bool delivered;
    };
}