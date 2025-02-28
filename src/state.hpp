#include <random>
#include <vector>

#include "deck.hpp"
#include "discard_pile.hpp"
#include "player.hpp"

enum class Direction { Clockwise = 1, CounterClockwise = -1 };

class State {
  public:
    State() : seed_(std::random_device {}()), rng_(seed_) {
        deck_.shuffle(rng_);

        for (const auto position :
             {Position::North, Position::East, Position::South, Position::West
             }) {
            players_.emplace_back(position, deck_);
        }

        // FIXME(ShenMian): Skip wild card for now.
        while (auto card = deck_.draw().value()) {
            if (dynamic_cast<const WildCard*>(card.get()) != nullptr) {
                continue;
            }
            discard_pile_.push_back(std::move(card));
            break;
        }
    }

    void reverse_direction() {
        switch (direction_) {
            case Direction::Clockwise:
                direction_ = Direction::CounterClockwise;
                break;
            case Direction::CounterClockwise:
                direction_ = Direction::Clockwise;
                break;
        }
    }

    void render(sf::RenderTarget& render_target) const {
        deck_.render(render_target);
        discard_pile_.render(render_target);
        for (const auto& player : players_) {
            player.render(render_target, discard_pile_);
        }
    }

  private:
    Deck deck_;
    DiscardPile discard_pile_;

    std::vector<Player> players_;
    [[maybe_unused]] size_t player_index_ = 0;
    Direction direction_ = Direction::Clockwise;

    unsigned int seed_;
    std::mt19937 rng_;
};
