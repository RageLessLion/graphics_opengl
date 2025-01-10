#include "lab_m1/lab5/lab5.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Lab5::Lab5()
{
}

Lab5::~Lab5()
{
}

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
};

bool CheckAABBCollision(const AABB &a, const AABB &b)
{
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

std::vector<glm::vec3> pillarPositions;
std::vector<float> pillarsRadius;

double getRandomNumber(double min, double max)
{
    std::random_device rd;  // Seed source
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

Mesh *CreateCircleMesh(const std::string &name, unsigned int numSegments = 64, float radius = 5.0f)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Generate circle vertices
    for (unsigned int i = 0; i <= numSegments; ++i)
    {
        float angle = 2.0f * glm::pi<float>() * i / numSegments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.emplace_back(glm::vec3(x, 0, z), glm::vec3(1, 0, 0)); // Red color
        indices.push_back(i);
    }

    Mesh *circle = new Mesh(name);
    circle->InitFromData(vertices, indices);
    circle->SetDrawMode(GL_LINE_LOOP); // Draw as a looped line

    return circle;
}

Mesh *CreateRectangleMesh(const std::string &name, float width, float height)
{
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(0.0f, 0.0f, 0.0f)),    // Bottom-left
        VertexFormat(glm::vec3(width, 0.0f, 0.0f)),   // Bottom-right
        VertexFormat(glm::vec3(width, height, 0.0f)), // Top-right
        VertexFormat(glm::vec3(0.0f, height, 0.0f))   // Top-left
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0};

    Mesh *rectangle = new Mesh(name);
    rectangle->InitFromData(vertices, indices);
    rectangle->SetDrawMode(GL_TRIANGLES);
    return rectangle;
}

