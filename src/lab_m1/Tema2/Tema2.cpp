#include "lab_m1/Tema2/Tema2.h"
#include "lab_m1/Tema2/transform3D.h"
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <random>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp> 
#include "utils/glm_utils.h"
#include "utils/math_utils.h"

using namespace std;
using namespace m1;

Tema2::Tema2()
{
}

Tema2::~Tema2()
{
}

void Tema2::Init()
{


    tank = new tank::tank();
    light = new tank::light();
    light2 = new tank::light();
    camera = new cameraTema2::Camera();
    camerarezerva = new cameraTema2::Camera();
    camera->Set(glm::vec3(4, 2, 0), glm::vec3(0, 2, 0), glm::vec3(0, 1, 0));
    {
        Mesh* mesh = new Mesh("senile");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tank"), "Senile.obj");
        meshes[mesh->GetMeshID()] = mesh;

    }
    {
        Mesh* mesh = new Mesh("Frame");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tank"), "Frame.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("Turela");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tank"), "Turela.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("Tun");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tank"), "Tun.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("Proiectil");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tank"), "Sfera.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("cladire1");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "vegetation"), "cladire1.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("cladire2");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "vegetation"), "cladire2.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    light->position.x = tank->position.x -1.3 ;
    light->position.y = 0.5;
    light->position.z = tank->position.z  + 0.7;

    light2->position.x = tank->position.x - 1.3;
    light2->position.y = 0.5;
    light2->position.z = tank->position.z - 0.7;

        
    materialShininess = 70;
    materialKd = 0.7;
    materialKs = 0.7;

    {
        Shader *shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
  
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
    cladiri_nr = rand() % 25 + 10;
    for (int index = 0; index < cladiri_nr; index++)
    {
        cladire cladire;
        cladire.type = rand() % 2;
        if (cladire.type == 0)
        {
            cladire.length = 7.6;
            cladire.width = 7.6;
        }
        else
        {
            cladire.width = 23;
            cladire.length = 13;
        }
        do {
            cladire.position.x = rand() % 200 - 100; 
            cladire.position.z = rand() % 200 - 100; 
        } while ((cladire.position.x <= 7 && cladire.position.x >= -7) || (cladire.position.z <= 7 && cladire.position.z >= -7));
        cladire.position.y = 0;
        cladire.maxx = cladire.position.x + (cladire.width / 2 +1);
        cladire.minx = cladire.position.x - (cladire.width / 2 +1);
        cladire.maxz = cladire.position.z + (cladire.length / 2 +1);
        cladire.minz = cladire.position.z - (cladire.length / 2 + 1);
        cladiri.push_back(cladire);
    }
    enemy_nr =rand() % 8 + 5;
    for (int index = 0; index < enemy_nr; index++)
    {
		tank::Enemy *enemy = new tank::Enemy();
        bool isEnemyPlaced = false;
        while(!isEnemyPlaced)
        {
            enemy->position.x = rand() % 181 - 90;
          
            enemy->position.z = rand() % 181 - 90;
            enemy->position.y = 0;

            if (IsInsideAnyCube(enemy->position) &&
                abs(enemy->position.x) > 15 && abs(enemy->position.z) > 15)
            {
                isEnemyPlaced = true; 
            }
           
        } 
        enemies.push_back(*enemy);
    }
    check = false;
    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(1000, 50, resolution.x / 5.f, resolution.y / 5.f);

    

}

bool Tema2::IsInsideAnyCube( glm::vec3 position) {
    float dist = 0;
    
    for (int index = 0; index < cladiri.size(); index++)
    {
        cladire cladire = cladiri[index];
       
        if (position.x >= cladire.minx && position.x <= cladire.maxx && position.z >= cladire.minz && position.z <= cladire.maxz)
        {
          
           return false;
        }
    }
    for (int index = 0; index < enemies.size(); index++)
    {
        tank::Enemy enemy = enemies[index];
		glm::vec3 position2 = enemy.position;
		dist = sqrt((position.x - position2.x) * (position.x - position2.x) + (position.z - position2.z) * (position.z - position2.z));

        if (dist < 2 * enemy.raza)
        {
			
			return false;

		}
    }
    return true; 
}

void Tema2::DrawScene()
{
    
   
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0, -0.1, 0);
    modelMatrix *= transform3D::Scale(4, 4, 4);
    RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix,glm::vec3(0.075,0.327,0.082), 0);
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0, 25, 0);
    modelMatrix *= transform3D::Scale(4, 4, 4);
    RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix, glm::vec3(0.133, 0.655, 0.941), 0);
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(100, 0.1, 0);
    modelMatrix *= transform3D::Scale(0.2, 1, 4);
    modelMatrix *= transform3D::RotateOX(RADIANS(90));
    RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix, glm::vec3(0.133, 0.655, 0.941), 0);
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(-100, 0.1, 0);
    modelMatrix *= transform3D::Scale(0.2, 1, 4);
    modelMatrix *= transform3D::RotateOX(RADIANS(90));
    RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix, glm::vec3(0.133, 0.655, 0.941), 0);
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0, 0.1, 100);
    modelMatrix *= transform3D::Scale(4, 1, 0.2);
    modelMatrix *= transform3D::RotateOY(RADIANS(90));
    modelMatrix *= transform3D::RotateOX(-RADIANS(90));
    RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix, glm::vec3(0.133, 0.655, 0.941), 0);
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0, 0.1, -100);
    modelMatrix *= transform3D::Scale(4, 1, 0.2);
    modelMatrix *= transform3D::RotateOY(RADIANS(90));
    modelMatrix *= transform3D::RotateOX(-RADIANS(90));
    RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix, glm::vec3(0.133, 0.655, 0.941), 0);
    for (int index = 0; index < cladiri_nr; index++)
    {
		cladire cladire = cladiri[index];
		modelMatrix = glm::mat4(1);
		modelMatrix *= transform3D::Translate(cladire.position.x, cladire.position.y, cladire.position.z);
        if (cladire.type == 0)
        {
            RenderSimpleMesh(meshes["cladire1"], shaders["LabShader"], modelMatrix, glm::vec3(0.7, 0.7, 0.7), 0);
        }
        else
        {
           RenderSimpleMesh(meshes["cladire2"], shaders["LabShader"], modelMatrix, glm::vec3(0.7, 0.7, 0.7), 0);
        }
	}
    for (int index = 0; index < enemy_nr; index++)
    {
		tank::Enemy enemy = enemies[index];
		modelMatrix = glm::mat4(1);
       
		modelMatrix *= transform3D::Translate(enemy.position.x, enemy.position.y, enemy.position.z);
        modelMatrix *= transform3D::RotateOY(enemy.angle);
       
        glm::mat4 modelMatrix2 = glm::mat4(1);
        modelMatrix2 *= transform3D::Translate(enemy.position.x, enemy.position.y, enemy.position.z);
        modelMatrix2 *= transform3D::RotateOY(enemy.angleturret );
	
        RenderSimpleMesh(meshes["senile"], shaders["LabShader"], modelMatrix,glm::vec3(0.3,0.3,0.3), 0);
        RenderSimpleMesh(meshes["Frame"], shaders["LabShader"], modelMatrix, glm::vec3(1, 0.1, 0.1), enemy.hit);
        RenderSimpleMesh(meshes["Turela"], shaders["LabShader"], modelMatrix2, glm::vec3(0.8, 0.1, 0.1), enemy.hit);
        RenderSimpleMesh(meshes["Tun"], shaders["LabShader"], modelMatrix2, glm::vec3(0, 0, 0), 0);


	}
    glm::vec3 position = tank->position;
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(position.x, 0, position.z);
    modelMatrix *= transform3D::RotateOY(tank->angle);

    glm::mat4 modelMatrix2 = glm::mat4(1);
    modelMatrix2 *= transform3D::Translate(position.x, 0, position.z);
    modelMatrix2 *= transform3D::RotateOY(tank->angleturret + tank->angle);

    RenderSimpleMesh(meshes["senile"], shaders["LabShader"], modelMatrix, glm::vec3(0.3, 0.3, 0.3), 0);
    RenderSimpleMesh(meshes["Frame"], shaders["LabShader"], modelMatrix, glm::vec3(0, 0.4, 0.7), tank->hit);
    RenderSimpleMesh(meshes["Turela"], shaders["LabShader"], modelMatrix2, glm::vec3(0, 0.6, 0.7), tank->hit);
    RenderSimpleMesh(meshes["Tun"], shaders["LabShader"], modelMatrix2, glm::vec3(0, 0, 0), 0);

    
    

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(light->position.x, light->position.y, light->position.z);
    
   modelMatrix *= transform3D::Scale(0.1, 0.1, 0.1);
    
    
    RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix,glm::vec3(1,1,1),0);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(light2->position.x, light2->position.y, light2->position.z);

    modelMatrix *= transform3D::Scale(0.1, 0.1, 0.1);


    RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix, glm::vec3(1, 1, 1), 0);

   
    
}

