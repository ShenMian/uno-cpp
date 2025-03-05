#include "../deck.hpp"
#include <cassert>
#include <iostream>

void testDeckCreation() {
    Deck deck;
    assert(deck.cards.size() == 40);
    deck.shuffle();
    Card drawn = deck.draw();
    assert(deck.cards.size() == 39);
    std::cout << "✅ Deck tests passed!" << std::endl;
}

int main() {
    testDeckCreation();
    return 0;
}
