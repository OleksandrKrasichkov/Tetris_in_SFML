#include <iostream>
#include <SFML/Graphics.hpp>

void rotatePiece(int[4][4]);
void lockPieceToBoard(int[4][4], int, int, int[20][10]);
bool checkCollision(const int[4][4], int, int, const int[20][10]);

int main()
{
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
    for (int i = 0; i <= 10; ++i)
    {
        float x = startX + (i * cellSize);
        grid.append(sf::Vertex({ x, startY }, gridColor));
        grid.append(sf::Vertex({ x, startY + gridHeight }, gridColor));
    }

    for (int i = 0; i <= 20; ++i)
    {
        float y = startY + (i * cellSize);
        grid.append(sf::Vertex({ startX, y }, gridColor));
        grid.append(sf::Vertex({ startX + gridWidth, y }, gridColor));
    }

    //create a grid, 0-nothing 1-block
    int board[20][10] = { 0 };


    //create a shape
    int piece[4][4] = {
        {0,0,1,0},
        {0,0,1,0},
        {0,0,1,0},
        {0,0,1,0}
    };

    //create block for 1
    sf::RectangleShape cellBlock(sf::Vector2f(cellSize - 1.f, cellSize - 1.f));
    cellBlock.setFillColor(sf::Color::Cyan);

    //grid position of a piece
    int pieceGridX = 3;
    int pieceGridY = -4;



    //tracks time between frames
    sf::Clock clock;



    //drops a row every ? second
    float dropTimer = 0.0f;
    float dropInterval = 0.5f;
    float currentInterval = dropInterval;
    float spedupInterval = 0.1f;

    float sideTimer = 0.0f;
    float sideInitialDelay = 0.18f;
    float sideRepeatInterval = 0.07f;



    while (window.isOpen())
    {
        //get time one frame spent
        float frameTime = clock.restart().asSeconds();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad2))
        {
            currentInterval = spedupInterval;
        }
        else { currentInterval = dropInterval; }

        bool leftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad4);
        bool rightPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad6);

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
                lockPieceToBoard(piece, pieceGridX, pieceGridY, board);
                pieceGridX = 3;
                pieceGridY = -4;
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
            if (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Z)
            {
                rotatePiece(piece);
                if (checkCollision(piece, pieceGridX, pieceGridY, board))
                {
                    for (int i = 0; i < 3; ++i) rotatePiece(piece);
                }
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
                if (board[r][c] == 1)
                {
                    cellBlock.setPosition(sf::Vector2f((startX + c * cellSize), (startY + r * cellSize)));

                    window.draw(cellBlock);
                }
            }
        }


        for (int r = 0; r < 4; ++r)
        {
            for (int c = 0; c < 4; ++c)
            {
                if (piece[r][c] == 1)
                {
                    // Convert grid coordinates to pixel positions
                    float pixelX = startX + ((pieceGridX + c) * cellSize);
                    float pixelY = startY + ((pieceGridY + r) * cellSize);

                    cellBlock.setPosition(sf::Vector2f(pixelX, pixelY));
                    if (pixelY >= startY) window.draw(cellBlock);
                }
            }
        }

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

void lockPieceToBoard(int p[4][4], int gridX, int gridY, int board[20][10])
{
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            int pieceX = gridX + c;
            int pieceY = gridY + r;
            if (p[r][c] == 1 && pieceY < 20 && pieceY >= 0 && pieceX < 10 && pieceX >= 0)
            {
                board[pieceY][pieceX] = 1;
            }
        }
    }
}
bool checkCollision(const int p[4][4], int targetX, int targetY, const int board[20][10])
{
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            if (p[r][c] == 1)
            {
                int nextX = targetX + c;
                int nextY = targetY + r;

                if (nextY >= 20 || nextX < 0 || nextX >= 10) return true;
                else if (nextY >= 0)
                {
                    if (board[nextY][nextX] == 1) return true;
                }

            }
        }
    }
    return false;
}


