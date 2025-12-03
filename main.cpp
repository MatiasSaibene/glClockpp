#include "main.hpp"
#include <SDL3/SDL.h>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <ctime>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "glad/include/glad/glad.h"

#include <glm/trigonometric.hpp>
#include <iostream>

glClockpp::glClockpp(){

    //initialize camera
    lastX = SCREEN_WIDTH / 2.0f;
    lastY = SCREEN_HEIGHT / 2.0f;
    firstMouse = true;
    rotating = false;
    camera.Yaw = 0.0f;

    //initialize timing
    deltaTime = 0.0f;
    lastFrame = 0.0f;

    //initialize time variables
    hours = 0;
    minutes = 0;
    hourAngle = 0.0f;
    minuteAngle = 0.0f;

    //initialize window
    gWindow = nullptr;
    gRenderer = nullptr;

    window_Width = 4;
    window_Height = 3;

    SDL_zero(event);
}

glClockpp::~glClockpp(){

}

bool glClockpp::initializeSDL(){

    bool success{true};

    if(SDL_Init(SDL_INIT_VIDEO) == false){
        SDL_Log("SDL couldt not initialize! SDL error: %s\n", SDL_GetError());
        success = false;
    } else {
        if(SDL_CreateWindowAndRenderer("Initializing glClock++ - v0.2 Beta - (c)2025 Matías Saibene", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, &gWindow, &gRenderer) == false){
            SDL_Log( "Window could not be created! SDL error: %s\n", SDL_GetError() );
            success = false;
        } else {
            ctx = SDL_GL_CreateContext(gWindow);
            if(!ctx){
                SDL_Log("GL Context error: %s\n", SDL_GetError());
                success = false;
            } else {
                SDL_GL_MakeCurrent(gWindow, ctx);
                success = true;
            }
        }
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    return success;

}

int main(int argc, char *argv[]){

    //Useful variables
    int exitCode{0};
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;

    glClockpp glClock;

    //glfw: initialize and configure;
    if(glClock.initializeSDL() == false){
        SDL_Log("Unable to initialize program!\n");
        exitCode = 1;
    }

    Camera &camera = glClock.getCamera();
    camera.Yaw = 0.0f;

    SDL_Window *window = glClock.getWindow();
    

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glEnable(GL_CULL_FACE);

    // build and compile shaders
    // -------------------------
    Shader modelShader("res/model_shader.vs", "res/model_shader.fs");

    // load models
    // -----------
    Model clockModel("res/3DClock.obj");
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Model hourHand("res/Hours_hand.obj");
    Model minutesHand("res/Minutes_hand.obj");
    Model glassCover("res/glass.obj");

    glClock.UpdateWindowTitle(window);

    bool quit{false};

    SDL_Event *e = glClock.getEvent();

    // render loop
    // -----------
    while(quit == false){

        while(SDL_PollEvent(e) == true){
            
            switch(e->type){

                case SDL_EVENT_QUIT:
                    quit = true;
                    break;

                case SDL_EVENT_KEY_DOWN:
                    glClock.handleKeyboardEvent(*e);
                    break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    glClock.handleMouseEvent(window, *e);
                    break;

                case SDL_EVENT_MOUSE_MOTION:
                    glClock.handleMouseMotionEvent(*e);
                    break;

                case SDL_EVENT_MOUSE_WHEEL:
                    glClock.handleMouseScrollEvent(*e);
                    break;                

                case SDL_EVENT_WINDOW_RESIZED:
                    glClock.handleWindowSizeChange();
                    break;
            }
        }

        // per-frame time logic
        // --------------------
        constexpr Uint64 nsPerFrame = 1000000000 / 60;
        Uint64 frameNS{SDL_GetTicksNS()};
        if(frameNS < nsPerFrame){
            SDL_DelayNS(nsPerFrame - frameNS);
        }

        //Delta time
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        glClock.setDeltaTime((double)((NOW - LAST)*1000 / (double)SDL_GetPerformanceFrequency()));

        // render
        // ------
        glClearColor(0.06301f, 0.024157f, 0.283149f, 1.0f);
        //glClearColor(1.0f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glClock.drawGirodNormal(modelShader, clockModel, hourHand, minutesHand, glassCover);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        SDL_GL_SwapWindow(window);
    }

    return exitCode;
}

//Clock drawing functions

void glClockpp::drawGirodNormal(Shader &modelShader, Model &clockModel, Model &hoursHandModel, Model &minutesHandModel, Model &glassCoverModel, ...){

    // don't forget to enable shader before setting uniforms
    modelShader.use();
    // Material settings
    modelShader.setFloat("material.shininess", 32.0f);
    modelShader.setVec3("viewPos", camera.Position);


    auto *lTime = getLocalTime();
    hours = lTime->tm_hour;
    minutes = lTime->tm_min;

    hourAngle = -((hours + minutes / 60.0f) * 30.0f);
    minuteAngle = -(minutes * 6.0f);

    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
    glm::vec3(0.2f, 0.1f, -0.1f),
    glm::vec3(-0.2f, 0.1f, -0.1f),
    glm::vec3(0.0f, 0.1f, 0.2f)
    };

    // point light 1
        modelShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        modelShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        modelShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        modelShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        modelShader.setFloat("pointLights[0].constant", 1.0f);
        modelShader.setFloat("pointLights[0].linear", 0.09f);
        modelShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        modelShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        modelShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        modelShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        modelShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        modelShader.setFloat("pointLights[1].constant", 1.0f);
        modelShader.setFloat("pointLights[1].linear", 0.09f);
        modelShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        modelShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        modelShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        modelShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        modelShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        modelShader.setFloat("pointLights[2].constant", 1.0f);
        modelShader.setFloat("pointLights[2].linear", 0.09f);
        modelShader.setFloat("pointLights[2].quadratic", 0.032f);
        // spotLight...


        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)window_Width / window_Height, 0.1f, 100.0f);
        //glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 view = camera.GetViewMatrix();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);

        clockModel.Draw(modelShader);

        glm::mat4 hourModel = glm::rotate(glm::mat4(1.0f), glm::radians(hourAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        modelShader.setMat4("model", hourModel);
        hoursHandModel.Draw(modelShader);

        glm::mat4 minuteModel = glm::rotate(glm::mat4(1.0f), glm::radians(minuteAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        modelShader.setMat4("model", minuteModel);
        minutesHandModel.Draw(modelShader);

        glassCoverModel.Draw(modelShader);
}

//Misc functions

std::tm *glClockpp::getLocalTime(){

    std::time_t now = std::time(nullptr);
    std::tm *localtime = std::localtime(&now);

    return localtime;
}

void glClockpp::UpdateWindowTitle(SDL_Window *window){
    
    auxinfo.clear();

    auxinfo = "glClock++ v0.2 Beta - (c)2025, Matías Saibene";
    auxinfo += " | ";
    auxinfo += "OpenGL info:";
    auxinfo += " ";
    auxinfo += reinterpret_cast<const char *>(glGetString(GL_VERSION));
    auxinfo += " ";
    auxinfo += reinterpret_cast<const char *>(glGetString(GL_RENDERER));
    auxinfo += " ";
    auxinfo += reinterpret_cast<const char *>(glGetString(GL_VENDOR));


    title = auxinfo;
    SDL_SetWindowTitle(window, title.c_str());
}

//Handlers

void glClockpp::handleKeyboardEvent(SDL_Event &e){

    SDL_Event quit_event;
    SDL_zero(quit_event);
    quit_event.type = SDL_EVENT_QUIT;

    Camera &camera = getCamera();

    float dTime = getDeltaTime();

    switch(e.key.key){

        case SDLK_ESCAPE:
            SDL_PushEvent(&quit_event);
            break;
        
        case SDLK_W:
            camera.ProcessKeyboard(FORWARD, dTime/10);
            break;
        
        case SDLK_S:
            camera.ProcessKeyboard(BACKWARD, dTime/10);
            break;

        case SDLK_A:
            camera.ProcessKeyboard(LEFT, dTime/10);
            break;

        case SDLK_D:
            camera.ProcessKeyboard(RIGHT, dTime/10);
            break;

        default:
            break;

    }

}

void glClockpp::handleMouseEvent(SDL_Window *window, SDL_Event &e) {
    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            setMouseRotating(true);
            SDL_SetWindowRelativeMouseMode(window, true);
        }
    } else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            setMouseRotating(false);
            SDL_SetWindowRelativeMouseMode(window, false);
        }
    }
}

void glClockpp::handleMouseMotionEvent(SDL_Event &e) {
    if (!getMouseRotating()) return;

    float xoffset = e.motion.xrel;
    float yoffset = e.motion.yrel;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void glClockpp::handleMouseScrollEvent(SDL_Event &e){
    Camera &camera = getCamera();

    if(e.type == SDL_EVENT_MOUSE_WHEEL){
        camera.ProcessMouseScroll(e.wheel.y);
    }
}

void glClockpp::handleWindowSizeChange(){

    SDL_GetWindowSizeInPixels(gWindow, &window_Width, &window_Height);

    glViewport(0, 0, window_Width, window_Height);

}