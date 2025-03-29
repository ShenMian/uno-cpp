#pragma once

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cassert>
#include <memory>
#include <optional>
#include <random>
#include <vector>

#include "card/action_card.hpp"
#include "card/number_card.hpp"
#include "card/wild_card.hpp"

using std::optional;
using std::unique_ptr;
using std::vector;

/// A deck of UNO cards.
class Deck {
  public:
    /// Constructs a deck with all 108 UNO cards.
    Deck(std::mt19937 rng) : rng_(rng) {
        cards_.reserve(108);
        initialize_cards();
    }

    /// Draws a card from the deck.
    optional<unique_ptr<Card>> draw() noexcept {
        if (cards_.empty()) {
            initialize_cards();
        }
        auto card = std::move(cards_.back());
        cards_.pop_back();
        return card;
    }

    void render(sf::RenderTarget& render_target) const {
        auto sprite = Card::back_sprite();
        sprite.setPosition(
            sf::Vector2f(render_target.getSize()) / 2.0f
            - sf::Vector2f(170.0f, 0.0f)
        );
        render_target.draw(sprite);
    }

  private:
    void initialize_cards() {
        // UNO includes 108 cards: 25 in each of four color suits (red, yellow,
        // green, blue), each suit consisting of one zero, two each of 1
        // through 9, and two each of the action cards "Skip", "Draw Two", and
        // "Reverse". The deck also contains four "Wild" cards and four "Wild
        // Draw Four".
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
            cards_.push_back(
                std::make_unique<WildCard>(WildSymbol::WildDrawFour)
            );
        }
        shuffle();
    }

    /// Shuffles the deck.
    void shuffle() {
        std::ranges::shuffle(cards_, rng_);
    }

    vector<unique_ptr<Card>> cards_;
    std::mt19937 rng_;
};
