#include "device/kit/driver.h"
#include "device/kit/storage.h"
#include "logger.h"
#include "basic.storage.h"

#include <string.h>
#include <stdlib.h>

#define containerof(P, T, F) ((T*)(((char*)(P)) - offsetof(T, F)))

#define L (aq_logger)
#define LOG(...) L(0, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOGE(...) L(1, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define CHECK(e)                                                               \
    do {                                                                       \
        if (!(e)) {                                                            \
            LOGE("Expression was false:\n\t%s\n", #e);                         \
            goto Error;                                                        \
        }                                                                      \
    } while (0)
#define EXPECT(e, ...)                                                         \
    do {                                                                       \
        if (!(e)) {                                                            \
            LOGE(__VA_ARGS__);                                                 \
            goto Error;                                                        \
        }                                                                      \
    } while (0)

#ifdef WIN32
#define export __declspec(dllexport)
#else
#define export
#endif

//
//                  EXTERN
//

// These get defined in e.g. devices/storage/win32/tiff.cpp
//
// Each of these allocates a storage object on `init`. This should happen in
// `storage_open()`.
//
// The deallocate themselves when their `destroy()` method is called.
// That happens in `storage_close()`.
struct Storage*
framecheck_init();

struct Storage*
raw_init();

struct Storage*
tiff_init();

struct Storage*
trash_init();

struct Storage*
side_by_side_tiff_init();

//
//                  GLOBALS
//

static struct
{
    struct Storage* (**constructors)();
} globals = { 0 };

//
//                  IMPL
//

void
basics_storage_shutdown(struct Driver* driver)
{
    free(globals.constructors);
    globals.constructors = 0;
}

struct Storage*
basics_make_storage(enum BasicDeviceKind kind)
{
    if (!globals.constructors) {
        const size_t nbytes =
          sizeof(globals.constructors[0]) * BasicDeviceKindCount;
        CHECK(globals.constructors = (struct Storage * (**)()) malloc(nbytes));
        struct Storage* (*impls[])() = {
            [BasicDevice_Storage_Raw] = raw_init,
            [BasicDevice_Storage_Tiff] = tiff_init,
            [BasicDevice_Storage_Trash] = trash_init,
            [BasicDevice_Storage_SideBySideTiffJson] = side_by_side_tiff_init,
        };
        memcpy(
          globals.constructors, impls, nbytes); // cppcheck-suppress uninitvar
    }
    CHECK(kind < BasicDeviceKindCount);
    if (globals.constructors[kind])
        return globals.constructors[kind]();
    else {
        LOGE("No storage device found for %s",
             basic_device_kind_to_string(kind));
    }
Error:
    return NULL;
}