void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

bool Tema2::CollisionSphereRectangle(cladire cladire, tank::Proiectil proiectil)
{
    glm::vec3 position2 = cladire.position;
    glm::vec3 position = proiectil.position;
		
        float closestX = std::max(position2.x - cladire.width / 2.0f, std::min(position.x, position2.x + cladire.width / 2.0f));

      
        float closestZ = std::max(position2.z - cladire.length / 2.0f, std::min(position.z, position2.z + cladire.length / 2.0f));

        
        float distanceX = position.x - closestX;
        float distanceZ = position.z - closestZ;

      
        float distanceSquared = distanceX * distanceX + distanceZ * distanceZ;
      
        if (distanceSquared < 0.1 * 0.1) {
            return true;

        }
        return false;
}

bool Tema2::Collision2Rectangle(cladire cladire,glm::vec3 position,float halfWidth2,float halfHeight2)
{
        glm::vec3 position2 = cladire.position;
        float halfWidth1 = cladire.width / 2;
        float    halfHeight1 = cladire.length / 2;
       
        float   deltaX = abs(position.x - position2.x);
        float   deltaZ = abs(position.z - position2.z);

        float   combinedHalfWidth = halfWidth1 + halfWidth2;
        float   combinedHalfHeight = halfHeight1 + halfHeight2;

        if ((deltaX < combinedHalfWidth) && (deltaZ < combinedHalfHeight)) {
            return true;
        }
        return false;
}

