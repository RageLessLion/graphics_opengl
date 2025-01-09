#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/2dobject.h"


using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */



std::vector<glm::vec3> terrainPoints;
Mesh* terrainMesh;

float tank_x2 = 650.0f;      
float tank_y2 = 400.0f;     
float tank_2_y2 = 400.0f;
float tank_2_x2 = 700.0f;
float tank_speed2 = 200.0f;
float tank_2_speed2 = 200.0f;  
const float squareSide2 = 100.0f;
const float circleRadius2 = 10.0f;
float squareAngle2 = glm::pi<float>() / 2.0f; 
const float squareRotationSpeed2 = glm::radians(90.0f);
float rectangleAngle2 = 0.0f;
float turretOffsetX2 = 0.0f;
float turretOffsetY2 = 0.0f;
float rectPosX2;
float rectPosY2;

     float bottomWidth2 = 100.0f; 
    float topWidth2 = 60.0f;
    float height2 = 50.0f;
    float trapezoid2BottomWidth2 = 80.0f; 
    float trapezoid2TopWidth2 = 50.0f;    
    float trapezoid2Height2 = 40.0f;     


float tank_x = 650.0f;      
float tank_y = 400.0f;     
float tank_2_y = 400.0f;
float tank_2_x = 700.0f;
float tank_speed = 200.0f;
float tank_2_speed = 200.0f;  
const float squareSide = 100.0f;
const float circleRadius = 10.0f;
float squareAngle = glm::pi<float>() / 2.0f;
const float squareRotationSpeed = glm::radians(90.0f);
float rectangleAngle = 0.0f;
float turretOffsetX = 0.0f;
float turretOffsetY = 0.0f;
float rectPosX;
float rectPosY;

     float bottomWidth = 100.0f;
    float topWidth = 60.0f; 
    float height = 50.0f;
    float trapezoid2BottomWidth = 80.0f; 
    float trapezoid2TopWidth = 50.0f;    
    float trapezoid2Height = 40.0f;      

Tema1::Tema1()
    : canFire(true), fireCooldown(0.3f), currentCooldown(0.0f)
{
}


Tema1::~Tema1()
{
}

void Tema1::GenerateTerrain()
{
    int numPoints = 200;
    float frequency = 0.008f;
    float offsetY = 250.0f; 
    glm::ivec2 resolution = window->GetResolution();
    float xMax = static_cast<float>(resolution.x);
    terrainPoints.clear();
    std::vector<float> hillAmplitudes = { 100.0f, 800.0f, 80.0f, 200.0f, 110.0f, 200.0f};
    float period = 2 * M_PI / frequency;
    int numHills = static_cast<int>(xMax / period) + 1;
    if (hillAmplitudes.size() < numHills + 1) {
        size_t originalSize = hillAmplitudes.size();
        for (size_t i = originalSize; i < numHills + 1; ++i) {
            hillAmplitudes.push_back(hillAmplitudes[i % originalSize]);
        }
    }

   
    for (int i = 0; i <= numPoints; ++i) {
        float x = i * xMax / numPoints;
        int hillIndex = static_cast<int>(x / period);
        int nextHillIndex = hillIndex + 1;

        
        float t = fmod(x, period) / period;

        
        float amplitudeCurrent = hillAmplitudes[hillIndex % hillAmplitudes.size()];
        float amplitudeNext = hillAmplitudes[nextHillIndex % hillAmplitudes.size()];
        float localAmplitude = amplitudeCurrent * (1 - t) + amplitudeNext * t;
        float y = localAmplitude * (sin(frequency * x) + 1) / 2 + offsetY;

        terrainPoints.push_back(glm::vec3(x, y, 0));
    }

   
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    for (size_t i = 0; i < terrainPoints.size() - 1; ++i) {
        glm::vec3 topLeft = terrainPoints[i];
        glm::vec3 topRight = terrainPoints[i + 1];
        glm::vec3 bottomLeft = glm::vec3(topLeft.x, 0.0f, 0.0f);
        glm::vec3 bottomRight = glm::vec3(topRight.x, 0.0f, 0.0f);

        vertices.push_back(VertexFormat(bottomLeft, glm::vec3(0.0f, 0.5f, 0.0f)));
        vertices.push_back(VertexFormat(topLeft,    glm::vec3(0.0f, 1.0f, 0.0f)));
        vertices.push_back(VertexFormat(bottomRight,glm::vec3(0.0f, 0.5f, 0.0f)));

        vertices.push_back(VertexFormat(topLeft,    glm::vec3(0.0f, 1.0f, 0.0f)));
        vertices.push_back(VertexFormat(topRight,   glm::vec3(0.0f, 1.0f, 0.0f)));
        vertices.push_back(VertexFormat(bottomRight,glm::vec3(0.0f, 0.5f, 0.0f)));

        indices.push_back(static_cast<unsigned int>(i * 6 + 0));
        indices.push_back(static_cast<unsigned int>(i * 6 + 1));
        indices.push_back(static_cast<unsigned int>(i * 6 + 2));

        indices.push_back(static_cast<unsigned int>(i * 6 + 3));
        indices.push_back(static_cast<unsigned int>(i * 6 + 4));
        indices.push_back(static_cast<unsigned int>(i * 6 + 5));
    }

    
    if (terrainMesh != nullptr) {
        delete terrainMesh;
    }
    terrainMesh = new Mesh("terrain");
    terrainMesh->InitFromData(vertices, indices);
    AddMeshToList(terrainMesh);
}


