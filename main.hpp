#pragma once

#include "Model.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <ctime>
#ifndef MAIN_HPP
#define MAIN_HPP

#include "Camera.hpp"

#include <SDL3/SDL.h>
#include <assimp/light.h>
#include "Shader.hpp"
#include "stb_image.h"

//window settings
constexpr unsigned int SCREEN_WIDTH{640};
constexpr unsigned int SCREEN_HEIGHT{480};

class glClockpp{
    public:
        
        glClockpp();
        ~glClockpp();

        void drawGirodNormal(Shader &modelShader, Model &clockModel, Model &hourModel, Model &minuteModel, Model &glassCoverModel, ...);

        std::tm *getLocalTime();

        bool initializeSDL();

        //Handlers
        void handleKeyboardEvent(SDL_Event &event);
        void handleMouseEvent(SDL_Window *window, SDL_Event &event);
        void handleMouseMotionEvent(SDL_Event &event);
        void handleMouseScrollEvent(SDL_Event &event);
        void handleWindowSizeChange();
        
        //Getters and setters
        Camera &getCamera(){return camera;}
        SDL_Window *getWindow(){return gWindow;}
        SDL_Event *getEvent(){return &event;}
        float getDeltaTime() const {return deltaTime;}
        void setDeltaTime(float dTime){deltaTime = dTime;}
        bool getMouseRotating() const {return rotating;}
        void setMouseRotating(bool rMouse){rotating = rMouse;}
        float getWindowWidth() const {return window_Width;}
        float getWindowHeight() const {return window_Height;}

        void UpdateWindowTitle(SDL_Window *window);

    private:

        //camera variables
        Camera camera;
        float lastX;
        float lastY;
        bool firstMouse;
        bool rotating;

        //timing
        float deltaTime;
        float lastFrame;

        //Time variables
        int hours;
        int minutes;
        float hourAngle;
        float minuteAngle;

        //Window and title info
        int window_Width;
        int window_Height;
        SDL_Window *gWindow;
        SDL_Renderer *gRenderer;
        SDL_Event event;
        SDL_GLContext ctx;
        std::string title;
        std::string auxconst;
        std::string auxinfo;
        std::string oglInfo;
};



#endif //!_MAIN_HPP