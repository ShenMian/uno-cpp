#pragma once

#include "card.hpp"

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
