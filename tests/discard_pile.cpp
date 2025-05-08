#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../src/discard_pile.hpp"

#include <doctest/doctest.h>

#include "../src/card/action_card.hpp"
#include "../src/card/number_card.hpp"
#include "../src/card/wild_card.hpp"


TEST_CASE("DiscardPile basic operations") {
    DiscardPile pile;
    CHECK(pile.cards().empty());

    // Add a NumberCard
    pile.push_back(std::make_unique<NumberCard>(Color::Red, 5));
    CHECK(pile.cards().size() == 1);
    CHECK(dynamic_cast<NumberCard*>(pile.cards().back().get()) != nullptr);
    CHECK(pile.peek_top().value() == 5);

    // Add an ActionCard
    pile.push_back(
        std::make_unique<ActionCard>(Color::Blue, ActionSymbol::Skip)
    );
    CHECK(pile.cards().size() == 2);
    CHECK(dynamic_cast<ActionCard*>(pile.cards().back().get()) != nullptr);
    CHECK(pile.peek_top().value() == 20);

    // Add a WildCard
    pile.push_back(std::make_unique<WildCard>(WildSymbol::WildDrawFour));
    CHECK(pile.cards().size() == 3);
    CHECK(dynamic_cast<WildCard*>(pile.cards().back().get()) != nullptr);
    CHECK(pile.peek_top().value() == 50);
}
