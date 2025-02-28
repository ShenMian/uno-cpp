#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <random>
#include <vector>

#include "card.hpp"
#include "config.hpp"

class DiscardPile {
  public:
    void push_back(std::unique_ptr<Card> card) {
        cards_.push_back(std::move(card));
    }

    const std::vector<std::unique_ptr<Card>>& cards() const {
        return cards_;
    }

    void render(sf::RenderTarget& render_target) const {
        static auto seed = std::random_device {}();
        std::mt19937 gen(seed);
        std::uniform_real_distribution<float> distrib(-20.0f, 20.0f);
        for (size_t i = 0; i < cards_.size(); i += 1) {
            auto sprite = cards_[i]->sprite();
            sprite.setScale({2.0f, 2.0f});
            sprite.setPosition(sf::Vector2f(render_target.getSize()) / 2.0f);

            // Generate random offset to make cards look naturally stacked.
            const sf::Vector2f offset(distrib(gen), distrib(gen));
            sprite.setPosition(
                sf::Vector2f(render_target.getSize()) / 2.0f + offset
            );

            // Dim the cards below the top card.
            if (i < cards_.size() - 1) {
                sprite.setColor(DIM_COLOR);
            }
            render_target.draw(sprite);
        }
    }

    const Card& peek_top() const {
        return *cards_.back().get();
    }

  private:
    std::vector<std::unique_ptr<Card>> cards_;
};
