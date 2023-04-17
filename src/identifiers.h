#ifndef H_ACQUIRE_DRIVER_BASICS_IDENTIFIERS_V0
#define H_ACQUIRE_DRIVER_BASICS_IDENTIFIERS_V0

#ifdef __cplusplus
extern "C"
{
#endif

    enum BasicDeviceKind
    {
        BasicDevice_Camera_Random,
        BasicDevice_Camera_Sin,
        BasicDevice_Camera_Empty,
        BasicDevice_Storage_Raw,
        BasicDevice_Storage_Tiff,
        BasicDevice_Storage_Trash,
        BasicDevice_Storage_SideBySideTiffJson,
        BasicDeviceKindCount
    };

    const char* basic_device_kind_to_string(enum BasicDeviceKind kind);

#ifdef __cplusplus
};
#endif

#endif // H_ACQUIRE_DRIVER_BASICS_IDENTIFIERS_V0
