#include <SFML/Graphics.hpp>
#include <thread>

#include "state.hpp"

int main() {
    auto window = sf::RenderWindow(sf::VideoMode({1536u, 864u}), "UNO");
    window.setFramerateLimit(144);

    State state(window);

    Audio::instance();

    std::thread thread([&]() {
        while (window.isOpen()) {
            state.update();
        }
    });
    thread.detach();

    sf::Texture background_texture("assets/images/background.png");
    sf::Sprite background_sprite(background_texture);
    sf::FloatRect background_bounds = background_sprite.getGlobalBounds();
    background_sprite.setOrigin({
        background_bounds.size.x / 2,
        background_bounds.size.y / 2
    });

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (auto resized = event->getIf<sf::Event::Resized>()) {
                window.setView(
                    sf::View(
                        sf::FloatRect({0.0f, 0.0f}, sf::Vector2f(resized->size))
                    )
                );
            }
        }

        sf::Vector2u window_size = window.getSize();
        float scale_x = static_cast<float>(window_size.x) / background_bounds.size.x;
        float scale_y = static_cast<float>(window_size.y) / background_bounds.size.y;

        float scale = std::max(scale_x, scale_y);

        background_sprite.setScale(sf::Vector2f(scale, scale));

        background_sprite.setPosition({
            static_cast<float>(window_size.x) / 2, 
            static_cast<float>(window_size.y) / 2
        });

        window.clear();
        window.draw(background_sprite);
        state.render(window);
        window.display();
    }

    return 0;
}
