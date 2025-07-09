#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
using namespace sf;
using namespace std;

const int windowWidth = 400; // Window width
const int windowHeight = 800; // window height
const int blockSize = 20;   // block size
const int gridWidth = windowWidth / blockSize;   // grid width
const int gridHeight = windowHeight / blockSize;   // gridth height
const int normalSpeed = 200;    // normal falling speed of blocks
const int fastSpeed = 50;       // fast falling speed of blocks when down key is pressed
bool gameStarted = false;      // game started is false
int Hscore[10]; // array to store highscores
int speedIncrease;  // variable to increase speed of the blocks

Font font;   // using font

void loadFont()     // using a function to call font 
{
    if (!font.loadFromFile("Simple Handmade.otf"))   // if condition to handle error of image 
    {
        cout << "Error loading font\n";
    }
}

// function to draw tetris blocks
void drawBlock(sf::RenderWindow& window, int posX, int posY, sf::Color color)
{
    sf::RectangleShape shape(sf::Vector2f(blockSize - 1, blockSize - 1));  // Block shape
    shape.setFillColor(color);                                             // Block color
    shape.setPosition(posX * blockSize, posY * blockSize);                 // Block position
    window.draw(shape);
}

// function to draw grid
void drawGrid(sf::RenderWindow& window)
{
    sf::RectangleShape gridLine;                   
    gridLine.setFillColor(sf::Color::Blue);           // set grid color to blue
    gridLine.setSize(sf::Vector2f(windowWidth, 1));    // set grid size

    // Horizontal lines
    for (int i = 0; i < windowHeight; i += blockSize)
    {
        gridLine.setPosition(0, i);
        window.draw(gridLine);
    }

    // Vertical lines
    for (int i = 0; i < windowWidth; i += blockSize)
    {
        gridLine.setPosition(i, 0);
        gridLine.setSize(sf::Vector2f(1, windowHeight));
        window.draw(gridLine);
    }
}

// Function to check collision with other blocks
bool checkCollision(int x, int y, int block[4][4], int posX, int posY)
{
    for (int i = 0; i < 4; ++i)         // for loop to iterate over each cell of the blocks
    {
        for (int j = 0; j < 4; ++j)
        {
            if (block[i][j] == 1)        // if condition to check if there is a block
            {
                int tileX = j + posX;        // checking the x position of the block
                int tileY = i + posY;        // checking the y posiiton of the block
                if (tileX == x && tileY == y)    // if this condition is true, then blocks are colliding
                    return true;   
            }
        }
    }
    return false;         // Else return false
}

// Function to check collision with other blocks
bool checkBlockCollision(int x, int y, int block[4][4], int posX, int posY)
{
    return checkCollision(x, y, block, posX, posY);
}

// Function to place block on the grid
void placeBlock(int x, int y, int block[4][4], sf::Color color, sf::Color grid[gridHeight][gridWidth])
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (block[i][j] == 1)      // if condition to check if the grid is filled
            {
                grid[y + i][x + j] = color;    // places that block on the grid
            }
        }
    }
}

// Function to check if the position of the block is valid or not to place on the grid
bool isValidPosition(int x, int y, int block[4][4], const sf::Color grid[gridHeight][gridWidth])
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (block[i][j] == 1)
            {
                int tileX = j + x;
                int tileY = i + y;
                if (tileX < 0 || tileX >= gridWidth || tileY >= gridHeight)    // condition to check it the block does not go out of the window screen
                    return false;
                if (tileY >= 0 && grid[tileY][tileX] != sf::Color::Black)    // condition to check if the particular is occupied by another block or not
                    return false;
            }
        }
    } 
    return true;          // or else return true
}  

//Function to clear the row which is full and move the line above it down
void clearLine(sf::Color grid[gridHeight][gridWidth], int row)
{
    for (int i = row; i > 0; --i)     // for loop to move down the above lines down below
    {
        for (int j = 0; j < gridWidth; ++j)
        {
            grid[i][j] = grid[i - 1][j];
        }
    }
    for (int j = 0; j < gridWidth; ++j)    // set the line above it to black (meaning empty)
    {
        grid[0][j] = sf::Color::Black;
    }
}

