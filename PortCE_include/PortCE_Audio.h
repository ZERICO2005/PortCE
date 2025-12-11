#ifndef PORTCE_AUDIO_H
#define PORTCE_AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif



/* Uncomment this to add music/audio to your PortCE project. */
// #define PortCE_Enable_Audio



#ifdef PortCE_Enable_Audio

/* `.wav`, `.mp3`, and probably other file formats are supported. */

/* Only one music track can be played at a time. Can be paused/resumed/halted */
static const char* const PortCE_Music_Files[] = {
    // "./audio/example_music_track.mp3",
};

/* Multiple sound effects can be played at a time. */
static const char* const PortCE_SoundEffect_Files[] = {
    // "./audio/example_sound_effect.wav",
};

#endif

#ifdef __cplusplus
}
#endif

#endif /* PORTCE_AUDIO_H */
