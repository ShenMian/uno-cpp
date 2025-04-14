#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "dummy_card.hpp"

TEST_CASE("DummyCard basic functionality") {
    DummyCard c1(3, 12);
    DummyCard c2(3, 45);
    DummyCard c3(7, 12);

    CHECK(c1.can_play_on(c2));  // Same value
    CHECK(c1.can_play_on(c3));  // Same index
    CHECK_FALSE(c2.can_play_on(c3));  // Different value & index
}
