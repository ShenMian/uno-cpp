#include <SFML/Graphics.hpp>
#include <thread>
#include "state.hpp"

void resize_background(sf::Sprite&, sf::Window&);

int main() {
    sf::RenderWindow window(sf::VideoMode({1536u, 864u}), "UNO");
    window.setFramerateLimit(144);

    State state(window);
    Audio::get();

    std::atomic<bool> running{true};
    std::thread update_thread([&]() {
        while (running && window.isOpen()) {
            state.update();
        }
    });

    sf::Texture background_texture("assets/images/background.png");
    sf::Sprite background_sprite(background_texture);
    background_sprite.setOrigin(background_sprite.getLocalBounds().getCenter());
    resize_background(background_sprite, window);

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                running = false;
                window.close();
            }
            if (auto resized = event->getIf<sf::Event::Resized>()) {
                window.setView(sf::View(sf::FloatRect({0.0f, 0.0f}, sf::Vector2f(resized->size))));
                resize_background(background_sprite, window);
            }
        }

        window.clear();
        window.draw(background_sprite);
        state.render(window);
        window.display();
    }

    if (update_thread.joinable()) {
        update_thread.join();
    }
    return 0;
}

/// Scale the background sprite to fit the window size
void resize_background(sf::Sprite& background_sprite, sf::Window& window) {
    const auto window_size = sf::Vector2f(window.getSize());
    background_sprite.setPosition(window_size / 2.0f);
    const auto scale = sf::Vector2f(
        window_size.x / background_sprite.getLocalBounds().size.x,
        window_size.y / background_sprite.getLocalBounds().size.y
    );
    const auto max_scale = std::max(scale.x, scale.y);
    background_sprite.setScale({max_scale, max_scale});
}