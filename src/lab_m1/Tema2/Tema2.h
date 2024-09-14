#pragma once



#include "components/simple_scene.h"
#include "lab_m1/Tema2/tank.h"
#include "lab_m1/Tema2/lab_camera.h"
#include "lab_m1/Tema2/Proiectil.h"
#include "lab_m1/Tema2/Turela.h"
#include "lab_m1/Tema2/Enemy.h"
#include "lab_m1/Tema2/light.h"


#include <vector>
#include <iostream>
#include <glm/glm.hpp> 


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
     public:
        Tema2();
        ~Tema2();

        void Init() override;
        struct cladire {
            glm::vec3 position;
            float width;
            float length;
            float minx,maxx,minz,maxz;
            int type;


        };
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

    ;


        Mesh *CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices);

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix,glm::vec3 color,int hit);
        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        bool IsInsideAnyCube(const glm::vec3 position);
        void DrawScene();
        bool CollisionSphereRectangle(cladire cladire, tank::Proiectil proiectil);
        bool Collision2Rectangle(cladire cladire, glm::vec3 position, float halfWidth2, float halfHeight2);

  

    protected:
        tank::tank* tank;
        tank::light* light;
        tank::light* light2;
        float posx;
        float posz;
        float angle;
        float angleturret;
        cameraTema2::Camera *camera;
        cameraTema2::Camera* camerarezerva;
        cameraTema2::Turela *turela;
        bool check;
        float znear;
        float zfar;
        float fov;
        glm::mat4 projectionMatrix;
        int cladiri_nr;
        int enemy_nr;
        
        float time;
        int hit = 0;
        int cameraSave = 0;
        float gametime = 0;
        int score = 0;
        
        glm::vec3 lightPosition2;
        
        unsigned int materialShininess;
        float materialKd;
        float materialKs;
        ViewportArea miniViewportArea;
        glm::vec3 distancecamera;

       
        

   
    std::vector <tank::Proiectil> proiectile;
    std::vector <cladire> cladiri;
    std::vector <tank::Enemy> enemies;

    

    };


   
   
       
}   // namespace m1
