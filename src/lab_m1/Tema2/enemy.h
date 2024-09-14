#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <random>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp> 








namespace tank
{
    class Enemy
    {
     public:
        Enemy()
        {
            position    = glm::vec3(0, 0, 0);
            forward     = glm::vec3(-1, 0, 0);
            up          = glm::vec3(0, 1, 0);
            right       = glm::vec3(1, 0, 0);
            distanceToTarget = 2;
        }

        Enemy(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
        {
            Set(position, center, up);
        }

        ~Enemy()
        { }

        int GetRandomNumberInRange(const int minInclusive, const int maxInclusive)
        {
            static std::random_device randomDevice;
            static std::default_random_engine randomEngine(randomDevice());

            std::uniform_int_distribution<int> uniformDist(minInclusive, maxInclusive);

            return uniformDist(randomEngine);
        }

        enum class MovementState
        {
            GoingForward,
            GoingBackward,
            InPlaceRotationLeft,
            InPlaceRotationRight
        };

        MovementState GetNextMovementState(const MovementState currentState)
        {
            int randomChange = GetRandomNumberInRange(0, 3);
            MovementState nextState = currentState;

            if (randomChange == 0)
            {
                nextState = MovementState::GoingForward;
            }
            else if (randomChange == 1)
            {
				nextState = MovementState::GoingBackward;
			}
            else if (randomChange == 2)
            {
				nextState = MovementState::InPlaceRotationLeft;
			}
            else if (randomChange == 3)
            {
				nextState = MovementState::InPlaceRotationRight;
			}


            return nextState;
        }

        std::string GetMovementStateName(const MovementState state)
        {
            static const std::map<MovementState, std::string> kStateNames
            {
                { MovementState::GoingForward, "GoingForward" },
                { MovementState::GoingBackward, "GoingBackward" },
                { MovementState::InPlaceRotationLeft, "InPlaceRotationLeft" },
                { MovementState::InPlaceRotationRight, "InPlaceRotationRight" },
            };

            std::string s = "";

            if (kStateNames.find(state) != kStateNames.end())
            {
                s = kStateNames.at(state);
            }

            return s;
        }

        // Update camera
        void Set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
        {
            this->position = position;
            forward     = glm::normalize(center - position);
            right       = glm::cross(forward, up);
            this->up    = glm::cross(right, forward);
        }

        void MoveForward(float distance)
        {
            // Translates the camera using the `dir` vector computed from
            // `forward`. Movement will always keep the camera at the same
            // height. For example, if you rotate your head up/down, and then
            // walk forward, then you will still keep the same relative
            // distance (height) to the ground!
            glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
            position += dir * distance;
        }

        void TranslateForward(float distance)
        {
            // TODO(student): Translate the camera using the `forward` vector.
            // What's the difference between `TranslateForward()` and
            // `MoveForward()`?
            position += glm::normalize(forward) * distance;
        }

        void TranslateUpward(float distance)
        {
            // TODO(student): Translate the camera using the `up` vector.
            position += glm::normalize(up) * distance;

        }

        void TranslateRight(float distance)
        {
            // TODO(student): See instructions below. Read the entire thing!
            // You need to translate the camera using the `right` vector.
            // Usually, however, translation using camera's `right` vector
            // is not very useful, because if the camera is rotated around the
            // `forward` vector, then the translation on the `right` direction
            // will have an undesired effect, more precisely, the camera will
            // get closer or farther from the ground. The solution is to
            // actually use the projected `right` vector (projected onto the
            // ground plane), which makes more sense because we will keep the
            // same distance from the ground plane.
            position += glm::normalize(right) * distance;


        }

        void RotateFirstPerson_OX(float angle)
        {
            // TODO(student): Compute the new `forward` and `up` vectors.
            // Don't forget to normalize the vectors! Use `glm::rotate()`.
            forward = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, right) * glm::vec4(forward, 1)));
            up = glm::normalize(glm::cross(right, forward));



        }

        void RotateFirstPerson_OY(float angle)
        {
            // TODO(student): Compute the new `forward`, `up` and `right`
            // vectors. Use `glm::rotate()`. Don't forget to normalize the
            // vectors!
            forward = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 1)));
            right = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1)));
            up = glm::normalize(glm::cross(right, forward));
        }

        void RotateFirstPerson_OZ(float angle)
        {
            // TODO(student): Compute the new `right` and `up`. This time,
            // `forward` stays the same. Use `glm::rotate()`. Don't forget
            // to normalize the vectors!
            right = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle,forward) * glm::vec4(right, 1)));
            up = glm::normalize(glm::cross(right, forward));

        }

        void RotateThirdPerson_OX(float angle)
        {
            // TODO(student): Rotate the camera in third-person mode around
            // the OX axis. Use `distanceToTarget` as translation distance.
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OX(angle);
            TranslateForward(-distanceToTarget);

        }

        void RotateThirdPerson_OY(float angle)
        {
            // TODO(student): Rotate the camera in third-person mode around
            // the OY axis.
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OY(angle);
            TranslateForward(-distanceToTarget);

        }

        void RotateThirdPerson_OZ(float angle)
        {
            // TODO(student): Rotate the camera in third-person mode around
            // the OZ axis.
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OZ(angle);
            TranslateForward(-distanceToTarget);


        }

        glm::mat4 GetViewMatrix()
        {
            // Returns the view matrix
            return glm::lookAt(position, position + forward, up);
        }

        glm::vec3 GetTargetPosition()
        {
            return position;
        }
        glm::vec3 GetTargetDirection()
        {
            return forward;
        }
        glm::vec3 GetTargetUp()
        {
			return up;
		}
        glm::vec3 GetTargetRight()
        {
            return right;
        }
        void setHit(int x)
		{
			hit = x;
		}

     



     public:
        float distanceToTarget;
        glm::vec3 position;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;
        float raza = 2.0f;
        float time = 0;
        float angle = 0;
        float angleturret = 0;
        int hit = 0;
        float timebullet = 0;
        MovementState state =GetNextMovementState(MovementState::GoingForward);
        float  width = 4;
        float  length = 3;
        int score = 0;
    };
}   // namespace implemented
