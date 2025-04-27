#pragma once

#include <cassert>
#include <chrono>
#include <cstdint>
#include <memory>
#include <random>
#include <vector>

#include "app_state.hpp"
#include "audio.hpp"
#include "card/action_card.hpp"
#include "card/wild_card.hpp"
#include "deck.hpp"
#include "discard_pile.hpp"
#include "player/ai_player.hpp"
#include "player/local_player.hpp"

constexpr std::chrono::duration DRAW_CARD_DELAY =
    std::chrono::milliseconds(700);

enum class Direction : int8_t { Clockwise = 1, CounterClockwise = -1 };

/// A game state of an Uno card game.
class State {
  public:
    State(sf::RenderTarget& render_target) :
        seed_(std::random_device {}()),
        rng_(seed_),
        deck_(rng_) {
        players_.push_back(std::make_unique<AiPlayer>(Position::North, deck_));
        players_.push_back(std::make_unique<AiPlayer>(Position::East, deck_));
        players_.push_back(
            std::make_unique<LocalPlayer>(Position::South, deck_, render_target)
        );
        players_.push_back(std::make_unique<AiPlayer>(Position::West, deck_));

        auto card = deck_.draw().value();
        if (auto wild_card = dynamic_cast<WildCard*>(card.get())) {
            wild_card->set_color(current_player().select_wild_color());
        }
        discard_pile_.push_back(std::move(card));
    }

    AppState update() {
        auto& player = current_player();

        while (!player.has_playable_card(discard_pile_)) {
            player.draw_from_deck(deck_);
            Audio::get().play_random_slide_sound();
            std::this_thread::sleep_for(DRAW_CARD_DELAY);
        }

        auto card = player.play_card(discard_pile_);
        Audio::get().play_random_place_sound();

        if (player.is_hand_empty()) {
            return AppState::GameOver;
        }

        if (auto wild_card = dynamic_cast<WildCard*>(card.get())) {
            wild_card->set_color(player.select_wild_color());
            if (wild_card->symbol() == WildSymbol::WildDrawFour) {
                next_turn();
                auto& next_player = current_player();
                for (uint8_t i = 0; i < 4; i += 1) {
                    std::this_thread::sleep_for(DRAW_CARD_DELAY);
                    next_player.draw_from_deck(deck_);
                    Audio::get().play_random_slide_sound();
                }
            }
        }
        if (auto action_card = dynamic_cast<const ActionCard*>(card.get())) {
            switch (action_card->symbol()) {
                case ActionSymbol::DrawTwo: {
                    next_turn();
                    auto& next_player = current_player();
                    for (uint8_t i = 0; i < 2; i += 1) {
                        std::this_thread::sleep_for(DRAW_CARD_DELAY);
                        next_player.draw_from_deck(deck_);
                        Audio::get().play_random_slide_sound();
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

        assert(card->can_play_on(discard_pile_.peek_top()));
        discard_pile_.push_back(std::move(card));

        return AppState::Gameplay;
    }

    void render(sf::RenderWindow& window) const {
        deck_.render(window);
        discard_pile_.render(window);
        for (size_t i = 0; i < players_.size(); i += 1) {
            players_[i]->render(
                window,
                discard_pile_,
                i == static_cast<uint8_t>(position_)
            );
        }
        // TODO: Add direction indicators
        render_player_indicator(window);
    }

    Position position() const {
        return position_;
    }

  private:
    void render_player_indicator(sf::RenderTarget& render_target) const {
        const auto player_index = static_cast<int8_t>(position_);
        sf::CircleShape indicator(40.f, 3);
        indicator.setOrigin(
            indicator.getLocalBounds().getCenter()
            + sf::Vector2f(0.f, player_index % 2 == 0 ? 200.f : 300.f)
        );
        indicator.setPosition(sf::Vector2f(render_target.getSize()) / 2.0f);
        indicator.setRotation(
            sf::degrees(90.f) * static_cast<float>(player_index)
        );
        render_target.draw(indicator);
    }

    Player& current_player() {
        return *players_[static_cast<uint8_t>(position_)].get();
    }

    void next_turn() {
        position_ = static_cast<Position>(
            (static_cast<uint8_t>(position_) + static_cast<int8_t>(direction_))
            % players_.size()
        );
    }

    void reverse_direction() {
        direction_ = static_cast<Direction>(-static_cast<int8_t>(direction_));
    }

    unsigned int seed_;
    std::mt19937 rng_;

    Deck deck_;
    DiscardPile discard_pile_;

    std::vector<std::unique_ptr<Player>> players_;

    Position position_ = Position::South; // Position of the current player
    Direction direction_ = Direction::Clockwise; // Direction of play
};
