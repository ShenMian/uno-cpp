#pragma once

#include <SFML/Audio.hpp>
#include <format>
#include <random>

class Audio {
  public:
    Audio(Audio&) = delete;

    void play_random_place_sound() {
        std::uniform_int_distribution<size_t> dist(0, 3);
        std::mt19937 rng(std::random_device {}());
        const auto index = dist(rng);
        place_sounds_[index].play();
    }

    void play_random_slide_sound() {
        std::uniform_int_distribution<size_t> dist(0, 7);
        std::mt19937 rng(std::random_device {}());
        const auto index = dist(rng);
        slide_sounds_[index].play();
    }

    static Audio& get() {
        static Audio instance;
        return instance;
    }

  private:
    Audio() :
        place_sounds_ {
            sf::Sound(place_buffers_[0]),
            sf::Sound(place_buffers_[1]),
            sf::Sound(place_buffers_[2]),
            sf::Sound(place_buffers_[3])
        },
        slide_sounds_ {
            sf::Sound(slide_buffers_[0]),
            sf::Sound(slide_buffers_[1]),
            sf::Sound(slide_buffers_[2]),
            sf::Sound(slide_buffers_[3]),
            sf::Sound(slide_buffers_[4]),
            sf::Sound(slide_buffers_[5]),
            sf::Sound(slide_buffers_[6]),
            sf::Sound(slide_buffers_[7])
        } {
        for (size_t i = 0; i < 4; ++i) {
            if (!place_buffers_[i].loadFromFile(
                    std::format("assets/audio/card-place-{}.ogg", i + 1)
                )) {
                std::abort();
            }
            place_sounds_[i].setVolume(70);
        }
        for (size_t i = 0; i < 8; ++i) {
            if (!slide_buffers_[i].loadFromFile(
                    std::format("assets/audio/card-slide-{}.ogg", i + 1)
                )) {
                std::abort();
            }
            slide_sounds_[i].setVolume(70);
        }
    }

    sf::SoundBuffer place_buffers_[4];
    sf::Sound place_sounds_[4];

    sf::SoundBuffer slide_buffers_[8];
    sf::Sound slide_sounds_[8];
};