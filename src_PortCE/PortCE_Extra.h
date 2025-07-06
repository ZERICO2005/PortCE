/**
 * @file
 *
 * @author "zerico2005"
 */

/**
 * @brief Add extra functionality to the PortCE version of your program by
 * using the functions in `PortCE_Extra.h`.
 */

#ifndef PORTCE_EXTRA_H
#define PORTCE_EXTRA_H

#include "PortCE.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum PortCE_Mouse_State {
    PortCE_Mouse_Button_None   = 0,
    PortCE_Mouse_Button_Left   = (1 << 0),
    PortCE_Mouse_Button_Middle = (1 << 1),
    PortCE_Mouse_Button_Right  = (1 << 2),
    PortCE_Mouse_Button_X1     = (1 << 3),
    PortCE_Mouse_Button_X2     = (1 << 4)
} PortCE_Mouse_State;

#ifdef _EZ80

    #define PortCE_get_mouse_state(...) (0)

    #define PortCE_SoundEffect(...)
    #define PortCE_PlayMusic(...)
    #define PortCE_PauseMusic()
    #define PortCE_ResumeMusic()
    #define PortCE_HaltMusic()
    #define PortCE_PlayingMusic() (false)
#else

    /**
     * @brief Returns the position of the cursor relative to the calculator screen.
     *
     * @param[out] posX position or `INT_MIN` for out of bounds
     * @param[out] posY position or `INT_MIN` for out of bounds
     * @return `uint32_t` mask of `PortCE_Mouse_State`
     */
    uint32_t PortCE_get_mouse_state(int32_t* posX, int32_t* posY);

    /* Audio */

        /**
         * @brief Plays a sound effect. Multiple sound effects can be played at a time.
         * @param track (Will eventually be trackname)
         */
        void PortCE_SoundEffect(int track);

        /**
         * @brief Plays music indefinitely. Stops any currently playing music.
         * @param track (Will eventually be trackname)
         */
        void PortCE_PlayMusic(int track);

        /**
         * @brief Plays music once. Stops any currently playing music.
         * @param track (Will eventually be trackname)
         */
        void PortCE_PlayMusicOnce(int track);

        /**
         * @brief Plays music a certain number of times. Stops any currently playing music.
         * @param track (Will eventually be trackname)
         * @param loops `0` to loop indefinitely
         */
        void PortCE_PlayMusicLoop(int track, int loops);

        /**
         * Pauses the music (Idempotent).
         */
        void PortCE_PauseMusic(void);

        /**
         * Resumes the music (Idempotent).
         */
        void PortCE_ResumeMusic(void);

        /**
         * Stops the music (Idempotent).
         */
        void PortCE_HaltMusic(void);

        /**
         * Restarts the music.
         */
        void PortCE_RewindMusic(void);

        /**
         * Returns true if music is playing or paused.
         */
        bool PortCE_PlayingMusic(void);

#endif

#ifdef __cplusplus
}
#endif

#endif /* PORTCE_EXTRA_H */
