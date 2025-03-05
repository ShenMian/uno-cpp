#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../src/card.hpp"

#include <doctest/doctest.h>

TEST_CASE("NumberCard properties and behavior") {
    NumberCard red_five(Color::Red, 5);
    NumberCard blue_nine(Color::Blue, 9);
    NumberCard green_zero(Color::Green, 0);

    SUBCASE("Properties are correctly set") {
        CHECK(red_five.color() == Color::Red);
        CHECK(red_five.number() == 5);
        CHECK(red_five.value() == 5);
        CHECK(red_five.atlas_index() == 5);

        CHECK(blue_nine.color() == Color::Blue);
        CHECK(blue_nine.number() == 9);
        CHECK(blue_nine.value() == 9);
        CHECK(blue_nine.atlas_index() == 22);
    }

    SUBCASE("can_play_on logic works correctly") {
        CHECK(red_five.can_play_on(red_five));
        CHECK(red_five.can_play_on(NumberCard(Color::Yellow, 5)));
        CHECK_FALSE(red_five.can_play_on(blue_nine));
    }

    SUBCASE("Invalid number throws exception") {
        CHECK_THROWS_AS(NumberCard(Color::Red, 10), std::invalid_argument);
    }
}

TEST_CASE("ActionCard properties and behavior") {
    ActionCard red_draw_two(Color::Red, ActionSymbol::DrawTwo);
    ActionCard blue_reverse(Color::Blue, ActionSymbol::Reverse);
    ActionCard yellow_skip(Color::Yellow, ActionSymbol::Skip);

    SUBCASE("Properties are correctly set") {
        CHECK(red_draw_two.color() == Color::Red);
        CHECK(red_draw_two.symbol() == ActionSymbol::DrawTwo);
        CHECK(red_draw_two.value() == 20);
        CHECK(red_draw_two.atlas_index() == 10);

        CHECK(blue_reverse.color() == Color::Blue);
        CHECK(blue_reverse.symbol() == ActionSymbol::Reverse);
        CHECK(blue_reverse.value() == 20);
        CHECK(blue_reverse.atlas_index() == 24);
    }

    SUBCASE("can_play_on logic works correctly") {
        CHECK(red_draw_two.can_play_on(red_draw_two));
        CHECK(red_draw_two.can_play_on(NumberCard(Color::Red, 5)));
        CHECK_FALSE(red_draw_two.can_play_on(blue_reverse));
        CHECK(red_draw_two.can_play_on(
            ActionCard(Color::Yellow, ActionSymbol::DrawTwo)
        ));
    }
}

TEST_CASE("WildCard properties and behavior") {
    WildCard wild(WildSymbol::Wild);
    WildCard wild_draw_four(WildSymbol::WildDrawFour);

    SUBCASE("Properties before color set") {
        CHECK_FALSE(wild.color().has_value());
        CHECK(wild.symbol() == WildSymbol::Wild);
        CHECK(wild.value() == 50);
        CHECK(wild.atlas_index() == 52);

        CHECK_FALSE(wild_draw_four.color().has_value());
        CHECK(wild_draw_four.symbol() == WildSymbol::WildDrawFour);
        CHECK(wild_draw_four.value() == 50);
        CHECK(wild_draw_four.atlas_index() == 53);
    }

    SUBCASE("Properties after color set") {
        wild.set_color(Color::Red);
        wild_draw_four.set_color(Color::Blue);

        CHECK(wild.color().has_value());
        CHECK(wild.color().value() == Color::Red);
        CHECK(wild.atlas_index() == 56);

        CHECK(wild_draw_four.color().has_value());
        CHECK(wild_draw_four.color().value() == Color::Blue);
        CHECK(wild_draw_four.atlas_index() == 61);
    }

    SUBCASE("can_play_on logic works correctly") {
        NumberCard blue_five(Color::Blue, 5);
        CHECK(wild.can_play_on(blue_five));
        CHECK(wild_draw_four.can_play_on(blue_five));
    }
}
