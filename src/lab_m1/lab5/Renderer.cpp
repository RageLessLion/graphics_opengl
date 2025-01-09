#include "lab5.h"

using namespace m1;

std::vector<glm::vec3> droneVertices;

float Lab5::ComputeBoundingSphere(const std::vector<glm::vec3> &vertices) {
    // Find center (e.g., average of all vertices)
    glm::vec3 center(0.0f);
    for (auto &v : vertices) {
        center += v;
    }
    center /= (float)vertices.size();

    // Compute max distance to center
    float radius = 0.0f;
    for (auto &v : vertices) {
        float dist = glm::distance(v, center);
        if (dist > radius) {
            radius = dist;
        }
    }
    return radius;
}


void Lab5::RenderParcel(parcel::Parcel &parcel, drone::Drone &drone)
{
    Mesh *box = meshes["box"];
    glm::mat4 boxMatrix = glm::mat4(1.0f);

    if (parcel.delivered)
    {
        boxMatrix = glm::translate(boxMatrix, glm::vec3(parcel.delivery_position.x, parcel.delivery_position.y, parcel.delivery_position.z));
    }
    else
    {
        if (!parcel.latched)
            boxMatrix = glm::translate(boxMatrix, glm::vec3(parcel.position.x, parcel.position.y, parcel.position.z));
        else
            boxMatrix = glm::translate(boxMatrix, glm::vec3(drone.position.x, drone.position.y - 1.0f, drone.position.z));
    }
    RenderMesh(box, shaders["VertexColor"], boxMatrix);

    Shader *cirlceColor = shaders["Color"];
    cirlceColor->Use();
    GLint colorLoc = glGetUniformLocation(cirlceColor->program, "color");
    if (colorLoc != -1)
    {
        if (drone.isDroneInsidePickUpCircle)
            glUniform3f(colorLoc, 192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
        else
            glUniform3f(colorLoc, 0, 0, 0);
    }

    Mesh *pickup_circle = meshes["pickup_circle"];
    glm::mat4 circleModelMatrix = glm::mat4(1);
    circleModelMatrix = glm::translate(circleModelMatrix, glm::vec3(parcel.position.x, parcel.position.y + 0.2f, parcel.position.z)); // Slightly above ground to prevent z-fighting
    RenderMesh(pickup_circle, cirlceColor, circleModelMatrix);

    Shader *delivery_circle_color = shaders["Color"];
    delivery_circle_color->Use();
    GLint colorLoc1 = glGetUniformLocation(delivery_circle_color->program, "color");
    if (colorLoc1 != -1)
    {
        if (drone.isDroneInsideDeliveryCircle)
            glUniform3f(colorLoc1, 192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
        else
            glUniform3f(colorLoc1, 0, 0, 0);
    }

    Mesh *delivery_circle = meshes["delivery_circle"];
    glm::mat4 circleModeDeliveryMatrix = glm::mat4(1);
    circleModeDeliveryMatrix = glm::translate(circleModeDeliveryMatrix, glm::vec3(parcel.delivery_position.x, parcel.delivery_position.y, parcel.delivery_position.z)); // Slightly above ground to prevent z-fighting
    RenderMesh(delivery_circle, delivery_circle_color, circleModeDeliveryMatrix);
}

void Lab5::RenderHelicopter(drone::Drone &drone, float deltaTimeSeconds, float propellerRotation)
{
    Shader *grey = shaders["Color"];
    grey->Use();
    GLint colorLoc = glGetUniformLocation(grey->program, "color");
    if (colorLoc != -1)
    {
        glUniform3f(colorLoc, 192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
    }
    // Render the helicopter body
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(drone.position.x, drone.position.y, drone.position.z + 4.0f));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(drone.y_rotation), glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(drone.x_rotation), glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(drone.z_rotation), glm::vec3(0, 0, 1));
    RenderMesh(meshes["Hbody"], grey, modelMatrix);

    // Render the helicopter propeller
    glm::mat4 propellerMatrix = modelMatrix;

    // Move up to the propeller's Y position
    float propellerXOffset = 0.5f;
    propellerMatrix = glm::translate(propellerMatrix, glm::vec3(propellerXOffset, 0.0f, 0.0f));

    // Move up to the propeller's Y position
    float propellerYOffset = 4.0f;
    propellerMatrix = glm::translate(propellerMatrix, glm::vec3(0.0f, propellerYOffset, 0.0f));

    // Move back along the Z-axis
    float propellerZOffset = -3.0f; // Adjust this value as needed
    propellerMatrix = glm::translate(propellerMatrix, glm::vec3(0.0f, 0.0f, propellerZOffset));

    // Rotate the propeller around its center
    propellerMatrix = glm::rotate(propellerMatrix, glm::radians(propellerRotation), glm::vec3(0, 1, 0));

    RenderMesh(meshes["Hblade"], shaders["VertexColor"], propellerMatrix);
}

void Lab5::RenderDrone(drone::Drone &drone, float deltaTimeSeconds, float propellerRotation)
{
    Shader *grey = shaders["Color"];
    grey->Use();
    GLint colorLoc = glGetUniformLocation(grey->program, "color");
    if (colorLoc != -1)
    {
        glUniform3f(colorLoc, 192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
    }
    if (propellerRotation >= 360.0f)
        propellerRotation -= 360.0f;

    // Drone body, X shape
    {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, drone.position);
        modelMatrix = glm::rotate(modelMatrix, RADIANS(45.0f), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(drone.y_rotation), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(drone.x_rotation), glm::vec3(1, 0, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(drone.z_rotation), glm::vec3(0, 0, 1));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 0.3f, 0.20f));
        
        // Collect transformed vertices
        for (const auto &v : meshes["box"]->positions)
        {
            glm::vec4 transformed = modelMatrix * glm::vec4(v, 1.0f);
            droneVertices.emplace_back(glm::vec3(transformed));
        }

        RenderMesh(meshes["box"], grey, modelMatrix);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, drone.position);
        modelMatrix = glm::rotate(modelMatrix, RADIANS(-45.0f), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(drone.y_rotation), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(drone.x_rotation), glm::vec3(1, 0, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(drone.z_rotation), glm::vec3(0, 0, 1));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 0.3f, 0.20f));
        
        // Collect transformed vertices
        for (const auto &v : meshes["box"]->positions)
        {
            glm::vec4 transformed = modelMatrix * glm::vec4(v, 1.0f);
            droneVertices.emplace_back(glm::vec3(transformed));
        }

        RenderMesh(meshes["box"], grey, modelMatrix);
    }

    float halfLength = 1.0f;
    float cubeOffset = 0.35f;

    float angles[2] = {45.0f, -45.0f};

    // Engine placements
    std::vector<glm::vec3> cubeOffsets;
    for (int i = 0; i < 2; ++i)
    {
        float angleRad = RADIANS(angles[i]);
        glm::vec3 direction = glm::vec3(cos(angleRad), 0, sin(angleRad));
        glm::vec3 offset = direction * halfLength;
        cubeOffsets.push_back(offset);
        cubeOffsets.push_back(-offset);
    }

    // Render each engine
    for (const auto &offset : cubeOffsets)
    {
        glm::mat4 cubeMatrix = glm::mat4(1.0f);
        cubeMatrix = glm::translate(cubeMatrix, drone.position);
        cubeMatrix = glm::rotate(cubeMatrix, RADIANS(drone.y_rotation), glm::vec3(0, 1, 0));
        cubeMatrix = glm::rotate(cubeMatrix, RADIANS(drone.x_rotation), glm::vec3(1, 0, 0));
        cubeMatrix = glm::rotate(cubeMatrix, RADIANS(drone.z_rotation), glm::vec3(0, 0, 1));
        cubeMatrix = glm::translate(cubeMatrix, offset);
        cubeMatrix = glm::scale(cubeMatrix, glm::vec3(0.25f, 1.0f, 0.25f));
        
        // Collect transformed vertices
        for (const auto &v : meshes["box"]->positions)
        {
            glm::vec4 transformed = cubeMatrix * glm::vec4(v, 1.0f);
            droneVertices.emplace_back(glm::vec3(transformed));
        }

        RenderMesh(meshes["box"], grey, cubeMatrix);
    }

    Shader *shader = shaders["Color"];
    shader->Use();
    colorLoc = glGetUniformLocation(shader->program, "color");
    if (colorLoc != -1)
    {
        glUniform3f(colorLoc, 0, 0, 0);
    }

    // Render propeller on each engine
    for (const auto &offset : cubeOffsets)
    {
        glm::mat4 propellerMatrix = glm::mat4(1.0f);
        propellerMatrix = glm::translate(propellerMatrix, drone.position);
        propellerMatrix = glm::rotate(propellerMatrix, glm::radians(drone.y_rotation), glm::vec3(0, 1, 0));
        propellerMatrix = glm::rotate(propellerMatrix, glm::radians(drone.x_rotation), glm::vec3(1, 0, 0));
        propellerMatrix = glm::rotate(propellerMatrix, glm::radians(drone.z_rotation), glm::vec3(0, 0, 1));
        propellerMatrix = glm::translate(propellerMatrix, offset);
        float propellerYOffset = 0.55f;
        propellerMatrix = glm::translate(propellerMatrix, glm::vec3(0.0f, propellerYOffset, 0.0f));
        propellerMatrix = glm::rotate(propellerMatrix, glm::radians(propellerRotation), glm::vec3(0, 1, 0));
        propellerMatrix = glm::scale(propellerMatrix, glm::vec3(0.9f, 0.03f, 0.1f));
        
        // Collect transformed vertices
        for (const auto &v : meshes["box"]->positions)
        {
            glm::vec4 transformed = propellerMatrix * glm::vec4(v, 1.0f);
            droneVertices.emplace_back(glm::vec3(transformed));
        }

        RenderMesh(meshes["box"], shader, propellerMatrix);
    }

    // Compute bounding sphere after rendering all parts
    if (!droneVertices.empty())
    {
        drone.radius = ComputeBoundingSphere(droneVertices);
        // You can store or use droneRadius as needed for collision detection
    }

    // Clear the vertices for the next frame
    droneVertices.clear();
}

