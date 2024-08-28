#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <objects/airplane.h>
#include <objects/cube.h>
#include <objects/lightning_cube.h>
#include <objects/floor.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION    
#include <stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(12.0f, 2.0f, 12.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


// airplane 
glm::vec3 airplanePosition = glm::vec3(5.0f, 7.0f, 5.0f);
Airplane airplane(airplanePosition);
float lastX_airplane = SCR_WIDTH / 2.0f;
float lastY_airplane = SCR_HEIGHT / 2.0f;



// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("shaders/camera/camera.vs", "shaders/camera/camera.fs");
    //Shader lightingShader("shaders/lightning/multiple_lights.vs", "shaders/lightning/multiple_lights.fs");
    Shader lightCubeShader("shaders/lightning/light_cube.vs", "shaders/lightning/light_cube.fs");

    // world space positions of our cubes

    float distance_between_cubes = 10.0f;
    int n_cubes = 4;

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  distance_between_cubes),
        glm::vec3(distance_between_cubes,  0.0f, 0.0f),
        glm::vec3(distance_between_cubes, 0.0f, distance_between_cubes),
        glm::vec3(0.0f, 0.0f, 0.0f)
    };

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.0f,  2.0f,  distance_between_cubes),
        glm::vec3(distance_between_cubes,  2.0f, 0.0f),
        glm::vec3(distance_between_cubes, 2.0f, distance_between_cubes),
        glm::vec3(0.0f, 2.0f, 0.0f)
    };

    // Bufury dla sześciościanów
    unsigned int VBO_cube, VAO_cube;

    // Tekstury dla sześciościanów
    unsigned int texture_cube1, texture_cube2;
    
    // Tworzenie bufforów dla sześcioboków.
    createCube(VBO_cube, VAO_cube);
    loadTextureCube(texture_cube1, texture_cube2);


    // Bufury dla samolotu
    unsigned int VBO_plane, VAO_plane, EBO_plane;

    // Tekstury dla samolotu
    unsigned int texture_plane;

    float* aircraftVertices;
    unsigned int aircraftVertexCount;
    unsigned int* aircraftIndices;
    unsigned int aircraftIndexCount;

    // Stwórz model samolotu
    createAircraft(aircraftVertices, aircraftVertexCount, aircraftIndices, aircraftIndexCount);

    // Tworzenie bufforów dla samolotu.
    createAirplane(VBO_plane, VAO_plane, EBO_plane, texture_plane, aircraftVertices, aircraftVertexCount, aircraftIndices, aircraftIndexCount);

    // Tworzenie bufforów dla podłogi
    // Bufury dla podłogi
    unsigned int VBO_floor, VAO_floor, EBO_floor;

    // Tekstury dla podłogi
    unsigned int texture_floor;

    // Tworzenie bufforów dla podłogi.
    createInfFloor(VBO_floor, VAO_floor, EBO_floor, texture_floor);


    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int VAO_lightning_cube, VBO_lightning_cube, texture_lightning_cube;
    
    createLightningCube(VBO_lightning_cube, VAO_lightning_cube, texture_lightning_cube);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_cube1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_cube2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture_plane);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture_floor);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texture_lightning_cube);

        // Kolor tła
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // world transformation
        glm::mat4 view = camera.GetViewMatrix();
        //camera.updateViewMatrix(view, camera.Position, airplane.PointAboveNose, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 model = glm::mat4(1.0f);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // activate shader
        ourShader.use(); 

        // pass projection matrix to shader (note that in this case it could change every frame)
        ourShader.setMat4("projection", projection);

        // Obserwator z perspektywy samolotu, samolot zablokowany
        if (camera.ObjectMovment == true)
        {
            // Kierowanie samolotem zablokowane
            if (airplane.CycleMovment == true)
            {
                view = camera.GetViewMatrix();
                camera.Position = airplane.PointBehindTail;
                camera.updateViewMatrix(view, camera.Position, airplane.PointAboveNose, glm::vec3(0.0f, 1.0f, 0.0f));
            }

            // Kierowanie samolotem odblokowane
            if (airplane.CycleMovment == false) {
                view = camera.GetViewMatrix();
                camera.Position = airplane.PointBehindTail;
                camera.updateViewMatrix(view, camera.Position, airplane.PointAboveNose, glm::vec3(0.0f, 1.0f, 0.0f));
            }
        }

        // Obserwator wolny
        if (camera.ObjectMovment == false) {
            // camera/view transformation
            view = camera.GetViewMatrix();
        }

        ourShader.setMat4("view", view);

        // Ruch samolotu
        airplane.MoveAirplane(currentFrame, deltaTime);

        // renderowanie samolotu
        airplane.renderAirplane(ourShader, VAO_plane);

        // renderowanie podłogi
        renderFloor(ourShader, VAO_floor, 3);

        // render sześcianów
        ourShader.use();
        ourShader.setInt("texture1", 0);
        ourShader.setInt("texture2", 1);

        glBindVertexArray(VAO_cube);
        for (unsigned int i = 0; i < n_cubes; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, cubePositions[i]);
            //float angle = 0.0f;//20.0f * i;
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f)); //glm::vec3(1.0f, 0.3f, 0.5f)
            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // also draw the lamp object(s)
        lightCubeShader.use();
        lightCubeShader.setInt("texture1", 4);
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        
        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(VAO_lightning_cube);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            lightCubeShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO_cube);
    glDeleteBuffers(1, &VBO_cube);
    glDeleteVertexArrays(1, &VAO_plane);
    glDeleteBuffers(1, &VBO_plane);
    glDeleteBuffers(1, &EBO_plane);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (camera.FreeMovment == true) {

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
    }

    if (camera.ObjectMovment == true && airplane.FreeMovment==true) 
    {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            airplane.ProcessKeyboard(Airplane_Movement::UP_airplane, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            airplane.ProcessKeyboard(Airplane_Movement::FORWARD_airplane, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            airplane.ProcessKeyboard(Airplane_Movement::BACKWARD_airplane, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            airplane.ProcessKeyboard(Airplane_Movement::LEFT_airplane, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            airplane.ProcessKeyboard(Airplane_Movement::RIGHT_airplane, deltaTime);
    }

    //Przełączanie trybów kamery
    // Tryb wolny
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && airplane.FreeMovment == false)
    {
        camera.FreeMovmentOn();
    }

    // Tryb poruszania się za przedmiotem
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        camera.ObjectMovmentOn(airplane.GetPosition());
    }

    //Przełączanie trybów samolotu
    // Jeżeli camera jest z perspektywy samolotu
    if(camera.ObjectMovment == true)
    // Oraz został wciśnięty przycisk C to odbluj sterowanie samolotem
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        airplane.FreeMovmentOn();
    }
    else if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) 
    {
        airplane.CycleMovmentOn();
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (camera.ObjectMovment == true && airplane.CycleMovment == false)
    { 
        airplane.ProcessMouseMovement(xoffset, yoffset);
        //camera.ProcessMouseMovement(xoffset, yoffset);
    }
    if (camera.FreeMovment == true)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
