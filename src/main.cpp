#include <SFML/Graphics.hpp>
#include <thread>

#include "state.hpp"

int main() {
    auto window = sf::RenderWindow(sf::VideoMode({1536u, 864u}), "UNO");
    window.setFramerateLimit(144);

    State state(window);

    std::thread thread([&]() {
        while (window.isOpen()) {
            state.update();
        }
    });
    thread.detach();

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

        window.clear();
        state.render(window);
        window.display();
    }

    return 0;
}
