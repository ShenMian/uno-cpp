#include "player.hpp"
#include <cassert>
#include <iostream>

void testPlayerActions() {
    Player p("Alice");
    Card c(Color::GREEN, Type::NUMBER, 7);
    p.drawCard(c);
    assert(p.hand.size() == 1);
    Card played = p.playCard(0);
    assert(p.hand.empty());
    std::cout << "✅ Player tests passed!" << std::endl;
}

int main() {
    testPlayerActions();
    return 0;
}
