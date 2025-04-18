#pragma once

#include <algorithm>
#include <cassert>
#include <memory>
#include <mutex>

#include "../button.hpp"
#include "player.hpp"

int hovered_card_index = -1;

class LocalPlayer: public Player {
  public:
    LocalPlayer(
        Position position,
        Deck& deck,
        sf::RenderTarget& render_target
    ) :
        Player(position, deck) {}

    optional<unique_ptr<Card>>
    play_card(const DiscardPile& discard_pile) override {
        if (!has_playable_card(discard_pile)) {
            return std::nullopt;
        }
        while (!selected_card_index_.has_value())
            ;
        std::lock_guard lock(cards_mutex_);
        auto card = std::move(cards_[selected_card_index_.value()]);
        cards_.erase(std::next(cards_.begin(), selected_card_index_.value()));
        selected_card_index_ = std::nullopt;
        return card;
    }

    Color select_wild_color() const override {
        is_picking_color_ = true;
        while (is_picking_color_)
            ;
        return picked_color_;
    }

    virtual void render(
        sf::RenderWindow& window,
        const DiscardPile& discard_pile,
        bool is_current_player
    ) const override {
        render_hand(window, discard_pile, is_current_player);
        if (is_picking_color_) {
            render_color_picker(window);
        }
    }

  private:
    void render_hand(
        sf::RenderWindow& window,
        const DiscardPile& discard_pile,
        bool is_current_player
    ) const {
        std::vector<sf::Sprite> sprites;
        sprites.reserve(cards_.size());

        std::lock_guard lock(cards_mutex_);
        for (size_t i = 0; i < cards_.size(); i += 1) {
            auto sprite = cards_[i]->sprite();

            const auto spacing = std::min(
                window.getSize().x * 0.5f / cards_.size(),
                MAX_SPACING
            );
            const auto width = sprite.getGlobalBounds().size.x - spacing
                + (cards_.size() - 1) * spacing;
            sprite.setPosition(
                {window.getSize().x / 2.0f - width / 2.0f + i * spacing,
                 window.getSize().y - sprite.getGlobalBounds().size.y / 2.0f}
            );

            sprites.push_back(std::move(sprite));
        }

        for (size_t i = 0; i < cards_.size(); i += 1) {
            if (is_current_player) {
                // Dim the cards that cannot be played.
                if (!cards_[i]->can_play_on(discard_pile.peek_top())) {
                    sprites[i].setColor(DIM_COLOR);
                }

                // Highlight the hovered card.
                if (sprites[i].getGlobalBounds().contains(
                        get_mouse_position(window)
                    )) {
                    // if (!(i + 1 < cards_.size()
                    //       && sprites[i + 1].getGlobalBounds().contains(
                    //           get_mouse_position(window)
                    //       ))) {
                    //     on_card_hovered(i, discard_pile, sprites);
                    //     hovered_card_index = static_cast<int>(i);
                    // }
                    if (i + 1 < cards_.size()
                        && sprites[i].getGlobalBounds().contains(
                            get_mouse_position(window))
                        && sprites[i + 1].getGlobalBounds().contains(
                            get_mouse_position(window))) {
                        if (i != hovered_card_index) {
                            hovered_card_index = static_cast<int>(i + 1);
                        }
                    } else if (sprites[i].getGlobalBounds().contains(
                        get_mouse_position(window))
                        && !sprites[i + 1].getGlobalBounds().contains(
                            get_mouse_position(window))
                        && !sprites[i - 1].getGlobalBounds().contains(
                            get_mouse_position(window))) {
                        hovered_card_index = static_cast<int>(i);
                    }
                }
            } else {
                sprites[i].setColor(DIM_COLOR);
            }

            if (hovered_card_index != -1
                && !sprites[hovered_card_index].getGlobalBounds().contains(
                    get_mouse_position(window))) {
                hovered_card_index = -1;
            }

            // Draw all not hovered cards first.
            if (i != hovered_card_index) {
                window.draw(sprites[i]);
            }
        }

        // Draw the hovered card on top of the others.
        if (hovered_card_index != -1) {
            on_card_hovered(hovered_card_index, discard_pile, sprites);
            window.draw(sprites[hovered_card_index]);
        }
    }

    void on_card_hovered(
        size_t card_index,
        const DiscardPile& discard_pile,
        std::vector<sf::Sprite>& sprites
    ) const {
        sprites[card_index].move({0.0f, -20.0f});
        if (cards_[card_index]->can_play_on(discard_pile.peek_top())
            && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            on_card_left_clicked(card_index);
        }
    }

    void on_card_left_clicked(size_t card_index) const {
        selected_card_index_ = card_index;
    }

    void render_color_picker(sf::RenderWindow& render_target) const {
        constexpr Color PICKER_COLORS[] =
            {Color::Red, Color::Green, Color::Blue, Color::Yellow};
        constexpr sf::Color PICKER_SFML_COLORS[] = {
            sf::Color(207, 87, 60),
            sf::Color(70, 130, 50),
            sf::Color(79, 143, 186),
            sf::Color(222, 158, 65),
        };
        constexpr float BUTTON_SIZE = 90.0f;

        for (int i = 0; i < 4; i += 1) {
            auto rectangle = std::make_unique<sf::RectangleShape>(
                sf::Vector2f(BUTTON_SIZE, BUTTON_SIZE)
            );
            rectangle->setOrigin(
                rectangle->getGeometricCenter() - sf::Vector2f(0.0f, -100.0f)
            );
            rectangle->setPosition(sf::Vector2f(render_target.getSize() / 2u));
            rectangle->setFillColor(PICKER_SFML_COLORS[i]);
            rectangle->setRotation(sf::degrees(i * 90.0f));

            Button button(std::move(rectangle));
            button.render(render_target);
            if (button.is_left_clicked(render_target)) {
                picked_color_ = PICKER_COLORS[i];
                is_picking_color_ = false;
            }
        }
    }

    bool has_playable_card(const DiscardPile& discard_pile) const {
        return std::ranges::any_of(cards_, [&](auto& card) {
            return card->can_play_on(discard_pile.peek_top());
        });
    }

    sf::Vector2f get_mouse_position(sf::RenderWindow& window) const {
        return window.mapPixelToCoords(sf::Mouse::getPosition(window));
    }

    mutable std::mutex cards_mutex_;

    mutable bool is_picking_color_ = false;
    mutable Color picked_color_;
    mutable optional<size_t> selected_card_index_ = std::nullopt;
};
