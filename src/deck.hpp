#pragma once

#include <algorithm>
#include <cassert>
#include <memory>
#include <optional>
#include <vector>

#include "card.hpp"

// A deck of UNO cards.
class Deck {
  public:
    // Constructs a deck with all 108 UNO cards.
    Deck() {
        cards_.reserve(108);
        for (const auto color :
             {Color::Red, Color::Blue, Color::Green, Color::Yellow}) {
            cards_.push_back(std::make_unique<NumberCard>(color, 0));
            for (uint8_t number = 1; number <= 9; number++) {
                cards_.push_back(std::make_unique<NumberCard>(color, number));
                cards_.push_back(std::make_unique<NumberCard>(color, number));
            }
            for (const auto symbol :
                 {ActionSymbol::DrawTwo,
                  ActionSymbol::Reverse,
                  ActionSymbol::Skip}) {
                cards_.push_back(std::make_unique<ActionCard>(color, symbol));
                cards_.push_back(std::make_unique<ActionCard>(color, symbol));
            }
        }
        for (int i = 0; i < 4; i++) {
            cards_.push_back(std::make_unique<WildCard>(WildSymbol::Wild));
            cards_.push_back(std::make_unique<WildCard>(WildSymbol::WildDrawFour
            ));
        }
        assert(cards_.size() == 108);
    }

    // Shuffles the deck.
    template<typename Rng>
    void shuffle(Rng& rng) {
        std::ranges::shuffle(cards_, rng);
    }

    // Draws a card from the deck.
    std::optional<std::unique_ptr<Card>> draw() noexcept {
        if (cards_.empty()) {
            return std::nullopt;
        }
        auto card = std::move(cards_.back());
        cards_.pop_back();
        return card;
    }

  private:
    std::vector<std::unique_ptr<Card>> cards_;
};
