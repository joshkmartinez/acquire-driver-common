#ifndef ACQUIRE_DRIVER_BASICS_BASIC_STORAGE_H
#define ACQUIRE_DRIVER_BASICS_BASIC_STORAGE_H

#include "../identifiers.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct Storage* basics_make_storage(enum BasicDeviceKind kind);
    void basics_storage_shutdown(struct Driver* driver);

#ifdef __cplusplus
};
#endif

#endif // ACQUIRE_DRIVER_BASICS_BASIC_STORAGE_H
