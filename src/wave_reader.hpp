//
//  wave_reader.hpp
//  audio-test
//
//  Created by Bartholomew Joyce on 27/03/2018.
//  Copyright © 2018 Bartholomew Joyce All rights reserved.
//

#ifndef wave_reader_hpp
#define wave_reader_hpp

#include "buffer.hpp"

StereoBuffer read_wave_file(const char* filename);

#endif