float Tema1::GetTerrainHeight(float x) {
    if (terrainPoints.empty()){
        return 0.0f;
    }
    if (x < terrainPoints.front().x)
        x = terrainPoints.front().x;
    if (x > terrainPoints.back().x)
        x = terrainPoints.back().x;

    for (size_t i = 0; i < terrainPoints.size() - 1; ++i) {
        if (x >= terrainPoints[i].x && x <= terrainPoints[i + 1].x) {
            float t = (x - terrainPoints[i].x) / (terrainPoints[i + 1].x - terrainPoints[i].x);
            return glm::mix(terrainPoints[i].y, terrainPoints[i + 1].y, t);
        }
    }

    return 0.0f; 
}

float Tema1::GetTerrainSlope(float x)
{
    for (size_t i = 0; i < terrainPoints.size() - 1; ++i) {
        if (x >= terrainPoints[i].x && x <= terrainPoints[i + 1].x) {
            float deltaX = terrainPoints[i + 1].x - terrainPoints[i].x;
            float deltaY = terrainPoints[i + 1].y - terrainPoints[i].y;
            return deltaY / deltaX;
        }
    }
    return 0.0f;
}


void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);
    GenerateTerrain();
    glm::vec3 corner = glm::vec3(0,0,0);
    tank1 = new Tank(650.f,400.f);
    tank1->Init();
    for(const auto& meshPair : tank1->getMeshes()){
        AddMeshToList(meshPair.second);
    }
    tank1->setY(GetTerrainHeight(tank1->getX()));
     Mesh* circle2 = object2D::CreateCircle(
        "circle2",
        glm::vec3(0, 1, 0.0f),
        circleRadius,
        glm::vec3(0, 1, 0.0f),
        true
    );
    AddMeshToList(circle2);


    tank2 = new Tank(800.f,400.f);
    tank2->Init();
    for(const auto& meshPair : tank2->getMeshes()){
        AddMeshToList(meshPair.second);
    }
    tank2->setY(GetTerrainHeight(tank2->getX()));


    Mesh* circle3 = object2D::CreateCircle(
        "circle3",
        glm::vec3(0, 1, 0.0f),
        circleRadius,
        glm::vec3(0, 1, 0.0f),
        true
    );
    AddMeshToList(circle3);
}


void Tema1::FrameStart()
{
   
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    
    glViewport(0, 0, resolution.x, resolution.y);
}


glm::vec2 Tema1::CalculateTurretEnd(const glm::vec2& tankPosition, float turretRotation, float turretLength)
{
    glm::vec2 direction = glm::vec2(cos(turretRotation), sin(turretRotation));
    return tankPosition + direction * turretLength;
}