// function to check if any row is full or not
int checkLines(sf::Color grid[gridHeight][gridWidth])
{
    int linesCleared = 0;                       // count how many lines cleared
    for (int i = gridHeight - 1; i >= 0; --i)    // for loop to iterated through each row of the grid
    {
        bool lineComplete = true;
        for (int j = 0; j < gridWidth; ++j)
        {
            if (grid[i][j] == sf::Color::Black)     // if row not complete then linecomplete is eqaul to false
            {
                lineComplete = false;
                break;
            }
        }
        if (lineComplete)      // if it true 
        {
            clearLine(grid, i);
            ++linesCleared;           
            ++i; // Recheck the same row since it has been cleared
        }
    }
    return linesCleared;
}

// Function to end game if the blocks hits the top of the window
bool isGameOver(const sf::Color grid[gridHeight][gridWidth])
{
    for (int j = 0; j < gridWidth; ++j)
    {
        if (grid[0][j] != sf::Color::Black)
            return true;
    }
    return false;
}

// Function to draw shadow of the blocks 
void drawShadow(sf::RenderWindow& window, int blockPosX, int blockPosY, int currentBlock, int currentRotation, int block[7][4][4][4], sf::Color blockColors[7], const sf::Color grid[gridHeight][gridWidth])
{
    int shadowPosY = blockPosY;

    // Move the shadow down until it reaches the valid position 
    while (isValidPosition(blockPosX, shadowPosY + 1, block[currentBlock][currentRotation], grid))
    {
        shadowPosY++;
    }

    for (int i = 0; i < 4; ++i)    // Draw the shadow on the grid
    {
        for (int j = 0; j < 4; ++j)
        {
            if (block[currentBlock][currentRotation][i][j] == 1)
            {                                                            // coordinates for grey color
                drawBlock(window, blockPosX + j, shadowPosY + i, sf::Color(100, 100, 100, 150));
            }
        }
    }
}

// Function to draw score 
void drawScore(sf::RenderWindow& window, int score, int level)
{
    sf::RectangleShape scoreBox(sf::Vector2f(180, 80));
    scoreBox.setFillColor(sf::Color::Black);
    scoreBox.setPosition(windowWidth, windowHeight / 2 - 70);
    scoreBox.setOutlineThickness(8);
    scoreBox.setOutlineColor(sf::Color::Blue);

    Text scoreText;
    scoreText.setFont(font);
    scoreText.setString("Score: " + to_string(score));
    scoreText.setCharacterSize(50);
    scoreText.setFillColor(Color::Blue);
    scoreText.setPosition(windowWidth + 20, windowHeight / 2 - 60);

    // Draw Level text
    Text levelText;
    levelText.setFont(font);
    levelText.setString("Level " + to_string(level));
    levelText.setCharacterSize(50);
    levelText.setFillColor(Color::White);
    levelText.setPosition(windowWidth + 40, windowHeight / 2 - 350);

    window.draw(scoreBox);
    window.draw(scoreText);
    window.draw(levelText);
}



// Function to update high scores
void updateHighscore(sf::RenderWindow& window, int score)
{

    int temp;

    for (int i = 0; i < 10; i++)
    {
        fstream ds("Highscore.txt", ios::binary | ios::in);    // binary file handling
        if (ds.is_open() == 0)
        {
            cout << "Can not open file";
        }
        else
        {
            ds.read((char*)&Hscore, sizeof(Hscore));       // reading the array from file

            for (int i = 0; i < 10; i++)            // updating the array
            {
                if (Hscore[i] == 0)
                {
                    Hscore[i] = score;
                    break;
                }
                else
                {
                    if (score > Hscore[i])      // if the array is full, then remove the last row score and update accordingly
                    {
                        for (int j = 9; j > i; j--)
                        {
                            Hscore[j] = Hscore[j - 1];
                        }
                        Hscore[i] = score;
                        break;
                    }
                }
            }

            for (int j = i + 1; j < 10; j++)    // descending order for the array
            {
                if (Hscore[j] > Hscore[i])
                {
                    temp = Hscore[i];
                    Hscore[i] = Hscore[j];
                    Hscore[j] = temp;
                }
            }
            ds.close();
            break;
        }
    }

    fstream fs("Highscore.txt", ios::binary | ios::out);     // writing in the file
    if (fs.is_open() == 0)
    {
        cout << "Can not load file";
    }
    else
    {
        fs.write((char*)&Hscore, sizeof(Hscore));
    }

}

