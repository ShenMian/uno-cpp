#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

class Button {
  public:
    Button(std::unique_ptr<sf::Shape> shape) : shape_(std::move(shape)) {}

    bool is_hovered(sf::RenderWindow& window) const {
        return shape_->getGlobalBounds().contains(
            window.mapPixelToCoords(sf::Mouse::getPosition(window))
        );
    }

    bool is_left_clicked(sf::RenderWindow& window) const {
        return is_hovered(window)
            && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    }

    virtual void set_position(sf::Vector2f position) {
        shape_->setPosition(position);
    }

    const sf::Shape& shape() const {
        return *shape_;
    }

    virtual void render(sf::RenderWindow& window) const {
        constexpr float HIGHLIGHT_THICKNESS = 4.0f;

        if (is_hovered(window)) {
            shape_->setOutlineThickness(HIGHLIGHT_THICKNESS);
            shape_->setOutlineColor(sf::Color::Yellow);
        } else {
            shape_->setOutlineThickness(HIGHLIGHT_THICKNESS);
            shape_->setOutlineColor(sf::Color::Black);
        }
        window.draw(*shape_);
    }

  private:
    std::unique_ptr<sf::Shape> shape_;
};
