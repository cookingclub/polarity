#pragma once

namespace Polarity {

enum PlayerColor {
    WHITE = 0,
    BLACK = 1
}

struct PlayerState {

    PlayerColor color;
    bool hasBuzzPowerup;        // placeholder for powerup 1 (makes buzzing noise)
    bool tasWoowooPowerup;      // placeholder for powerup 2 (makes woowoo noise)
    int timesDiedThisLevel;
    bool isFalling;

    PlayerState() :
        color(WHITE),
        hasBuzzPowerup(false),
        hasWoowooPowerup(false),
        timesDiedThisLevel(0),
        isFalling(false)
    {
    }
};


struct GameState {

    bool musicOn;
    bool soundEffectsOn;
    double musicMaxVolume;  // should be between 0.0 and 1.0
    int timesDiedTotal;
    int currentLevel;
    bool isPaused;

    GameState() :
        musicOn(true),
        soundEffectsOn(true),
        musicMaxVolume(1.0),
        timesDiedTotal(0),
        currentLevel(0),
        isPause(false)
    {
    }
};

}