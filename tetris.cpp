#include <iostream>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <filesystem>

void rotatePiece(int[4][4]);
void lockPieceToBoard(int[4][4], int, int, int[20][10], int&, bool&, int&);
bool checkCollision(const int[4][4], int, int, const int[20][10]);
void spawnRandomPiece(int[4][4], int&, int&);

const int TETROMINOES[7][4][4] = {
    // I-Piece
    {
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0}
    },
    // J-Piece
    {
        {0, 2, 0, 0},
        {0, 2, 0, 0},
        {2, 2, 0, 0},
        {0, 0, 0, 0}
    },
    // L-Piece
    {
        {0, 3, 0, 0},
        {0, 3, 0, 0},
        {0, 3, 3, 0},
        {0, 0, 0, 0}
    },
    // O-Piece
    {
        {0, 0, 0, 0},
        {0, 4, 4, 0},
        {0, 4, 4, 0},
        {0, 0, 0, 0}
    },
    // S-Piece
    {
        {0, 5, 5, 0},
        {5, 5, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // T-Piece
    {
        {0, 6, 0, 0},
        {6, 6, 6, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // Z-Piece
    {
        {7, 7, 0, 0},
        {0, 7, 7, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }
};

const sf::Color PIECE_COLOR[8] = {
    sf::Color::Transparent, // 0: Empty
    sf::Color::Cyan,        // 1: I-Piece
    sf::Color::Blue,        // 2: J-Piece
    sf::Color(255, 165, 0), // 3: L-Piece (Orange)
    sf::Color::Yellow,      // 4: O-Piece
    sf::Color::Green,       // 5: S-Piece
    sf::Color(128, 0, 128), // 6: T-Piece (Purple)
    sf::Color::Red          // 7: Z-Piece
};
const int POINTS_FOR_LINES[5] = { 0, 100, 300, 500, 800 };
int main()
{
    srand(time(NULL));

    //get screen resolution and 100% the window. const used for just optimizing cause compiler knows its never gonna change
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const unsigned int width = desktop.size.x;
    const unsigned int height = desktop.size.y;

    //open a window
    sf::RenderWindow window(sf::VideoMode({ width, height }), "My Game",sf::Style::None);

    //get one cell size
    const float cellSize = height * 0.035f;
    
    //get grid width and height (tetris borders)
    const float gridWidth = 10 * cellSize;
    const float gridHeight = 20 * cellSize;

    //get line thickness
    const float lineTHickness = width * 0.01f;

    //position of the grid (tetris borders)
    const float startX = width / 6.0f;
    const float startY = height / 10.0f;

    //set updates per second
    window.setFramerateLimit(60);

    //create the grid
    sf::VertexArray grid(sf::PrimitiveType::Lines);
    sf::Color gridColor(100, 100, 100, 150);

    //array, where every next two elements create a line
    // Vertical lines
    for (int i = 0; i <= 10; ++i)
    {
        float x = startX + (i * cellSize);
        // Correct SFML 3 aggregate syntax: { position, color }
        grid.append(sf::Vertex{ sf::Vector2f(x, startY), gridColor });
        grid.append(sf::Vertex{ sf::Vector2f(x, startY + gridHeight), gridColor });
    }

    // Horizontal lines
    for (int i = 0; i <= 20; ++i)
    {
        float y = startY + (i * cellSize);
        grid.append(sf::Vertex{ sf::Vector2f(startX, y), gridColor });
        grid.append(sf::Vertex{ sf::Vector2f(startX + gridWidth, y), gridColor });
    }

    //create a grid, 0-nothing 1-block
    int board[20][10] = { 0 };

    

    int piece[4][4] = { 0 };

    //create block for 1
    sf::RectangleShape cellBlock(sf::Vector2f(cellSize - 1.f, cellSize - 1.f));
    cellBlock.setFillColor(sf::Color::Cyan);

    //grid position of a piece
    int pieceGridX = 3;
    int pieceGridY = -4;

    spawnRandomPiece(piece, pieceGridX, pieceGridY);


    //tracks time between frames
    sf::Clock clock;



    //drops a row every ? second
    float dropTimer = 0.0f;
    float startInterval = 0.5f;
    float currentInterval = startInterval;
    float spedupInterval = 0.1f;

    float sideTimer = 0.0f;
    float sideInitialDelay = 0.18f;
    float sideRepeatInterval = 0.07f;

    sf::Font font;
    if (!font.openFromFile("../assets/fonts/russian.ttf"))
    {
        if (!font.openFromFile("../assets/fonts/arial.ttf"))
        {
            std::cout << "No font found lol";
        }
    }
    int score = 0;
    sf::Text scoreText(font);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setCharacterSize(height * 0.04);
    scoreText.setFillColor(sf::Color::White);
    sf::FloatRect bounds = scoreText.getLocalBounds();
    scoreText.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f));
    scoreText.setPosition(sf::Vector2f(startX + gridWidth/2.0f, height - height / 10));

    int totalLinesCleared = 0;

    bool isGameOver = false;
    while (window.isOpen())
    {
        //get time one frame spent
        float frameTime = clock.restart().asSeconds();

        int level = totalLinesCleared / 10;

        float dropInterval = std::max(0.05f, startInterval - (level * 0.08f));

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad2) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        {
            currentInterval = std::min(spedupInterval, dropInterval);
        }
        else { currentInterval = dropInterval; }

        bool leftPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad4)) || (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left));
        bool rightPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad6)) || (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right));

        if (leftPressed || rightPressed)
        {
            int nextX = pieceGridX;

            if (sideTimer == 0.0f)
            {
                if (leftPressed) nextX--;
                else nextX++;
            }

            sideTimer += frameTime;

            if(sideTimer >= sideInitialDelay)
            {
                if (leftPressed) nextX--;
                else nextX++;

                sideTimer = sideInitialDelay - sideRepeatInterval;
            }
            if (!checkCollision(piece, nextX, pieceGridY, board))
            {
                pieceGridX = nextX;
            }

        }
        else {
            sideTimer = 0.0f;
        }



        dropTimer += frameTime;



        if (dropTimer >= currentInterval)
        {
            if (checkCollision(piece, pieceGridX, pieceGridY + 1, board))
            {
                lockPieceToBoard(piece, pieceGridX, pieceGridY, board, score, isGameOver, totalLinesCleared);
                if (!isGameOver)
                {
                    spawnRandomPiece(piece, pieceGridX, pieceGridY);
                }



            }
            else {
                pieceGridY++;
                dropTimer = 0.0f;
            }

        }

        //optional - smth(true) or nothing(false). PollEvent asks for an event from a queue of events windows created
        while (std::optional<sf::Event> event = window.pollEvent())
        {
            //check if that event is close(X) or esc is pressed
            if (((*event).is<sf::Event::Closed>()) || ((*event).is<sf::Event::KeyReleased>() && (*((*event).getIf<sf::Event::KeyReleased>())).code == sf::Keyboard::Key::Escape))
            {
                window.close();
            }

            //rotate by 90 if z was pressed
            if (event->is<sf::Event::KeyPressed>() && (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Z || event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Up))
            {
                rotatePiece(piece);
                if (checkCollision(piece, pieceGridX, pieceGridY, board))
                {
                    for (int i = 0; i < 3; ++i) rotatePiece(piece);
                }
            }
            if (event->is<sf::Event::KeyPressed>() && (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::R))
            {
                for (int r = 0; r < 20; ++r)
                    for (int c = 0; c < 10; ++c)
                    {
                        board[r][c] = 0;
                    }
                score = 0;
                spawnRandomPiece(piece, pieceGridX, pieceGridY);
                totalLinesCleared = 0;
                isGameOver = false;
            }
        }
               


        

      
        //clear a screen with a color
        window.clear(sf::Color::Black);

        //window.draw(block);

        window.draw(grid);


        for (int r = 0; r < 20; ++r)
        {
            for (int c = 0; c < 10; ++c)
            {
                if (board[r][c] > 0)
                {
                    cellBlock.setPosition(sf::Vector2f((startX + c * cellSize), (startY + r * cellSize)));
                    cellBlock.setFillColor(PIECE_COLOR[board[r][c]]);

                    window.draw(cellBlock);
                }
            }
        }


        for (int r = 0; r < 4; ++r)
        {
            for (int c = 0; c < 4; ++c)
            {
                if (piece[r][c] > 0)
                {
                    // Convert grid coordinates to pixel positions
                    float pixelX = startX + ((pieceGridX + c) * cellSize);
                    float pixelY = startY + ((pieceGridY + r) * cellSize);

                    cellBlock.setPosition(sf::Vector2f(pixelX, pixelY));
                    cellBlock.setFillColor(PIECE_COLOR[piece[r][c]]);
                    if (pixelY >= startY) window.draw(cellBlock);
                }
            }
        }
        scoreText.setString("Score: " + std::to_string(score));
        bounds = scoreText.getLocalBounds();
        scoreText.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f));
        scoreText.setPosition(sf::Vector2f(startX + gridWidth / 2.0f, height - height / 10));
        window.draw(scoreText);

        //displays everything you did, without it there is no change
        window.display();
    }

    return 0;
}

