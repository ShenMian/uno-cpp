#include <SFML/Graphics.hpp>
#include <atomic>
#include <memory>
#include <thread>

#include "app_state.hpp"
#include "game_over_menu.hpp"
#include "player/player.hpp"
#include "start_menu.hpp"
#include "state.hpp"

void resize_background(sf::Sprite&, sf::Window&);

int main() {
    auto window = sf::RenderWindow(sf::VideoMode({1536u, 864u}), "UNO");
    window.setFramerateLimit(144);

    std::atomic<AppState> app_state = AppState::StartMenu;

    std::unique_ptr<StartMenu> start_menu = std::make_unique<StartMenu>(window);

    std::unique_ptr<GameOverMenu> game_over_menu;

    std::unique_ptr<State> state;
    std::unique_ptr<std::thread> gameplay_thread;

    sf::Texture background_texture("assets/images/background.png");
    sf::Sprite background_sprite(background_texture);
    background_sprite.setOrigin(background_sprite.getLocalBounds().getCenter());
    resize_background(background_sprite, window);

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
                resize_background(background_sprite, window);
            }
        }

        window.clear();
        window.draw(background_sprite);

        switch (app_state) {
            case AppState::StartMenu:
                app_state = start_menu->update(window);
                start_menu->render(window);

                if (app_state != AppState::StartMenu) {
                    start_menu.reset();
                }
                break;

            case AppState::Gameplay:
                if (state == nullptr) {
                    state = std::make_unique<State>(window);
                }
                if (gameplay_thread == nullptr) {
                    gameplay_thread = std::make_unique<std::thread>([&]() {
                        while (app_state == AppState::Gameplay) {
                            app_state = state->update();
                        }
                    });
                    // gameplay_thread->detach();
                }

                state->render(window);

                if (app_state != AppState::Gameplay) {
                    // Used when initializing the `GameOver` state
                    // state.reset();
                    gameplay_thread.reset();
                }
                break;

            case AppState::GameOver:
                if (game_over_menu == nullptr) {
                    game_over_menu = std::make_unique<GameOverMenu>(
                        window,
                        state->position() == Position::South
                    );
                }

                app_state = game_over_menu->update(window);
                game_over_menu->render(window);

                if (app_state != AppState::GameOver) {
                    game_over_menu.reset();
                }
                break;

            case AppState::Exit:
                window.close();
                break;
        }

        window.display();
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
