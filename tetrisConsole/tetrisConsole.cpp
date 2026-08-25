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

    //set updates per second
    window.setFramerateLimit(60);

    //create a shape
    sf::RectangleShape block(sf::Vector2f(100.0f, 100.0f));
    //setting its center
    block.setOrigin(sf::Vector2f(50.0f, 50.0f));
    block.setFillColor(sf::Color::Cyan);
    block.setPosition(sf::Vector2f(width / 2.0f, height / 2.0f));

    while (window.isOpen())
    {
        //optional - smth(true) or nothing(false). PollEvent asks for an event from a queue of events windows created
        while (std::optional<sf::Event> event = window.pollEvent())
        {
            //check if that event is close(X) or esc is pressed
            if ( ((*event).is<sf::Event::Closed>() ) || ((*event).is<sf::Event::KeyReleased>() && (*((*event).getIf<sf::Event::KeyReleased>())).code == sf::Keyboard::Key::Escape)) 
            {
                window.close();
            }
        }
        //clear a screen with a color
        window.clear(sf::Color::Red);

        window.draw(block);
        //displays everything you did, without it there is no change
        window.display();
    }

    return 0;
}