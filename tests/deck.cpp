#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../src/deck.hpp"

#include <doctest/doctest.h>

#include <random>

TEST_CASE("Deck initializes with 108 cards and draws correctly") {
    std::mt19937 rng {42};
    Deck deck(rng);

    // Draw all 108 cards
    int draw_count = 0;
    for (int i = 0; i < 108; ++i) {
        auto card = deck.draw();
        CHECK(card.has_value());
        ++draw_count;
    }
    CHECK_EQ(draw_count, 108);

    // After 108 draws, deck should auto-reset and allow drawing again
    auto card = deck.draw();
    CHECK(card.has_value());
}

TEST_CASE("Deck reinitializes after empty") {
    std::mt19937 rng {123};
    Deck deck(rng);

    // Draw all cards to empty the deck
    for (int i = 0; i < 108; ++i) {
        deck.draw();
    }
    // Deck should now be empty and reinitialize on next draw
    auto card = deck.draw();
    CHECK(card.has_value());
}

TEST_CASE("Deck shuffles cards on initialization") {
    std::mt19937 rng1 {1};
    std::mt19937 rng2 {2};
    Deck deck1(rng1);
    Deck deck2(rng2);

    // Draw the first card from each deck and compare
    auto card1 = deck1.draw();
    auto card2 = deck2.draw();

    CHECK(card1.has_value());
    CHECK(card2.has_value());

    CHECK(card1.value().get() != card2.value().get());
}
