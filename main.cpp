#include "main.hpp"
#include <GLFW/glfw3.h>

#include <cstring>
#include <ctime>
#include <cstddef>
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

glClock_NG::glClock_NG() : camera(glm::vec3(0.0f, 0.0f, 0.25f)){

    //initialize camera
    lastX = SCREEN_WIDTH / 2.0f;
    lastY = SCREEN_HEIGHT / 2.0f;
    firstMouse = true;
    rotating = false;

    //initialize timing
    deltaTime = 0.0f;
    lastFrame = 0.0f;

    //initialize window
    window = nullptr;
}

glClock_NG::~glClock_NG(){

}

int main(){

    glClock_NG glClock;

    auto window = glClock.getWindow();

    //glfw: initialize and configure;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //glfw window creation
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Initializing glClock++ - v1.0 Beta - (c)2025 Matías Saibene", NULL, NULL);
    if(window == NULL){
        std::cout << "Failed to create a GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, &glClock);
    glfwSetFramebufferSizeCallback(window, glClock_NG::framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, glClock_NG::mouse_button_callback);
    glfwSetCursorPosCallback(window, glClock_NG::mouse_callback);
    glfwSetScrollCallback(window, glClock_NG::scroll_callback);


    Camera &camera = glClock.getCamera();
    
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(-0.1305f, 0.0973f, 0.1043f),
        glm::vec3(0.1468f, 0.0973f, 0.1043f),
    };

    // build and compile shaders
    // -------------------------
    Shader modelShader("model_shader.vs", "model_shader.fs");

    // load models
    // -----------
    Model clockModel("3DClock.obj");
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Model hourHand("Hours_hand.obj");
    Model minutesHand("Minutes_hand.obj");

    int hours = 0;
    int minutes = 0;
    float hourAngle = 0.0f;
    float minuteAngle = 0.0f;

    glClock.UpdateWindowTitle(window);


    // render loop
    // -----------
    while(!glfwWindowShouldClose(window)){

        auto *lTime = glClock.getLocalTime();
        hours = lTime->tm_hour;
        minutes = lTime->tm_min;

        hourAngle = -((hours + minutes / 60.0f) * 30.0f);
        minuteAngle = -(minutes * 6.0f);

        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        float dTime = currentFrame - glClock.getLastFrame();

        glClock.setDeltaTime(dTime);
        glClock.setLastFrame(currentFrame);

        glClock.processInput(window);

        // render
        // ------
        glClearColor(0.06301f, 0.024157f, 0.283149f, 1.0f);
        //glClearColor(1.0f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        modelShader.use();

        // Material
        modelShader.setFloat("material.shininess", 32.0f);
        modelShader.setVec3("viewPos", camera.Position);

        // directional light
        modelShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        modelShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        modelShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        modelShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
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
        // spotLight...


        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
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
        hourHand.Draw(modelShader);

        glm::mat4 minuteModel = glm::rotate(glm::mat4(1.0f), glm::radians(minuteAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        modelShader.setMat4("model", minuteModel);
        minutesHand.Draw(modelShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void glClock_NG::processInput(GLFWwindow *window){

    auto *instance = static_cast<glClock_NG *>(glfwGetWindowUserPointer(window));

    Camera &camera = instance->getCamera();

    float dTime = instance->getDeltaTime();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, dTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, dTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, dTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, dTime);

}

void glClock_NG::framebuffer_size_callback(GLFWwindow *window, int width, int height){

    glViewport(0, 0, width, height);

}

void glClock_NG::mouse_button_callback(GLFWwindow *window, int button, int action, int mods){

    auto *instance = static_cast<glClock_NG *>(glfwGetWindowUserPointer(window));

    if(button == GLFW_MOUSE_BUTTON_LEFT){
        if(action == GLFW_PRESS){
            instance->setMouseRotating(true);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else if(action == GLFW_RELEASE){
            instance->setMouseRotating(false);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void glClock_NG::mouse_callback(GLFWwindow *window, double xposIn, double yposIn){
    
    auto *instance = static_cast<glClock_NG *>(glfwGetWindowUserPointer(window));

    auto fMouse = instance->getMouse();
    auto rMouse = instance->getMouseRotating();

    if(!rMouse) return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if(fMouse){
        instance->lastX = xpos;
        instance->lastY = ypos;
        instance->setMouse(false);
    }

    float xoffset = xpos - instance->lastX;
    float yoffset = instance->lastY - ypos;

    instance->lastX = xpos;
    instance->lastY = ypos;

    instance->camera.ProcessMouseMovement(xoffset, yoffset);

}

void glClock_NG::scroll_callback(GLFWwindow *window, double xoffset, double yoffset){

    auto *instance = static_cast<glClock_NG *>(glfwGetWindowUserPointer(window));

    Camera &camera = instance->getCamera();

    camera.ProcessMouseScroll(yoffset);
}

std::tm *glClock_NG::getLocalTime(){

    std::time_t now = std::time(nullptr);
    std::tm *localtime = std::localtime(&now);

    return localtime;
}

void glClock_NG::UpdateWindowTitle(GLFWwindow *window){
    
    auxinfo.clear();

    auxinfo = "glClock++ v1.0 Beta - (c)2025, Matías Saibene";
    auxinfo += " | ";
    auxinfo += "OpenGL info:";
    auxinfo += " ";
    auxinfo += reinterpret_cast<const char *>(glGetString(GL_VERSION));
    auxinfo += " ";
    auxinfo += reinterpret_cast<const char *>(glGetString(GL_RENDERER));
    auxinfo += " ";
    auxinfo += reinterpret_cast<const char *>(glGetString(GL_VENDOR));


    title = auxinfo;
    glfwSetWindowTitle(window, title.c_str());
}
