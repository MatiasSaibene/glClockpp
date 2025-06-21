#pragma once

#include <ctime>
#ifndef MAIN_HPP
#define MAIN_HPP

#include "Camera.hpp"

#include <GLFW/glfw3.h>
#include <assimp/light.h>
#include "Shader.hpp"
#include "stb_image.h"

//window settings
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

class glClock_NG{
    public:
        
        glClock_NG();
        ~glClock_NG();

        static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
        static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
        static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
        static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
        static void processInput(GLFWwindow *window);

        void UpdateWindowTitle(GLFWwindow *window);

        //Getters and setters
        bool getMouse() const {return firstMouse;}
        void setMouse(bool fMouse){firstMouse = fMouse;}

        bool getMouseRotating() const {return rotating;}
        void setMouseRotating(bool rMouse){rotating = rMouse;}

        float getDeltaTime() const {return deltaTime;}
        void setDeltaTime(float dTime){deltaTime = dTime;}

        float getLastFrame() const {return lastFrame;}
        void setLastFrame(float lFrame){lastFrame = lFrame;}

        GLFWwindow *getWindow() const {return window;}

        Camera &getCamera(){return camera;}

        std::tm *getLocalTime();

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

        //Window and title info
        GLFWwindow *window;
        std::string title;
        std::string auxconst;
        std::string auxinfo;
        std::string oglInfo;
};


#endif //!_MAIN_HPP