// Function to draw highscore on console and window
void drawHighscore(sf::RenderWindow& window)
{
    sf::Text Highscore;
    Highscore.setFont(font);
    Highscore.setCharacterSize(45);
    Highscore.setString("HighScore");
    Highscore.setFillColor(sf::Color::Black);
    Highscore.setPosition(20, 20);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    return; // Go back to the main menu
                }
            }
        }

        fstream ds("Highscore.txt", ios::binary | ios::in);
        if (ds.is_open() == 0)
        {
            cout << "can not load file";
        }
        else
        {
            ds.read((char*)&Hscore, sizeof(Hscore));
            for (int i = 0; i < 10; i++)
            {
                Text highscore;
                highscore.setFont(font);
                highscore.setCharacterSize(50);
                highscore.setString("Player: " + to_string(Hscore[i]));
                highscore.setFillColor(sf::Color::White);
                highscore.setPosition(100, 20 + i * 10);
            }
            cout << "HighScore" << endl;
            cout << "---------" << endl;
            for (int i = 0; i < 10; i++)
            {
                cout << Hscore[i] << endl;
            }
            cout << endl << endl;
            break;
        }

        window.clear(sf::Color::Blue);
        window.draw(Highscore);
        window.display();
    }
}

// Function to draw Help window
void drawHelpWindow(sf::RenderWindow& window)
{
    sf::Font font;
    if (!font.loadFromFile("Simple Handmade.otf"))
    {
        std::cout << "Error loading font\n";
    }

    sf::Text helpText;                     // writing text
    helpText.setFont(font);
    helpText.setCharacterSize(45);
    helpText.setString(
        "Welcome to Tetris!\n\n"
        "Controls:\n"
        "Left arrow: Move block left\n"
        "Right arrow: Move block right\n"
        "Up arrow: Rotate block\n"
        "Down arrow: Speed up block\n"
        "Space: Start game\n"
        "Escape: Quit game\n\n"
        "Goal:\n"
        "Fill horizontal lines to clear them and\n"
        "score points. The game ends if blocks\n"
        "reach the top of the grid.\n\n"
        "Press ESCAPE to go back to the main menu.");

    helpText.setFillColor(sf::Color::Red);
    helpText.setPosition(20, 20);


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    return; // Go back to the main menu
                }
            }
        }

        window.clear(sf::Color::Blue);
        window.draw(helpText);
        window.display();
    }
}

