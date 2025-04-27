#pragma once

#include <SFML/Graphics.hpp>

#include "../card/card.hpp"
#include "../deck.hpp"
#include "../discard_pile.hpp"

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
    virtual unique_ptr<Card> play_card(const DiscardPile&) = 0;
    /// Choose a color for a Wild card.
    virtual Color select_wild_color() const = 0;

    virtual void render(
        sf::RenderWindow& window,
        const DiscardPile& discard_pile,
        bool is_current_player
    ) const {
        switch (position_) {
            case Position::North:
                render_north_hand(window);
                break;
            case Position::East:
            case Position::West:
                render_vertical_hand(window);
                break;
            default:
                assert(false); // Unreachable.
                return;
        }
    }

    /// Draw a card from the deck.
    virtual void draw_from_deck(Deck& deck) {
        auto new_card = deck.draw().value();
        cards_.insert(
            std::lower_bound(
                cards_.begin(),
                cards_.end(),
                *new_card,
                [](const auto& ptr, const auto& value) -> bool {
                    return *ptr < value;
                }
            ),
            std::move(new_card)
        );
    }

    bool has_playable_card(const DiscardPile& discard_pile) const {
        return std::ranges::any_of(cards_, [&](auto& card) {
            return card->can_play_on(discard_pile.peek_top());
        });
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
            draw_from_deck(deck);
        }
    }

    vector<unique_ptr<Card>> cards_;

  private:
    void render_north_hand(sf::RenderTarget& render_target) const {
        assert(position_ == Position::North);
        for (size_t i = 0; i < cards_.size(); i += 1) {
            auto sprite = Card::get_back_sprite();

            const auto spacing = std::min(
                render_target.getSize().x * 0.5f / cards_.size(),
                MAX_SPACING
            );
            const auto total_width = sprite.getGlobalBounds().size.x - spacing
                + (cards_.size() - 1) * spacing;
            sprite.setPosition(
                {render_target.getSize().x / 2.0f - total_width / 2.0f
                     + i * spacing,
                 sprite.getGlobalBounds().size.y / 2.0f}
            );

            render_target.draw(sprite);
        }
    }

    void render_vertical_hand(sf::RenderTarget& render_target) const {
        assert(position_ == Position::East || position_ == Position::West);
        for (size_t i = 0; i < cards_.size(); i += 1) {
            auto sprite = Card::get_back_sprite();

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
            const auto total_height = sprite.getGlobalBounds().size.x - spacing
                + (cards_.size() - 1) * spacing;
            sprite.setPosition(
                {x_position,
                 render_target.getSize().y / 2.0f - total_height / 2.0f
                     + i * spacing}
            );

            render_target.draw(sprite);
        }
    }

    Position position_;
};