void Lab5::Init()
{
    renderCameraTarget = false;
    isThirdPerson = false;
    // In Init()
    camera = new implemented::Camera();
    drone = new drone::Drone();
    parcel = new parcel::Parcel();
    camera->Set(glm::vec3(0, 2, 5.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    drone->Set(dronePosition, glm::vec3(0, 1, 0));
    for (int i = 0; i < 20; ++i)
    {
        // Using modulo and division to create a 3x4 grid pattern
        pillarPositions.push_back(glm::vec3(getRandomNumber(-200, 200), 0.0f, getRandomNumber(-200, 200)));
    }
    {
        Mesh *mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh *mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh *mesh = new Mesh("Hbody");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "helicopter_body_v2.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh *mesh = new Mesh("Hblade");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "blade_v4.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh *mesh = new Mesh("Pillar");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "pillar.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Shader *shader = new Shader("Plane1");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Lab5", "shaders", "Plane.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Lab5", "shaders", "Plane.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
    }

    {
        Mesh *mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    meshes["pickup_circle"] = CreateCircleMesh("pickup_circle", 64, 5.0f);
    meshes["delivery_circle"] = CreateCircleMesh("pickup_circle", 64, 5.0f);
    meshes["counter"] = CreateRectangleMesh("counter", 5.0f, 10.0f);
    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
}

void Lab5::FrameStart()
{

    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    RenderPillars(pillarPositions, *drone, pillarsRadius);
}

bool Lab5::CheckCollision(const glm::vec3 &posA, float radiusA, const glm::vec3 &posB, float radiusB)
{
    float distance = glm::length(posA - posB);
    return distance < (radiusA + radiusB);
}

void Lab5::Update(float deltaTimeSeconds)
{
    {
        Shader *planeShader = shaders["Plane1"];
        planeShader->Use();
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.10f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(10, 1, 10));
        RenderMesh(meshes["plane"], planeShader, modelMatrix);
    }

    AABB droneAABB;
    droneAABB.min = drone->position - glm::vec3(drone->radius);
    droneAABB.max = drone->position + glm::vec3(drone->radius);
    for (size_t i = 0; i < pillarPositions.size(); i++)
    {
        if (i >= pillarsRadius.size())
        {
            // safety check to prevent out-of-bounds access
            continue;
        }

        AABB pillarAABB;
        glm::vec3 scaledRadius = glm::vec3(pillarsRadius[i] * 0.25f, pillarsRadius[i], pillarsRadius[i] * 0.25f);
        glm::vec3 meshOffset = glm::vec3(1.0f, 0.0f, 0.0f);
        pillarAABB.min = (pillarPositions[i] + meshOffset) - scaledRadius;
        pillarAABB.max = (pillarPositions[i] + meshOffset) + scaledRadius;

        if (CheckAABBCollision(droneAABB, pillarAABB))
        {
             float deltaX = drone->position.x - pillarPositions[i].x;
            float deltaZ = drone->position.z - pillarPositions[i].z;
            float deltaY = drone->position.y - pillarPositions[i].y;
            float threshold = 0.1f;
            // if (deltaY > threshold)
            // {
            //     {
            //         // Collision from the top (landing on the pillar)
            //         drone->position.y += 0.25;
            //     }
            // }

            // Determine collision side
            if (std::abs(deltaX) > std::abs(deltaZ))
            {
                if (deltaX > 0)
                {
                    // Collision on the right side
                    drone->position.x += 0.25f;
                    drone->isMovingRight = false;
                }
                else
                {
                    // Collision on the left side
                    drone->position.x -= 0.25f;
                    drone->isMovingLeft = false;
                }
            }
            else
            {
                if (deltaZ > 0)
                {
                    // Collision on the front side
                    drone->position.z += 0.25f;
                    drone->isMovingForward = false;
                }
                else
                {
                    // Collision on the back side
                    drone->position.z -= 0.25f;
                    drone->isMovingBackward = false;
                }
            }
        }
        else
        {
            //cout<<0;
        }
    }

    propellerRotation += deltaTimeSeconds * drone->position.y * 500;
    RenderDrone(*drone, deltaTimeSeconds, propellerRotation);
     //RenderHelicopter(*drone, deltaTimeSeconds, propellerRotation);

    drone->checkCollision();
    drone->resetNeutralPosition();
    RenderParcel(*parcel, *drone);
    if (parcel->latched)
    {
        drone->parcel = parcel;
        if (drone->parcel->delivered)
        {
            parcel = new parcel::Parcel();
            RenderParcel(*parcel, *drone);
            drone->parcel = parcel;
        }
    }

    drone->isDroneInsidePickUpCircle = drone->isDroneInsideCircle(
        glm::vec2(parcel->position.x, parcel->position.z),
        5.0);

    drone->isDroneInsideDeliveryCircle = drone->isDroneInsideCircle(
        glm::vec2(parcel->delivery_position.x, parcel->delivery_position.z),
        5.0);

    if (renderCameraTarget)
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
    }
}

void Lab5::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Lab5::RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void Lab5::OnInputUpdate(float deltaTime, int mods)
{
    drone->controls(*camera, window, deltaTime);
    // drone
    drone->alignWithCamera(*camera, glm::vec3(0.0f, 2.0f, -5.0f));
    // helicopter
     //drone->alignWithCamera(*camera, glm::vec3(-2.0f, 2.0f, -13.0f));
}

void Lab5::OnKeyPress(int key, int mods)
{
    if (window->KeyHold(GLFW_KEY_L) && drone->isDroneInsidePickUpCircle)
    {
        parcel->latched = !parcel->latched;
    }
    if (window->KeyHold(GLFW_KEY_L) && drone->isDroneInsideDeliveryCircle && parcel->latched)
    {
        parcel->delivered = true;
        drone->packetsDelivered++;
    }
}

void Lab5::OnKeyRelease(int key, int mods)
{
    /*ith w , s, d , a i control the movements of the player drone (the release of them
    resulting in neutral position)*/
    if (key == GLFW_KEY_W)
    {
        drone->isMovingForward = false;
    }

    if (key == GLFW_KEY_S)
    {
        drone->isMovingBackward = false;
    }

    if (key == GLFW_KEY_D)
    {
        drone->isMovingRight = false;
    }

    if (key == GLFW_KEY_A)
    {
        drone->isMovingLeft = false;
    }
}

void Lab5::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    //
}
void Lab5::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}

void Lab5::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}

void Lab5::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    //
}

void Lab5::OnWindowResize(int width, int height)
{
    //
}
