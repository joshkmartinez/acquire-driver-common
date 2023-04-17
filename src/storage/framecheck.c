#include "device/props/storage.h"
#include "device/kit/storage.h"
#include "device/props/components.h"
#include "platform.h"
#include "logger.h"

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define LOG(...) aq_logger(0, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOGE(...) aq_logger(1, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define CHECK(e)                                                               \
    do {                                                                       \
        if (!(e)) {                                                            \
            LOGE("Expression evaluated as false:\n\t%s", #e);                  \
            goto Error;                                                        \
        }                                                                      \
    } while (0)

#define containerof(ptr, T, V) ((T*)(((char*)(ptr)) - offsetof(T, V)))

struct FrameCheck
{
    struct Storage writer;
    struct StorageProperties settings;
    uint64_t iframe;
};

static enum DeviceState
set(struct Storage* self_, const struct StorageProperties* settings)
{
    struct FrameCheck* self = containerof(self_, struct FrameCheck, writer);
    CHECK(storage_properties_copy(&self->settings, settings));
    return DeviceState_Armed;
Error:
    return DeviceState_AwaitingConfiguration;
}

static void
get(const struct Storage* self_, struct StorageProperties* settings)
{
    struct FrameCheck* self = containerof(self_, struct FrameCheck, writer);
    *settings = self->settings;
}

static enum DeviceState
start(struct Storage* self_)
{
    struct FrameCheck* self = containerof(self_, struct FrameCheck, writer);
    self->iframe = self->settings.first_frame_id;
    return DeviceState_Running;
}

static enum DeviceState
stop(struct Storage* self_)
{
    return DeviceState_Armed;
}

static enum DeviceState
append(struct Storage* self_, const struct VideoFrame* frames, size_t* nbytes)
{
    struct FrameCheck* self = containerof(self_, struct FrameCheck, writer);

    const uint8_t* const beg = (const uint8_t*)frames;
    const uint8_t* const end = beg + *nbytes;
    const uint8_t* cur = beg;
    while (cur < end) {
        const struct VideoFrame* im = (const struct VideoFrame*)cur;
        const size_t delta = im->bytes_of_frame;
        CHECK(self->iframe == im->frame_id);
        ++self->iframe;
        cur += delta;
    }
    *nbytes = end - beg;
    return DeviceState_Running;
Error:
    *nbytes = cur - beg;
    return stop(self_);
}

static void
destroy(struct Storage* self_)
{
    struct FrameCheck* self = containerof(self_, struct FrameCheck, writer);
    storage_properties_destroy(&self->settings);
    free(self);
}

struct Storage*
framecheck_init()
{
    struct FrameCheck* self;
    CHECK(self = malloc(sizeof(*self)));
    memset(self, 0, sizeof(*self));
    const struct PixelScale pixel_scale_um = { 1, 1 };

    CHECK(storage_properties_init(&self->settings,
                                  0,
                                  "framecheck",
                                  sizeof("framecheck"),
                                  0,
                                  0,
                                  pixel_scale_um,
                                  0));
    self->writer = (struct Storage){ .state = DeviceState_AwaitingConfiguration,
                                     .set = set,
                                     .get = get,
                                     .start = start,
                                     .append = append,
                                     .stop = stop,
                                     .destroy = destroy };
    return &self->writer;
Error:
    if (self)
        free(self);
    return 0;
}
