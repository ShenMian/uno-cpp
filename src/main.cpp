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
            if (auto resized = event->getIf<sf::Event::Resized>()) {
                window.setView(sf::View(
                    sf::FloatRect({0.0f, 0.0f}, sf::Vector2f(resized->size))
                ));
            }
        }

        static sf::Clock clock;
        if (clock.getElapsedTime().asSeconds() > 1.5f) {
            state.update();
            clock.restart();
        }

        window.clear();
        state.render(window);
        window.display();
    }

    return 0;
}