void Tema1::Update(float deltaTimeSeconds)
{

    float currentX = tank1->getX();
    float terrainHeight = GetTerrainHeight(currentX);
    float terrainSlope = GetTerrainSlope(currentX); 

  
    tank1->setY(terrainHeight);

  
    float rotationAngle = glm::atan(terrainSlope);
    tank1->setRotation(rotationAngle);

    float trapezoidWidth = 100.0f; 
    float trapezoidHeight = 20.0f;  
    float centerX = currentX;
    float centerY = terrainHeight + trapezoidHeight / 2.0f;

    glm::mat3 tankModelMatrix = glm::mat3(1);
    tankModelMatrix *= transform2D::Translate(centerX, centerY);     
    tankModelMatrix *= transform2D::Rotate(-(tank1->getRotation()));    
    tankModelMatrix *= transform2D::Scale(1.5f, 1.0f);               

    glm::mat3 baseModelMatrix = glm::mat3(1);
    baseModelMatrix *= transform2D::Translate(-trapezoidWidth / 2.0f, -trapezoidHeight / 2.0f);
    baseModelMatrix = tankModelMatrix * baseModelMatrix;
    RenderMesh2D(meshes["trapezoid1"], shaders["VertexColor"], baseModelMatrix);

  
    float trapezoid2OffsetX = 0.0f;
    float trapezoid2OffsetY = -trapezoidHeight / 2.0f - 10.0f; 

    glm::mat3 trapezoid2ModelMatrix = glm::mat3(1);
    trapezoid2ModelMatrix *= transform2D::Translate(trapezoid2OffsetX + 43.0f, trapezoid2OffsetY + 10.0f);
    trapezoid2ModelMatrix *= transform2D::Rotate(M_PI); 
    trapezoid2ModelMatrix *= transform2D::Scale(1.0f, 0.5f);
    trapezoid2ModelMatrix = tankModelMatrix * trapezoid2ModelMatrix;
    RenderMesh2D(meshes["trapezoid2"], shaders["VertexColor"], trapezoid2ModelMatrix);

   
    turretOffsetX = 0.0f;
    turretOffsetY = trapezoidHeight / 2.0f + 10.0f; 

    glm::mat3 turretModelMatrix = glm::mat3(1);
    turretModelMatrix *= transform2D::Translate(turretOffsetX, turretOffsetY + 15.0f);
    turretModelMatrix *= transform2D::Scale(2.5f, 2.5f);
    turretModelMatrix = tankModelMatrix * turretModelMatrix;
    RenderMesh2D(meshes["circle1"], shaders["VertexColor"], turretModelMatrix);

   
    glm::mat3 rectangleModelMatrix = glm::mat3(1);
    float rectangleWidth = 3.0f;    
    float rectangleHeight = 1.0f; 

   
    float turretRadius = 20.5f; 
    float turretCenterX = turretOffsetX;
    float turretCenterY = turretOffsetY + 15.0f;

    rectPosX = turretCenterX + turretRadius * cos(rectangleAngle);
    rectPosY = turretCenterY + turretRadius * sin(rectangleAngle);
    tank1->turretEndX = rectPosX;
    tank1->turretEndY = rectPosY;

    rectangleModelMatrix *= transform2D::Translate(rectPosX, rectPosY);
    rectangleModelMatrix *= transform2D::Rotate(-rectangleAngle); 
    rectangleModelMatrix *= transform2D::Scale(rectangleWidth, rectangleHeight);


    rectangleModelMatrix = tankModelMatrix * rectangleModelMatrix;
    RenderMesh2D(meshes["square"], shaders["VertexColor"], rectangleModelMatrix);



    float currentX2 = tank2->getX();
    float terrainHeight2 = GetTerrainHeight(currentX2);
    float terrainSlope2 = GetTerrainSlope(currentX2);

   
    tank2->setY(terrainHeight2);

   
    float rotationAngle2 = glm::atan(terrainSlope2);
    tank2->setRotation(rotationAngle2);

   
    float trapezoidWidth2 = 100.0f;  
    float trapezoidHeight2 = 20.0f;  
    float centerX2 = currentX2;
    float centerY2 = terrainHeight2 + trapezoidHeight2 / 2.0f;

   
    glm::mat3 tankModelMatrix2 = glm::mat3(1);
    tankModelMatrix2 *= transform2D::Translate(centerX2, centerY2);     
    tankModelMatrix2 *= transform2D::Rotate(-(tank2->getRotation()));  
    tankModelMatrix2 *= transform2D::Scale(1.5f, 1.0f);               

    
    glm::mat3 baseModelMatrix2 = glm::mat3(1);
    baseModelMatrix2 *= transform2D::Translate(-trapezoidWidth2 / 2.0f, -trapezoidHeight2 / 2.0f);
    baseModelMatrix2 = tankModelMatrix2 * baseModelMatrix2;
    RenderMesh2D(meshes["trapezoid1"], shaders["VertexColor"], baseModelMatrix2);

    
    float trapezoid2OffsetX2 = 0.0f;
    float trapezoid2OffsetY2 = -trapezoidHeight2 / 2.0f - 10.0f; 

    glm::mat3 trapezoid2ModelMatrix2 = glm::mat3(1);
    trapezoid2ModelMatrix2 *= transform2D::Translate(trapezoid2OffsetX2 + 43.0f, trapezoid2OffsetY2 + 10.0f);
    trapezoid2ModelMatrix2 *= transform2D::Rotate(M_PI);
    trapezoid2ModelMatrix2 *= transform2D::Scale(1.0f, 0.5f);
    trapezoid2ModelMatrix2 = tankModelMatrix2 * trapezoid2ModelMatrix2;
    RenderMesh2D(meshes["trapezoid2"], shaders["VertexColor"], trapezoid2ModelMatrix2);

   
    turretOffsetX2 = 0.0f;
    turretOffsetY2 = trapezoidHeight2 / 2.0f + 10.0f; 

    glm::mat3 turretModelMatrix2 = glm::mat3(1);
    turretModelMatrix2 *= transform2D::Translate(turretOffsetX2, turretOffsetY2 + 15.0f);
    turretModelMatrix2 *= transform2D::Scale(2.5f, 2.5f); 
    turretModelMatrix2 = tankModelMatrix2 * turretModelMatrix2;
    RenderMesh2D(meshes["circle1"], shaders["VertexColor"], turretModelMatrix2);

   
    glm::mat3 rectangleModelMatrix2 = glm::mat3(1);
    float rectangleWidth2 = 3.0f;    
    float rectangleHeight2 = 1.0f; 

    
    float turretRadius2 = 20.5f; 
    float turretCenterX2 = turretOffsetX2;
    float turretCenterY2 = turretOffsetY2 + 15.0f;

   
    rectPosX2 = turretCenterX2 + turretRadius2 * cos(rectangleAngle2);
    rectPosY2 = turretCenterY2 + turretRadius2 * sin(rectangleAngle2);
    tank2->turretEndX = rectPosX2;
    tank2->turretEndY = rectPosY2;

    rectangleModelMatrix2 *= transform2D::Translate(rectPosX2, rectPosY2);
    rectangleModelMatrix2 *= transform2D::Rotate(-rectangleAngle2); 
    rectangleModelMatrix2 *= transform2D::Scale(rectangleWidth2, rectangleHeight2);


    rectangleModelMatrix2 = tankModelMatrix2 * rectangleModelMatrix2;
    RenderMesh2D(meshes["square"], shaders["VertexColor"], rectangleModelMatrix2);

    const float gravity = 300.0f;

    for (auto& proj : projectiles) {
        if (proj.active) {
            proj.position += proj.velocity * deltaTimeSeconds;

            //  gravity to the y-component of velocity
            proj.velocity -= glm::vec2(0, gravity) * deltaTimeSeconds;

            //deactivate if below the screen
            if (proj.position.y < 0.0f) {
                proj.active = false;
                continue;
            }

            glm::mat3 projectileMatrix = glm::mat3(1);
            projectileMatrix *= transform2D::Translate(proj.position.x, proj.position.y);
            // projectileMatrix *= transform2D::Scale(proj.radius, proj.radius);
            RenderMesh2D(meshes["circle2"], shaders["VertexColor"], projectileMatrix);
        }
    }

    //inactive projectiles
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const Projectile& p) { return !p.active; }),
        projectiles.end()
    );


    for (auto& proj2 : projectiles2) {
        if (proj2.active) {
            proj2.position += proj2.velocity * deltaTimeSeconds;

            //  gravity to the y-component of velocity
            proj2.velocity -= glm::vec2(0, gravity) * deltaTimeSeconds;

            //deactivate if below the screen
            if (proj2.position.y < 0.0f) {
                proj2.active = false;
                continue;
            }

            glm::mat3 projectileMatrix2 = glm::mat3(1);
            projectileMatrix2 *= transform2D::Translate(proj2.position.x, proj2.position.y);
            RenderMesh2D(meshes["circle3"], shaders["VertexColor"], projectileMatrix2);
        }
    }

    //inactive projectiles
    projectiles2.erase(
        std::remove_if(projectiles2.begin(), projectiles2.end(),
            [](const Projectile& p) { return !p.active;}),
        projectiles2.end()
    );


    glm::mat3 terrainModelMatrix = glm::mat3(1);
    RenderMesh2D(meshes["terrain"], shaders["VertexColor"], terrainModelMatrix);
}


