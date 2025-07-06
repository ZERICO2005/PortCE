#ifndef HID_HID_H
#define HID_HID_H

#include <stdbool.h>
#include <usbdrvce.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HID_SUCCESS,
    HID_IGNORE,
    HID_ERROR_SYSTEM,
    HID_ERROR_INVALID_PARAM,
    HID_ERROR_SCHEDULE_FULL,
    HID_ERROR_NO_DEVICE,
    HID_ERROR_NO_MEMORY,
    HID_ERROR_NOT_SUPPORTED,
    HID_ERROR_TIMEOUT,
    HID_ERROR_FAILED,
    HID_NO_INTERFACE,
    HID_USER_ERROR = 100
} hid_error_t;

typedef enum {
    HID_NON_BOOT = 0,
    HID_BOOT     = 1
} hid_subclass_t;

typedef enum {
    HID_NONE     = 0,
    HID_KEYBOARD = 1,
    HID_MOUSE    = 2
} hid_device_type_t;

enum {
    LED_NUM_LOCK    = (1 << 0),
    LED_CAPS_LOCK   = (1 << 1),
    LED_SCROLL_LOCK = (1 << 2),
    LED_COMPOSE     = (1 << 3),
    LED_KANA        = (1 << 4)
};
typedef uint8_t hid_leds_t;

typedef enum {
    HID_MOUSE_LEFT,
    HID_MOUSE_RIGHT,
    HID_MOUSE_MIDDLE,
    HID_MOUSE_BUTTON_4,
    HID_MOUSE_BUTTON_5,
    HID_MOUSE_BUTTON_6,
    HID_MOUSE_BUTTON_7
} hid_mouse_button_t;

typedef struct {
    uint8_t modifiers;
    uint8_t reserved_1;
    uint8_t pressed[6];
} hid_keyboard_report_t;

typedef struct {
    uint8_t buttons;
    int8_t x;
    int8_t y;
} hid_mouse_report_t;

typedef union {
    hid_keyboard_report_t kb;
    hid_mouse_report_t mouse;
} hid_report_t;

typedef enum {
    HID_EVENT_KEY_DOWN,
    HID_EVENT_KEY_UP,
    HID_EVENT_MODIFIER_DOWN,
    HID_EVENT_MODIFIER_UP,
    HID_EVENT_MOUSE_DOWN,
    HID_EVENT_MOUSE_UP,
    HID_EVENT_MOUSE_MOVE,

    HID_EVENT_DISCONNECTED
} hid_event_t;

typedef struct HID_State hid_state_t;

/**
 * Type of the function to be called when a HID event occurs
 * @param event Event type
 * @param code Key code, modifier code, or mouse button
 * @param callback_data Opaque pointer passed to \c hid_SetEventCallback
 */
typedef void (*hid_callback_t)(
    hid_state_t *hid, hid_event_t event, uint8_t code, void *callback_data
);

struct HID_State {
    bool active;
    bool stopped;
    uint8_t type;
    usb_device_t dev;
    usb_endpoint_t in;
    usb_endpoint_t out;
    uint8_t interface;
    uint8_t report_size;
    hid_report_t report;
    hid_report_t last_report;
    int24_t delta_x;
    int24_t delta_y;
    hid_callback_t callback;
    void *callback_data;
};

/**
 * Start listening to an HID interface.
 * @note Must be called before @param hid can be used with other functions.
 * @param hid HID state from \c hid_GetNext
 * @param dev USB device
 * @param interface Interface to use
 * @return HID_SUCCESS if initialization succeeded
 */
hid_error_t hid_Init(hid_state_t *hid, usb_device_t dev, uint8_t interface);

/**
 * Stop listening on an HID interface
 * @note Call before freeing \c hid
 */
void hid_Stop(hid_state_t *hid);

#define HID_IDLE_TIME_INFINITE 0

/**
 * Set the idle time between updates for the HID interface.
 * @param time Time in ms (up to 1023 ms)
 * @return HID_SUCCESS if idle time was set
 */
hid_error_t hid_SetIdleTime(hid_state_t *hid, uint24_t time);

/**
 * Check if a key is down
 * @param key_code Key to check
 * @return true if key is down, false otherwise
 */
bool hid_KbdIsKeyDown(hid_state_t *hid, uint8_t key_code);

/**
 * Check if a modifier key (Ctrl, Alt, Shift, etc.) is down
 * @param modifier Modifier to check
 * @return true if modifier key is down, false otherwise
 */
bool hid_KbdIsModifierDown(hid_state_t *hid, uint8_t modifier);

/**
 * Set the keyboard LEDs
 * @param leds New LED bitmap
 * @return HID_SUCCESS if LEDs were set successfully
 */
hid_error_t hid_KbdSetLEDs(hid_state_t *hid, hid_leds_t leds);

/**
 * Check if a mouse button is down
 * @param button Button to check
 * @return true if button is down, false otherwise
 */
bool hid_MouseIsButtonDown(hid_state_t *hid, hid_mouse_button_t button);

/**
 * Get the change in mouse position since the
 * last time this function was called
 * @param x Returns the change in x position
 * @param y Returns the change in y position
 */
void hid_MouseGetDeltas(hid_state_t *hid, int24_t *x, int24_t *y);

/**
 * Set the HID event handler function for an interface
 * @param callback Event handler function
 * @param callback_data Opaque pointer passed to event handler
 */
void hid_SetEventCallback(hid_state_t *hid, hid_callback_t callback, void *callback_data);

#ifdef __cplusplus
}
#endif

#endif //HID_HID_H
