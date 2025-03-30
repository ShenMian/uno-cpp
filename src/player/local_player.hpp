#pragma once

#include <algorithm>
#include <mutex>

#include "../button.hpp"
#include "player.hpp"

class LocalPlayer: public Player {
  public:
    LocalPlayer(
        Position position,
        Deck& deck,
        sf::RenderTarget& render_target
    ) :
        Player(position, deck),
        buttons_ {
            Button(nullptr),
            Button(nullptr),
            Button(nullptr),
            Button(nullptr)
        } {
        constexpr float BUTTON_SIZE = 90.0f;
        for (int i = 0; i < 4; i += 1) {
            auto rectangle = std::make_unique<sf::RectangleShape>(
                sf::Vector2f(BUTTON_SIZE, BUTTON_SIZE)
            );
            rectangle->setOrigin(
                rectangle->getSize() / 2.0f - sf::Vector2f(0.0f, -100.0f)
            );
            rectangle->setPosition(sf::Vector2f(render_target.getSize() / 2u));
            rectangle->setFillColor(PICKER_SFML_COLORS[i]);
            rectangle->setRotation(sf::degrees(i * 90.0f));
            buttons_[i] = Button(std::move(rectangle));
        }
    }

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
        render_cards(window, discard_pile, is_current_player);
        render_color_picker(window);
    }

  private:
    void render_cards(
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

            sprites.emplace_back(std::move(sprite));
        }

        for (size_t i = 0; i < cards_.size(); i += 1) {
            if (is_current_player) {
                // Dim the cards that cannot be played.
                if (!is_current_player
                    || !cards_[i]->can_play_on(discard_pile.peek_top())) {
                    sprites[i].setColor(DIM_COLOR);
                }

                // Highlight the hovered card.
                if (sprites[i].getGlobalBounds().contains(
                        window.mapPixelToCoords(sf::Mouse::getPosition(window))
                    )) {
                    if (!(i + 1 < cards_.size()
                          && sprites[i + 1].getGlobalBounds().contains(
                              window.mapPixelToCoords(
                                  sf::Mouse::getPosition(window)
                              )
                          ))) {
                        sprites[i].move({0.0f, -20.0f});

                        if (cards_[i]->can_play_on(discard_pile.peek_top())
                            && !selected_card_index_.has_value()
                            && sf::Mouse::isButtonPressed(
                                sf::Mouse::Button::Left
                            )) {
                            selected_card_index_ = i;
                        }
                    }
                }
            } else {
                sprites[i].setColor(DIM_COLOR);
            }

            window.draw(sprites[i]);
        }
    }

    void render_color_picker(sf::RenderWindow& render_target) const {
        if (is_picking_color_) {
            for (int i = 0; i < 4; i += 1) {
                buttons_[i].render(render_target);
                if (buttons_[i].is_clicked(render_target)) {
                    picked_color_ = PICKER_COLORS[i];
                    is_picking_color_ = false;
                }
            }
        }
    }

    bool has_playable_card(const DiscardPile& discard_pile) const {
        return std::ranges::any_of(cards_, [&](auto&& card) {
            return card->can_play_on(discard_pile.peek_top());
        });
    }

    Button buttons_[4];

    mutable std::mutex cards_mutex_;

    mutable bool is_picking_color_ = false;
    mutable Color picked_color_;
    mutable optional<size_t> selected_card_index_ = std::nullopt;
};
