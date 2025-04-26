#include <SFML/Graphics.hpp>
#include <atomic>
#include <memory>
#include <thread>

#include "start_menu.hpp"
#include "state.hpp"

void resize_background(sf::Sprite&, sf::Window&);

enum class GameState { Menu, Playing };

int main() {
    auto window = sf::RenderWindow(sf::VideoMode({1536u, 864u}), "UNO");
    window.setFramerateLimit(144);

    // Initialize game state
    GameState current_state = GameState::Menu;
    StartMenu menu(window);
    std::unique_ptr<State> game_state;
    std::unique_ptr<std::thread> game_thread;
    std::atomic<bool> game_running {false}; // Flag to control game thread

    Audio::get();

    sf::Texture background_texture("assets/images/background.png");
    sf::Sprite background_sprite(background_texture);
    background_sprite.setOrigin(background_sprite.getLocalBounds().getCenter());
    resize_background(background_sprite, window);

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                // Set flag to stop game thread when window is closed
                game_running = false;
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

            // Handle menu events
            if (current_state == GameState::Menu) {
                menu.handle_events(window);

                if (menu.should_start_game()) {
                    // Switch to game state
                    current_state = GameState::Playing;
                    game_state = std::make_unique<State>(window);

                    // Set flag and start game thread
                    game_running = true;
                    game_thread = std::make_unique<std::thread>([&]() {
                        while (window.isOpen() && game_running
                               && current_state == GameState::Playing) {
                            game_state->update();
                        }
                    });
                    game_thread->detach();

                    menu.reset();
                } else if (menu.should_exit_game()) {
                    // Exit game if exit button was clicked in menu
                    game_running = false;
                    window.close();
                }
            }
        }

        // Exit check after handling events
        if (!window.isOpen()) {
            break;
        }

        window.clear();
        window.draw(background_sprite);

        // Render based on current state
        if (current_state == GameState::Menu) {
            menu.render(window);
        } else if (current_state == GameState::Playing && game_state) {
            game_state->render(window);
        }

        window.display();
    }

    // Ensure game thread stops when application exits
    game_running = false;
    if (game_thread && game_thread->joinable()) {
        game_thread->join();
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