void Tema2::Update(float deltaTimeSeconds)
{
    for (int index = 0; index < enemies.size(); index++)
    {
        if (enemies[index].hit == 3 && enemies[index].score == 0)
        {
            enemies[index].score = 1;
			score +=40;
			
		}
	}
   

    float timpul = Engine::GetElapsedTime();
 
    if (timpul > 100)
    {
        printf("Scorul tau este %d\n", score);
        
    }
   
    DrawScene();
    if (tank->hit == 3)
    {
        printf("Game over\n");
    }
    glm::vec3 position = tank->GetTargetPosition();
    if (abs(position.x) > 100 || abs(position.z) > 100)
    {
        tank->SetOriigin();
        light->SetOrigin();
        light2->SetOrigin();
        camera->Set(glm::vec3(4, 2, 0), glm::vec3(0, 2, 0), glm::vec3(0, 1, 0));
        tank->angle = 0;
        tank->angleturret = 0;
        light->position.x = tank->position.x - 1.3;
        light->position.y = 0.5;
        light->position.z = tank->position.z + 0.7;

        light2->position.x = tank->position.x - 1.3;
        light2->position.y = 0.5;
        light2->position.z = tank->position.z - 0.7;

	}
    tank->timebulet += deltaTimeSeconds;

    for (int index = 0; index < proiectile.size(); index++)
    {
        proiectile[index].TranslateForward(30.0f * deltaTimeSeconds);
		glm::vec3 position = proiectile[index].position;
        proiectile[index].life += deltaTimeSeconds;
        if (proiectile[index].life > 3)
        {
			proiectile.erase(proiectile.begin() + index);
			continue;
		}
        glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix *= transform3D::Translate(position.x, position.y, position.z);
        modelMatrix *= transform3D::RotateOY(proiectile[index].angle);
        RenderSimpleMesh(meshes["Proiectil"], shaders["LabShader"], modelMatrix,glm::vec3(0,0,0), 0);

      
            for (int index2 = 0; index2 < enemies.size(); index2++)
            {
                if (proiectile[index].enemy == index2)
                {
                    					continue;
                }
                tank::Enemy* enemy = &enemies[index2];
                glm::vec3 position2 = enemy->position;
                float dist = sqrt((position.x - position2.x) * (position.x - position2.x) + (position.z - position2.z) * (position.z - position2.z));

                if (dist < enemy->raza + 0.1)
                {
                    enemy->setHit(enemy->hit + 1);
                    proiectile.erase(proiectile.begin() + index);
                    check = 1;
                    break;
                }
            }
     

            glm::vec3 positon1 = tank->position;
            float dist = sqrt((position.x - positon1.x) * (position.x - positon1.x) + (position.z - positon1.z) * (position.z - positon1.z));
            if (dist < tank->raza)
            {

                tank->hit += 1;
                proiectile.erase(proiectile.begin() + index);
                check = 1;
                break;
            }
        
    
	}
    for (int index = 0; index < cladiri.size(); index++)
    {
        cladire cladire = cladiri[index];
        glm::vec3 position2 = cladire.position;
        if (Collision2Rectangle(cladire,tank->position,tank->width/2,tank->length/2) == true)
        {
			float dist = sqrt((position.x - position2.x) * (position.x - position2.x) + (position.z - position2.z) * (position.z - position2.z));
            //float p = cladire.length/2  + tank->raza - dist;
			glm::vec3 dif = glm::normalize(position - position2);
            float d = std::sqrt(std::pow(cladire.width, 2) + std::pow(cladire.length, 2));
			glm::vec3 p1 = (dist-tank->raza + d/2) * dif;
            if (cladire.type == 0)
            {

                tank->position += p1 * 0.02f;
                camera->position += p1 * 0.02f;
                light->position += p1 * 0.02f;
                light2->position += p1 * 0.02f;
            }
            else
            {
                tank->position += p1 * 0.01f;
                camera->position += p1 * 0.01f;
                light->position += p1 * 0.01f;
                light2->position += p1 * 0.01f;

            }
		}
        for (int index2 = 0; index2 < proiectile.size(); index2++)
        {
            tank::Proiectil proiectil = proiectile[index2];
            if (CollisionSphereRectangle(cladire, proiectil) == true)
            {
                proiectile.erase(proiectile.begin() + index2);
            }


        }
          
        for (int index2 = 0; index2 < enemies.size(); index2++)
        {
            tank::Enemy* enemy = &enemies[index2];
            glm::vec3 position3 = enemy->position;
            if (Collision2Rectangle(cladire, enemy->position,enemy->width/2,enemy->length/2) == true)
            {
                float dist = sqrt((position3.x - position2.x) * (position3.x - position2.x) + (position3.z - position2.z) * (position3.z - position2.z));
          
                glm::vec3 dif = glm::normalize(position3 - position2);
                float d = std::sqrt(std::pow(cladire.width, 2) + std::pow(cladire.length, 2));
                glm::vec3 p1 = (dist - enemy->raza + d / 2) * dif;
                if (cladire.type == 0)
                {

                    enemy->position += p1 * 0.02f;
                 
                }
                else
                {
                    enemy->position += p1 * 0.01f;

                }
            }

        }
    }
    for (int index = 0; index < enemies.size(); index++)
    {
        tank::Enemy* enemy = &enemies[index];
        glm::vec3 position2 = enemy->position;
        float dist = sqrt((position.x - position2.x) * (position.x - position2.x) + (position.z - position2.z) * (position.z - position2.z));
      
        if (dist < enemy->raza + tank->raza)
        {
            float p = abs(enemy->raza + tank->raza - dist);
            glm::vec3 dif = glm::normalize(position - position2);
            glm::vec3 p1 = p * dif;
            tank->position += p1 * 0.5f;
            camera->position += p1 * 0.5f;
            enemies[index].position += p1 * -0.5f;
            light->position += p1 * 0.5f;
            light2->position += p1 * 0.5f;

        }
        for (int index2 = 0; index2 < enemies.size(); index2++)
        {
            if (index == index2)
            {
                continue;
            }
            tank::Enemy* enemy = &enemies[index2];
            glm::vec3 position3 = enemy->position;
            float dist = sqrt((position3.x - position2.x) * (position3.x - position2.x) + (position3.z - position2.z) * (position3.z - position2.z));

            if (dist < enemy->raza + tank->raza)
            {
                float p = abs(enemy->raza + tank->raza - dist);
                glm::vec3 dif = glm::normalize(position3 - position2);
                glm::vec3 p1 = p * dif;
              
                enemies[index].position += p1 * -0.5f;
                enemies[index2].position += p1 * +0.5f;
             

            }
        }
        enemy->time += deltaTimeSeconds;
       
        if (enemy->hit < 3 &&  tank->hit != 3 && timpul <100)
        {
            if (enemy->time > 6)
            {
                enemy->time = 0;
                enemy->state = enemy->GetNextMovementState(enemy->state);
            }
            if (enemy->GetMovementStateName(enemy->state) == "GoingForward")
            {
                enemy->TranslateForward(1.0f * deltaTimeSeconds);
            }
            else if (enemy->GetMovementStateName(enemy->state) == "GoingBackward")
            {
                enemy->TranslateForward(-1.0f * deltaTimeSeconds);

            }
            else if (enemy->GetMovementStateName(enemy->state) == "InPlaceRotationLeft")
            {
                enemy->RotateFirstPerson_OY(0.3f * deltaTimeSeconds);
                enemy->angle += 0.3f * deltaTimeSeconds;

            }
            else
            {
                enemy->RotateFirstPerson_OY(-0.3f * deltaTimeSeconds);
                enemy->angle -= 0.3f * deltaTimeSeconds;
            }

            if (dist < 20)
            {
                glm::vec3 vector1(enemy->position.x, 0, enemy->position.z);
                glm::vec3 vector2(tank->position.x, 0, tank->position.z);
                glm::vec3 directionToPlayer = vector1 - vector2;
                enemy->angleturret = -atan2(directionToPlayer.z, directionToPlayer.x);
                enemy->timebullet += deltaTimeSeconds;
                if (enemy->timebullet > 2 && enemy->hit != 3)
                {

                    enemy->timebullet = 0;
                    tank::Proiectil* proiectil = new tank::Proiectil();
                    proiectil->Set(enemy->GetTargetPosition(), enemy->GetTargetDirection(), enemy->GetTargetUp(), enemy->GetTargetRight());
                    proiectil->RotateFirstPerson_OY(enemy->angleturret - enemy->angle);
                    proiectil->angle = enemy->angleturret;
                    proiectil->life = 0;
                    proiectil->player = 0;
                    proiectil->enemy = index;
                    
                    proiectile.push_back(*proiectil);
                }
            }
        }
    }
    
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);
    check = true;
    DrawScene();
    check = false;
}

