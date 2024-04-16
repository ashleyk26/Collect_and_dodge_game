#include "engine.h"
#include <vector>

//States represent the screen, and difficulty that is being played
enum state {start, info, select, playE, playM, playH, playD, play, over, lost};
state screen;
float speedModifier = 0;
//for the direction that the enemies are moving, separate y axis and x axis vectors
vector<bool> xDirection;
vector<bool> yDirection;
int lives = 3;

// Colors
color originalFill, hoverFill, pressFill;

Engine::Engine() : keys() {
    this->initWindow();
    this->initShaders();
    this->initShapes();

    originalFill = {1, 0, 0, 1};
    hoverFill.vec = originalFill.vec + vec4{0.5, 0.5, 0.5, 0};
    pressFill.vec = originalFill.vec - vec4{0.5, 0.5, 0.5, 0};
}

Engine::~Engine() {}

unsigned int Engine::initWindow(bool debug) {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    //size of the window is declared in ene engine.h file
    window = glfwCreateWindow(width, height, "engine", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // OpenGL configuration
    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(1);

    return 0;
}

void Engine::initShaders() {
    // load shader manager
    shaderManager = make_unique<ShaderManager>();

    // Load shader into shader manager and retrieve it
    shapeShader = this->shaderManager->loadShader("../res/shaders/shape.vert", "../res/shaders/shape.frag",  nullptr, "shape");

    // Configure text shader and renderer
    textShader = shaderManager->loadShader("../res/shaders/text.vert", "../res/shaders/text.frag", nullptr, "text");
    fontRenderer = make_unique<FontRenderer>(shaderManager->getShader("text"), "../res/fonts/MxPlus_IBM_BIOS.ttf", 24);

    // Set uniforms
    textShader.setVector2f("vertex", vec4(100, 100, .5, .5));
    shapeShader.use();
    shapeShader.setMatrix4("projection", this->PROJECTION);
}

void Engine::initShapes() {
    // User is spawned in the middle of the left side of the screen
    user = make_unique<Rect>(shapeShader, vec2{30,height/2}, vec2{15, 15}, color{0.537, 0.811, 0.941, .9});

    // Safe zone that makes where enemies and supplies can't spawn or move
    safeZone = make_unique<Rect>(shapeShader, vec2{30,height/2}, vec2{60, height}, color{0.349, 0.901, 0.349, .3});

    // All parts of battery, charge1-3 move/change color based on number of lives
    batteryMain = make_unique<Rect>(shapeShader, vec2{30,height - 90}, vec2{50, 130}, color{0.411, 0.411, 0.411, .75});
    batteryTop = make_unique<Rect>(shapeShader, vec2{30,height - 19}, vec2{25, 12}, color{0.411, 0.411, 0.411, .75});
    charge1 = make_unique<Rect>(shapeShader, vec2{30,height - 50}, vec2{40, 40}, color{0.9, 0.9, 0, .3});
    charge2 = make_unique<Rect>(shapeShader, vec2{30,height - 90}, vec2{40, 40}, color{0.9, 0.9, 0, .3});
    charge3 = make_unique<Rect>(shapeShader, vec2{30,height - 130}, vec2{40, 40}, color{0.9, 0.9, 0, .3});
}

void Engine::processInput() {
    glfwPollEvents();

    // Set keys to true if pressed, false if released
    for (int key = 0; key < 1024; ++key) {
        if (glfwGetKey(window, key) == GLFW_PRESS)
            keys[key] = true;
        else if (glfwGetKey(window, key) == GLFW_RELEASE)
            keys[key] = false;
    }

    // Close window if escape key is pressed
    if (keys[GLFW_KEY_ESCAPE])
        glfwSetWindowShouldClose(window, true);

    // Mouse position saved to check for collisions
    glfwGetCursorPos(window, &MouseX, &MouseY);

    // If we're in the start screen and the user presses c, change screen to info
    if (keys[GLFW_KEY_C] && screen == start)
        screen = info;

    // If we're in the start screen and the user presses s, change screen to select
    if (keys[GLFW_KEY_S] && screen == info)
        screen = select;

    // If we're in the start screen and the user presses e, change screen to playE
    if (keys[GLFW_KEY_E] && screen == select) {
        screen = playE;
        createSupplies();
    }

    // If we're in the start screen and the user presses m, change screen to playM
    if (keys[GLFW_KEY_M] && screen == select) {
        screen = playM;
        createSupplies();
    }

    // If we're in the start screen and the user presses h, change screen to playH
    if (keys[GLFW_KEY_H] && screen == select) {
        screen = playH;
        createSupplies();
    }

    // If we're in the start screen and the user presses d, change screen to playD
    if (keys[GLFW_KEY_D] && screen == select) {
        screen = playD;
        createSupplies();
    }

    // If we're in any play screen and an arrow key is pressed, move the user
    if (keys[GLFW_KEY_UP] && screen != start && screen != select && screen != over && user->getTop() <= height)
        user->move(vec2(0, 2));
    if (keys[GLFW_KEY_DOWN] && screen != start && screen != select && screen != over && user->getBottom() >= 0)
        user->move(vec2(0, -2));
    if (keys[GLFW_KEY_LEFT] && screen != start && screen != select && screen != over && user->getLeft() >= 0)
        user->move(vec2(-2, 0));
    if (keys[GLFW_KEY_RIGHT] && screen != start && screen != select && screen != over && user->getRight() <= width)
        user->move(vec2(2, 0));
}

void Engine::update() {
    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    //this is establishing the movements of the enemies. Alternating between directions based on the integer i being even and odd
    int i = 0;
    // Moves enemies left and right OR up and down based on if there spot in the vector is even or odd
    while(i < enemies.size()){
        if (i % 2 == 0) {
            if (xDirection[i] && screen != start && screen != select) {
                enemies[i]->move(vec2(2 * speedModifier, 0));
                if (enemies[i]->getPos().x > 790) {
                    xDirection[i] = false;
                    cout << "Enemy " << i << " moving left" << endl;
                }
            } else if (!xDirection[i]) {
                enemies[i]->move(vec2(-2 * speedModifier, 0));
                //cout << enemies[i]->getPos().x << endl;
                if (enemies[i]->getPos().x < 80) {
                    xDirection[i] = true;
                    cout << "Enemy " << i << " moving right" << endl;
                }
            }
        } else {
            if (yDirection[i] && screen != start && screen != select) {
                enemies[i]->move(vec2(0, 2 * speedModifier));
                if (enemies[i]->getPos().y > 580) {
                    yDirection[i] = false;
                    cout << "Enemy " << i << " moving down" << endl;
                }
            } else if (!yDirection[i]) {
                enemies[i]->move(vec2(0, -2 * speedModifier));
                //cout << enemies[i]->getPos().x << endl;
                if (enemies[i]->getPos().y < 20) {
                    yDirection[i] = true;
                    cout << "Enemy " << i << " moving up" << endl;
                }
            }
        }
        i++;
    }

    // Calls checks for if the user is overlapping something
    collectingSupplies();
    deadByEnemy();

}

void Engine::render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color

    glClear(GL_COLOR_BUFFER_BIT);

    // Set shader to use for all shapes
    shapeShader.use();

    // Render differently depending on screen
    switch (screen) {
        // Game begins on this screen. Has the general info about the game
        case start: {
            string welcome = "Welcome!";
            string info1 = "In this game, you (the blue box) must go around and";
            string info2 = "collect supplies (purple boxes) to build your ship ";
            string info3 = "and fly away! You can control your box using the";
            string info4 = "arrow keys. Be careful collecting though, there are";
            string info5 = "enemies (red boxes) around that are trying to stop you!";
            string begin = "Press c to continue";
            // (12 * message.length()) is the offset to center text.
            // 12 pixels is the width of each character scaled by 1.
            this->fontRenderer->renderText(welcome, width/2 - (12 * welcome.length()), height/2 + 150, 1, vec3{1, 1, 1});
            this->fontRenderer->renderText(info1, width/2 - (12 * (.55) * info1.length()), height/2 + 100, .55, vec3{1, 1, 1});
            this->fontRenderer->renderText(info2, width/2 - (12 * (.55) * info2.length()), height/2 + 50, .55, vec3{1, 1, 1});
            this->fontRenderer->renderText(info3, width/2 - (12 * (.55) * info3.length()), height/2, .55, vec3{1, 1, 1});
            this->fontRenderer->renderText(info4, width/2 - (12 * (.55) * info4.length()), height/2 - 50, .55, vec3{1, 1, 1});
            this->fontRenderer->renderText(info5, width/2 - (12 * (.55) * info5.length()), height/2 - 100, .55, vec3{1, 1, 1});
            this->fontRenderer->renderText(begin, width/2 - (12 * (.8) * begin.length()), height/2 - 150, .8, vec3{1, 1, 1});
            break;
        }

        // An additional screen giving more info on lives and safe zone
        case info: {
            string welcome = "-= Lives =-";
            string info1 = "In this game you will start in a safe zone where no";
            string info2 = "enemies will spawn or move to. However, if you are";
            string info3 = "outside the zone and you hit an enemy, you will lose a";
            string info4 = "life. Your lives will be shown in how charged the battery";
            string info5 = "in the top right is, and its game over when you run out!";
            string begin = "Press s to start!";
            // (12 * message.length()) is the offset to center text.
            // 12 pixels is the width of each character scaled by 1.
            this->fontRenderer->renderText(welcome, width/2 - (12 * welcome.length()), height/2 + 150, 1, vec3{1, 1, 1});
            this->fontRenderer->renderText(info1, width/2 - (12 * (.55) * info1.length()), height/2 + 100, .55, vec3{1, 1, 1});
            this->fontRenderer->renderText(info2, width/2 - (12 * (.55) * info2.length()), height/2 + 50, .55, vec3{1, 1, 1});
            this->fontRenderer->renderText(info3, width/2 - (12 * (.55) * info3.length()), height/2, .55, vec3{1, 1, 1});
            this->fontRenderer->renderText(info4, width/2 - (12 * (.55) * info4.length()), height/2 - 50, .55, vec3{1, 1, 1});
            this->fontRenderer->renderText(info5, width/2 - (12 * (.55) * info5.length()), height/2 - 100, .55, vec3{1, 1, 1});
            this->fontRenderer->renderText(begin, width/2 - (12 * (.8) * begin.length()), height/2 - 150, .8, vec3{1, 1, 1});
            break;
        }

        // Gives the user a prompt on what difficulty they want to play on
        case select: {
            string selectMessage = "-= Press letter for difficulty =-";
            string selectE = "E - Easy";
            string selectM = "M - Medium";
            string selectH = "H - Hard";
            string selectD = "D - Death";
            // (12 * message.length()) is the offset to center text.
            // 12 pixels is the width of each character scaled by 1.
            this->fontRenderer->renderText(selectMessage, width/2 - (12 * (.85) * selectMessage.length()), height/2 + 100, .85, vec3{1, 1, 1});
            this->fontRenderer->renderText(selectE, width/2 - (12 * (.7) * selectE.length()), height/2 + 50, .7, vec3{1, 1, 1});
            this->fontRenderer->renderText(selectM, width/2 - (12 * (.7) * selectM.length()), height/2, .7, vec3{1, 1, 1});
            this->fontRenderer->renderText(selectH, width/2 - (12 * (.7) * selectH.length()), height/2 - 50, .7, vec3{1, 1, 1});
            this->fontRenderer->renderText(selectD, width/2 - (12 * (.7) * selectD.length()), height/2 - 100, .7, vec3{1, 1, 1});
            break;
        }

        // Easy difficulty of game: 10 supplies, 5 enemies, speed modifier of 0.5
        case playE: {
            numberOfSupplies = 10;
            numberOfEnemies = 5;

            // setUniforms and draw all supplies and enemies
            for(int i = 0; i < numberOfSupplies; i ++){
                supplies[i]-> setUniforms();
                supplies[i]-> draw();
            }
            for(int i = 0; i < numberOfEnemies; i ++){
                enemies[i]-> setUniforms();
                enemies[i]-> draw();
            }
            // call setUniforms and draw on all shapes
            safeZone -> setUniforms();
            safeZone -> draw();
            user -> setUniforms();
            user -> draw();
            batteryMain -> setUniforms();
            batteryMain -> draw();
            batteryTop -> setUniforms();
            batteryTop -> draw();
            charge1 -> setUniforms();
            charge1 -> draw();
            charge2 -> setUniforms();
            charge2 -> draw();
            charge3 -> setUniforms();
            charge3 -> draw();
            // Render font on top of user
            fontRenderer->renderText("YOU", user->getPos().x - 7, user->getPos().y - 1, 0.2, vec3{1, 1, 1});
            break;
        }

        // Medium difficulty of game: 15 supplies, 10 enemies, enemy speed modifier of 1
        case playM: {
            numberOfSupplies = 15;
            numberOfEnemies =  10;

            // setUniforms and draw all supplies and enemies
            for(int i = 0; i < numberOfSupplies; i ++){
                supplies[i]-> setUniforms();
                supplies[i]-> draw();
            }
            for(int i = 0; i < numberOfEnemies; i ++){
                enemies[i]-> setUniforms();
                enemies[i]-> draw();
            }

            // call setUniforms and draw on all shapes
            safeZone -> setUniforms();
            safeZone -> draw();
            user -> setUniforms();
            user -> draw();
            batteryMain -> setUniforms();
            batteryMain -> draw();
            batteryTop -> setUniforms();
            batteryTop -> draw();
            charge1 -> setUniforms();
            charge1 -> draw();
            charge2 -> setUniforms();
            charge2 -> draw();
            charge3 -> setUniforms();
            charge3 -> draw();

            // Render font on top of user
            fontRenderer->renderText("YOU", user->getPos().x - 7, user->getPos().y - 1, 0.2, vec3{1, 1, 1});
            break;
        }

        // Hard difficulty of game: 20 supplies, 15 enemies, enemy speed modifier of 3
        case playH: {
            numberOfSupplies = 20;
            numberOfEnemies =  15;

            // setUniforms and draw all supplies and enemies
            for(int i = 0; i < numberOfSupplies; i ++){
                supplies[i]-> setUniforms();
                supplies[i]-> draw();
            }
            for(int i = 0; i < numberOfEnemies; i ++){
                enemies[i]-> setUniforms();
                enemies[i]-> draw();
            }

            // call setUniforms and draw on all shapes
            safeZone -> setUniforms();
            safeZone -> draw();
            user -> setUniforms();
            user -> draw();
            batteryMain -> setUniforms();
            batteryMain -> draw();
            batteryTop -> setUniforms();
            batteryTop -> draw();
            charge1 -> setUniforms();
            charge1 -> draw();
            charge2 -> setUniforms();
            charge2 -> draw();
            charge3 -> setUniforms();
            charge3 -> draw();

            // Render font on top of user
            fontRenderer->renderText("YOU", user->getPos().x - 7, user->getPos().y - 1, 0.2, vec3{1, 1, 1});
            break;
        }

        // Death difficulty of game: 30 supplies, 25 enemies, enemy speed modifier of 5
        case playD: {
            numberOfSupplies = 30;
            numberOfEnemies =  25;

            // setUniforms and draw all supplies and enemies
            for(int i = 0; i < numberOfSupplies; i ++){
                supplies[i]-> setUniforms();
                supplies[i]-> draw();
            }
            for(int i = 0; i < numberOfEnemies; i ++){
                enemies[i]-> setUniforms();
                enemies[i]-> draw();
            }

            // call setUniforms and draw on all shapes
            safeZone -> setUniforms();
            safeZone -> draw();
            user -> setUniforms();
            user -> draw();
            batteryMain -> setUniforms();
            batteryMain -> draw();
            batteryTop -> setUniforms();
            batteryTop -> draw();
            charge1 -> setUniforms();
            charge1 -> draw();
            charge2 -> setUniforms();
            charge2 -> draw();
            charge3 -> setUniforms();
            charge3 -> draw();

            // Render font on top of user
            fontRenderer->renderText("YOU", user->getPos().x - 7, user->getPos().y - 1, 0.2, vec3{1, 1, 1});
            break;
        }
        case over: {
            string message = "You win!";
            string message2 = "Press R to go back to start screen";
            // TO DO: Display the message on the screen
            this->fontRenderer->renderText(message, width/2 - (12 * message.length()), height/2 + 25, 1, vec3{1, 1, 1});
            this->fontRenderer->renderText(message2, width/2 - (12 * .75 * message2.length()), height/2 - 25, .75, vec3{1, 1, 1});
            restartGame();
            break;
        }
        case lost: {
            string message = "You LOSE!";
            string message2 = "Press R to go back to start screen";
            // TO DO: Display the message on the screen
            this->fontRenderer->renderText(message, width/2 - (12 * message.length()), height/2 + 25, 1, vec3{1, 1, 1});
            this->fontRenderer->renderText(message2, width/2 - (12 * .75 * message2.length()), height/2 - 25, .75, vec3{1, 1, 1});
            restartGame();
            break;
        }
    }

    glfwSwapBuffers(window);
}

