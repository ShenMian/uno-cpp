#include <SFML/Graphics.hpp>
#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

// DEBUG
#include <iostream>

#include "card.hpp"
#include "config.hpp"
#include "deck.hpp"
#include "discard_pile.hpp"

using std::optional;
using std::unique_ptr;
using std::vector;

constexpr float MAX_SPACING = 70.0f;

enum class Position : uint8_t { North, East, South, West };

/// A player in the Uno game.
class Player {
  public:
    virtual ~Player() = default;
    /// Play a card from the player's hand.
    virtual optional<unique_ptr<Card>> play_card(const DiscardPile&) = 0;
    /// Choose a color for a Wild card.
    virtual Color select_wild_color() const = 0;

    virtual void render(
        sf::RenderTarget& render_target,
        const DiscardPile& discard_pile,
        bool is_current_player
    ) const {
        switch (position_) {
            case Position::North:
                render_north(render_target);
                break;
            case Position::East:
            case Position::West:
                render_vertical(render_target);
                break;
            default:
                assert(false); // Unreachable.
                return;
        }
    }

    /// Draw a card from the deck.
    void draw_card_from_deck(Deck& deck) {
        auto card = deck.draw().value();
        cards_.insert(
            std::lower_bound(
                cards_.begin(),
                cards_.end(),
                *card,
                [](const auto& ptr, const auto& value) -> bool {
                    return *ptr < value;
                }
            ),
            std::move(card)
        );
    }

    /// Returns true if the player has no cards left in their hand.
    bool is_hand_empty() const noexcept {
        return cards_.empty();
    }

    /// Returns the position of the player.
    Position position() const noexcept {
        return position_;
    }

  protected:
    Player(Position position, Deck& deck) : position_(position) {
        for (size_t i = 0; i < 7; i += 1) {
            draw_card_from_deck(deck);
        }
    }

    vector<unique_ptr<Card>> cards_;

  private:
    void render_north(sf::RenderTarget& render_target) const {
        assert(position_ == Position::North);
        for (size_t i = 0; i < cards_.size(); i += 1) {
            auto sprite = Card::back_sprite();

            const auto spacing = std::min(
                render_target.getSize().x * 0.5f / cards_.size(),
                MAX_SPACING
            );
            const auto width = sprite.getGlobalBounds().size.x - spacing
                + (cards_.size() - 1) * spacing;
            sprite.setPosition(
                {render_target.getSize().x / 2.0f - width / 2.0f + i * spacing,
                 sprite.getGlobalBounds().size.y / 2.0f}
            );

            render_target.draw(sprite);
        }
    }

    void render_vertical(sf::RenderTarget& render_target) const {
        assert(position_ == Position::East || position_ == Position::West);
        for (size_t i = 0; i < cards_.size(); i += 1) {
            auto sprite = Card::back_sprite();

            float x_position;
            switch (position_) {
                case Position::East:
                    sprite.rotate(sf::degrees(90.0f));
                    x_position = render_target.getSize().x
                        - sprite.getGlobalBounds().size.x / 2.0f;
                    break;
                case Position::West:
                    sprite.rotate(sf::degrees(-90.0f));
                    x_position = sprite.getGlobalBounds().size.x / 2.0f;
                    break;
                default:
                    assert(false); // Unreachable.
                    return;
            }

            const auto spacing = std::min(
                render_target.getSize().y * 0.5f / cards_.size(),
                MAX_SPACING
            );
            const auto height = sprite.getGlobalBounds().size.x - spacing
                + (cards_.size() - 1) * spacing;
            sprite.setPosition(
                {x_position,
                 render_target.getSize().y / 2.0f - height / 2.0f + i * spacing}
            );

            render_target.draw(sprite);
        }
    }

    Position position_;
};

class LocalPlayer: public Player {
  public:
    LocalPlayer(Position position, Deck& deck) : Player(position, deck) {}

    optional<unique_ptr<Card>>
    play_card(const DiscardPile& discard_pile) override {
        // DEBUG
        std::cout << "LOCAL PLAYER: play_card" << std::endl;
        int i;
        std::cin >> i;
        return std::nullopt;
    }

    Color select_wild_color() const override {
        return Color::Red;
    }

    virtual void render(
        sf::RenderTarget& render_target,
        const DiscardPile& discard_pile,
        bool is_current_player
    ) const override {
        for (size_t i = 0; i < cards_.size(); i += 1) {
            auto sprite = cards_[i]->sprite();

            const auto spacing = std::min(
                render_target.getSize().x * 0.5f / cards_.size(),
                MAX_SPACING
            );
            const auto width = sprite.getGlobalBounds().size.x - spacing
                + (cards_.size() - 1) * spacing;
            sprite.setPosition(
                {render_target.getSize().x / 2.0f - width / 2.0f + i * spacing,
                 render_target.getSize().y
                     - sprite.getGlobalBounds().size.y / 2.0f}
            );

            // Dim the cards that cannot be played.
            if (!is_current_player
                || !cards_[i]->can_play_on(discard_pile.peek_top())) {
                sprite.setColor(DIM_COLOR);
            }

            render_target.draw(sprite);
        }
    }
};

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