//rotate a piece
void rotatePiece(int p[4][4])
{
    int rotated[4][4];
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            rotated[i][j] = p[3 - j][i];
        }
    }
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            p[i][j] = rotated[i][j];
        }
    }

}

void lockPieceToBoard(int p[4][4], int gridX, int gridY, int board[20][10], int &score, bool &isGameOver, int &totalLinesCleared)
{
    int fullLines = 0;
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {

            if (p[r][c] > 0)
            {
                int pieceX = gridX + c;
                int pieceY = gridY + r;
                if (pieceY <= 0) isGameOver = true;

                if (pieceY < 20 && pieceY >= 0 && pieceX < 10 && pieceX >= 0)
                {
                    board[pieceY][pieceX] = p[r][c];

                }
            }
            
        }
    }
    for (int r = 0; r < 20; ++r)
    {
        int count = 0;
        for (int c = 0; c < 10; ++c)
        {
            if (board[r][c] > 0)
            {
                    count++;
            }
        }
        if (count == 10)
        {
            ++fullLines;
            for (int c = 0; c < 10; ++c)
            {
                board[r][c] = 0;
            }
            for (int r2 = r-1; r2 > 0; --r2)
            {
                for (int c = 0; c < 10; ++c)
                {
                    if (board[r2][c] > 0)
                    {
                        board[r2 + 1][c] = board[r2][c];
                        board[r2][c] = 0;
                    }
                }
            }
        }
        count = 0;
    }
    totalLinesCleared += fullLines;
    score += POINTS_FOR_LINES[fullLines];
}
bool checkCollision(const int p[4][4], int targetX, int targetY, const int board[20][10])
{
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            if (p[r][c] > 0)
            {
                int nextX = targetX + c;
                int nextY = targetY + r;

                if (nextY >= 20 || nextX < 0 || nextX >= 10) return true;
                else if (nextY >= 0)
                {
                    if (board[nextY][nextX] > 0) return true;
                }
            }
        }
    }
    return false;
}
void spawnRandomPiece(int p[4][4], int &pieceGridX, int &pieceGridY)
{
    int randomNum = rand() % 7;
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            p[r][c] = TETROMINOES[randomNum][r][c];
        }
    }
    int maxRow = 0;
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            if (p[r][c] > 0)
            {
                maxRow = std::max(maxRow, r);
            }

        }
    }
    pieceGridY = -(maxRow + 1);
    pieceGridX = 3;
}




