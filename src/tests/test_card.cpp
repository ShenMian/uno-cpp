#include <gtest/gtest.h>
#include "../card.hpp"

// Test sprite generation
TEST(CardTest, SpriteGeneration) {
    Card card;
    sf::Sprite sprite = card.sprite();

    // Check if sprite texture is set
    EXPECT_NE(sprite.getTexture(), nullptr);
}

// Test back sprite generation
TEST(CardTest, BackSpriteGeneration) {
    sf::Sprite backSprite = Card::back_sprite();

    // Check if back sprite has texture
    EXPECT_NE(backSprite.getTexture(), nullptr);
}

// Test NumberCard can_play_on method
TEST(CardTest, NumberCardCanPlayOn) {
    NumberCard card1(Color::Red, 5);
    NumberCard card2(Color::Red, 3);
    NumberCard card3(Color::Blue, 5);

    EXPECT_TRUE(card1.can_play_on(card2));  // Same color
    EXPECT_TRUE(card1.can_play_on(card3));  // Same number
    EXPECT_FALSE(card2.can_play_on(card3)); // Neither same color nor number
}

// Test ActionCard can_play_on method
TEST(CardTest, ActionCardCanPlayOn) {
    ActionCard skipRed(Color::Red, Symbol::Skip);
    ActionCard reverseRed(Color::Red, Symbol::Reverse);
    ActionCard skipBlue(Color::Blue, Symbol::Skip);

    EXPECT_TRUE(skipRed.can_play_on(reverseRed)); // Same color
    EXPECT_TRUE(skipRed.can_play_on(skipBlue));   // Same symbol
    EXPECT_FALSE(reverseRed.can_play_on(skipBlue)); // Neither same color nor symbol
}

// Test WildCard can_play_on method
TEST(CardTest, WildCardCanPlayOn) {
    WildCard wild(Color::None); // Assuming wild has no fixed color
    NumberCard redCard(Color::Red, 7);
    EXPECT_TRUE(wild.can_play_on(redCard));  // Should be able to play on anything
}

// Main function to run tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
