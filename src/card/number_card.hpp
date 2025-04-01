#pragma once

#include "card.hpp"

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