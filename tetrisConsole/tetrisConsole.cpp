#include <iostream>
#include <SFML/Graphics.hpp>

void rotatePiece(int[4][4]);

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






    //bottom of the grid
    const float gridBottom = startY + gridHeight;

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
    int pieceGridY = 0;



    //tracks time between frames
    sf::Clock clock;

    //speed a block per second
    float fallSpeed = 5 * cellSize;

    //drops a row every ? second
    float dropTimer = 0.0f;
    float dropInterval = 0.5f;
    float currentInterval = dropInterval;
    float spedupInterval = 0.2f;
    int maxRow = 0;
    int maxLeftCol = 3;
    int maxRightCol = 0;

    while (window.isOpen())
    {
        //get time one frame spent
        float frameTime = clock.restart().asSeconds();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad2))
        {
            currentInterval = spedupInterval;
        }
        else { currentInterval = dropInterval; }



        

        //optional - smth(true) or nothing(false). PollEvent asks for an event from a queue of events windows created
        while (std::optional<sf::Event> event = window.pollEvent())
        {
            //check if that event is close(X) or esc is pressed
            if ( ((*event).is<sf::Event::Closed>() ) || ((*event).is<sf::Event::KeyReleased>() && (*((*event).getIf<sf::Event::KeyReleased>())).code == sf::Keyboard::Key::Escape)) 
            {
                window.close();
            }

            //rotate by 90 if z was pressed
            if (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Z)
            {
                rotatePiece(piece);
            }

            //move left right
            if (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Numpad4)
            {
                pieceGridX--;
            }
            if (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Numpad6)
            {
                pieceGridX++;
            }
        }




        dropTimer += frameTime;

        if (dropTimer >= currentInterval)
        {
            pieceGridY++;
            dropTimer = 0.0f;
        }

        maxRow = 0;
        maxLeftCol = 3;
        maxRightCol = 0;
        //find the lowest row with 1
        for (int r = 0; r < 4; ++r)
        {
            for (int c = 0; c < 4; ++c)
            {
                if (piece[r][c] == 1)
                {
                    maxRow = std::max(maxRow, r);
                }
            }
        }

        if (pieceGridY + maxRow >= 20)
        {
            pieceGridY = 20 - maxRow - 1; // Clamp to bottom row
        }


        //find the leftest column with 1
        for (int r = 0; r < 4; ++r)
        {
            for (int c = 0; c < 4; ++c)
            {
                if (piece[r][c] == 1)
                {
                    maxLeftCol = std::min(maxLeftCol, c);
                }
            }
        }

        if (pieceGridX + maxLeftCol < 0)
        {
            pieceGridX = 0 - maxLeftCol;
        }
        //find the rightest column with 1
        for (int r = 0; r < 4; ++r)
        {
            for (int c = 0; c < 4; ++c)
            {
                if (piece[r][c] == 1)
                {
                    maxRightCol = std::max(maxRightCol, c);
                }
            }
        }

        if (pieceGridX + maxRightCol >= 10)
        {
            pieceGridX = 10 - maxRightCol - 1;
        }


        //move block
       // piece.move(sf::Vector2f(0.0f, fallSpeed * frameTime));

        //check if block passes the grid bottom
       /* if (block.getPosition().y >= gridBottom)
        {
            block.setPosition(startPos);
        }*/

        //clear a screen with a color
        window.clear(sf::Color::Black);

        //window.draw(block);

        window.draw(grid);


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
                    window.draw(cellBlock);
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
