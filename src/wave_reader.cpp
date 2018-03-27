//
//  wave_reader.cpp
//  audio-test
//
//  Created by Bartholomew Joyce on 27/03/2018.
//  Copyright © 2018 Bartholomew Joyce All rights reserved.
//

#include "wave_reader.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define WAVE_FORMAT_PCM 1

static int read_word(char* buffer, FILE* source_file);
static int read_dword(char* buffer, FILE* source_file);
static uint32_t read_uint32(const char* bytes);
static uint16_t read_uint16(const char* bytes);
static float convert_sample(uint16_t value);
static int read_wave_file_header(StereoBuffer* audio_buffer, int* channels, FILE* source_file);

StereoBuffer read_wave_file(const char* filename) {
    FILE* source_file;
    int err;
    StereoBuffer audio_buffer;
    int channels;

    // Open the file
    source_file = fopen(filename, "rb");
    if (source_file == NULL) {
        fclose(source_file);
        return StereoBuffer();
    }

    // Read the wave file header
    err = read_wave_file_header(&audio_buffer, &channels, source_file);
    if (err) {
        fclose(source_file);
        return StereoBuffer();
    }

    // Allocate the left and right sample buffers
    audio_buffer.left_channel.samples = new float[audio_buffer.left_channel.number_of_samples];
    audio_buffer.right_channel.samples = new float[audio_buffer.right_channel.number_of_samples];
    if (audio_buffer.left_channel.samples == NULL) {
        printf("Not enough memory\n");
        fclose(source_file);
        return StereoBuffer();
    }
    if (audio_buffer.right_channel.samples == NULL) {
        printf("Not enough memory\n");
        free(audio_buffer.left_channel.samples);
        fclose(source_file);
        return StereoBuffer();
    }

    // Read in all the samples
    if (channels == 1) {
        // Mono

        char bytes[2];
        float* left_sample_ptr = audio_buffer.left_channel.samples;
        float* right_sample_ptr = audio_buffer.right_channel.samples;

        for (int i = audio_buffer.left_channel.number_of_samples; i > 0; --i) {
            err = read_word(bytes, source_file);
            if (err) {
                free(audio_buffer.left_channel.samples);
                free(audio_buffer.right_channel.samples);
                fclose(source_file);
                return StereoBuffer();
            }
            *left_sample_ptr++ = convert_sample(read_uint16(bytes));
            *right_sample_ptr++ = convert_sample(read_uint16(bytes));
        }

    } else {
        // Stereo

        char bytes[2];
        float* left_sample_ptr = audio_buffer.left_channel.samples;
        float* right_sample_ptr = audio_buffer.right_channel.samples;

        for (int i = audio_buffer.left_channel.number_of_samples; i > 0; --i) {
            // Left
            err = read_word(bytes, source_file);
            if (err) {
                free(audio_buffer.left_channel.samples);
                free(audio_buffer.right_channel.samples);
                fclose(source_file);
                return StereoBuffer();
            }
            *left_sample_ptr++ = convert_sample(read_uint16(bytes));

            // Right
            err = read_word(bytes, source_file);
            if (err) {
                free(audio_buffer.left_channel.samples);
                free(audio_buffer.right_channel.samples);
                fclose(source_file);
                return StereoBuffer();
            }
            *right_sample_ptr++ = convert_sample(read_uint16(bytes));
        }
    }

    fclose(source_file);

    return audio_buffer;
}

int read_wave_file_header(StereoBuffer* audio_buffer, int* channels, FILE* source_file) {
    char bytes[4];
    int err;

    // Check for the "RIFF" chunk code
    err = read_dword(bytes, source_file);
    if (err || strncmp(bytes, "RIFF", 4) != 0) {
        return 1;
    }

    // Read the chunk size
    err = read_dword(bytes, source_file);
    if (err) {
        return 1;
    }

    // Check for the "WAVE" chunk code
    err = read_dword(bytes, source_file);
    if (err || strncmp(bytes, "WAVE", 4) != 0) {
        return 1;
    }

    // Check for the "fmt " chunk code
    err = read_dword(bytes, source_file);
    if (err || strncmp(bytes, "fmt ", 4) != 0) {
        return 1;
    }

    // Confirm that the "fmt " chunk is 16 bytes in size
    err = read_dword(bytes, source_file);
    if (err || read_uint32(bytes) != 16) {
        return 1;
    }

    // Confirm that the PCM is the format
    err = read_word(bytes, source_file);
    if (err || read_uint16(bytes) != WAVE_FORMAT_PCM) {
        return 1;
    }

    // Read the number of channels
    err = read_word(bytes, source_file);
    if (err || read_uint16(bytes) < 1 || read_uint16(bytes) > 2) {
        return 1;
    }
    *channels = read_uint16(bytes);

    // Read the sample rate
    err = read_dword(bytes, source_file);
    if (err) {
        return 1;
    }
    audio_buffer->left_channel.sample_rate = audio_buffer->right_channel.sample_rate = read_uint32(bytes);

    // Read the average bytes per second
    err = read_dword(bytes, source_file);
    if (err) {
        return 1;
    }

    // Confirm that the block alignment is 2 * channels
    err = read_word(bytes, source_file);
    if (err || read_uint16(bytes) != 2 * *channels) {
        return 1;
    }

    // Confirm that the audio samples are 16-bit
    err = read_word(bytes, source_file);
    if (err || read_uint16(bytes) != 16) {
        return 1;
    }

    // Check for the "data" chunk code
    err = read_dword(bytes, source_file);
    if (err || strncmp(bytes, "data", 4) != 0) {
        return 1;
    }

    // Read the chunk size (number of samples)
    err = read_dword(bytes, source_file);
    if (err) {
        return 1;
    }
    int chunk_size = read_uint32(bytes);
    audio_buffer->left_channel.number_of_samples = audio_buffer->right_channel.number_of_samples = chunk_size / (2 * *channels);
    return 0;
}

int read_word(char* buffer, FILE* source_file) {
    int number_of_bytes = fread(buffer, 1, 2, source_file);
    if (number_of_bytes != 2) {
        return 1;
    } else {
        return 0;
    }
}
int read_dword(char* buffer, FILE* source_file) {
    int number_of_bytes = fread(buffer, 1, 4, source_file);
    if (number_of_bytes != 4) {
        return 1;
    } else {
        return 0;
    }
}
uint32_t read_uint32(const char* bytes) {
    return (unsigned char)bytes[0] + 256*(unsigned char)bytes[1] + 256*256*(unsigned char)bytes[2] + 256*256*256*(unsigned char)bytes[3];
}
uint16_t read_uint16(const char* bytes) {
    return (unsigned char)bytes[0] + 256*(unsigned char)bytes[1];
}
float convert_sample(uint16_t value) {
    int16_t signed_int = *(int16_t*)(&value);
    return (float)signed_int / (float)INT16_MAX;
}
