#ifndef GRAPHICS_ENGINE_H
#define GRAPHICS_ENGINE_H

#include <vector>
#include <memory>
#include <iostream>
#include <GLFW/glfw3.h>

#include "shader/shaderManager.h"
#include "font/fontRenderer.h"
#include "shapes/rect.h"
#include "shapes/shape.h"

using std::vector, std::unique_ptr, std::make_unique, glm::ortho, glm::mat4, glm::vec3, glm::vec4;

/**
 * @brief The Engine class.
 * @details The Engine class is responsible for initializing the GLFW window, loading shaders, and rendering the game state.
 */
class Engine {
private:
    /// @brief The actual GLFW window.
    GLFWwindow* window{};

    ///@brief this is a counter to keep track of how much supplies is collected
    int amountCollected = 0;

    /// @brief The width and height of the window.
    const unsigned int width = 800, height = 600; // Window dimensions

    /// @brief Keyboard state (True if pressed, false if not pressed).
    /// @details Index this array with GLFW_KEY_{key} to get the state of a key.
    bool keys[1024];

    /// @brief Responsible for loading and storing all the shaders used in the project.
    /// @details Initialized in initShaders()
    unique_ptr<ShaderManager> shaderManager;

    /// @brief Responsible for rendering text on the screen.
    /// @details Initialized in initShaders()
    unique_ptr<FontRenderer> fontRenderer;

    // Shapes
    unique_ptr<Shape> user;
    unique_ptr<Shape> safeZone;
    unique_ptr<Shape> batteryMain;
    unique_ptr<Shape> batteryTop;
    unique_ptr<Shape> charge1;
    unique_ptr<Shape> charge2;
    unique_ptr<Shape> charge3;
    vector<unique_ptr<Shape>> supplies;
    vector<unique_ptr<Shape>> enemies;
    vector<unique_ptr<Shape>> rocketship;

    //attributes of the vectors
    vec2 sizeE = {15,15};
    vec2 sizeS = {10,10};
    color purple = {1, 0, 1, 1.0f};
    color red = {1, 0, 0, 1.0f};
    int numberOfSupplies = 0;
    int numberOfEnemies = 0;
    vec2 pos;

    bool allGone = false;

    // Shaders
    Shader shapeShader;
    Shader textShader;

    double MouseX, MouseY;
    bool mousePressedLastFrame = false;

    /// @note Call glCheckError() after every OpenGL call to check for errors.
    GLenum glCheckError_(const char *file, int line);
    /// @brief Macro for glCheckError_ function. Used for debugging.
#define glCheckError() glCheckError_(__FILE__, __LINE__)

public:
    /// @brief Constructor for the Engine class.
    /// @details Initializes window and shaders.
    Engine();

    /// @brief Destructor for the Engine class.
    ~Engine();

    /// @brief Initializes the GLFW window.
    /// @return 0 if successful, -1 otherwise.
    unsigned int initWindow(bool debug = false);

    /// @brief Loads shaders from files and stores them in the shaderManager.
    /// @details Renderers are initialized here.
    void initShaders();

    /// @brief Initializes the shapes to be rendered.
    void initShapes();

    /// @brief creates the rocketship for the end screens
    void createRocketship();

    ///@brief deletes all vectors to prepare for restart
    void restartGame();

    /// @brief if you hit enemy you die
    void deadByEnemy();

    /// @brief method for collecting supplies
    void collectingSupplies();


    ///@brief creates the supplies object's qualities
    void createSupplies();

    /// @brief Processes input from the user.
    /// @details (e.g. keyboard input, mouse input, etc.)
    void processInput();

    /// @brief Updates the game state.
    /// @details (e.g. collision detection, delta time, etc.)
    void update();

    /// @brief Renders the game state.
    /// @details Displays/renders objects on the screen.
    void render();

    /* deltaTime variables */
    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame (used to calculate deltaTime)

    // -----------------------------------
    // Getters
    // -----------------------------------

    /// @brief Returns true if the window should close.
    /// @details (Wrapper for glfwWindowShouldClose()).
    /// @return true if the window should close
    /// @return false if the window should not close
    bool shouldClose();

    /// Projection matrix used for 2D rendering (orthographic projection).
    /// We don't have to change this matrix since the screen size never changes.
    /// OpenGL uses the projection matrix to map the 3D scene to a 2D viewport.
    /// The projection matrix transforms coordinates in the camera space into normalized device coordinates (view space to clip space).
    /// @note The projection matrix is used in the vertex shader.
    // 4th quadrant
    mat4 PROJECTION = ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);
    // 1st quadrant
//        mat4 PROJECTION = ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

    void checkBounds(unique_ptr<Shape> &shape) const;
};

#endif //GRAPHICS_ENGINE_H