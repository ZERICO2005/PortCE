/**
 * @file
 *
 * @author "zerico2005"
 */

#include "PortCE_config/PortCE_Audio.h"

#ifdef PortCE_Enable_Audio

#include <PortCE_Common.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

static bool PortCE_SDL2_Audio_initialized = false;

static Mix_Music* Music_List[ARRAY_LENGTH(PortCE_Music_Files)];
static Mix_Chunk* SoundEffect_List[ARRAY_LENGTH(PortCE_SoundEffect_Files)];

/**
 * Loads in audio files, returns false if no audio files were found.
 */
static bool PortCE_load_audio(void) {
	size_t audio_count = 0;
	for (size_t i = 0; i < ARRAY_LENGTH(PortCE_Music_Files); i++) {
		Music_List[i] = Mix_LoadMUS(PortCE_Music_Files[i]);
		if (Music_List[i] != NULL) {
			audio_count++;
		}
	}
	for (size_t i = 0; i < ARRAY_LENGTH(PortCE_SoundEffect_Files); i++) {
		SoundEffect_List[i] = Mix_LoadWAV(PortCE_SoundEffect_Files[i]);
		if (SoundEffect_List[i] != NULL) {
			audio_count++;
		}
	}
	return (audio_count > 0) ? true : false;
}

static void PortCE_free_audio(void) {
	for (size_t i = 0; i < ARRAY_LENGTH(PortCE_SoundEffect_Files); i++) {
		Mix_FreeChunk(SoundEffect_List[i]);
	}
	for (size_t i = 0; i < ARRAY_LENGTH(PortCE_Music_Files); i++) {
		Mix_FreeMusic(Music_List[i]);
	}
}

void PortCE_terminate_sound(void) {
	PortCE_SDL2_Audio_initialized = false;
	PortCE_free_audio();
}

void PortCE_initialize_sound(void) {
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) < 0) {
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		return;
	}
	if (PortCE_load_audio() == false) {
		printf("Warning: No audio loaded\n");
		PortCE_terminate_sound();
		return;
	}
	PortCE_SDL2_Audio_initialized = true;
}

void PortCE_SoundEffect(int track) {
	if (PortCE_SDL2_Audio_initialized == false) {
		return;
	}
	if ((size_t)track >= ARRAY_LENGTH(SoundEffect_List)) {
		return;
	}
	Mix_PlayChannel(-1, SoundEffect_List[track], 0);
}

void PortCE_PlayMusicLoop(int track, int loops) {
	if (PortCE_SDL2_Audio_initialized == false) {
		return;
	}
	if ((size_t)track >= ARRAY_LENGTH(Music_List)) {
		return;
	}
	if (loops < 0) {
		loops = 0;
	}
	Mix_PlayMusic(Music_List[track], loops - 1);
}

void PortCE_PlayMusic(int track) {
	PortCE_PlayMusicLoop(track, -1);
}

void PortCE_PlayMusicOnce(int track) {
	PortCE_PlayMusicLoop(track, 0);
}

void PortCE_PauseMusic(void) {
	Mix_PauseMusic();
}
void PortCE_ResumeMusic(void) {
	Mix_ResumeMusic();
}
void PortCE_HaltMusic(void) {
	Mix_HaltMusic();
}
void PortCE_RewindMusic(void) {
	Mix_RewindMusic();
}
bool PortCE_PlayingMusic(void) {
	return (Mix_PlayingMusic() != 0) ? true : false;
}

#else

void PortCE_terminate_sound(void) { return; }
void PortCE_initialize_sound(void) { return; }
void PortCE_SoundEffect(__attribute__((unused)) int track) { return; }
void PortCE_PlayMusicLoop(__attribute__((unused)) int track, __attribute__((unused)) int loops) { return; }
void PortCE_PlayMusic(__attribute__((unused)) int track) { return; }
void PortCE_PlayMusicOnce(__attribute__((unused)) int track) { return; }

void PortCE_PauseMusic(void) { return; }
void PortCE_ResumeMusic(void) { return; }
void PortCE_HaltMusic(void) { return; }
void PortCE_RewindMusic(void) { return; }
bool PortCE_PlayingMusic(void) { return false; }

#endif