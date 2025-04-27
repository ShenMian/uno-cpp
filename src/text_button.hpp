#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

#include "button.hpp"

class TextButton: public Button {
  public:
    TextButton(sf::Text&& text, sf::Color text_color, sf::Color button_color) :
        Button(create_shape(button_color)),
        text_(text) {
        text_.setFillColor(text_color);
        text_.setOrigin(text_.getLocalBounds().getCenter());
        text_.setPosition(shape().getGlobalBounds().getCenter());
    }

    virtual void render(sf::RenderWindow& window) const override {
        Button::render(window);
        window.draw(text_);
    }

    virtual void set_position(sf::Vector2f position) override {
        Button::set_position(position);
        text_.setPosition(shape().getGlobalBounds().getCenter());
    }

    void set_text(const std::string& text) {
        text_.setString(text);
        text_.setOrigin(text_.getLocalBounds().getCenter());
        text_.setPosition(shape().getGlobalBounds().getCenter());
    }

    void set_text_color(const sf::Color& color) {
        text_.setFillColor(color);
    }

  private:
    std::unique_ptr<sf::Shape> create_shape(sf::Color button_color) const {
        auto shape =
            std::make_unique<sf::RectangleShape>(sf::Vector2f(200.f, 80.f));
        shape->setFillColor(button_color);
        shape->setOrigin(shape->getLocalBounds().getCenter());
        return shape;
    }

    sf::Text text_;
};