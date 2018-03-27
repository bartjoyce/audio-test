//
//  buffer.hpp
//  audio-test
//
//  Created by Bartholomew Joyce on 27/03/2018.
//  Copyright © 2018 Bartholomew Joyce All rights reserved.
//

#ifndef buffer_hpp
#define buffer_hpp

typedef struct {
    int sample_rate;
    int number_of_samples;
    float* samples;
} MonoBuffer;

typedef struct {
    MonoBuffer left_channel;
    MonoBuffer right_channel;
} StereoBuffer;

#endif
