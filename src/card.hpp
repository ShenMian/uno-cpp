#pragma once

#include <SFML/Graphics.hpp>
#include <cassert>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <vector>

using std::optional;

/// A UNO card.
class Card {
  public:
    virtual ~Card() = default;
    /// Returns the value of the card.
    virtual uint8_t value() const noexcept = 0;
    /// Returns the index of the card in the atlas (spritesheet).
    virtual uint8_t atlas_index() const noexcept = 0;
    /// Returns whether the card can be played on another card.
    virtual bool can_play_on(const Card& other) const noexcept = 0;

    /// Returns a sprite representing the card.
    sf::Sprite sprite() const;

    auto operator<=>(const Card& rhs) const noexcept {
        return atlas_index() <=> rhs.atlas_index();
    }

    /// Returns a sprite representing the back of the card.
    static sf::Sprite back_sprite();

  private:
    static sf::Texture spritesheet_;
    static std::vector<sf::Sprite> sprites_;
};

enum class Color : uint8_t { Red, Blue, Green, Yellow };

/// A number UNO card.
class NumberCard: public Card {
  public:
    NumberCard(Color color, uint8_t number) : color_(color), number_(number) {
        if (number < 0 || number > 9) {
            throw std::invalid_argument("number must be between 0 and 9");
        }
    }

    Color color() const noexcept {
        return color_;
    }

    uint8_t number() const noexcept {
        return number_;
    }

    uint8_t value() const noexcept override {
        return number_;
    }

    uint8_t atlas_index() const noexcept override {
        return static_cast<uint8_t>(color_) * 13
            + static_cast<uint8_t>(number_);
    }

    bool can_play_on(const Card& other) const noexcept override;

  private:
    Color color_;
    uint8_t number_;
};

enum class ActionSymbol : uint8_t {
    DrawTwo,
    Reverse,
    Skip,
};

/// An action UNO card.
class ActionCard: public Card {
  public:
    ActionCard(Color color, ActionSymbol symbol) :
        color_(color),
        symbol_(symbol) {}

    Color color() const noexcept {
        return color_;
    }

    ActionSymbol symbol() const noexcept {
        return symbol_;
    }

    uint8_t value() const noexcept override {
        return 20;
    }

    uint8_t atlas_index() const noexcept override {
        return static_cast<uint8_t>(color_) * 13 + 10
            + static_cast<uint8_t>(symbol_);
    }

    bool can_play_on(const Card& other) const noexcept override;

  private:
    Color color_;
    ActionSymbol symbol_;
};

enum class WildSymbol : uint8_t { Wild, WildDrawFour };

/// A wild UNO card.
class WildCard: public Card {
  public:
    WildCard(WildSymbol symbol) : symbol_(symbol) {}

    optional<Color> color() const noexcept {
        return color_;
    }

    void set_color(Color color) noexcept {
        color_ = color;
    }

    WildSymbol symbol() const noexcept {
        return symbol_;
    }

    uint8_t value() const noexcept override {
        return 50;
    }

    uint8_t atlas_index() const noexcept override {
        if (!color_.has_value()) {
            return 4 * 13 + static_cast<uint8_t>(symbol_);
        }
        return 56 + static_cast<uint8_t>(symbol_) * 4
            + static_cast<uint8_t>(color_.value());
    }

    bool can_play_on(const Card&) const noexcept override {
        return true;
    }

  private:
    optional<Color> color_;
    WildSymbol symbol_;
};
