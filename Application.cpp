#include "Application.h"
#include "Physics/Constants.h"
#include "Physics/Force.h"
#include <iostream>

bool Application::IsRunning() {
    return running;
}

///////////////////////////////////////////////////////////////////////////////
// Setup function (executed once in the beginning of the simulation)
///////////////////////////////////////////////////////////////////////////////
void Application::Setup() {
    running = Graphics::OpenWindow();

    Particle* smallBall = new Particle(Graphics::Width() / 2, 100, 2.0, 10);
    particles.push_back(smallBall);

    anchor = Vec2(Graphics::Width() / 2, 30);

    // Particle* bigBall = new Particle(200, 100, 3.0);
    // bigBall->radius = 12;
    // particles.push_back(bigBall);

    /*liquid.x = 0;
    liquid.y = Graphics::Height() / 2;
    liquid.w = Graphics::Width();
    liquid.h = Graphics::Height() / 2;*/
}

///////////////////////////////////////////////////////////////////////////////
// Input processing
///////////////////////////////////////////////////////////////////////////////
void Application::Input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT)
                    // Create a new particle on click
                    particles.push_back(new Particle(event.button.x, event.button.y, 1.0, 4));
                if (event.button.button == SDL_BUTTON_RIGHT)
                    // Create a new particle on click
                    particles.push_back(new Particle(event.button.x, event.button.y, 100, 12));
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    running = false;
                if (event.key.keysym.sym == SDLK_UP)
                    pushForce.y = -50 * PIXELS_PER_METER;
                if (event.key.keysym.sym == SDLK_RIGHT)
                    pushForce.x = 50 * PIXELS_PER_METER;
                if (event.key.keysym.sym == SDLK_DOWN)
                    pushForce.y = 50 * PIXELS_PER_METER;
                if (event.key.keysym.sym == SDLK_LEFT)
                    pushForce.x = -50 * PIXELS_PER_METER;
                break;
            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_UP)
                    pushForce.y = 0;
                if (event.key.keysym.sym == SDLK_RIGHT)
                    pushForce.x = 0;
                if (event.key.keysym.sym == SDLK_DOWN)
                    pushForce.y = 0;
                if (event.key.keysym.sym == SDLK_LEFT)
                    pushForce.x = 0;
                break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Update function (called several times per second to update objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Update() {
    // Wait until we hit the target frame time in milliseconds
    static int timePreviousFrame;
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - timePreviousFrame);
    if (timeToWait > 0) {
        SDL_Delay(timeToWait);
    }

    // Calc the deltatime in seconds
    float deltaTime = (SDL_GetTicks() - timePreviousFrame) / 1000.0f;
    if (deltaTime > 0.016) {
        deltaTime = 0.016;
    }

    // Set the time of the current frame to be checked in the next
    timePreviousFrame = SDL_GetTicks();

    // Apply "wind" force to particle
    Vec2 wind = Vec2(0.2 * PIXELS_PER_METER, 0.0);

    for (auto particle : particles) {
        // Add wind
        // particle->AddForce(wind);

        // Add weight
         Vec2 weight = Vec2(0.0, GRAVITY * particle->mass * PIXELS_PER_METER);
         particle->AddForce(weight);

        particle->AddForce(pushForce);

        Vec2 drag = Force::GenerateDragForce(*particle, 0.001);
        particle->AddForce(drag);


        // TODO: Apply a drag force if we are inside the liquid
        /*if (particle->position.y >= liquid.y) {
            Vec2 drag = Force::GenerateDragForce(*particle, 0.03);
            particle->AddForce(drag);
        }*/

        // This allows us to add gravitation force between all particles
        //for (auto check_particle : particles) {
        //    if (check_particle != particle) {
        //        Vec2 attraction = Force::GenerateGravitationalForce(*particle, *check_particle, 10.0 * PIXELS_PER_METER, 20, 300);
        //        particle->AddForce(attraction);
        //    }
        //}

        // Apply a spring force between all particles
        for (auto check_particle : particles) {
            if (check_particle != particle) {
                Vec2 springForce = Force::GenerateSpringForce(*particle, check_particle->position, restLenght, k);
                particle->AddForce(springForce);
            }
        }
    }
    
    
    for (auto particle : particles) {
        // Integrate
        particle->Integrate(deltaTime);
    }

    for (auto particle : particles) {
        // TODO:
        // Check the particle position against the window edge
        if ((particle->position.y + particle->radius / 2) >= Graphics::Height()) {
            particle->position = Vec2(particle->position.x, Graphics::Height() - particle->radius);
            particle->velocity.y *= -0.9;
        }
        else if ((particle->position.y - particle->radius / 2) <= 0) {
            particle->position = Vec2(particle->position.x, 0 + particle->radius);
            particle->velocity.y *= -0.9;
        }
        if ((particle->position.x + particle->radius / 2) >= Graphics::Width() || (particle->position.y + particle->radius / 2) <= 0) {
            particle->position = Vec2(Graphics::Width() - particle->radius, particle->position.y);
            particle->velocity.x *= -0.9;
        }
        else if ((particle->position.x - particle->radius / 2) <= 0) {
            particle->position = Vec2(0 + particle->radius, particle->position.y);
            particle->velocity.x *= -0.9;
        }
    }


}

///////////////////////////////////////////////////////////////////////////////
// Render function (called several times per second to draw objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Render() {
    Graphics::ClearScreen(0xFF056263);
    
    //Draw spring
    // Graphics::DrawLine(anchor.x, anchor.y, particles[0]->position.x, particles[0]->position.y, 0xFF313131);
    // Draw the anchor
    // Graphics::DrawFillCircle(anchor.x, anchor.y, 5, 0xFF001155);
    // Draw the bob

    // Draw the scenes liquid
    //Graphics::DrawFillRect(liquid.x + liquid.w / 2, liquid.y + liquid.h / 2, liquid.w, liquid.h, 0xFF6e3713);

    // Render all particles
    for (auto particle : particles) {
        Graphics::DrawFillCircle(particle->position.x, particle->position.y, particle->radius, 0xFFFFFFFF);
        // Draw a spring line between each particle
        for (auto check_particle : particles) {
            Graphics::DrawLine(check_particle->position.x, check_particle->position.y, particle->position.x, particle->position.y, 0xFF313131);
        }
    }
    Graphics::RenderFrame();
}

///////////////////////////////////////////////////////////////////////////////
// Destroy function to delete objects and close the window
///////////////////////////////////////////////////////////////////////////////
void Application::Destroy() {
    // TODO: destroy all objects in the scene
    for (auto particle : particles) {
        delete particle;
    }
    Graphics::CloseWindow();
}