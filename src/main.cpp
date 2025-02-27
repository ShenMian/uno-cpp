#include <SFML/Graphics.hpp>
#include <vector>

#include "deck.hpp"

int main() {
    auto window = sf::RenderWindow(sf::VideoMode({1536u, 864u}), "UNO");
    window.setFramerateLimit(144);

    const auto seed = std::random_device{}();
    std::mt19937 rng(seed);

    Deck deck;
    deck.shuffle(rng);

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear();
        window.display();
    }
}