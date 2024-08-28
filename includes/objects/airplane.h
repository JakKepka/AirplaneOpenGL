
#define AIRPLANE_H

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <stb_image.h>

// Defines several possible options for airplane movement.
enum Airplane_Movement {
    FORWARD_airplane,
    BACKWARD_airplane,
    LEFT_airplane,
    RIGHT_airplane,
    UP_airplane
};

// Default airplane values
const float YAW_airplane = -90.0f;
const float PITCH_airplane = 0.0f;
const float SPEED_airplane = 2.5f;
const float SENSITIVITY_airplane = 0.1f;
const float ZOOM_airplane = 45.0f;

int airplane_verices_size = 256;

// Funkcja dodająca wierzchołki prostokątnego pryzmatu do wektora wierzchołków
void addRectangularPrism(std::vector<float>& vertices, std::vector<unsigned int>& indices, glm::vec3 pos, glm::vec3 size) {
    float x = pos.x;
    float y = pos.y;
    float z = pos.z;
    float width = size.x;
    float height = size.y;
    float depth = size.z;

    unsigned int startIndex = vertices.size() / 8;

    // 8 wierzchołków pryzmatu
    vertices.insert(vertices.end(), {
        x, y, z, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // 0
        x + width, y, z, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // 1
        x + width, y + height, z, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // 2
        x, y + height, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // 3
        x, y, z + depth, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // 4
        x + width, y, z + depth, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // 5
        x + width, y + height, z + depth, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  // 6
        x, y + height, z + depth, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f   // 7
        });

    // 12 trójkątów (2 na każdą ścianę)
    indices.insert(indices.end(), {
        startIndex, startIndex + 1, startIndex + 2, startIndex, startIndex + 2, startIndex + 3,  // Przód
        startIndex + 4, startIndex + 5, startIndex + 6, startIndex + 4, startIndex + 6, startIndex + 7,  // Tył
        startIndex, startIndex + 1, startIndex + 5, startIndex, startIndex + 5, startIndex + 4,  // Dół
        startIndex + 2, startIndex + 3, startIndex + 7, startIndex + 2, startIndex + 7, startIndex + 6,  // Góra
        startIndex + 1, startIndex + 2, startIndex + 6, startIndex + 1, startIndex + 6, startIndex + 5,  // Prawo
        startIndex, startIndex + 3, startIndex + 7, startIndex, startIndex + 7, startIndex + 4   // Lewo
        });
}

void createAircraft(float*& vertices, unsigned int& vertexCount, unsigned int*& indices, unsigned int& indexCount) {
    std::vector<float> verticesVec;
    std::vector<unsigned int> indicesVec;

    // Kadłub (prostokątny pryzmat, symetryczny wzdłuż osi Z)
    addRectangularPrism(verticesVec, indicesVec, glm::vec3(-0.25f, -0.25f, -1.0f), glm::vec3(0.5f, 0.5f, 2.0f));

    // Skrzydła (prostokątny pryzmat, symetryczne wzdłuż osi Y i Z)
    addRectangularPrism(verticesVec, indicesVec, glm::vec3(-1.5f, -0.05f, -0.5f), glm::vec3(3.0f, 0.1f, 1.0f));

    // Ogon pionowy (prostokątny pryzmat, symetryczny wzdłuż osi Z)
    addRectangularPrism(verticesVec, indicesVec, glm::vec3(-0.25f, 0.25f, -1.5f), glm::vec3(0.5f, 0.5f, 0.1f));

    // Ogon poziomy (prostokątny pryzmat, symetryczny wzdłuż osi Y i Z)
    addRectangularPrism(verticesVec, indicesVec, glm::vec3(-0.5f, 0.25f, -1.5f), glm::vec3(1.0f, 0.1f, 0.75f));

    // Konwersja wektorów na tablice
    vertexCount = verticesVec.size();
    indexCount = indicesVec.size();

    vertices = new float[vertexCount];
    indices = new unsigned int[indexCount];

    std::copy(verticesVec.begin(), verticesVec.end(), vertices);
    std::copy(indicesVec.begin(), indicesVec.end(), indices);
}