void Engine::deadByEnemy() {
    for(int i = 0; i < enemies.size(); i ++){
        if(user->isOverlapping(vec2(enemies[i]->getLeft(), enemies[i]->getTop()))
           || user->isOverlapping(vec2(enemies[i]->getLeft(), enemies[i]->getBottom()))
           || user->isOverlapping(vec2(enemies[i]->getRight(), enemies[i]->getTop()))
           || user->isOverlapping(vec2(enemies[i]->getRight(), enemies[i]->getBottom()))){
            if (lives > 0) {
                user->setPos(vec2{30,height/2});
                lives = lives - 1;
                if (lives == 0) {
                    screen = lost;
                }
                if (lives == 1)
                {
                    charge2->setPosX(-30);
                    charge3->setColor(color{.3,0,0});
                }
                if (lives == 2)
                {
                    charge1->setPosX(-30);
                }
            }
        }
    }
}

void Engine::restartGame() {
    bool stillEnemies = true;
    bool stillSupplies = true;
    bool stillDirections1 = true;
    bool stillDirections2 = true;
    if (keys[GLFW_KEY_R] && (screen == lost || screen == over)){
        amountCollected = 0;
        allGone = false;
        user = make_unique<Rect>(shapeShader, vec2{15,height/2}, vec2{15, 15}, color{0.537, 0.811, 0.941, .9});
        safeZone = make_unique<Rect>(shapeShader, vec2{30,height/2}, vec2{60, height}, color{0.349, 0.901, 0.349, .3});
        batteryMain = make_unique<Rect>(shapeShader, vec2{30,height - 90}, vec2{50, 130}, color{0.411, 0.411, 0.411, .75});
        batteryTop = make_unique<Rect>(shapeShader, vec2{30,height - 19}, vec2{25, 12}, color{0.411, 0.411, 0.411, .75});
        charge1 = make_unique<Rect>(shapeShader, vec2{30,height - 50}, vec2{40, 40}, color{0.9, 0.9, 0, .3});
        charge2 = make_unique<Rect>(shapeShader, vec2{30,height - 90}, vec2{40, 40}, color{0.9, 0.9, 0, .3});
        charge3 = make_unique<Rect>(shapeShader, vec2{30,height - 130}, vec2{40, 40}, color{0.9, 0.9, 0, .3});
        lives = 3;

        while(stillEnemies){
            if(enemies.size() > 0){
                enemies.pop_back();
            }
            else{
                stillEnemies = false;
            }
        }
        while(stillSupplies){
            if(supplies.size() > 0){
                supplies.pop_back();
            }
            else{
                stillSupplies = false;
            }
        }
        while(stillDirections1){
            if(xDirection.size() > 0){
                xDirection.pop_back();
            }
            else{
                stillDirections1 = false;
            }
        }
        while(stillDirections2){
            if(yDirection.size() > 0){
                yDirection.pop_back();
            }
            else{
                stillDirections2 = false;
            }
        }
        screen = select;
    }
}

