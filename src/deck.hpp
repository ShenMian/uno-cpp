#pragma once

#include <algorithm>
#include <cassert>
#include <memory>
#include <optional>
#include <random>
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
            cards_.push_back(std::make_unique<ColoredCard>(color, Symbol::Zero)
            );
            for (const auto symbol :
                 {Symbol::One,
                  Symbol::Two,
                  Symbol::Three,
                  Symbol::Four,
                  Symbol::Five,
                  Symbol::Six,
                  Symbol::Seven,
                  Symbol::Eight,
                  Symbol::Nine,
                  Symbol::DrawTwo,
                  Symbol::Reverse,
                  Symbol::Skip}) {
                for (int i = 0; i < 2; i++) {
                    cards_.push_back(
                        std::make_unique<ColoredCard>(color, symbol)
                    );
                }
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
    void shuffle() {
        std::ranges::shuffle(cards_, std::default_random_engine {});
    }

    // Draws a card from the deck.
    std::optional<std::unique_ptr<Card>> draw() {
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