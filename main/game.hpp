#include <vector>
#include <memory>
namespace Polarity{
class AudioChannelPlayer;
struct PlayerState;
class Canvas;
class AsyncIOTask;
struct GameState;



class Game {
public:
    Game();
    void startGame(const std::shared_ptr<Canvas> &screen, const std::string &level);
    void resetGame(const std::string &level);
    bool injectInput(SDL_Event*);
    void performTick();
    void stopGame();
    void stopGameAndCleanupGraphicsAndEvents();
    ~Game();
    AsyncIOTask& getAsyncIOTask() {
        return *asyncIOTask;
    }
    static Game& getSingleton();
private:
    std::shared_ptr<Canvas> screen;
    std::shared_ptr<AsyncIOTask> asyncIOTask;
    std::shared_ptr<AudioChannelPlayer> audioPlayer;
    std::shared_ptr<PlayerState> playerState;
    std::shared_ptr<GameState> gameState;
    std::vector<int> keyUps;

};
}