void createAirplane(unsigned int& VBO, unsigned int& VAO, unsigned int& EBO, unsigned int& textureID, GLfloat* vertices, unsigned int vertexCount, unsigned int* indices, unsigned int indexCount) {

    // Tworzenie VAO, VBO i EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexCount, indices, GL_STATIC_DRAW);

    // Wskaźnik do pozycji wierzchołków
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Wskaźnik do współrzędnych tekstury
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Ładowanie tekstury
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(FileSystem::getPath("resources/textures/block.png").c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
}

// An abstract airplane class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices
class Airplane
{
public:
    // airplane Attributes
    glm::vec3 Position_airplane;
    glm::vec3 PointAboveNose;
    glm::vec3 PointBehindTail;
    glm::vec3 Front_airplane;
    glm::vec3 Right_airplane;
    glm::vec3 Up_airplane;
    glm::vec3 WorldUp_airplane;

    // euler Angles
    float Yaw_airplane;
    float Pitch_airplane;
    // airplane options
    float MovementSpeed_airplane;
    float MouseSensitivity_airplane;
    float Zoom_airplane;

    bool FreeMovment = false;
    bool CycleMovment = true;

    // Zmienna przechowująca pozycję i orientację samolotu
    glm::vec3 PreviousPosition_airplane = glm::vec3(0.0f);
    glm::mat4 Rotation_airplane = glm::mat4(1.0f);

    glm::vec3 UP_DIRECTION_airplane = glm::vec3(0, 1, 0);

    // constructor with vectors
    Airplane(glm::vec3 position_airplane = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up_airplane = glm::vec3(0.0f, 1.0f, 0.0f), float yaw_airplane = YAW, float pitch_airplane = PITCH)
        : Front_airplane(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed_airplane(SPEED), MouseSensitivity_airplane(SENSITIVITY), Zoom_airplane(ZOOM)
    {
        Position_airplane = position_airplane;
        WorldUp_airplane = up_airplane;
        Yaw_airplane = yaw_airplane;
        Pitch_airplane = pitch_airplane;
        updateAirplaneVectors();
    }
    // constructor with scalar values
    Airplane(float posX_airplane, float posY_airplane, float posZ_airplane, float upX_airplane, float upY_airplane, float upZ_airplane, float yaw_airplane, float pitch_airplane)
        : Front_airplane(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed_airplane(SPEED), MouseSensitivity_airplane(SENSITIVITY), Zoom_airplane(ZOOM)
    {
        Position_airplane = glm::vec3(posX_airplane, posY_airplane, posZ_airplane);
        WorldUp_airplane = glm::vec3(upX_airplane, upY_airplane, upZ_airplane);
        Yaw_airplane = yaw_airplane;
        Pitch_airplane = pitch_airplane;
        updateAirplaneVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position_airplane, Position_airplane + Front_airplane, Up_airplane);
    }

    glm::vec3 GetPosition()
    {
        return Position_airplane;
    }

    // Funkcja obliczająca punkty nad dziobem i za ogonem
    void UpdatePoints() {
        // Przesunięcie w górę od dziobu
        float offsetY = 1.0f; // Przykładowa wartość przesunięcia w górę
        // Przesunięcie za ogonem samolotu
        float offsetBack = 2.0f; // Przykładowa wartość przesunięcia za ogon

        PointAboveNose = Position_airplane + Front_airplane * 2.0f + UP_DIRECTION_airplane * offsetY;
        PointBehindTail = Position_airplane - Front_airplane * offsetBack + UP_DIRECTION_airplane * offsetY;
    }


    void ProcessKeyboard(Airplane_Movement direction_airplane, float deltaTime_airplane) {
        float velocity_airplane = MovementSpeed_airplane * deltaTime_airplane;

        // Zaktualizuj poprzednią pozycję
        PreviousPosition_airplane = Position_airplane;
        updateAirplaneVectors();

        
        // Poruszanie się do przodu i do tyłu
        if (direction_airplane == FORWARD_airplane) {
            Position_airplane -= Front_airplane * velocity_airplane;
        }
        if (direction_airplane == BACKWARD_airplane) {
            Position_airplane += Front_airplane * velocity_airplane;
        }

        // Poruszanie się w lewo i w prawo
        if (direction_airplane == LEFT_airplane) {
            Position_airplane += Right_airplane * velocity_airplane;
        }

        if (direction_airplane == RIGHT_airplane) {
            Position_airplane -= Right_airplane * velocity_airplane;
        }

        // Poruszanie się w górę
        if (direction_airplane == UP_airplane) {
            Position_airplane += UP_DIRECTION_airplane * velocity_airplane;
        }

        // Oblicz wektor kierunku lotu
        glm::vec3 direction = glm::normalize(Position_airplane - PreviousPosition_airplane);

        std::cout << direction.x << " " << direction.y << " " << direction.z << std::endl;
        // Wektor "góra" (zwykle (0, 1, 0))
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        // Oblicz prawy wektor jako iloczyn wektorowy up i direction
        glm::vec3 right = glm::normalize(glm::cross(up, direction));

        // Oblicz nowy wektor "up" jako iloczyn wektorowy direction i right (aby zachować ortogonalność)
        up = glm::cross(direction, right);

        // Stwórz macierz obrotu z nowo obliczonymi wektorami bazowymi
        Rotation_airplane = glm::mat4(1.0f);
        Rotation_airplane[0] = glm::vec4(right, 0.0f);     // right -> x-axis
        Rotation_airplane[1] = glm::vec4(up, 0.0f);        // up -> y-axis
        Rotation_airplane[2] = glm::vec4(direction, 0.0f); // direction -> z-axis
        Rotation_airplane[3] = glm::vec4(Position_airplane, 1.0f); // translation -> position

        // Zmienna przechowująca punkt nad dziobem samolotu
        glm::vec3 nosePosition = GetNosePosition();
        PointAboveNose = GetCoordinatesAboveNose(nosePosition, up, 0.2f);

        // Zmienna przechowująca punkt za ogonem samolotu
        glm::vec3 tailPosition = GetTailPosition();
        PointBehindTail = GetCoordinatesBehindTail(tailPosition, direction, up, 1.2f, 1.5f);

    }

    void MoveAirplaneStraightForward(float deltaTime_airplane) {
        float velocity_airplane = MovementSpeed_airplane * deltaTime_airplane;

        // Zaktualizuj poprzednią pozycję
        PreviousPosition_airplane = Position_airplane;
        updateAirplaneVectors();

        Position_airplane += Front_airplane * velocity_airplane;

        // Oblicz wektor kierunku lotu
        glm::vec3 direction = glm::normalize(Position_airplane - PreviousPosition_airplane);

        std::cout << direction.x << " " << direction.y << " " << direction.z << std::endl;
        // Wektor "góra" (zwykle (0, 1, 0))
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        // Oblicz prawy wektor jako iloczyn wektorowy up i direction
        glm::vec3 right = glm::normalize(glm::cross(up, direction));

        // Oblicz nowy wektor "up" jako iloczyn wektorowy direction i right (aby zachować ortogonalność)
        up = glm::cross(direction, right);

        // Stwórz macierz obrotu z nowo obliczonymi wektorami bazowymi
        Rotation_airplane = glm::mat4(1.0f);
        Rotation_airplane[0] = glm::vec4(right, 0.0f);     // right -> x-axis
        Rotation_airplane[1] = glm::vec4(up, 0.0f);        // up -> y-axis
        Rotation_airplane[2] = glm::vec4(direction, 0.0f); // direction -> z-axis
        Rotation_airplane[3] = glm::vec4(Position_airplane, 1.0f); // translation -> position

        // Zmienna przechowująca punkt nad dziobem samolotu
        glm::vec3 nosePosition = GetNosePosition();
        PointAboveNose = GetCoordinatesAboveNose(nosePosition, up, 0.2f);

        // Zmienna przechowująca punkt za ogonem samolotu
        glm::vec3 tailPosition = GetTailPosition();
        PointBehindTail = GetCoordinatesBehindTail(tailPosition, direction, up, 1.2f, 1.5f);

    }

    void FreeMovmentOn() {
        FreeMovment = true;
        CycleMovment = false;
    }

    void CycleMovmentOn() {
        FreeMovment = false;
        CycleMovment = true;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset_airplane, float yoffset_airplane, GLboolean constrainPitch_airplane = true)
    {
        xoffset_airplane *= MouseSensitivity_airplane;
        yoffset_airplane *= MouseSensitivity_airplane;

        Yaw_airplane += xoffset_airplane;
        Pitch_airplane += yoffset_airplane;

        // make sure that when pitch is out of bounds, the airplane doesn't get flipped
        if (constrainPitch_airplane)
        {
            if (Pitch_airplane > 89.0f)
                Pitch_airplane = 89.0f;
            if (Pitch_airplane < -89.0f)
                Pitch_airplane = -89.0f;
        }

        // update Front, RIGHT_airplane and Up Vectors using the updated Euler angles
        updateAirplaneVectors();

    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset_airplane)
    {
        Zoom_airplane -= (float)yoffset_airplane;
        if (Zoom_airplane < 1.0f)
            Zoom_airplane = 1.0f;
        if (Zoom_airplane > 45.0f)
            Zoom_airplane = 45.0f;
    }

    void MoveAirplane(float currentFrame, float deltaTime) {

        if (FreeMovment == true) {
            MoveAirplaneStraightForward(deltaTime);
        }
        
        if (CycleMovment == true) {
            FlyInFigureEight(currentFrame, 5.0f, 0.2f);
        }
   }

    void FlyInFigureEight(float deltaTime_airplane, float radius_airplane, float speed_airplane) {
        float time_airplane = speed_airplane * deltaTime_airplane;

        // Zaktualizuj poprzednią pozycję
        PreviousPosition_airplane = Position_airplane;

        // Oblicz nową pozycję
        Position_airplane.x = radius_airplane * sin(time_airplane);
        Position_airplane.z = radius_airplane * sin(time_airplane * 2.0f);  // Tworzy tor ósemki przez zmianę współrzędnej z

        // Oblicz wektor kierunku lotu
        glm::vec3 direction = glm::normalize(Position_airplane - PreviousPosition_airplane);

        // Wektor "góra" (zwykle (0, 1, 0))
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        // Oblicz prawy wektor jako iloczyn wektorowy up i direction
        glm::vec3 right = glm::normalize(glm::cross(up, direction));

        // Oblicz nowy wektor "up" jako iloczyn wektorowy direction i right (aby zachować ortogonalność)
        up = glm::cross(direction, right);

        // Stwórz macierz obrotu z nowo obliczonymi wektorami bazowymi
        Rotation_airplane = glm::mat4(1.0f);
        Rotation_airplane[0] = glm::vec4(right, 0.0f);     // right -> x-axis
        Rotation_airplane[1] = glm::vec4(up, 0.0f);        // up -> y-axis
        Rotation_airplane[2] = glm::vec4(direction, 0.0f); // direction -> z-axis
        Rotation_airplane[3] = glm::vec4(Position_airplane, 1.0f); // translation -> position


        // Zmienna przechowująca punkt nad dziobem samolotu
        glm::vec3 nosePosition = GetNosePosition();
        PointAboveNose = GetCoordinatesAboveNose(nosePosition, up, 0.2f);


        // Zmienna przechowująca punkt za ogonem samolotu
        glm::vec3 tailPosition = GetTailPosition();
        PointBehindTail = GetCoordinatesBehindTail(tailPosition, direction, up, 1.2f, 1.5f);

    }

    // Funkcja zwracająca współrzędne dziobu samolotu
    glm::vec3 GetNosePosition()
    {
        // Przykładowa pozycja dziobu w lokalnym układzie współrzędnych (zdefiniowana względem środka samolotu)
        glm::vec3 noseLocalPosition(0.0f, 0.0f, 1.5f); // Zakładamy, że dziób jest na końcu kadłuba (1.5f w osi Z)

        // Przekształć lokalną pozycję dziobu na pozycję globalną, używając macierzy rotacji i pozycji
        glm::vec4 noseWorldPosition = Rotation_airplane * glm::vec4(noseLocalPosition, 1.0f);

        return glm::vec3(noseWorldPosition);
    }

    // Funkcja zwracająca współrzędne nad dziobem samolotu
    glm::vec3 GetCoordinatesAboveNose(const glm::vec3& nosePosition, const glm::vec3& upVector, float offsetY)
    {
        // Dodajemy offset do wektora "up", aby uzyskać punkt nad dziobem
        return nosePosition + upVector * offsetY;
    }

    glm::vec3 GetTailPosition()
    {
        // Przykładowa pozycja ogona w lokalnym układzie współrzędnych (zdefiniowana względem środka samolotu)
        glm::vec3 tailLocalPosition(0.0f, 0.0f, -1.5f); // Zakładamy, że ogon jest na końcu kadłuba (-1.5f w osi Z)

        // Przekształć lokalną pozycję ogona na pozycję globalną, używając macierzy rotacji i pozycji
        glm::vec4 tailWorldPosition = Rotation_airplane * glm::vec4(tailLocalPosition, 1.0f);

        return glm::vec3(tailWorldPosition);
    }

    glm::vec3 GetCoordinatesBehindTail(const glm::vec3& tailPosition, const glm::vec3& direction, const glm::vec3& upVector, float offsetY, float offsetBack)
    {
        // Obliczamy punkt za samolotem (kierunek -direction)
        glm::vec3 backVector = -direction * offsetBack;
        
        return tailPosition + upVector * offsetY + backVector;
    }
    
    glm::vec3 GetCoordinatesAboveTail(float offsetY)
    {
        // Dodajemy offset do współrzędnej Y, aby uzyskać punkt nad ogonem
        return glm::vec3(Front_airplane.x, Front_airplane.y + offsetY, Front_airplane.z);
    }


    void renderAirplane(Shader ourShader, unsigned int VAO_plane) {

        // Rysuj samolot 
        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        //model = glm::translate(model, GetPosition());
        model = Rotation_airplane;
        ourShader.setMat4("model", model);

        // renderowanie samolotu
        ourShader.use();
        ourShader.setInt("texture1", 2);
        ourShader.setInt("texture2", 2);

        glBindVertexArray(VAO_plane);
        glDrawArrays(GL_TRIANGLES, 0, airplane_verices_size);
        glDrawElements(GL_TRIANGLES, airplane_verices_size, GL_UNSIGNED_INT, 0);
    }

private:
    // calculates the front vector from the Airplane's (updated) Euler Angles
    void updateAirplaneVectors()
    {
        // calculate the new Front vector
        glm::vec3 front_airplane;
        front_airplane.x = cos(glm::radians(Yaw_airplane)) * cos(glm::radians(Pitch_airplane));
        front_airplane.y = sin(glm::radians(Pitch_airplane));
        front_airplane.z = sin(glm::radians(Yaw_airplane)) * cos(glm::radians(Pitch_airplane));
        Front_airplane = glm::normalize(front_airplane);

        // also re-calculate the RIGHT_airplane and Up vector
        Right_airplane = glm::normalize(glm::cross(Front_airplane, WorldUp_airplane));
        Up_airplane = glm::normalize(glm::cross(Right_airplane, Front_airplane));
    }
};