void Lab5::RenderPillars(const std::vector<glm::vec3> &pillarPositions, drone::Drone &drone,std::vector<float> &pillarsRadius)
{
    Shader *grey = shaders["Color"];
    grey->Use();
    GLint colorLoc = glGetUniformLocation(grey->program, "color");
    if (colorLoc != -1)
    {
        glUniform3f(colorLoc, 54.0f / 255.0f, 69.0f / 255.0f, 79.0f / 255.0f);
    }

    // Iterate through each pillar position
    for (const auto &position : pillarPositions)
    {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position + glm::vec3(0.0f, 4.0f, 0.0f)); // Adjust Y if needed

        // Render the pillar
        RenderMesh(meshes["Pillar"], grey, modelMatrix);

        // Collect transformed vertices for the pillar
        std::vector<glm::vec3> pillarVertices;
        for (const auto &v : meshes["Pillar"]->positions)
        {
            glm::vec4 transformed = modelMatrix * glm::vec4(v, 1.0f);
            pillarVertices.emplace_back(glm::vec3(transformed));
        }

        // Compute bounding sphere for the pillar
        float radius = ComputeBoundingSphere(pillarVertices);
        pillarsRadius.push_back(radius);
        // glm::cout << CheckCollision(drone.position,drone.radius,position,pillarRadius);
        // Collision detection between drone and this pillar
    }
}
