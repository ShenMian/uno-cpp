#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "audio.hpp"
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

    void handle_events(sf::RenderWindow& window) {
        if (start_button_->is_left_clicked(window)) {
            Audio::get().play_random_place_sound();
            should_start_game_ = true;
        }
        if (exit_button_->is_left_clicked(window)) {
            Audio::get().play_random_slide_sound();
            should_exit_game_ = true;
            // Note: We don't directly close the window here anymore
            // This allows main.cpp to handle proper cleanup
        }
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

    bool should_start_game() const {
        return should_start_game_;
    }

    bool should_exit_game() const {
        return should_exit_game_;
    }

    void reset() {
        should_start_game_ = false;
        should_exit_game_ = false;
    }

  private:
    std::unique_ptr<Button> start_button_;
    std::unique_ptr<Button> exit_button_;
    sf::Font font_;
    std::unique_ptr<sf::Text> title_text_;
    bool should_start_game_ = false;
    bool should_exit_game_ = false;
};