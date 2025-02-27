#include "card.hpp"

sf::Sprite Card::sprite() const {
    static sf::Texture card_spritesheet("assets/images/cards.png");

    const sf::Vector2i region_size(50, 66);
    const sf::Vector2i grid_size(8, 8);
    const sf::IntRect region(
        {region_size.x * (atlas_index() % grid_size.x),
         region_size.y * (atlas_index() / grid_size.x)},
        region_size
    );
    sf::Sprite sprite(card_spritesheet, region);
    sprite.setOrigin(sprite.getGlobalBounds().size / 2.0f);

    return sprite;
}

bool NumberCard::can_play_on(const Card& other) const noexcept {
    if (auto card = dynamic_cast<const NumberCard*>(&other)) {
        return color_ == card->color() || number_ == card->number();
    } else if (auto card = dynamic_cast<const ActionCard*>(&other)) {
        return color_ == card->color();
    } else if (auto card = dynamic_cast<const WildCard*>(&other)) {
        return color_ == card->color().value();
    }
    std::abort(); // Unreachable.
}

bool ActionCard::can_play_on(const Card& other) const noexcept {
    if (auto card = dynamic_cast<const ActionCard*>(&other)) {
        return color_ == card->color() || symbol_ == card->symbol();
    } else if (auto card = dynamic_cast<const NumberCard*>(&other)) {
        return color_ == card->color();
    } else if (auto card = dynamic_cast<const WildCard*>(&other)) {
        return color_ == card->color().value();
    }
    std::abort(); // Unreachable.
}
