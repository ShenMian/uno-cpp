#pragma once

#include <cstdint>
#include <thread>

#include "player.hpp"

constexpr std::chrono::duration THINKING_DELAY =
    std::chrono::milliseconds(1500);

/// An AI-controlled player for the UNO game.
class AiPlayer: public Player {
  public:
    AiPlayer(Position position, Deck& deck) : Player(position, deck) {}

    optional<unique_ptr<Card>>
    play_card(const DiscardPile& discard_pile) override {
        for (auto it = cards_.begin(); it != cards_.end(); ++it) {
            if ((*it)->can_play_on(discard_pile.peek_top())) {
                auto card = std::move(*it);
                cards_.erase(it);
                std::this_thread::sleep_for(THINKING_DELAY);
                return card;
            }
        }
        return std::nullopt;
    }

    Color select_wild_color() const override {
        // Choose the most common color in the player's hand.
        std::array<uint8_t, 4> color_counts = {0, 0, 0, 0};
        for (const auto& card : cards_) {
            if (auto number_card = dynamic_cast<NumberCard*>(card.get())) {
                color_counts[static_cast<uint8_t>(number_card->color())] += 1;
            }
            if (auto action_card = dynamic_cast<ActionCard*>(card.get())) {
                color_counts[static_cast<uint8_t>(action_card->color())] += 1;
            }
        }
        return static_cast<Color>(std::distance(
            color_counts.begin(),
            std::max_element(color_counts.begin(), color_counts.end())
        ));
    }
};
