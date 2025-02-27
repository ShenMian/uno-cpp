#include <SFML/Graphics.hpp>
#include <cstddef>
#include <memory>
#include <random>
#include <vector>

#include "card.hpp"
#include "deck.hpp"

constexpr sf::Color DIM_COLOR = sf::Color(
    static_cast<uint8_t>(0.7 * 255),
    static_cast<uint8_t>(0.7 * 255),
    static_cast<uint8_t>(0.7 * 255),
    255
);

void draw_discard_pile(
    sf::RenderTarget& render_target,
    const std::vector<std::unique_ptr<Card>>& discard_pile
) {
    static auto seed = std::random_device {}();
    std::mt19937 gen(seed);
    std::uniform_real_distribution<float> distrib(-20.0f, 20.0f);
    for (size_t i = 0; i < discard_pile.size(); i += 1) {
        auto sprite = discard_pile[i]->sprite();
        sprite.setScale({2.0f, 2.0f});
        sprite.setPosition(sf::Vector2f(render_target.getSize()) / 2.0f);

        // Generate random offset to make cards look naturally stacked.
        const sf::Vector2f offset(distrib(gen), distrib(gen));
        sprite.setPosition(
            sf::Vector2f(render_target.getSize()) / 2.0f + offset
        );

        // Dim the cards below the top card.
        if (i < discard_pile.size() - 1) {
            sprite.setColor(DIM_COLOR);
        }
        render_target.draw(sprite);
    }
}

void draw_hand(
    sf::RenderTarget& render_target,
    const std::vector<std::unique_ptr<Card>>& hand,
    std::unique_ptr<Card>& top_card
) {
    constexpr auto INTERVAL = 70.0f; // The space between cards.
    for (size_t i = 0; i < hand.size(); i += 1) {
        auto sprite = hand[i]->sprite();
        sprite.setScale({2.0f, 2.0f});

        const auto width = sprite.getGlobalBounds().size.x - INTERVAL
            + (hand.size() - 1) * INTERVAL;
        sprite.setPosition(
            {render_target.getSize().x / 2.0f - width / 2.0f + i * INTERVAL,
             render_target.getSize().y - sprite.getGlobalBounds().size.y / 2.0f}
        );

        // Dim the cards that cannot be played.
        if (!hand[i]->can_play_on(*top_card)) {
            sprite.setColor(DIM_COLOR);
        }

        render_target.draw(sprite);
    }
}

int main() {
    auto window = sf::RenderWindow(sf::VideoMode({1536u, 864u}), "UNO");
    window.setFramerateLimit(144);

    const auto seed = std::random_device {}();
    std::mt19937 rng(seed);

    Deck deck;
    deck.shuffle(rng);

    std::vector<std::unique_ptr<Card>> hand;
    for (size_t i = 0; i < 7; ++i) {
        hand.push_back(deck.draw().value());
    }
    std::sort(
        hand.begin(),
        hand.end(),
        [](const std::unique_ptr<Card>& lhs, const std::unique_ptr<Card>& rhs) {
            return *lhs < *rhs;
        }
    );

    std::vector<std::unique_ptr<Card>> discard_pile;
    // FIXME(ShenMian): Skip wild card for now.
    while (auto card = deck.draw().value()) {
        if (dynamic_cast<const WildCard*>(card.get()) != nullptr) {
            continue;
        }
        discard_pile.push_back(std::move(card));
        break;
    }

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear();
        draw_discard_pile(window, discard_pile);
        draw_hand(window, hand, discard_pile.back());
        window.display();
    }
}
