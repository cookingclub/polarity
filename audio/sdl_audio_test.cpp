#include <stdio.h>
#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

//The music that will be played
Mix_Music *music = NULL;

//The sound effects that will be used
Mix_Chunk *scratch = NULL;
Mix_Chunk *high = NULL;
Mix_Chunk *med = NULL;
Mix_Chunk *low = NULL;

int main() {
//    SDL_AudioSpec wanted;
    //Initialization
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1)
    {
        return 1;
    }

    SDL_Surface* Surface = SDL_SetVideoMode(1680, 1050, 0, SDL_HWSURFACE | SDL_FULLSCREEN);
    if(Surface == NULL)
    {
        SDL_Quit();
        return 2;
    }

    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        return false;
    }

    music = Mix_LoadMUS( "/Users/iveygman/Desktop/bathed_in_the_light.wav" );
    printf("aslkjdfhalksdjfhalsdf\n");
    std::cout << "aalksdjhfalksjdfhalskdjhf cout\n";
    std::cerr << "alsdjfhalskdjfhds cerr\n";

    //If there was a problem loading the music
    if( music == NULL )
    {
        return false;
    }
    printf("Loaded audio\n");

    //Play the music
    if( Mix_PlayMusic( music, -1 ) == -1 )
    {
        return 1;
    }

    SDL_Delay(5000);

    SDL_Quit();

    return 0;
}