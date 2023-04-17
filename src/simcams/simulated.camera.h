#ifndef H_ACQUIRE_DRIVER_BASICS_SIMULATED_CAMERA_V0
#define H_ACQUIRE_DRIVER_BASICS_SIMULATED_CAMERA_V0

#include "../identifiers.h"
#include "device/kit/driver.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct Camera* simcam_make_camera(enum BasicDeviceKind kind);
    enum DeviceStatusCode simcam_close_camera(struct Camera* camera);

#ifdef __cplusplus
};
#endif

#endif // H_ACQUIRE_DRIVER_BASICS_SIMULATED_CAMERA_V0
