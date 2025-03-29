#pragma once

#include "card.hpp"

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