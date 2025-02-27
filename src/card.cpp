#include "card.hpp"

bool NumberCard::can_play_on(const Card& other) const noexcept {
    if (auto card = dynamic_cast<const NumberCard*>(&other)) {
        return color_ == card->color() || number_ == card->number();
    } else if (auto card = dynamic_cast<const ActionCard*>(&other)) {
        return color_ == card->color();
    } else if (auto card = dynamic_cast<const WildCard*>(&other)) {
        return color_ == card->color().value();
    }
    std::abort(); // Unreachable.
}

bool ActionCard::can_play_on(const Card& other) const noexcept {
    if (auto card = dynamic_cast<const ActionCard*>(&other)) {
        return color_ == card->color() || symbol_ == card->symbol();
    } else if (auto card = dynamic_cast<const NumberCard*>(&other)) {
        return color_ == card->color();
    } else if (auto card = dynamic_cast<const WildCard*>(&other)) {
        return color_ == card->color().value();
    }
    std::abort(); // Unreachable.
}
