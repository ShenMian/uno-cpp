#pragma once

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <optional>
#include <stdexcept>

// A UNO card.
class Card {
    // Returns the value of the card.
    virtual uint8_t value() const = 0;
    // Returns the index of the card in the atlas (spritesheet).
    virtual uint8_t atlas_index() const = 0;
    // Returns whether the card can be played on another card.
    virtual bool can_play_on(const Card& other) const = 0;
};

enum class Color : uint8_t { Red, Blue, Green, Yellow };

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

enum class Symbol : uint8_t {
    // Numbers.
    Zero,
    One,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    // Actions.
    DrawTwo,
    Reverse,
    Skip,
};

// A number or action UNO card.
class ColoredCard: public Card {
  public:
    ColoredCard(Color color, Symbol symbol) : color_(color), symbol_(symbol) {}

    ColoredCard(Color color, uint8_t number) :
        color_(color),
        symbol_(Symbol(number)) {
        if (number < 0 || number > 9) {
            throw std::invalid_argument("number must be between 0 and 9");
        }
    }

    Color color() const {
        return color_;
    }

    Symbol symbol() const {
        return symbol_;
    }

    uint8_t value() const override {
        switch (symbol_) {
            case Symbol::Zero:
                return 0;
            case Symbol::One:
                return 1;
            case Symbol::Two:
                return 2;
            case Symbol::Three:
                return 3;
            case Symbol::Four:
                return 4;
            case Symbol::Five:
                return 5;
            case Symbol::Six:
                return 6;
            case Symbol::Seven:
                return 7;
            case Symbol::Eight:
                return 8;
            case Symbol::Nine:
                return 9;
            case Symbol::Skip:
            case Symbol::Reverse:
            case Symbol::DrawTwo:
                return 20;
        }
        std::abort(); // Unreachable.
    }

    uint8_t atlas_index() const override {
        return static_cast<uint8_t>(color_) * 13
            + static_cast<uint8_t>(symbol_);
    }

    bool can_play_on(const Card& other) const override {
        if (auto card = dynamic_cast<const ColoredCard*>(&other)) {
            return color_ == card->color() || symbol_ == card->symbol();
        } else if (auto card = dynamic_cast<const WildCard*>(&other)) {
            return color_ == card->color().value();
        }
        std::abort(); // Unreachable.
    }

  private:
    Color color_;
    Symbol symbol_;
};
