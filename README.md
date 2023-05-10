# Acquire Common Driver

[![Build](https://github.com/acquire-project/acquire-driver-common/actions/workflows/build.yml/badge.svg)](https://github.com/acquire-project/acquire-driver-common/actions/workflows/build.yml)
[![Tests](https://github.com/acquire-project/acquire-driver-common/actions/workflows/test_pr.yml/badge.svg)](https://github.com/acquire-project/acquire-driver-common/actions/workflows/test_pr.yml)

This is an Acquire Driver that exposes commonly used devices.

## Devices

### Cameras

- **simulated: uniform random** - Produces uniform random noise for each pixel.
- **simulated: radial sin** - Produces an animated radial sin-wave pattern.
- **simulated: empty** - Produces no data, leaving a image buffers blank. Simulates going as fast as possible.

### Storage

- **raw** - Streams to a raw binary file.
- **tiff** - Streams to a [bigtiff][] file. Metadata is stored in the `ImageDescription` tag for each frame as a `json`
  string.
- **tiff-json** - Stores the video stream in a *bigtiff* (as above) and stores metadata in a `json` file. Both are
  located
  in a folder identified by the `filename` property.
- **Trash** - Writes nothing. Discards incoming data.

[bigtiff]: http://bigtiff.org/