void Engine::collectingSupplies() {
    for(int i = 0; i < supplies.size(); i ++){
        if(user->isOverlapping(vec2(supplies[i]->getLeft(), supplies[i]->getTop()))
        || user->isOverlapping(vec2(supplies[i]->getLeft(), supplies[i]->getBottom()))
        || user->isOverlapping(vec2(supplies[i]->getRight(), supplies[i]->getTop()))
        || user->isOverlapping(vec2(supplies[i]->getRight(), supplies[i]->getBottom()))){
            amountCollected++;
            cout << "Collecting" << endl;
            supplies[i]->setPos(vec2{1000,1000});
            if(amountCollected == supplies.size()){
                allGone = true;
            }
            if(allGone){
                screen = over;
            }
        }
    }
}

void Engine::createSupplies() {
    //position for supplies and enemies will be random

    //will change based on game mode
    if (screen == playE) {
        numberOfSupplies = 10;
        numberOfEnemies = 5;
        speedModifier = .5;
    } else if (screen == playM) {
        numberOfSupplies = 15;
        numberOfEnemies = 10;
        speedModifier = 1;
    } else if (screen == playH) {
        numberOfSupplies = 20;
        numberOfEnemies = 15;
        speedModifier = 3;
    } else if (screen == playD) {
        numberOfSupplies = 30;
        numberOfEnemies = 25;
        speedModifier = 5;
    }
    if (screen != start && screen != select) {

        for(int i = 0; i < numberOfSupplies; i++){
            int xSpot = rand() % ((int)width);
            while (xSpot <= 80 || xSpot >= 785) {
                xSpot = rand() % ((int)width);
            }
            int ySpot = rand() % ((int)height);
            while (ySpot <= 25 || ySpot >= 585) {
                ySpot = rand() % ((int)height);
            }
            cout << xSpot << " " << ySpot << endl;
            vec2 suppliesPos = {xSpot, ySpot};
            supplies.push_back(make_unique<Rect>(shapeShader, suppliesPos, sizeS, purple));
        }
        for(int i = 0; i < numberOfEnemies; i++){
            xDirection.push_back(true);
            yDirection.push_back(true);
            int xSpot = rand() % ((int)width);
            while (xSpot <= 80 || xSpot >= 785) {
                xSpot = rand() % ((int)width);
            }
            int ySpot = rand() % ((int)height);
            while (ySpot <= 25 || ySpot >= 585) {
                ySpot = rand() % ((int)height);
            }
            cout << i << " " << xSpot << " " << ySpot << endl;
            vec2 enemyPos = {xSpot, ySpot};
            enemies.push_back(make_unique<Rect>(shapeShader, enemyPos, sizeE, red));
        }
    }

}

bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}

GLenum Engine::glCheckError_(const char *file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        cout << error << " | " << file << " (" << line << ")" << endl;
    }
    return errorCode;
}
