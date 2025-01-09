#pragma once
#include "components/simple_scene.h"
#include "lab_m1/lab5/lab_camera.h"
#include "lab_m1/lab5/parcel.h"
#include "lab_m1/lab5/drone.h"

namespace m1
{
   class Lab5 : public gfxc::SimpleScene
   {
   public:
      struct ViewportArea
      {
         ViewportArea() : x(0), y(0), width(1), height(1) {}
         ViewportArea(int x, int y, int width, int height)
             : x(x), y(y), width(width), height(height) {}
         int x;
         int y;
         int width;
         int height;
      };
      Lab5();
      ~Lab5();

      void Init() override;
      glm::ivec2 resolution = window->GetResolution();
      bool isThirdPerson;
      int secondsOnGround = 0;
      float propellerRotation;
      glm::vec3 dronePosition = glm::vec3(0.0f, 5.0f, 0.0f);

   private:
      void FrameStart() override;
      void Update(float deltaTimeSeconds) override;
      void FrameEnd() override;
      bool CheckCollision(const glm::vec3& posA, float radiusA,const glm::vec3& posB, float radiusB);
      void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;
      void RenderDrone(drone::Drone &drone, float delaTimeSeconds, float propellerRotation);
      void RenderParcel(parcel::Parcel &parcel, drone::Drone &drone);
      void RenderHelicopter(drone::Drone &drone,float deltaTimeSeconds,float propellerRotation );
      void RenderPillars(const std::vector<glm::vec3> &pillarPositions,drone::Drone &drone,std::vector<float> &pillarsRadius);
      float ComputeBoundingSphere(const std::vector<glm::vec3> &vertices);
      void OnInputUpdate(float deltaTime, int mods) override;
      void OnKeyPress(int key, int mods) override;
      void OnKeyRelease(int key, int mods) override;
      void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
      void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
      void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
      void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
      void OnWindowResize(int width, int height) override;

   protected:
      implemented::Camera *camera;
      drone::Drone *drone;
      parcel::Parcel *parcel;
      glm::mat4 projectionMatrix;
      bool renderCameraTarget;
      ViewportArea miniViewportArea;
      // TODO(student): If you need any other class variables, define them here.
   };
} // namespace m1
