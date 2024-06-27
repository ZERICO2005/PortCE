/**
 * @file
 *
 * @author "zerico2005"
 */

#ifndef PORTCE_RENDER_H
#define PORTCE_RENDER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PortCE_Config PortCE_Config;

void initLCDcontroller(const char* window_title, const PortCE_Config* config);
int terminateLCDcontroller(void);

#ifdef __cplusplus
}
#endif

#endif /* PORTCE_RENDER_H */