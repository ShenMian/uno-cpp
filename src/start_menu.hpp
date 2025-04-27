#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "app_state.hpp"
#include "button.hpp"
#include "text_button.hpp"

/// A start menu that displays the start and exit buttons
class StartMenu {
  public:
    StartMenu(const sf::RenderTarget& render_target) :
        font_("assets/fonts/arial.ttf") {
        constexpr unsigned int title_font_size = 72;
        constexpr unsigned int option_font_size = 48;

        // Create title text
        title_text_ = std::make_unique<sf::Text>(font_, "UNO", title_font_size);
        title_text_->setFillColor(sf::Color::Yellow);
        title_text_->setOrigin(title_text_->getLocalBounds().getCenter());
        title_text_->setPosition(
            sf::Vector2f(render_target.getSize().x / 2.0f, 150.f)
        );

        // Create start button
        start_button_ = std::make_unique<TextButton>(
            sf::Text(font_, "Start", option_font_size),
            sf::Color::White,
            sf::Color(50, 150, 50)
        );
        start_button_->set_position(
            sf::Vector2f(render_target.getSize()) / 2.0f
            - sf::Vector2f(0.f, 60.f)
        );

        // Create exit button
        exit_button_ = std::make_unique<TextButton>(
            sf::Text(font_, "Exit", option_font_size),
            sf::Color::White,
            sf::Color(150, 50, 50)
        );
    }

    AppState update(sf::RenderWindow& window) {
        if (start_button_->is_left_clicked(window)) {
            return AppState::Gameplay;
        }
        if (exit_button_->is_left_clicked(window)) {
            return AppState::Exit;
        }
        return AppState::StartMenu;
    }

    void render(sf::RenderWindow& window) {
        // Resize text based on window size
        const auto window_size = sf::Vector2f(window.getSize());

        title_text_->setPosition(sf::Vector2f(window_size.x / 2.0f, 150.f));
        start_button_->set_position(
            window_size / 2.0f - sf::Vector2f(0.f, 60.f)
        );
        exit_button_->set_position(
            window_size / 2.0f + sf::Vector2f(0.f, 60.f)
        );

        window.draw(*title_text_);
        start_button_->render(window);
        exit_button_->render(window);
    }

  private:
    std::unique_ptr<Button> start_button_;
    std::unique_ptr<Button> exit_button_;
    sf::Font font_;
    std::unique_ptr<sf::Text> title_text_;
};