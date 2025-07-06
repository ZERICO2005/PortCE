#include <PortCE_Common.h>
#include "PortCE_include/hiddrvce/hiddrvce.h"
#include "PortCE_include/usbdrvce/usbdrvce.h"

#include <string.h>
#include <debug.h>

#if 0

/* Internal functions declaration */
static usb_error_t hid_ReportCallback(
    usb_endpoint_t pEndpoint, usb_transfer_status_t status,
    size_t size, hid_state_t *hid
);

static bool hid_IsKeyDownInReport(
    hid_keyboard_report_t *report, uint8_t key_code
);

hid_error_t hid_SetProtocol(hid_state_t *hid, bool report);

static usb_error_t hid_ReportCallback(
    usb_endpoint_t pEndpoint, usb_transfer_status_t status,
    size_t size, hid_state_t *hid
) {
    if(status) {
        printf("hiddrvce: callback called with status %u\n", status);
        if(status & USB_TRANSFER_NO_DEVICE) {
            hid->active = false;
            hid->stopped = true;
            if(hid->callback) {
                hid->callback(hid, HID_EVENT_DISCONNECTED, 0, hid->callback_data);
            }
            return USB_SUCCESS;
        }
    }
    if(!hid->active) {
        hid->stopped = true;
        return USB_SUCCESS;
    }
    if(hid->type == HID_KEYBOARD) {
        if(hid->callback) {
            uint8_t i;

            /* Check for keydowns */
            for(i = 0; i < sizeof(hid->report.kb.pressed); i++) {
                uint8_t key = hid->report.kb.pressed[i];
                if(!key || key == 1) { continue; }
                if(!hid_IsKeyDownInReport(&hid->last_report.kb, key)) {
                    hid->callback(
                        hid, HID_EVENT_KEY_DOWN, key,
                        hid->callback_data
                    );
                }
            }

            /* Check for keyups */
            for(i = 0; i < sizeof(hid->last_report.kb.pressed); i++) {
                uint8_t key = hid->last_report.kb.pressed[i];
                if(!key) { continue; }
                if(!hid_IsKeyDownInReport(&hid->report.kb, key)) {
                    hid->callback(
                        hid, HID_EVENT_KEY_UP, key,
                        hid->callback_data
                    );
                }
            }

            /* Check for modifiers */
            for(i = 0; i < 8; i++) {
                uint8_t mod = 1 << i;
                if(hid->report.kb.modifiers & mod) {
                    if(!(hid->last_report.kb.modifiers & mod)) {
                        hid->callback(
                            hid, HID_EVENT_MODIFIER_DOWN, mod,
                            hid->callback_data
                        );
                    }
                } else {
                    if(hid->last_report.kb.modifiers & mod) {
                        hid->callback(
                            hid, HID_EVENT_MODIFIER_UP, mod,
                            hid->callback_data
                        );
                    }
                }
            }
        }
    } else {
        uint8_t i;
        hid->delta_x += hid->report.mouse.x;
        hid->delta_y += hid->report.mouse.y;

        if(hid->callback) {
            if(hid->report.mouse.x || hid->report.mouse.y) {
                hid->callback(hid, HID_EVENT_MOUSE_MOVE, 0, hid->callback_data);
            }

            /* Check mouse buttons */
            for(i = 0; i < 8; i++) {
                uint8_t button = 1 << i;
                if(hid->report.mouse.buttons & button) {
                    if(!(hid->last_report.mouse.buttons & button)) {
                        hid->callback(
                            hid, HID_EVENT_MOUSE_DOWN, i,
                            hid->callback_data
                        );
                    }
                } else {
                    if(hid->last_report.mouse.buttons & button) {
                        hid->callback(
                            hid, HID_EVENT_MOUSE_UP, i,
                            hid->callback_data
                        );
                    }
                }
            }
        }
    }

    memcpy(&hid->last_report, &hid->report, sizeof(hid->report));

    usb_ScheduleTransfer(
        pEndpoint, &hid->report, hid->report_size,
        (usb_transfer_callback_t) hid_ReportCallback, hid
    );
    return USB_SUCCESS;
}

