#include "card.hpp"

const sf::Vector2i REGION_SIZE(50, 66);
const sf::Vector2i GRID_SIZE(8, 8);
const float CARD_SCALE = 2.0f;

// sf::Texture Card::atlas_texture_("assets/images/cards.png");
sf::Texture Card::atlas_texture_;
std::vector<sf::Sprite> Card::sprites_;

bool Card::init_texture(const std::string& path) {
    return atlas_texture_.loadFromFile(path);
}

sf::Sprite Card::sprite() const {
    if (sprites_.empty()) {
        for (int atlas_index = 0; atlas_index <= 63; atlas_index += 1) {
            const sf::IntRect region(
                {REGION_SIZE.x * (atlas_index % GRID_SIZE.x),
                 REGION_SIZE.y * (atlas_index / GRID_SIZE.x)},
                REGION_SIZE
            );
            sf::Sprite sprite(atlas_texture_, region);
            sf::FloatRect bounds = sprite.getGlobalBounds();
            sprite.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);

            // sprite.setOrigin(sprite.getGlobalBounds().getCenter());
            sprite.setScale(sf::Vector2f(CARD_SCALE, CARD_SCALE));
            sprites_.push_back(std::move(sprite));
        }
    }
    return sprites_[atlas_index()];
}

sf::Sprite Card::get_back_sprite() {
    const int ATLAS_INDEX = 55;
    const sf::IntRect REGION(
        {REGION_SIZE.x * (ATLAS_INDEX % GRID_SIZE.x),
         REGION_SIZE.y * (ATLAS_INDEX / GRID_SIZE.x)},
        REGION_SIZE
    );
    sf::Sprite sprite(atlas_texture_, REGION);
    sf::FloatRect bounds = sprite.getGlobalBounds();
    sprite.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    sprite.setScale(sf::Vector2f(CARD_SCALE, CARD_SCALE));
    return sprite;
}
