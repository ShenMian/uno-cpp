#pragma once

#include <cassert>
#include <cstdint>
#include <optional>
#include <stdexcept>

// A UNO card.
class Card {
  public:
    virtual ~Card() = default;
    // Returns the value of the card.
    virtual uint8_t value() const = 0;
    // Returns the index of the card in the atlas (spritesheet).
    virtual uint8_t atlas_index() const = 0;
    // Returns whether the card can be played on another card.
    virtual bool can_play_on(const Card& other) const = 0;

    auto operator<=>(const Card& rhs) const {
        return atlas_index() <=> rhs.atlas_index();
    }
};

enum class Color : uint8_t { Red, Blue, Green, Yellow };

// A number UNO card.
class NumberCard: public Card {
  public:
    NumberCard(Color color, uint8_t number) : color_(color), number_(number) {
        if (number < 0 || number > 9) {
            throw std::invalid_argument("number must be between 0 and 9");
        }
    }

    Color color() const {
        return color_;
    }

    uint8_t number() const {
        return number_;
    }

    uint8_t value() const override {
        return number_;
    }

    uint8_t atlas_index() const override {
        return static_cast<uint8_t>(color_) * 13
            + static_cast<uint8_t>(number_);
    }

    bool can_play_on(const Card& other) const override;

  private:
    Color color_;
    uint8_t number_;
};

enum class ActionSymbol : uint8_t {
    DrawTwo,
    Reverse,
    Skip,
};

// An action UNO card.
class ActionCard: public Card {
  public:
    ActionCard(Color color, ActionSymbol symbol) :
        color_(color),
        symbol_(symbol) {}

    Color color() const {
        return color_;
    }

    ActionSymbol symbol() const {
        return symbol_;
    }

    uint8_t value() const override {
        return 20;
    }

    uint8_t atlas_index() const override {
        return 4 * 13 + static_cast<uint8_t>(symbol_);
    }

    bool can_play_on(const Card& other) const override;

  private:
    Color color_;
    ActionSymbol symbol_;
};

enum class WildSymbol : uint8_t { Wild, WildDrawFour };

// A wild UNO card.
class WildCard: public Card {
  public:
    WildCard(WildSymbol symbol) : symbol_(symbol) {}

    std::optional<Color> color() const {
        return color_;
    }

    void set_color(Color color) {
        color_ = color;
    }

    WildSymbol symbol() const {
        return symbol_;
    }

    uint8_t value() const override {
        return 50;
    }

    uint8_t atlas_index() const override {
        return 4 * 13 + static_cast<uint8_t>(symbol_);
    }

    bool can_play_on(const Card&) const override {
        return true;
    }

  private:
    std::optional<Color> color_;
    WildSymbol symbol_;
};
