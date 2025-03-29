#pragma once

#include <SFML/Audio.hpp>
#include <format>
#include <random>

class Audio {
  public:
    Audio(Audio&) = delete;

    void play_place() {
        std::uniform_int_distribution<size_t> dist(0, 3);
        std::mt19937 rng(std::random_device {}());
        const auto index = dist(rng);
        place_sound_[index].play();
    }

    void play_slide() {
        std::uniform_int_distribution<size_t> dist(0, 7);
        std::mt19937 rng(std::random_device {}());
        const auto index = dist(rng);
        slide_sound_[index].play();
    }

    static Audio& instance() {
        static Audio instance;
        return instance;
    }

  private:
    Audio() :
        place_sound_ {
            sf::Sound(place_buffer_[0]),
            sf::Sound(place_buffer_[1]),
            sf::Sound(place_buffer_[2]),
            sf::Sound(place_buffer_[3])
        },
        slide_sound_ {
            sf::Sound(slide_buffer_[0]),
            sf::Sound(slide_buffer_[1]),
            sf::Sound(slide_buffer_[2]),
            sf::Sound(slide_buffer_[3]),
            sf::Sound(slide_buffer_[4]),
            sf::Sound(slide_buffer_[5]),
            sf::Sound(slide_buffer_[6]),
            sf::Sound(slide_buffer_[7])
        } {
        for (size_t i = 0; i < 4; ++i) {
            if (!place_buffer_[i].loadFromFile(
                    std::format("assets/audio/card-place-{}.ogg", i + 1)
                )) {
                std::abort();
            }
            place_sound_[i].setVolume(70);
        }
        for (size_t i = 0; i < 8; ++i) {
            if (!slide_buffer_[i].loadFromFile(
                    std::format("assets/audio/card-slide-{}.ogg", i + 1)
                )) {
                std::abort();
            }
            slide_sound_[i].setVolume(70);
        }
    }

    sf::SoundBuffer place_buffer_[4];
    sf::Sound place_sound_[4];

    sf::SoundBuffer slide_buffer_[8];
    sf::Sound slide_sound_[8];
};