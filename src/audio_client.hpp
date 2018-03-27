//
//  audio_client.hpp
//  audio-test
//
//  Created by Bartholomew Joyce on 27/03/2018.
//  Copyright © 2018 Bartholomew Joyce All rights reserved.
//

#ifndef audio_client_hpp
#define audio_client_hpp

#include "buffer.hpp"

int init_audio_client();
void play_sound(StereoBuffer* buffer);
void destroy_audio_client();

#endif
