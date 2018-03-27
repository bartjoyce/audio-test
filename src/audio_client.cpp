//
//  audio_client.cpp
//  audio-test
//
//  Created by Bartholomew Joyce on 27/03/2018.
//  Copyright © 2018 Bartholomew Joyce All rights reserved.
//

#include "audio_client.hpp"

#include <soundio/soundio.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

static struct SoundIo* soundio;
static struct SoundIoDevice* device;
static struct SoundIoOutStream* outstream;

static StereoBuffer* buffer_to_play = NULL;
static int samples_played = 0;

static void write_sample_s16ne(char *ptr, float sample) {
    int16_t *buf = (int16_t *)ptr;
    double range = (float)INT16_MAX - (float)INT16_MIN;
    double val = sample * range / 2.0;
    *buf = val;
}

static void write_sample_s32ne(char *ptr, float sample) {
    int32_t *buf = (int32_t *)ptr;
    double range = (float)INT32_MAX - (float)INT32_MIN;
    double val = sample * range / 2.0;
    *buf = val;
}

static void write_sample_float32ne(char *ptr, float sample) {
    float *buf = (float *)ptr;
    *buf = sample;
}

static void write_sample_float64ne(char *ptr, float sample) {
    double *buf = (double *)ptr;
    *buf = (double)sample;
}

static void (*write_sample)(char *ptr, float sample);
static const double PI = 3.14159265358979323846264338328;
static double seconds_offset = 0.0;
static volatile bool want_pause = false;
static void write_callback(struct SoundIoOutStream *outstream, int frame_count_min, int frame_count_max) {
    float float_sample_rate = outstream->sample_rate;
    float seconds_per_frame = 1.0 / float_sample_rate;
    struct SoundIoChannelArea *areas;
    int err;

    int frames_left = frame_count_max;

    for (;;) {
        int frame_count = frames_left;
        if ((err = soundio_outstream_begin_write(outstream, &areas, &frame_count))) {
            fprintf(stderr, "unrecoverable stream error: %s\n", soundio_strerror(err));
            exit(1);
        }

        if (!frame_count)
            break;

        const struct SoundIoChannelLayout *layout = &outstream->layout;
        if (layout->channel_count < 2) {
            break;
        }
        
        if (buffer_to_play == NULL) {
            for (int frame = 0; frame < frame_count; frame += 1) {
                write_sample(areas[0].ptr, 0.0);
                write_sample(areas[1].ptr, 0.0);
                areas[0].ptr += areas[0].step;
                areas[1].ptr += areas[1].step;
            }
        } else {
            int samples_left = buffer_to_play->left_channel.number_of_samples - samples_played;
            int num_frames = frame_count < samples_left ? frame_count : samples_left;
            
            for (int frame = 0; frame < num_frames; frame += 1) {
                write_sample(areas[0].ptr, buffer_to_play->left_channel.samples[samples_played + frame]);
                write_sample(areas[1].ptr, buffer_to_play->right_channel.samples[samples_played + frame]);
                areas[0].ptr += areas[0].step;
                areas[1].ptr += areas[1].step;
            }
            for (int frame = num_frames; frame < frame_count; frame += 1) {
                write_sample(areas[0].ptr, 0.0);
                write_sample(areas[1].ptr, 0.0);
                areas[0].ptr += areas[0].step;
                areas[1].ptr += areas[1].step;
            }
            
            samples_played += num_frames;
            if (samples_left <= num_frames) {
                buffer_to_play = NULL;
                samples_played = 0;
            }
        }

        if ((err = soundio_outstream_end_write(outstream))) {
            if (err == SoundIoErrorUnderflow)
                return;
            fprintf(stderr, "unrecoverable stream error: %s\n", soundio_strerror(err));
            exit(1);
        }

        frames_left -= frame_count;
        if (frames_left <= 0)
            break;
    }

    soundio_outstream_pause(outstream, want_pause);
}

static void underflow_callback(struct SoundIoOutStream *outstream) {
    static int count = 0;
    fprintf(stderr, "underflow %d\n", count++);
}

int init_audio_client() {
    char *stream_name = NULL;
    double latency = 0.0;
    int sample_rate = 0;

    soundio = soundio_create();
    if (!soundio) {
        fprintf(stderr, "out of memory\n");
        return 1;
    }

    int err = soundio_connect(soundio);

    if (err) {
        fprintf(stderr, "Unable to connect to backend: %s\n", soundio_strerror(err));
        return 1;
    }

    fprintf(stderr, "Backend: %s\n", soundio_backend_name(soundio->current_backend));

    soundio_flush_events(soundio);

    int device_index = soundio_default_output_device_index(soundio);
    if (device_index < 0) {
        fprintf(stderr, "Output device not found\n");
        return 1;
    }

    device = soundio_get_output_device(soundio, device_index);
    if (!device) {
        fprintf(stderr, "out of memory\n");
        return 1;
    }

    fprintf(stderr, "Output device: %s\n", device->name);

    if (device->probe_error) {
        fprintf(stderr, "Cannot probe device: %s\n", soundio_strerror(device->probe_error));
        return 1;
    }

    outstream = soundio_outstream_create(device);
    if (!outstream) {
        fprintf(stderr, "out of memory\n");
        return 1;
    }

    outstream->write_callback = write_callback;
    outstream->underflow_callback = underflow_callback;
    outstream->name = stream_name;
    outstream->software_latency = latency;
    outstream->sample_rate = sample_rate;

    if (soundio_device_supports_format(device, SoundIoFormatFloat32NE)) {
        outstream->format = SoundIoFormatFloat32NE;
        write_sample = write_sample_float32ne;
    } else if (soundio_device_supports_format(device, SoundIoFormatFloat64NE)) {
        outstream->format = SoundIoFormatFloat64NE;
        write_sample = write_sample_float64ne;
    } else if (soundio_device_supports_format(device, SoundIoFormatS32NE)) {
        outstream->format = SoundIoFormatS32NE;
        write_sample = write_sample_s32ne;
    } else if (soundio_device_supports_format(device, SoundIoFormatS16NE)) {
        outstream->format = SoundIoFormatS16NE;
        write_sample = write_sample_s16ne;
    } else {
        fprintf(stderr, "No suitable device format available.\n");
        return 1;
    }

    if ((err = soundio_outstream_open(outstream))) {
        fprintf(stderr, "unable to open device: %s", soundio_strerror(err));
        return 1;
    }

    fprintf(stderr, "Software latency: %f\n", outstream->software_latency);

    if (outstream->layout_error)
        fprintf(stderr, "unable to set channel layout: %s\n", soundio_strerror(outstream->layout_error));

    if ((err = soundio_outstream_start(outstream))) {
        fprintf(stderr, "unable to start device: %s\n", soundio_strerror(err));
        return 1;
    }

    return 0;
}

void play_sound(StereoBuffer* buffer) {
    buffer_to_play = buffer;
    samples_played = 0;
}

void destroy_audio_client() {
    soundio_outstream_destroy(outstream);
    soundio_device_unref(device);
    soundio_destroy(soundio);
}