#define RET_ERROR(a) do { \
    error = (hid_error_t)a; \
    if(error) { return error; } \
} while (0)

hid_error_t hid_Init(hid_state_t *hid, usb_device_t dev, uint8_t interface) {
    hid_error_t error;
    uint8_t config;
    union {
        uint8_t bytes[256];
        usb_configuration_descriptor_t conf;
        usb_descriptor_t descriptor;
    } conf_desc;
    size_t config_length;
    const usb_descriptor_t *search_pos, *end;
    bool interface_found = false;
    const usb_descriptor_t *next;

    hid->dev = dev;
    hid->active = false;
    hid->stopped = true;
    hid->in = NULL;
    hid->out = NULL;
    hid->interface = interface;
    hid->type = 0;
    hid->callback = NULL;
    hid->callback_data = NULL;
    hid->delta_x = 0;
    hid->delta_y = 0;
    hid->report_size = sizeof(hid->report);

    if(!(usb_GetDeviceFlags(dev) & USB_IS_ENABLED)) {
        usb_ResetDevice(dev);
        usb_WaitForEvents();
        printf("hiddrvce: reset device\n");
    }

    RET_ERROR(usb_GetConfiguration(dev, &config));
    printf("hiddrvce: got config %u\n", config);

    if(config) {
        RET_ERROR(usb_GetDescriptor(
            dev, USB_CONFIGURATION_DESCRIPTOR,
            config - 1, &conf_desc, 256, NULL
        ));
        config_length = conf_desc.conf.wTotalLength;
    } else {
        RET_ERROR(usb_GetDescriptor(
            dev, USB_CONFIGURATION_DESCRIPTOR,
            0, &conf_desc, 256, NULL
        ));
        config_length = conf_desc.conf.wTotalLength;
        if(config_length > 256) {
            printf("hiddrvce: config too long\n");
            return HID_ERROR_NO_MEMORY;
        }

        if(interface >= conf_desc.conf.bNumInterfaces) {
            printf("hiddrvce: not enough interfaces\n");
            return HID_NO_INTERFACE;
        }

        RET_ERROR(usb_SetConfiguration(dev, &conf_desc.conf, config_length));
        printf("hiddrvce: set config\n");
    }

        end = (usb_descriptor_t*)&conf_desc.bytes[config_length];

    for(search_pos = &conf_desc.descriptor; search_pos < end; search_pos = next) {
        next = (usb_descriptor_t *) ((uint8_t*)search_pos + search_pos->bLength);
        switch(search_pos->bDescriptorType) {
            case USB_INTERFACE_DESCRIPTOR: {
                usb_interface_descriptor_t *desc = (usb_interface_descriptor_t *) search_pos;
                if(interface_found) { goto found; }
                if(desc->bInterfaceNumber != interface) { break; }
                if(desc->bInterfaceClass != USB_HID_CLASS) {
                    return HID_NO_INTERFACE;
                }
                if(desc->bInterfaceSubClass != HID_BOOT) {
                    return HID_NO_INTERFACE;
                }
                interface_found = true;
                hid->type = desc->bInterfaceProtocol;
                break;
            }

            case USB_ENDPOINT_DESCRIPTOR: {
                usb_endpoint_descriptor_t *desc = (usb_endpoint_descriptor_t *) search_pos;
                if(!interface_found) { break; }
                if(desc->bEndpointAddress & 0x80) {
                    /* IN endpoint */
                    hid->in = usb_GetDeviceEndpoint(
                        dev, desc->bEndpointAddress
                    );
                } else {
                    /* OUT endpoint */
                    hid->out = usb_GetDeviceEndpoint(
                        dev, desc->bEndpointAddress
                    );
                }
                break;
            }

            default:
                break;
        }
        search_pos = next;
    }

    if(!interface_found) { return HID_NO_INTERFACE; }

    found:

    memset(&hid->report, 0, sizeof(hid->report));
    memset(&hid->last_report, 0, sizeof(hid->last_report));
    error = hid_SetProtocol(hid, 0);
    if(error) {
        printf("hiddrvce: error %u on protocol set\n", error);
        return error;
    }
    hid_SetIdleTime(hid, 1);
    if(hid->in) {
        error = (hid_error_t) usb_ScheduleTransfer(
            hid->in, &hid->report,
            hid->report_size,
            (usb_transfer_callback_t) hid_ReportCallback,
            hid
        );
        if(error) {
            printf("hiddrvce: error %u on initial schedule\n", error);
            return error;
        }
    }
    hid->active = true;
    hid->stopped = false;
    return HID_SUCCESS;
}

