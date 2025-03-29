#include "card.hpp"

constexpr sf::Vector2i REGION_SIZE(50, 66);
constexpr sf::Vector2i GRID_SIZE(8, 8);
constexpr float CARD_SCALE = 2.0f;

sf::Texture Card::spritesheet_("assets/images/cards.png");
std::vector<sf::Sprite> Card::sprites_;

sf::Sprite Card::sprite() const {
    if (sprites_.empty()) {
        for (int index = 0; index <= 63; index += 1) {
            const sf::IntRect region(
                {REGION_SIZE.x * (index % GRID_SIZE.x),
                 REGION_SIZE.y * (index / GRID_SIZE.x)},
                REGION_SIZE
            );
            sf::Sprite sprite(spritesheet_, region);
            sprite.setOrigin(sprite.getGlobalBounds().size / 2.0f);
            sprite.setScale(sf::Vector2f(CARD_SCALE, CARD_SCALE));
            sprites_.push_back(std::move(sprite));
        }
    }
    return sprites_[atlas_index()];
}

sf::Sprite Card::back_sprite() {
    constexpr int INDEX = 55;
    constexpr sf::IntRect REGION(
        {REGION_SIZE.x * (INDEX % GRID_SIZE.x),
         REGION_SIZE.y * (INDEX / GRID_SIZE.x)},
        REGION_SIZE
    );
    sf::Sprite sprite(spritesheet_, REGION);
    sprite.setOrigin(sprite.getGlobalBounds().size / 2.0f);
    sprite.setScale(sf::Vector2f(CARD_SCALE, CARD_SCALE));
    return sprite;
}