void Tema1::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    
    if (window->KeyHold(GLFW_KEY_A)) {
        float x = tank1->getX();
        tank1->setX(x - tank_speed * deltaTime);
    }

   
    if (window->KeyHold(GLFW_KEY_D)) {
        float x = tank1->getX();
        tank1->setX(x + tank_speed * deltaTime);
    }

    if (window->KeyHold(GLFW_KEY_LEFT)) {
        float x = tank2->getX();
        tank2->setX(x - tank_speed * deltaTime);
    }

   
    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        float x = tank2->getX();
        tank2->setX(x + tank_speed * deltaTime);
    }

    if (window->KeyHold(GLFW_KEY_W)) {
        rectangleAngle += squareRotationSpeed * deltaTime;
        if (rectangleAngle > glm::pi<float>())
            rectangleAngle = glm::pi<float>(); 
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        rectangleAngle -= squareRotationSpeed * deltaTime;
        if (rectangleAngle < 0.0f)
            rectangleAngle = 0.0f;
    }


    if (window->KeyHold(GLFW_KEY_UP)) {
        rectangleAngle2 += squareRotationSpeed2 * deltaTime;
        if (rectangleAngle2 > glm::pi<float>())
            rectangleAngle2 = glm::pi<float>(); 
    }

    if (window->KeyHold(GLFW_KEY_DOWN)) {
        rectangleAngle2 -= squareRotationSpeed2 * deltaTime;
        if (rectangleAngle2 < 0.0f)
            rectangleAngle2 = 0.0f;
    }


    if (!canFire) {
        currentCooldown -= deltaTime;
        if (currentCooldown <= 0.0f) {
            canFire = true;
            currentCooldown = 0.0f;
        }
    }

    if (canFire && window->KeyHold(GLFW_KEY_SPACE)) {
       
        float magnitude = 500.0f;
        float cannonAngle = rectangleAngle; 

       
        glm::vec2 tankPosition = glm::vec2(tank1->getX(), tank1->getY());
        float tankRotation = tank1->getRotation(); 

       
        float totalAngle = tankRotation + cannonAngle;

        
        float turretLength = 20.5f;   
        glm::vec2 turretEnd = CalculateTurretEnd(tankPosition, totalAngle, turretLength);

      
        glm::vec2 velocity = glm::vec2(cos(totalAngle), sin(totalAngle)) * magnitude;

       
        Projectile newProjectile(turretEnd, velocity, 2.5f); 
        projectiles.push_back(newProjectile);

       
        canFire = false;
        currentCooldown = fireCooldown;
    }




     if (canFire && window->KeyHold(GLFW_KEY_ENTER)) {
       
        float magnitude2 = 500.0f;
        float cannonAngle2 = rectangleAngle2; 

       
        glm::vec2 tankPosition2 = glm::vec2(tank2->getX(), tank2->getY());
        float tankRotation2 = tank2->getRotation(); 

       
        float totalAngle2 = tankRotation2 + cannonAngle2;

        
        float turretLength2 = 20.5f;   
        glm::vec2 turretEnd2 = CalculateTurretEnd(tankPosition2, totalAngle2, turretLength2);

      
        glm::vec2 velocity2 = glm::vec2(cos(totalAngle2), sin(totalAngle2)) * magnitude2;

       
        Projectile newProjectile2(turretEnd2, velocity2, 2.5f); 
        projectiles2.push_back(newProjectile2);
       
        canFire = false;
        currentCooldown = fireCooldown;
    }


    //terrain bounds
    float minX = terrainPoints.front().x;
    float maxX = terrainPoints.back().x;
    if (tank1->getX() < minX)
        tank1->setX(minX);
    if (tank1->getX() > maxX)
        tank1->setX(maxX);

    float currentX = tank1->getX();

    float terrainHeight = GetTerrainHeight(currentX);
    float terrainSlope = GetTerrainSlope(currentX); 

    
    tank1->setY(terrainHeight);

   
    float rotationAngle = glm::atan(terrainSlope);

   
    tank1->setRotation(rotationAngle);




    float minX2 = terrainPoints.front().x;
    float maxX2 = terrainPoints.back().x;
    if (tank2->getX() < minX2)
        tank2->setX(minX2);
    if (tank2->getX() > maxX2)
        tank2->setX(maxX2);

    float currentX2 = tank2->getX();

    float terrainHeight2 = GetTerrainHeight(currentX2);
    float terrainSlope2 = GetTerrainSlope(currentX2);

    tank2->setY(terrainHeight2);

    float rotationAngle2 = glm::atan(terrainSlope2);

    tank2->setRotation(rotationAngle2);


    
}


void Tema1::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
