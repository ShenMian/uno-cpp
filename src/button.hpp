#include <SFML/Graphics.hpp>
#include <memory>

class Button {
  public:
    Button(std::unique_ptr<sf::Shape> shape) : shape(std::move(shape)) {}

    bool is_hovered(sf::RenderWindow& window) const {
        return shape->getGlobalBounds().contains(
            window.mapPixelToCoords(sf::Mouse::getPosition(window))
        );
    }

    bool is_clicked(sf::RenderWindow& window) const {
        return is_hovered(window)
            && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    }

    void render(sf::RenderWindow& window) const {
        if (is_hovered(window)) {
            shape->setOutlineThickness(4.0f);
            shape->setOutlineColor(sf::Color::Yellow);
        } else {
            shape->setOutlineThickness(4.0f);
            shape->setOutlineColor(sf::Color::Black);
        }
        window.draw(*shape);
    }

  private:
    std::unique_ptr<sf::Shape> shape;
};
