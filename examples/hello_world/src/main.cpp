#include <PortCE.h>
#include <tice.h>
#include <graphx.h>

int main(void) {
    PortCE_initialize("Hello World");

    gfx_Begin();

    gfx_PrintStringXY("Hello PortCE", 10, 10);

    while(!os_GetCSC());

    gfx_End();

    PortCE_terminate();

    return 0;
}
