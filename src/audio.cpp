#include "audio.hpp"
#include "SFML/Audio/Music.hpp"

int Audio::load_sounds() {
    sf::SoundBuffer buffer;

    // load all sounds
    if (!buffer.loadFromFile("./assets/audio/card-fan-1.ogg"))
        return -1;
    if (!buffer.loadFromFile("./assets/audio/card-place-1.ogg"))
        return -1;
    if (!buffer.loadFromFile("./assets/audio/card-shove-1.ogg"))
        return -1;

    return 0;
}

sf::Music bgm;

int Audio::open_bgm() {
    // open background music
    if (!bgm.openFromFile("./assets/audio/star_light.wav")) {
        return -1;
    }

    return 0;
}

void Audio::play_bgm() {
    // set looping for bgm and play it.
    bgm.setLooping(true);
    bgm.play();
}