void hid_Stop(hid_state_t *hid) {
    if(!hid->active) { return; }
    hid->stopped = false;
    hid->active = false;
    while(!hid->stopped) { usb_WaitForEvents(); }
}

hid_error_t hid_SetProtocol(hid_state_t *hid, bool report) {
    usb_control_setup_t setup = {0x21, 0x0B, 0, 0, 0};
    setup.wIndex = hid->interface;
    setup.wValue = report;
    return (hid_error_t) usb_DefaultControlTransfer(
        hid->dev, &setup, NULL, 50, NULL
    );
}

hid_error_t hid_SetIdleTime(hid_state_t *hid, uint24_t time) {
    usb_control_setup_t setup = {0x21, 0x0A, 0, 0, 0};

    setup.wIndex = hid->interface;

    if(time >= 1024) { time = 1023; }
    time &= 0x03FD;
    setup.wValue = time << 6;

    return (hid_error_t) usb_DefaultControlTransfer(
        hid->dev, &setup, NULL, 50, NULL
    );
}

static bool hid_IsKeyDownInReport(
    hid_keyboard_report_t *report, uint8_t key_code
) {
    uint8_t i;
    for(i = 0; i < sizeof(report->pressed); i++) {
        if(report->pressed[i] == key_code) {
            return true;
        }
    }
    return false;
}

bool hid_KbdIsKeyDown(hid_state_t *hid, uint8_t key_code) {
    uint8_t i;
    if(hid->type != HID_KEYBOARD) { return false; }

    return hid_IsKeyDownInReport(&hid->report.kb, key_code);
}

bool hid_KbdIsModifierDown(hid_state_t *hid, uint8_t modifier) {
    if(hid->type != HID_KEYBOARD) { return false; }

    return hid->report.kb.modifiers & modifier;
}

hid_error_t hid_KbdSetLEDs(hid_state_t *hid, uint8_t leds) {
    if(hid->type != HID_KEYBOARD) { return HID_ERROR_NOT_SUPPORTED; }
    if(hid->out) {
        return (hid_error_t) usb_Transfer(hid->out, &leds, 1, 10, NULL);
    } else {
        usb_control_setup_t setup = {0x21, 0x09, 0x200, 0, 1};
        setup.wIndex = hid->interface;
        return (hid_error_t) usb_DefaultControlTransfer(
            hid->dev, &setup, &leds,
            1, NULL
        );
    }
}

bool hid_MouseIsButtonDown(hid_state_t *hid, hid_mouse_button_t button) {
    if(hid->type != HID_MOUSE) { return false; }

    return hid->report.mouse.buttons & (1 << button);
}

void hid_MouseGetDeltas(hid_state_t *hid, int24_t *x, int24_t *y) {
    *x = hid->delta_x;
    *y = hid->delta_y;
    hid->delta_x = hid->delta_y = 0;
}

void hid_SetEventCallback(
    hid_state_t *hid, hid_callback_t callback, void *callback_data
) {
    hid->callback = callback;
    hid->callback_data = callback_data;
}

#endif
