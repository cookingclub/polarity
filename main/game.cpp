#include <memory>
#include <vector>
#include "graphics/init.hpp"
#include "graphics/canvas.hpp"
#include "graphics/color.hpp"
#include "graphics/font_manager.hpp"
#include "game.hpp"
#include "util/async_io_task.hpp"
#include "world/sounds.hpp"
#include "world/player.hpp"
#include "world/world.hpp"

namespace Polarity{
Game::Game() {

}
void Game::startGame(const std::shared_ptr<Canvas> &screen_, const std::string &level) {
    screen = screen_;
    asyncIOTask = screen->asyncIOTask();
    srand(time(NULL));
    audioPlayer = Polarity::loadAudioChannels();
    resetGame(level);
}

void Game::resetGame(const std::string&level){
    playerState = std::shared_ptr<PlayerState>(new PlayerState());
    gameState.reset();
    Polarity::World::init(screen, audioPlayer, playerState, gameState, level.c_str());
}

bool Game::injectInput(SDL_Event*event) {
    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_ESCAPE) {
            return false;
        }
        // Maintain a strong reference to world so we can handle world changes.
        std::shared_ptr<World> currentWorldRef (world);
        world->keyEvent(event->key.keysym.sym, true);
    } else if (event->type == SDL_KEYUP) {
        keyUps.push_back(event->key.keysym.sym);
    }
#if SDL_MAJOR_VERSION < 2
    if (event->type == SDL_VIDEORESIZE) {
        //event->type == SDL_WINDOWEVENT_RESIZED // <- for SDL2, which we don't need
        screen->resize(event->resize.w, event->resize.h);
    }
#endif
    return true;
}

void Game::drawFrame() {
    drawFrame("Polarity version 0.1 alpha");
}

void Game::drawFrame(const std::string &title_text) {
    screen->beginFrame();
    screen->clear();
    world->draw(screen.get());
    screen->fontManager().drawText(
        screen.get(), 12, 2, "DroidSerif", 12, Color(128,128,64, 255),
        title_text);
    screen->endFrame();
}

void Game::performTick() {
    // Maintain a strong reference to world so we can handle world changes.
    std::shared_ptr<World> currentWorldRef (world);
    world->tick();
    // all key up have to happen after key downs so we get a full tick of downs
    for (auto &key : keyUps) {
      world->keyEvent(key, false);
    }
    keyUps.clear();
}

void Game::stopGame(){
    if (asyncIOTask) {
        asyncIOTask->quiesce();
    }
    world.reset();    
}

const std::shared_ptr<World>& Game::pinWorld() const{
    return world;
}

void Game::stopGameAndCleanupGraphicsAndEvents() {
    stopGame();
    std::weak_ptr<Canvas> wscreen(screen);
    screen.reset();
    asyncIOTask->quiesce();
    asyncIOTask.reset();
    if (!wscreen.lock()) {
        destroyGraphicsSystem();
    }else {
        std::cerr << "Not destroying graphics system because live reference still exists\n";
    }
}

Game::~Game() {
}

Game& Game::getSingleton() {
    static Game game;
    return game;
}
}
