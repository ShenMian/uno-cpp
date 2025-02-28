#include "card.hpp"

constexpr sf::Vector2i region_size(50, 66);
constexpr sf::Vector2i grid_size(8, 8);

sf::Texture Card::spritesheet_("assets/images/cards.png");

sf::Sprite Card::sprite() const {
    const sf::IntRect region(
        {region_size.x * (atlas_index() % grid_size.x),
         region_size.y * (atlas_index() / grid_size.x)},
        region_size
    );
    sf::Sprite sprite(spritesheet_, region);
    sprite.setOrigin(sprite.getGlobalBounds().size / 2.0f);

    return sprite;
}

sf::Sprite Card::back_sprite() {
    constexpr int index = 55;
    constexpr sf::IntRect region(
        {region_size.x * (index % grid_size.x),
         region_size.y * (index / grid_size.x)},
        region_size
    );
    sf::Sprite sprite(spritesheet_, region);
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
