#include <SFML/Graphics.hpp>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

#include "button.hpp"
#include "card.hpp"
#include "config.hpp"
#include "deck.hpp"
#include "discard_pile.hpp"

using std::optional;
using std::unique_ptr;
using std::vector;

constexpr float MAX_SPACING = 70.0f;
constexpr Color PICKER_COLORS[] =
    {Color::Red, Color::Green, Color::Blue, Color::Yellow};
constexpr sf::Color PICKER_SFML_COLORS[] =
    {sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow};

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
        sf::RenderWindow& window,
        const DiscardPile& discard_pile,
        bool is_current_player
    ) const {
        switch (position_) {
            case Position::North:
                render_north(window);
                break;
            case Position::East:
            case Position::West:
                render_vertical(window);
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

    optional<unique_ptr<Card>> play_card(const DiscardPile& discard_pile
    ) override {
        bool has_playable_card = false;
        for (auto it = cards_.begin(); it != cards_.end(); ++it) {
            if ((*it)->can_play_on(discard_pile.peek_top())) {
                has_playable_card = true;
                break;
            }
        }
        if (!has_playable_card) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            return std::nullopt;
        }
        while (!selected_card_index_.has_value())
            ;
        const std::lock_guard lock(cards_mutex_);
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
        std::vector<sf::Sprite> sprites;
        sprites.reserve(cards_.size());

        const std::lock_guard lock(cards_mutex_);
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

        render_color_picker(window);
    }

  private:
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

    Button buttons_[4];

    std::mutex cards_mutex_;

    mutable bool is_picking_color_ = false;
    mutable Color picked_color_;
    mutable optional<size_t> selected_card_index_ = std::nullopt;
};

/// An AI-controlled player for the UNO game.
class AiPlayer: public Player {
  public:
    AiPlayer(Position position, Deck& deck) : Player(position, deck) {}

    optional<unique_ptr<Card>> play_card(const DiscardPile& discard_pile
    ) override {
        for (auto it = cards_.begin(); it != cards_.end(); ++it) {
            if ((*it)->can_play_on(discard_pile.peek_top())) {
                auto card = std::move(*it);
                cards_.erase(it);
                std::this_thread::sleep_for(std::chrono::milliseconds(1500));
                return card;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
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
