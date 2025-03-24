#include <cassert>
#include <cstdint>
#include <memory>
#include <random>
#include <vector>

#include "card.hpp"
#include "deck.hpp"
#include "discard_pile.hpp"
#include "player.hpp"

enum class Direction : int8_t { Clockwise = 1, CounterClockwise = -1 };

/// A game state of an Uno card game.
class State {
  public:
    State() : seed_(std::random_device {}()), rng_(seed_), deck_(rng_) {
        for (const auto position :
             {Position::South,
              Position::West,
              Position::North,
              Position::East}) {
            players_.push_back(std::make_unique<AiPlayer>(position, deck_));
        }

        auto card = deck_.draw().value();
        if (auto wild_card = dynamic_cast<WildCard*>(card.get())) {
            wild_card->set_color(current_player().select_wild_color());
        }
        discard_pile_.push_back(std::move(card));
    }

    void update() {
        auto& player = current_player();

        auto card = player.play_card(discard_pile_);
        while (!card.has_value()) {
            player.draw_card_from_deck(deck_);
            card = player.play_card(discard_pile_);
        }
        if (player.is_hand_empty()) {
            assert(false); // TODO
        }
        if (auto wild_card = dynamic_cast<WildCard*>(card.value().get())) {
            wild_card->set_color(player.select_wild_color());
            if (wild_card->symbol() == WildSymbol::WildDrawFour) {
                next_turn();
                auto& next_player = current_player();
                for (uint8_t i = 0; i < 4; i += 1) {
                    next_player.draw_card_from_deck(deck_);
                }
            }
        }
        if (auto action_card =
                dynamic_cast<const ActionCard*>(card.value().get())) {
            switch (action_card->symbol()) {
                case ActionSymbol::DrawTwo: {
                    next_turn();
                    auto& next_player = current_player();
                    for (uint8_t i = 0; i < 2; i += 1) {
                        next_player.draw_card_from_deck(deck_);
                    }
                    break;
                }
                case ActionSymbol::Reverse:
                    reverse_direction();
                    break;
                case ActionSymbol::Skip:
                    next_turn();
                    break;
            }
        }
        next_turn();
        assert(card.value()->can_play_on(discard_pile_.peek_top()));
        discard_pile_.push_back(std::move(card.value()));
    }

    void reverse_direction() {
        direction_ = static_cast<Direction>(-static_cast<int8_t>(direction_));
    }

    void render(sf::RenderTarget& render_target) const {
        deck_.render(render_target);
        discard_pile_.render(render_target);
        for (const auto& player : players_) {
            player->render_hand(render_target, discard_pile_);
        }
    }

  private:
    Player& current_player() {
        return *players_[static_cast<uint8_t>(position_)].get();
    }

    void next_turn() {
        position_ = static_cast<Position>(
            (static_cast<uint8_t>(position_) + static_cast<int8_t>(direction_))
            % players_.size()
        );
    }

    unsigned int seed_;
    std::mt19937 rng_;

    Deck deck_;
    DiscardPile discard_pile_;

    std::vector<std::unique_ptr<Player>> players_;

    Position position_ = Position::South; // Position of the current player
    Direction direction_ = Direction::Clockwise; // Direction of play
};