void Tema2::FrameEnd()
{
    
}

void Tema2::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix,glm::vec3 color,int hit)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // TODO(student): Get shader location for uniform mat4 "Model"
     int location  = glGetUniformLocation(shader->program, "Model");

    // TODO(student): Set shader uniform "Model" to modelMatrix
     glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // TODO(student): Get shader location for uniform mat4 "View"
     int location2 = glGetUniformLocation(shader->program, "View");

    // TODO(student): Set shader uniform "View" to viewMatrix
    
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    if (check == true)
    {
        glUniformMatrix4fv(location2, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrixOrto(tank)));
    }
    else
    {
        glUniformMatrix4fv(location2, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    }

    // TODO(student): Get shader location for uniform mat4 "Projection"
    int location3 = glGetUniformLocation(shader->program, "Projection");

    // TODO(student): Set shader uniform "Projection" to projectionMatrix
   
    glUniformMatrix4fv(location3, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    int location4 = glGetUniformLocation(shader->program, "Color");
    glUniform3fv(location4, 1, glm::value_ptr(color));

    int location5 = glGetUniformLocation(shader->program, "hit");
    glUniform1i(location5, hit);

    glUniform3fv(glGetUniformLocation(shader->program, "light_position"), 1, glm::value_ptr(light->position));
    glUniform3fv(glGetUniformLocation(shader->program, "light_position2"), 1, glm::value_ptr(light2->position));
    glUniform3fv(glGetUniformLocation(shader->program, "light_direction"), 1, glm::value_ptr(light->forward));

    glm::vec3 eyePosition = camera->position;
    // TODO(student): Set eye position (camera position) uniform
    glUniform3fv(glGetUniformLocation(shader->program, "eye_position"), 1, glm::value_ptr(eyePosition));

    glUniform1i(glGetUniformLocation(shader->program, "material_shininess"), materialShininess);
    glUniform1f(glGetUniformLocation(shader->program, "material_kd"), materialKd);
    glUniform1f(glGetUniformLocation(shader->program, "material_ks"), materialKs);



    

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    float timpul = Engine::GetElapsedTime();
    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT) && tank->hit != 3 && timpul <100)
    {
        // Add key press event
        float tankspeed = 1.0f;

        if (window->KeyHold(GLFW_KEY_W)) {
        
            tank->TranslateForward(tankspeed*10 * deltaTime);
            camera->MoveForward(tankspeed*10 * deltaTime,tank->forward);
         
            light->TranslateForward(tankspeed * 10 * deltaTime);
            light2->TranslateForward(tankspeed * 10 * deltaTime);

        }
        if (window->KeyHold(GLFW_KEY_A)) {
            // TODO(student): Translate the camera to the left
            float sensivityOY = 0.01f;
            tank->RotateFirstPerson_OY(tankspeed * sensivityOY);
            camera->RotateThirdPerson_OY(tankspeed * sensivityOY);
            tank->angle += tankspeed * sensivityOY;
         

            light->TranslateRight(0.7f);
           light->TranslateForward(-1.3f);

            light->RotateFirstPerson_OY(tankspeed * sensivityOY);
           light->TranslateForward(+1.3f);
            light->TranslateRight(-0.7f);

            light2->TranslateRight(-0.7f);
            light2->TranslateForward(-1.3f);
          
            light2->RotateFirstPerson_OY(tankspeed * sensivityOY);
            light2->TranslateForward(+1.3f);
            light2->TranslateRight(+0.7f);
            
            
           

        

           

        }

        if (window->KeyHold(GLFW_KEY_S)) {
            // TODO(student): Translate the camera backward
            tank->TranslateForward(-tankspeed*10 * deltaTime);
            camera->MoveForward(-tankspeed*10 * deltaTime,tank->forward);
            light->TranslateForward(-tankspeed * 10 * deltaTime);
            light2->TranslateForward(-tankspeed * 10 * deltaTime);

        }

        if (window->KeyHold(GLFW_KEY_D)) {
            // TODO(student): Translate the camera to the right
            float sensivityOY = 0.01f;
            tank->RotateFirstPerson_OY(-tankspeed * sensivityOY);
            camera->RotateThirdPerson_OY(-tankspeed * sensivityOY);
            tank->angle -= tankspeed * sensivityOY;
            light->TranslateRight(0.7f);
            light->TranslateForward(-1.3f);
       
            light->RotateFirstPerson_OY(-tankspeed * sensivityOY);
            light->TranslateForward(+1.3f);
            light->TranslateRight(-0.7f);

            light2->TranslateRight(-0.7f);
            light2->TranslateForward(-1.3f);

            light2->RotateFirstPerson_OY(-tankspeed * sensivityOY);
            light2->TranslateForward(+1.3f);
            light2->TranslateRight(+0.7f);


        }
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
  

}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    float sensivityOX = 0.001f;
    float sensivityOY = 0.001f;

  
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT) && cameraSave == 1)
    {
    
        
        camera->RotateThirdPerson_OX(-deltaY * sensivityOX);
        camera->RotateThirdPerson_OY(-deltaX * sensivityOY);

    }
    else
    {
        if (deltaX < 300 && deltaX > -300)
        {
            tank->angleturret += -deltaX * 2.0f * sensivityOY;
            window->DisablePointer();
        //    OnMouseBtnPress(mouseX, mouseY, GLFW_MOUSE_BUTTON_2, 0);

        }
    }

}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_2 && tank->timebulet > 2 && tank->hit != 3)
    {
        tank::Proiectil* proiectil = new tank::Proiectil();
        proiectil->Set(tank->GetTargetPosition(), tank->GetTargetDirection(), tank->GetTargetUp(), tank->GetTargetRight());
        proiectil->RotateFirstPerson_OY(tank->angleturret);
        proiectil->angle = tank->angle + tank->angleturret;
        proiectil->TranslateForward(3.3f);
        proiectil->life = 0;
        proiectil->player = 1;
        tank->timebulet = 0;
     



            proiectile.push_back(*proiectil);

    }
    if (button == GLFW_MOUSE_BUTTON_3 && cameraSave == 0)
    {
        
        cameraSave = 1;
       // camera->SaveState();
    }
    
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_3 && cameraSave == 1)
    {
       
        cameraSave = 0;
    }
    
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
