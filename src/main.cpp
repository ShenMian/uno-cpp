#include <SFML/Graphics.hpp>

#include "state.hpp"

int main() {
    auto window = sf::RenderWindow(sf::VideoMode({1536u, 864u}), "UNO");
    window.setFramerateLimit(144);

    State state;

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear();
        state.render(window);
        window.display();
    }
}