// Function to draw menu 
void drawMenu(sf::RenderWindow& window, bool& gameStarted)
{
    sf::Font font2;
    if (!font2.loadFromFile("Bright Night.otf"))
    {
        std::cout << "Error loading font\n";
    }

    sf::Text startGame;         // writing text
    startGame.setFont(font2);
    startGame.setString("Start new game");
    startGame.setCharacterSize(50);
    startGame.setFillColor(sf::Color::White);
    startGame.setPosition(150, 400);

    sf::Text HighScore;         // writing text
    HighScore.setFont(font2);
    HighScore.setString("High Score");
    HighScore.setCharacterSize(50);
    HighScore.setFillColor(sf::Color::White);
    HighScore.setPosition(150, 450);

    sf::Text Help;                // writing text
    Help.setFont(font2);
    Help.setString("Help");
    Help.setCharacterSize(50);
    Help.setFillColor(sf::Color::White);
    Help.setPosition(150, 500);

    sf::Text quitGame;             // writing text
    quitGame.setFont(font2);
    quitGame.setString("Exit");
    quitGame.setCharacterSize(50);
    quitGame.setFillColor(sf::Color::White);
    quitGame.setPosition(150, 550);

    sf::Texture texture;

    if (!texture.loadFromFile("tetris_image_3.jpg"))     // load image
    {
        std::cout << "Error loading image\n";
    }
    sf::Sprite sprite(texture);
    sprite.setScale(1, 1);
    sprite.setPosition(0, 0);

    window.draw(sprite);
    window.draw(startGame);
    window.draw(HighScore);
    window.draw(Help);
    window.draw(quitGame);

    window.display();

    bool menuOpen = true;


    while (menuOpen)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)   
            {
                window.close();
                menuOpen = false;
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                // if space is pressed, the game starts
                if (event.key.code == sf::Keyboard::Space)
                {
                    menuOpen = false;
                    gameStarted = true;
                    return;
                }
                // if escape is pressed, then game ends
                else if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                    menuOpen = false;
                    gameStarted = false;
                    break;

                }
            }
            else if (event.type == sf::Event::MouseButtonPressed)   
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);   

                    // if click pressed on start game then game starts playing
                    if (startGame.getGlobalBounds().contains(mousePosition.x, mousePosition.y))  
                    {
                        menuOpen = false;
                        gameStarted = true;

                    }
                    // if click pressed on highscore, then highscore is shown
                    else if (HighScore.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    {
                        drawHighscore(window);
                        window.draw(sprite);
                        window.draw(startGame);
                        window.draw(HighScore);
                        window.draw(Help);
                        window.draw(quitGame);
                        window.display();
                    }
                    // if click is pressed on help, then help menu is open
                    else if (Help.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    {
                        drawHelpWindow(window);
                        window.clear();
                        window.draw(sprite);
                        window.draw(startGame);
                        window.draw(HighScore);
                        window.draw(Help);
                        window.draw(quitGame);
                        window.display();
                    }
                    // if quitgame is pressed, then window closes
                    else if (quitGame.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    {
                        window.close();
                        menuOpen = false;
                    }
                }
            }

            // This is the condition to turn the text red when mouse hovers over them
            else if (event.type == sf::Event::MouseMoved)
            {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

                if (startGame.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    startGame.setFillColor(sf::Color::Red);
                else
                    startGame.setFillColor(sf::Color::White);

                if (HighScore.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    HighScore.setFillColor(sf::Color::Red);
                else
                    HighScore.setFillColor(sf::Color::White);

                if (Help.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    Help.setFillColor(sf::Color::Red);
                else
                    Help.setFillColor(sf::Color::White);

                if (quitGame.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    quitGame.setFillColor(sf::Color::Red);
                else
                    quitGame.setFillColor(sf::Color::White);
            }
        }

        window.clear();
        window.draw(sprite);
        window.draw(startGame);
        window.draw(HighScore);
        window.draw(Help);
        window.draw(quitGame);
        window.display();
    }
}

// Function to draw Pause menu
void drawPauseMenu(sf::RenderWindow& window)
{

    Text pause;           // Writing text
    pause.setFont(font);
    pause.setString("Game Paused");
    pause.setCharacterSize(60);
    pause.setFillColor(sf::Color::White);
    pause.setPosition(100, windowHeight / 2 - 300);


    Text resumeText;         // Writing text
    resumeText.setFont(font);
    resumeText.setString("Resume");
    resumeText.setCharacterSize(50);
    resumeText.setFillColor(sf::Color::White);
    resumeText.setPosition(100, 300);

    Text highscore;            // Writing text
    highscore.setFont(font);
    highscore.setString("HighScore");
    highscore.setCharacterSize(50);
    highscore.setFillColor(sf::Color::White);
    highscore.setPosition(100, 350);

    Text help;                  // Writing text
    help.setFont(font);
    help.setString("Help");
    help.setCharacterSize(50);
    help.setFillColor(sf::Color::White);
    help.setPosition(100, 400);

    Text exit;                  // Writing text
    exit.setFont(font);
    exit.setString("Exit");
    exit.setCharacterSize(50);
    exit.setFillColor(sf::Color::White);
    exit.setPosition(100, 450);


    Text menuText;                // Writing text
    menuText.setFont(font);
    menuText.setString("Return to Menu");
    menuText.setCharacterSize(50);
    menuText.setFillColor(sf::Color::White);
    menuText.setPosition(100, 500);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    return; // Go back to the main menu
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                    // if mouse is pressed on resume, then the game continues
                    if (resumeText.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    {
                        return;
                    }
                    //if mouse clicked on menu, then it returns to menu
                    else if (menuText.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    {
                        gameStarted = false;
                        drawMenu(window, gameStarted);
                    }
                    // if mouse is clicked on highscore, then highscore is shown
                    else if (highscore.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    {
                        drawHighscore(window);
                    }
                    // if mouse is clicked on help, then help page is shown
                    else if (help.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    {
                        drawHelpWindow(window);
                    }
                    // if mouse clicked on exit, then window closes
                    else if (exit.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    {
                        window.close();
                    }

                }
            }
            // This is the condition to turn the text red when mouse hovers over them
            else if (event.type == sf::Event::MouseMoved)
            {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

                if (resumeText.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    resumeText.setFillColor(sf::Color::Red);
                else
                    resumeText.setFillColor(sf::Color::White);

                if (menuText.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    menuText.setFillColor(sf::Color::Red);
                else
                    menuText.setFillColor(sf::Color::White);

                if (help.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    help.setFillColor(sf::Color::Red);
                else
                    help.setFillColor(sf::Color::White);

                if (highscore.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    highscore.setFillColor(sf::Color::Red);
                else
                    highscore.setFillColor(sf::Color::White);

                if (exit.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    exit.setFillColor(sf::Color::Red);
                else
                    exit.setFillColor(sf::Color::White);

            }
        }

        window.clear(sf::Color::Blue);
        window.draw(pause);
        window.draw(resumeText);
        window.draw(highscore);
        window.draw(help);
        window.draw(exit);
        window.draw(menuText);
        window.display();
    }

}


// Function to increase difficulty
void increaseDifficulty(int& blockSpeed, int score)
{
    if (score < 20)
    {
        speedIncrease = normalSpeed;
        blockSpeed = speedIncrease;
    }
    else if (score >= 20 && score <= 60)
    {
        speedIncrease = 100;
        blockSpeed = speedIncrease;
    }
    else if (blockSpeed > 60)
    {
        speedIncrease = 50;
        blockSpeed = speedIncrease;
    }
}


int main()
{
    sf::RenderWindow window(sf::VideoMode(windowWidth + 200, windowHeight), "Tetris"); // window size
    loadFont();

    sf::Music gameMusic;                         // Playing music in the game
    if (!gameMusic.openFromFile("Kalinka.ogg"))
    {
        std::cout << "Error loading background music!" << std::endl;
    }
    gameMusic.setLoop(true);     // loop to keep playing music
    gameMusic.play();

    sf::Texture backgroundimage;                                  // Setting background image
    if (!backgroundimage.loadFromFile("background_image_4.jpg"))
    {
        cout << "Can not load image";
    }

    sf::Sprite backgroundSprite(backgroundimage);
    backgroundSprite.setScale(1, 1.8);
    backgroundSprite.setPosition(0, 0);

    srand(time(0));

    sf::Color grid[gridHeight][gridWidth];          // intialize grid color with black
    for (int i = 0; i < gridHeight; ++i)
    {
        for (int j = 0; j < gridWidth; ++j)
        {
            grid[i][j] = sf::Color::Black;
        }
    }

    int blockSpeed = normalSpeed; // Initial block speed
    int score = 0;                // score variable
    bool isPaused = false;        // pause variable



    while (window.isOpen())
    {
        if (!gameStarted)      // if condition to show menu until game starts
        {
            Event event;
            window.clear(sf::Color::Black);
            drawMenu(window, gameStarted);
            window.display();
        }
        else
        {
            int blockPosX = 4; // Initial x position of the block
            int blockPosY = 0;  // Intial y posiiton of the block
            bool blockReachedGround = false;   // blockreached variable intilized to false

            int currentBlock = rand() % 7;    // generate random blocks 
            int currentRotation = 0;
            sf::Color blockColors[7] = {     // array to store color of the blocks
                sf::Color::Cyan,
                sf::Color(255, 165, 0), // Orange
                sf::Color::Blue,
                sf::Color::Yellow,
                sf::Color::Green,
                sf::Color::Magenta,
                sf::Color::Red };

            int block[7][4][4][4] = {      // 4D array to store all possible rotations of the blocks 
                {
                    // I Block
                    {
                        {0, 0, 0, 0},
                        {1, 1, 1, 1},
                        {0, 0, 0, 0},
                        {0, 0, 0, 0}},
                    {
                        {0, 0, 1, 0},
                        {0, 0, 1, 0},
                        {0, 0, 1, 0},
                        {0, 0, 1, 0}},
                    {
                        {0, 0, 0, 0},
                        {0, 0, 0, 0},
                        {1, 1, 1, 1},
                        {0, 0, 0, 0}},
                    {
                        {0, 1, 0, 0},
                        {0, 1, 0, 0},
                        {0, 1, 0, 0},
                        {0, 1, 0, 0}}},
                {
                    // J Block
                    {
                        {0, 1, 0, 0},
                        {0, 1, 0, 0},
                        {1, 1, 0, 0},
                        {0, 0, 0, 0}},
                    {
                        {1, 0, 0, 0},
                        {1, 1, 1, 0},
                        {0, 0, 0, 0},
                        {0, 0, 0, 0}},
                    {
                        {0, 1, 1, 0},
                        {0, 1, 0, 0},
                        {0, 1, 0, 0},
                        {0, 0, 0, 0}},
                    {
                        {0, 0, 0, 0},
                        {1, 1, 1, 0},
                        {0, 0, 1, 0},
                        {0, 0, 0, 0}}},
                {
                    // L Block
                    {
                        {0, 1, 0, 0},
                        {0, 1, 0, 0},
                        {0, 1, 1, 0},
                        {0, 0, 0, 0}},
                    {
                        {0, 0, 0, 0},
                        {1, 1, 1, 0},
                        {1, 0, 0, 0},
                        {0, 0, 0, 0}},
                    {
                        {1, 1, 0, 0},
                        {0, 1, 0, 0},
                        {0, 1, 0, 0},
                        {0, 0, 0, 0}},
                    {
                        {0, 0, 1, 0},
                        {1, 1, 1, 0},
                        {0, 0, 0, 0},
                        {0, 0, 0, 0}}},
                {
                    // O Block
                    {
                        {0, 0, 0, 0},
                        {0, 1, 1, 0},
                        {0, 1, 1, 0},
                        {0, 0, 0, 0}},
                    {
                        {0, 0, 0, 0},
                        {0, 1, 1, 0},
                        {0, 1, 1, 0},
                        {0, 0, 0, 0}},
                    {
                        {0, 0, 0, 0},
                        {0, 1, 1, 0},
                        {0, 1, 1, 0},
                        {0, 0, 0, 0}},
                    {
                        {0, 0, 0, 0},
                        {0, 1, 1, 0},
                        {0, 1, 1, 0},
                        {0, 0, 0, 0}}},
                {
                    // S Block
                    {
                        {0, 0, 0, 0},
                        {0, 1, 1, 0},
                        {1, 1, 0, 0},
                        {0, 0, 0, 0}},
                    {
                        {1, 0, 0, 0},
                        {1, 1, 0, 0},
                        {0, 1, 0, 0},
                        {0, 0, 0, 0}},
                    {
                        {0, 0, 0, 0},
                        {0, 1, 1, 0},
                        {1, 1, 0, 0},
                        {0, 0, 0, 0}},
                    {
                        {1, 0, 0, 0},
                        {1, 1, 0, 0},
                        {0, 1, 0, 0},
                        {0, 0, 0, 0}}},
                {
                    // T Block
                    {
                        {0, 0, 0, 0},
                        {1, 1, 1, 0},
                        {0, 1, 0, 0},
                        {0, 0, 0, 0}},
                    {
                        {0, 1, 0, 0},
                        {1, 1, 0, 0},
                        {0, 1, 0, 0},
                        {0, 0, 0, 0}},
                    {
                        {0, 1, 0, 0},
                        {1, 1, 1, 0},
                        {0, 0, 0, 0},
                        {0, 0, 0, 0}},
                    {
                        {0, 1, 0, 0},
                        {0, 1, 1, 0},
                        {0, 1, 0, 0},
                        {0, 0, 0, 0}}},
                {
                    // Z Block
                    {
                        {0, 0, 0, 0},
                        {1, 1, 0, 0},
                        {0, 1, 1, 0},
                        {0, 0, 0, 0}},
                    {
                        {0, 1, 0, 0},
                        {1, 1, 0, 0},
                        {1, 0, 0, 0},
                        {0, 0, 0, 0}},
                    {
                        {0, 0, 0, 0},
                        {1, 1, 0, 0},
                        {0, 1, 1, 0},
                        {0, 0, 0, 0}},
                    {
                        {0, 1, 0, 0},
                        {1, 1, 0, 0},
                        {1, 0, 0, 0},
                        {0, 0, 0, 0}}} };

            int level = 2; // Level 2

            // Loop continued until blocks hits the ground
            while (!blockReachedGround)
            {
                sf::Event event;
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();
                    if (event.type == sf::Event::KeyPressed)
                    {
                        if (!blockReachedGround)   // if condition for when block not reached ground
                        {
                            // if condition to move block left 
                            if (event.key.code == sf::Keyboard::Left)
                            {
                                if (blockPosX > 0 && isValidPosition(blockPosX - 1, blockPosY, block[currentBlock][currentRotation], grid))
                                    blockPosX--;
                            }
                            // if condition to move block right
                            else if (event.key.code == sf::Keyboard::Right)
                            {
                                if (blockPosX < gridWidth - 3 && isValidPosition(blockPosX + 1, blockPosY, block[currentBlock][currentRotation], grid))
                                    blockPosX++;
                            }
                            // if condition to rotate the blocks
                            else if (event.key.code == sf::Keyboard::Up)
                            {
                                int newRotation = (currentRotation + 1) % 4;
                                if (isValidPosition(blockPosX, blockPosY, block[currentBlock][newRotation], grid))
                                    currentRotation = newRotation;
                            }
                            // if condition to increase speed of the block when down key is pressed
                            else if (event.key.code == sf::Keyboard::Down)
                            {
                                blockSpeed = fastSpeed;
                            }

                        }
                    }

                    // if condition to set the speed back to normal if the down key is not pressed anymore
                    if (event.type == sf::Event::KeyReleased)
                    {
                        if (event.key.code == sf::Keyboard::Down)
                        {
                            blockSpeed = speedIncrease;
                        }
                    }
                    // if condition to pause the game 
                    if (event.type == Event::KeyPressed && event.key.code == Keyboard::P)
                    {
                        drawPauseMenu(window);

                    }


                }

                window.clear(sf::Color::Black);

                // Drawing Background
                window.draw(backgroundSprite);

                // Drawing grid
                drawGrid(window);

                // Drawing blocks on the grid
                for (int i = 0; i < gridHeight; ++i)
                {
                    for (int j = 0; j < gridWidth; ++j)
                    {
                        if (grid[i][j] != sf::Color::Black)
                            drawBlock(window, j, i, grid[i][j]);
                    }
                }

                // Drawing shadow
                drawShadow(window, blockPosX, blockPosY, currentBlock, currentRotation, block, blockColors, grid);

                // Drawing block
                for (int i = 0; i < 4; ++i)
                {
                    for (int j = 0; j < 4; ++j)
                    {
                        if (block[currentBlock][currentRotation][i][j] == 1)
                        {
                            drawBlock(window, blockPosX + j, blockPosY + i, blockColors[currentBlock]);
                        }
                    }
                }

                // Move block downwards until it hits the ground
                if (!blockReachedGround)
                {
                    blockPosY++;
                    if (!isValidPosition(blockPosX, blockPosY, block[currentBlock][currentRotation], grid))
                    {
                        blockReachedGround = true;
                        placeBlock(blockPosX, blockPosY - 1, block[currentBlock][currentRotation], blockColors[currentBlock], grid);
                        int linesCleared = checkLines(grid);
                        // Calculate score 
                        switch (linesCleared)
                        {
                        case 1:
                            score += 10 * level;
                            break;
                        case 2:
                            score += 30 * level;
                            break;
                        case 3:
                            score += 60 * level;
                            break;
                        case 4:
                            score += 100 * level;
                            break;
                        }
                    }

                }

                // Draw score
                drawScore(window, score, level);

                window.display();

                // Time delay 
                sf::Time elapsed = sf::milliseconds(blockSpeed);
                sf::sleep(elapsed);

            }

            // New block
            blockReachedGround = false;
            if (isGameOver(grid))
            {
                cout << "Game Over! Score: " << score << "\n";
                updateHighscore(window, score);
                gameStarted = false;
                score = 0;
            }
            else
            {
                increaseDifficulty(blockSpeed, score);  // function to increase difficulty of the game
            }

        }
    }

    return 0;
}