#include <SFML/Graphics.hpp>
#include <atomic>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <thread>

#include "app_state.hpp"
#include "game_over_menu.hpp"
#include "player/player.hpp"
#include "start_menu.hpp"
#include "state.hpp"

void on_enter(AppState, sf::RenderWindow&);
void on_exit(AppState, sf::RenderWindow&);
void resize_background(sf::Sprite&, sf::Window&);

std::unique_ptr<StartMenu> start_menu;

std::unique_ptr<GameOverMenu> game_over_menu;

std::unique_ptr<State> state;
std::unique_ptr<std::thread> gameplay_thread;

std::atomic<AppState> app_state = AppState::StartMenu;
std::atomic<AppState> previous_app_state = AppState::None;
bool is_player_won;

int main() {
    auto window = sf::RenderWindow(sf::VideoMode({1536u, 864u}), "UNO");
    window.setFramerateLimit(144);

    sf::Texture background_texture("assets/images/background.png");
    sf::Sprite background_sprite(background_texture);
    background_sprite.setOrigin(background_sprite.getLocalBounds().getCenter());
    resize_background(background_sprite, window);

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                app_state = AppState::Exit;
            }
            if (auto resized = event->getIf<sf::Event::Resized>()) {
                window.setView(sf::View(
                    sf::FloatRect({0.0f, 0.0f}, sf::Vector2f(resized->size))
                ));
                resize_background(background_sprite, window);
            }
        }

        const auto previous_state =
            previous_app_state.exchange(app_state.load());
        if (previous_state != app_state) {
            on_exit(previous_state, window);
            on_enter(app_state, window);
        }

        window.clear();
        window.draw(background_sprite);

        switch (app_state) {
            case AppState::StartMenu:
                app_state = start_menu->update(window);
                start_menu->render(window);
                break;

            case AppState::Gameplay:
                state->render(window);
                break;

            case AppState::GameOver:
                app_state = game_over_menu->update(window);
                game_over_menu->render(window);
                break;

            case AppState::Exit:
                window.close();
                break;

            default:
                assert(false); // Unreachable.
                break;
        }

        window.display();
    }

    return 0;
}

void on_enter(AppState app_state_entered, sf::RenderWindow& window) {
    switch (app_state_entered) {
        case AppState::StartMenu:
            assert(start_menu == nullptr);
            start_menu = std::make_unique<StartMenu>(window);
            break;
        case AppState::Gameplay:
            assert(state == nullptr);
            assert(gameplay_thread == nullptr);
            state = std::make_unique<State>(window);
            gameplay_thread = std::make_unique<std::thread>([&]() {
                while (true) {
                    app_state = state->update();
                }
            });
            break;
        case AppState::GameOver:
            assert(game_over_menu == nullptr);
            game_over_menu =
                std::make_unique<GameOverMenu>(window, is_player_won);
            break;
        default:
            break;
    }
}

void on_exit(AppState app_state_exited, sf::RenderWindow& window) {
    switch (app_state_exited) {
        case AppState::StartMenu:
            start_menu.reset();
            break;
        case AppState::Gameplay:
            is_player_won = state->position() == Position::South;
            state.reset();
            gameplay_thread.reset();
            break;
        case AppState::GameOver:
            game_over_menu.reset();
            break;
        default:
            break;
    }
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
