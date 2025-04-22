#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include <optional>

#include "button.hpp"
#include "audio.hpp"

/// A game menu that displays the start and exit buttons
class GameMenu {
  public:
    GameMenu(sf::RenderTarget& render_target) {
        // Create start game button
        auto start_button_shape = std::make_unique<sf::RectangleShape>(
            sf::Vector2f(250.f, 80.f));
        start_button_shape->setFillColor(sf::Color(50, 150, 50));
        start_button_shape->setOrigin(
            start_button_shape->getLocalBounds().getCenter());
        start_button_shape->setPosition(
            sf::Vector2f(render_target.getSize()) / 2.0f - 
            sf::Vector2f(0.f, 60.f));
        start_button_ = std::make_unique<Button>(std::move(start_button_shape));

        // Create exit game button
        auto exit_button_shape = std::make_unique<sf::RectangleShape>(
            sf::Vector2f(250.f, 80.f));
        exit_button_shape->setFillColor(sf::Color(150, 50, 50));
        exit_button_shape->setOrigin(
            exit_button_shape->getLocalBounds().getCenter());
        exit_button_shape->setPosition(
            sf::Vector2f(render_target.getSize()) / 2.0f + 
            sf::Vector2f(0.f, 60.f));
        exit_button_ = std::make_unique<Button>(std::move(exit_button_shape));

        // Try to load font
        if (!font_.openFromFile("C:/Windows/Fonts/arial.ttf")) {
            if (!font_.openFromFile("C:/Windows/Fonts/calibri.ttf")) {
                // If both failed, just continue without text
                return;
            }
        }

        // Initialize texts
        start_text_ = std::make_unique<sf::Text>(font_, "Start Game", 24);
        start_text_->setFillColor(sf::Color::White);
        start_text_->setOrigin(start_text_->getLocalBounds().getCenter());
        start_text_->setPosition(
            sf::Vector2f(render_target.getSize()) / 2.0f - 
            sf::Vector2f(0.f, 60.f));

        exit_text_ = std::make_unique<sf::Text>(font_, "Exit Game", 24);
        exit_text_->setFillColor(sf::Color::White);
        exit_text_->setOrigin(exit_text_->getLocalBounds().getCenter());
        exit_text_->setPosition(
            sf::Vector2f(render_target.getSize()) / 2.0f + 
            sf::Vector2f(0.f, 60.f));

        title_text_ = std::make_unique<sf::Text>(font_, "UNO Game", 48);
        title_text_->setFillColor(sf::Color::Yellow);
        title_text_->setOrigin(title_text_->getLocalBounds().getCenter());
        title_text_->setPosition(
            sf::Vector2f(render_target.getSize().x / 2.0f, 150.f));
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
        
        if (start_text_) {
            start_text_->setPosition(
                window_size / 2.0f - sf::Vector2f(0.f, 60.f));
        }
        
        if (exit_text_) {
            exit_text_->setPosition(
                window_size / 2.0f + sf::Vector2f(0.f, 60.f));
        }
        
        if (title_text_) {
            title_text_->setPosition(
                sf::Vector2f(window_size.x / 2.0f, 150.f));
        }

        // Draw buttons and text
        start_button_->render(window);
        exit_button_->render(window);
        
        if (start_text_) window.draw(*start_text_);
        if (exit_text_) window.draw(*exit_text_);
        if (title_text_) window.draw(*title_text_);
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
    std::unique_ptr<sf::Text> start_text_;
    std::unique_ptr<sf::Text> exit_text_;
    std::unique_ptr<sf::Text> title_text_;
    bool should_start_game_ = false;
    bool should_exit_game_ = false;
}; 