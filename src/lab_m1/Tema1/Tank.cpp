#include "Tank.h"
#include "lab_m1/Tema1/2dobject.h"
#include "lab_m1/Tema1/transform2D.h"

Tank::Tank(float startX, float startY)
    : x(startX), y(startY), rotation(0.0f), speed(100.0f) {
    // Initialize tank dimensions
    bottomWidth = 100.0f;
    topWidth = 60.0f;
    height = 50.0f;
    circleRadius = 10.0f;

    // Initialize colors
    baseColor = glm::vec3(1, 0, 0);   // Red
    turretColor = glm::vec3(0, 1, 0); // Green
}
Tank::~Tank() {
    // Clean up meshes
    for (auto& meshPair : meshes) {
        delete meshPair.second;
    }
    meshes.clear();
}

void Tank::setX(float newX){
    x = newX; 
}

void Tank::setY(float newY){
    y = newY;
}

float Tank::getX(){
    return x;
}

float Tank::getRotation(){
    return rotation;
}

float Tank::setRotation(float newRotation){
    rotation = newRotation;
}

float Tank::getY(){
    return y;
}

const std::unordered_map<std::string, Mesh*>& Tank::getMeshes() const {
    return meshes;
}


void Tank::setMeshes(const std::unordered_map<std::string, Mesh*>& newMeshes) {
    for (auto& meshPair : meshes) {
        delete meshPair.second;
    }
    meshes.clear();

    // Copy new meshes
    for (const auto& meshPair : newMeshes) {
        // Deep copy the mesh
        meshes[meshPair.first] = new Mesh(*meshPair.second);
    }
}

void Tank::Init() {
    Mesh* trapezoid1 = object2D::CreateTrapezoid(
        "trapezoid1",
        glm::vec3(0.0f, 0.0f, 0.0f),
        bottomWidth,
        topWidth,
        height,
        baseColor,
        true
    );
    meshes["trapezoid1"] = trapezoid1;

    float trapezoid2BottomWidth = 80.0f;
    float trapezoid2TopWidth = 50.0f;
    float trapezoid2Height = 40.0f;
    glm::vec3 trapezoid2Color = glm::vec3(0, 0, 1); 

    Mesh* trapezoid2 = object2D::CreateTrapezoid(
        "trapezoid2",
        glm::vec3(0.0f, 0.0f, 0.0f),
        trapezoid2BottomWidth,
        trapezoid2TopWidth,
        trapezoid2Height,
        trapezoid2Color,
        true
    );
    meshes["trapezoid2"] = trapezoid2;


    Mesh* circle1 = object2D::CreateCircle(
        "circle1",
        glm::vec3(0, 1, 0.0f),
        circleRadius,
        turretColor,
        true
    );
    meshes["circle1"] = circle1;

    Mesh* square = object2D::CreateSquare(
        "square",
        glm::vec3(0.0f, 0.0f, 0.0f),
        10.0f,
        glm::vec3(1,0,0),
        true
    );
    meshes["square"] = square;
}