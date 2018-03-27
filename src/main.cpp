//
//  main.cpp
//  audio-test
//
//  Created by Bartholomew Joyce on 26/03/2018.
//  Copyright © 2018 Bartholomew Joyce All rights reserved.
//

#include <ddui/app>
#include <ddui/keyboard>
#include "audio_client.hpp"
#include "wave_reader.hpp"

static StereoBuffer sound_kick;
static StereoBuffer sound_snare;
static StereoBuffer sound_clap;

void update(Context ctx) {

    // The crucial update() function!
    // All the UI work is done in here:
    //    - redrawing all the UI
    //    - responding to user input

    if (ctx.key->action == keyboard::ACTION_PRESS &&
        ctx.key->character != NULL &&
        ctx.key->character[0] == 'k') {
        keyboard::consume_key_event(ctx);
        play_sound(&sound_kick);
    }
    
    if (ctx.key->action == keyboard::ACTION_PRESS &&
        ctx.key->character != NULL &&
        ctx.key->character[0] == 's') {
        keyboard::consume_key_event(ctx);
        play_sound(&sound_snare);
    }
    
    if (ctx.key->action == keyboard::ACTION_PRESS &&
        ctx.key->character != NULL &&
        ctx.key->character[0] == 'c') {
        keyboard::consume_key_event(ctx);
        play_sound(&sound_clap);
    }

}

int main(int argc, const char** argv) {

    app::init("Audio Test");

    app::load_font_face("regular",        "assets/SFRegular.ttf");
    app::load_font_face("medium",         "assets/SFMedium.ttf");
    app::load_font_face("bold",           "assets/SFBold.ttf");
    app::load_font_face("regular-italic", "assets/SFRegularItalic.ttf");
    app::load_font_face("medium-italic",  "assets/SFMediumItalic.ttf");
    app::load_font_face("bold-italic",    "assets/SFBoldItalic.ttf");
    
    sound_kick  = read_wave_file("assets/808_kick.wav");
    sound_snare = read_wave_file("assets/808_snare.wav");
    sound_clap  = read_wave_file("assets/808_clap.wav");

    int err = init_audio_client();
    if (err) {
        return 1;
    }

    app::run(&update);

    destroy_audio_client();

    return 0;
}

