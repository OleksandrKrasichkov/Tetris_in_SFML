#include <iostream>
#include <SFML/Graphics.hpp>

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
    for (int i = 0; i <= 10; i++)
    {
        float x = startX + (i * cellSize);
        grid.append(sf::Vertex({ x, startY }, gridColor));
        grid.append(sf::Vertex({ x, startY + gridHeight }, gridColor));
    }

    for (int i = 0; i <= 20; i++)
    {
        float y = startY + (i * cellSize);
        grid.append(sf::Vertex({ startX, y }, gridColor));
        grid.append(sf::Vertex({ startX + gridWidth, y }, gridColor));
    }






    //bottom of the grid
    const float gridBottom = startY + gridHeight;

    //create start postition where blocks spawn
    sf::Vector2f startPos = sf::Vector2f((startX + cellSize * 4 + cellSize / 2), startY);

    //create a shape
    sf::RectangleShape block(sf::Vector2f(cellSize, 3*cellSize));
    //setting its center
    block.setOrigin(sf::Vector2f(cellSize / 2.0f, 1.5*cellSize));
    block.setFillColor(sf::Color::Cyan);
    block.setPosition(startPos);

    //tracks time between frames
    sf::Clock clock;

    //speed a block per second
    float fallSpeed = 5 * cellSize;

    while (window.isOpen())
    {
        //get time one frame spent
        float frameTime = clock.restart().asSeconds();

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
                block.rotate(sf::degrees(90.0f));
            }
        }

        //move block
        block.move(sf::Vector2f(0.0f, fallSpeed * frameTime));

        //check if block passes the grid bottom
        if (block.getPosition().y >= gridBottom)
        {
            block.setPosition(startPos);
        }

        //clear a screen with a color
        window.clear(sf::Color::Red);

        window.draw(block);

        window.draw(grid);
        //displays everything you did, without it there is no change
        window.display();
    }

    return 0;
}