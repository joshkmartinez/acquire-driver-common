#include "device/props/storage.h"
#include "device/kit/storage.h"
#include "platform.h"
#include "logger.h"

#include <string.h>
#include <stdlib.h>

#define LOG(...) aq_logger(0, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOGE(...) aq_logger(1, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define CHECK(e)                                                               \
    do {                                                                       \
        if (!(e)) {                                                            \
            LOGE("Expression evaluated as false:\n\t%s", #e);                  \
            goto Error;                                                        \
        }                                                                      \
    } while (0)
#define TODO                                                                   \
    do {                                                                       \
        LOGE("TODO: Unimplemented");                                           \
        goto Error;                                                            \
    } while (0)

#define containerof(ptr, T, V) ((T*)(((char*)(ptr)) - offsetof(T, V)))

struct Raw
{
    struct Storage writer;
    struct StorageProperties properties;
    struct file file;
    size_t offset;
};

static enum DeviceState
set(struct Storage* self_, const struct StorageProperties* properties)
{
    struct Raw* self = containerof(self_, struct Raw, writer);
    const char* filename = properties->filename.str;
    const size_t nbytes = properties->filename.nbytes;

    // Validate
    CHECK(file_is_writable(filename, nbytes));

    // copy in the properties
    CHECK(storage_properties_copy(&self->properties, properties));

    return DeviceState_Armed;
Error:
    return DeviceState_AwaitingConfiguration;
}

static void
get(const struct Storage* self_, struct StorageProperties* settings)
{
    struct Raw* self = containerof(self_, struct Raw, writer);
    *settings = self->properties;
}

static enum DeviceState
start(struct Storage* self_)
{
    struct Raw* self = containerof(self_, struct Raw, writer);
    CHECK(file_create(&self->file,
                      self->properties.filename.str,
                      self->properties.filename.nbytes));
    LOG("RAW: Frame header size %d bytes", (int)sizeof(struct VideoFrame));
    return DeviceState_Running;
Error:
    return DeviceState_AwaitingConfiguration;
}

static enum DeviceState
stop(struct Storage* self_)
{
    struct Raw* self = containerof(self_, struct Raw, writer);
    file_close(&self->file);
    return DeviceState_Armed;
}

static enum DeviceState
append(struct Storage* self_, const struct VideoFrame* frames, size_t* nbytes)
{
    struct Raw* self = containerof(self_, struct Raw, writer);
    CHECK(file_write(&self->file,
                     self->offset,
                     (uint8_t*)frames,
                     ((uint8_t*)frames) + *nbytes));
    self->offset += *nbytes;

    return DeviceState_Running;
Error:
    *nbytes = 0;
    return stop(self_);
}

static void
destroy(struct Storage* writer_)
{
    struct Raw* self = containerof(writer_, struct Raw, writer);
    stop(writer_);
    storage_properties_destroy(&self->properties);
    free(self);
}

struct Storage*
raw_init()
{
    struct Raw* self;
    CHECK(self = malloc(sizeof(*self)));
    memset(self, 0, sizeof(*self));
    const struct PixelScale pixel_scale_um = { 1, 1 };

    CHECK(storage_properties_init(&self->properties,
                                  0,
                                  "out.raw",
                                  sizeof("out.raw"),
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
    return 0;
}
