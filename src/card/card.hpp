#pragma once

#include <SFML/Graphics.hpp>
#include <cassert>
#include <cstdint>
#include <optional>
#include <vector>

using std::optional;

enum class Color : uint8_t { Red, Blue, Green, Yellow };

/// A UNO card.
class Card {
  public:
    virtual ~Card() = default;
    /// Returns the value of the card.
    virtual uint8_t value() const noexcept = 0;
    /// Returns the index of the card in the atlas texture.
    virtual uint8_t atlas_index() const noexcept = 0;
    /// Returns whether the card can be played on another card.
    virtual bool can_play_on(const Card& other) const noexcept = 0;

    /// Returns a sprite representing the card.
    sf::Sprite sprite() const;

    auto operator<=>(const Card& rhs) const noexcept {
        return atlas_index() <=> rhs.atlas_index();
    }

    /// Returns a sprite representing the back of the card.
    static sf::Sprite get_back_sprite();
    static bool init_texture(const std::string& path);

  private:
    static sf::Texture atlas_texture_;
    static std::vector<sf::Sprite> sprites_;
};
