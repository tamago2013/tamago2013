/*

  卵掛け御飯2012で使っていたsound playerを2013に移植

  2013-11-13 MTM

 */

//===================================================
// include
//===================================================
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <csignal>
#include <ssm.hpp>

#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

#include "gnd-timer.hpp"

#include "ssm-sound.hpp"

//===================================================
// ssm-data / finalize
//===================================================
SSMApi< SoundType > ssm_sound;

void finalizeSSM()
{
    if( ssm_sound.isOpen() )
    {
        ssm_sound.close();
        std::cout << ssm_sound.getStreamName() << " closed.\n";
    }

    endSSM();
    std::cout << "ssm closed.\n";
}

//===================================================
// ctrl-c
//===================================================
bool is_shutting_down = false;
void ctrl_c(int aStatus)
{
    is_shutting_down = true;
    finalizeSSM();
    signal(SIGINT, NULL);
}


//===================================================
// main
//===================================================
int main(int argc, char **argv)
{

    signal( SIGINT , ctrl_c );

    std::cout << "++++++++++++++++++++++++++++++++++++++\n"
              << "効果音:魔王魂\n"
              << "http://maoudamashii.jokersounds.com/\n"
              << "音声:Open J Talk\n"
              << "http://open-jtalk.sp.nitech.ac.jp/\n"
              << "++++++++++++++++++++++++++++++++++++++\n";

    //-------------------------------------
    // SDL Mixerの初期化
    if( (SDL_Init( SDL_INIT_AUDIO ) == -1) )
    {
        std::cerr << "[\033[31mERROR\033[39m]: failed to initialize SDL-Mixer!!\n";
        return 1;
    }
    if( Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT,2,1024) < 0 )
    {
        std::cerr << "[\033[31mERROR\033[39m]: failed to open audio!!\n";
        SDL_Quit();
        return 1;
    }

    //-------------------------------------
    // SSMの初期化
    if( !initSSM() )
    {
        std::cerr << "[\033[31mERROR\033[39m]: failed to initialize SSM!!\n";
        return 1;
    }
    if( !ssm_sound.open( SNAME_SOUND , SID_SOUND , SSM_READ ) )
    {
        std::cerr << "[\033[31mWARNING\033[39m]: failed to open \"" << ssm_sound.getStreamName() << "\"!!( run with talking mode )\n";
    }

    //-------------------------------------
    // Mix_Chunkの初期化
    std::vector<Mix_Chunk *> sounds(14);
    sounds[0] = Mix_LoadWAV("../sounds/janjan.wav");
    sounds[1] = Mix_LoadWAV("../sounds/buinn.wav");
    sounds[2] = Mix_LoadWAV("../sounds/car_stop.wav");
    sounds[3] = Mix_LoadWAV("../sounds/goal.wav");
    sounds[4] = Mix_LoadWAV("../sounds/jann.wav");
    sounds[5] = Mix_LoadWAV("../sounds/kin.wav");
    sounds[6] = Mix_LoadWAV("../sounds/kyodaika.wav");
    sounds[7] = Mix_LoadWAV("../sounds/pi__.wav");
    sounds[8] = Mix_LoadWAV("../sounds/pin__.wav");
    sounds[9] = Mix_LoadWAV("../sounds/pinpon.wav");
    sounds[10] = Mix_LoadWAV("../sounds/pinpon_2.wav");
    sounds[11] = Mix_LoadWAV("../sounds/pinpon_3.wav");
    sounds[12] = Mix_LoadWAV("../sounds/piroriro.wav");
    sounds[13] = Mix_LoadWAV("../sounds/piroon.wav");

    Mix_Chunk* default_sound = Mix_LoadWAV("../sounds/talk.wav"); //readNew出来なかったときに一定間隔で読むやつ

    //-------------------------------------
    // タイマの設定
    gnd::inttimer clock_talk;
    clock_talk.begin( CLOCK_REALTIME , 20.0 , -1.0 );

    //-------------------------------------
    // メインループ
    std::cout << "mainloop started.\n";
    while( !is_shutting_down )
    {
        usleepSSM(5000);

        if( !ssm_sound.isOpen() || !ssm_sound.readNew() )
        {
            if( clock_talk.clock() )
            {
                Mix_PlayChannel( 0 , default_sound , 0 );
                std::cerr << "sound = talk(default)\n";
            }

            continue;
        }

        if( ssm_sound.data < 0 || ssm_sound.data >= (int)sounds.size() )
        {
            continue;
        }

        size_t sound_index = (size_t)ssm_sound.data;

        Mix_PlayChannel( 0 , sounds[ sound_index ] , 0 );
        std::cerr << "sound = " << sound_index << "\n";
        sleepSSM(0.5);
    }

    //-------------------------------------
    // 終了処理
    for(size_t i=0; i<sounds.size(); i++)
    {
        Mix_FreeChunk(sounds[i]);
    }

    Mix_CloseAudio();
    SDL_Quit();
    std::cout << "SDL finalized.\n";

    finalizeSSM();

    return 0;
}
