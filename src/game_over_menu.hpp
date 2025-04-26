#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "app_state.hpp"
#include "text_button.hpp"

/// A game over screen that displays the result and offers options to continue
class GameOverMenu {
  public:
    GameOverMenu(const sf::RenderTarget& render_target, bool player_won) :
        font_("assets/fonts/arial.ttf") {
        constexpr unsigned int title_font_size = 72;
        constexpr unsigned int option_font_size = 48;

        // Create title text
        title_text_ = std::make_unique<sf::Text>(
            font_,
            player_won ? "You Win!" : "You Lose! Try again",
            title_font_size
        );
        title_text_->setFillColor(sf::Color::Yellow);
        title_text_->setOrigin(title_text_->getLocalBounds().getCenter());
        title_text_->setPosition(
            sf::Vector2f(render_target.getSize().x / 2.0f, 150.f)
        );

        // Create menu button
        menu_button_ = std::make_unique<TextButton>(
            sf::Text(font_, "Back to Menu", option_font_size),
            sf::Color::White,
            sf::Color(50, 150, 50)
        );

        // Create exit button
        exit_button_ = std::make_unique<TextButton>(
            sf::Text(font_, "Exit", option_font_size),
            sf::Color::White,
            sf::Color(150, 50, 50)
        );
    }

    AppState update(sf::RenderWindow& window) {
        if (menu_button_->is_hovered(window)) {
            return AppState::StartMenu;
        } else if (exit_button_->is_hovered(window)) {
            return AppState::Exit;
        }
        return AppState::GameOver;
    }

    void render(sf::RenderWindow& window) {
        const auto window_size = sf::Vector2f(window.getSize());

        title_text_->setPosition(sf::Vector2f(window_size.x / 2.0f, 150.f));
        menu_button_->set_position(
            window_size / 2.0f - sf::Vector2f(0.f, 60.f)
        );
        exit_button_->set_position(
            window_size / 2.0f + sf::Vector2f(0.f, 60.f)
        );

        window.draw(*title_text_);
        menu_button_->render(window);
        exit_button_->render(window);
    }

  private:
    std::unique_ptr<sf::Text> title_text_;
    std::unique_ptr<TextButton> menu_button_;
    std::unique_ptr<TextButton> exit_button_;
    sf::Font font_;
};