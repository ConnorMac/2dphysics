#ifndef APPLICATION_H
#define APPLICATION_H

#include "./Graphics.h"
#include "Physics/Particle.h"
#include <vector>

class Application {
    private:
        bool running = false;

        std::vector<Particle*> particles;
        Vec2 pushForce = Vec2(0.0, 0.0);
        SDL_Rect liquid;

        Vec2 anchor = Vec2(0, 0);
        float k = 10;
        float restLenght = 300;

    public:
        Application() = default;
        ~Application() = default;
        bool IsRunning();
        void Setup();
        void Input();
        void Update();
        void Render();
        void Destroy();
};

#endif