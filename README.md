<p align="center">
  <img src=".github/banner.svg" width="100%" alt="Reference Tools · MPEG V3C Immersive Platform: V3C Decoder Plugin">
</p>

<p align="center">
  Source code and libraries for decoding and viewing MPEG Immersive Video (MIV) and Video Point
  Cloud Compression (V-PCC) content on the V3C Immersive Platform.
</p>

<p align="center">
  <img alt="Status" src="https://img.shields.io/endpoint?url=https%3A%2F%2Fwww.5g-mag.com%2Fbadges%2Fstatus%2Frt-v3c-decoder-plugin.json">
  <a href="https://github.com/5G-MAG/rt-v3c-decoder-plugin/releases"><img alt="Version"
    src="https://img.shields.io/github/v/release/5G-MAG/rt-v3c-decoder-plugin?label=Version"></a>
  <a href="License.md"><img alt="License: 5G-MAG Public License v1.0"
    src="https://img.shields.io/badge/License-5G--MAG%20PL%20v1.0-blue"></a>
</p>

<p align="center">
  <a href="https://www.5g-mag.com/reference-tools/v3c/">Project page</a> &nbsp;&middot;&nbsp;
  <a href="https://github.com/5G-MAG/rt-v3c-decoder-plugin/issues">Issues</a> &nbsp;&middot;&nbsp;
  <a href="https://www.5g-mag.com/contributing">Contributing</a>
</p>

---

## At a glance

|  |  |
|---|---|
| **Part of** | [MPEG V3C Immersive Platform](https://www.5g-mag.com/reference-tools/v3c/), alongside [rt-media-origin](https://github.com/5G-MAG/rt-media-origin), [rt-v3c-content](https://github.com/5G-MAG/rt-v3c-content), [rt-v3c-examples](https://github.com/5G-MAG/rt-v3c-examples), [rt-v3c-unity-player](https://github.com/5G-MAG/rt-v3c-unity-player) |

## Introduction

This repository holds the entry points for decoding and viewing MIV and V-PCC content on the V3C
Immersive Platform: the source code of the V3C native decoder, in `./Sources`, and precompiled
libraries, in `./Libraries`. It builds the libraries that
[rt-v3c-unity-player](https://github.com/5G-MAG/rt-v3c-unity-player) uses to play MIV and V-PCC
content on Windows and Android, and it includes
[rt-v3c-content](https://github.com/5G-MAG/rt-v3c-content), the V3C test content, as the Git
submodule `V3C-Content`.

![V3CImmersivePlatform components](docs/images/v3c-immersive-player-component.png)

The precompiled libraries are:

| Component | Library | Owner |
|---|---|---|
| V3C DASH Streamer Client: V3C Dash Streamer | v3c_dash_streamer | InterDigital R&D France |
| V3C DASH Streamer Client: V3C Extractor | v3c_extractor | InterDigital R&D France |
| MIV MPI Synthesizer | V3CImmersiveSynthesizerMPI | InterDigital R&D France |
| MIV MVD Synthesizer | V3CImmersiveSynthesizerMES | Philips |
| ILOJ Toolkit | iloj\* | InterDigital R&D France |
| ILOJ Toolkit: ILOJ AVCodec Wrapper | iloj_avcodec_\* | InterDigital R&D France |

The V3C DASH Streamer Client receives V3C DASH segments from a remote DASH server.

Questions on setting up or using the V3C Immersive Platform are answered in the
[FAQ in the Unity Player repository](https://github.com/5G-MAG/rt-v3c-unity-player/blob/main/docs/FAQ.md).

## Install dependencies

For the build:

- Visual Studio Professional 2022 (17.14.4)
- CMake (for example 3.30.4)
- For Android: Android Studio with NDK r27c (27.2.12479018), and API 35
- An audio/video decoder library, for example FFmpeg (7.1).
  [rt-common-shared](https://github.com/5G-MAG/rt-common-shared) has a helper script, in
  `avcodec-build`, that builds FFmpeg libraries for Android. To use another audio/video decoder
  library, please contact 5G-MAG.

To run the decoder:

- Windows: a device with an NVIDIA GPU that supports hardware HEVC decoding (CUVID) and OpenGL Core
  4.5.
- Android: a device that supports OpenGL ES 3.2 and HEVC hardware decoding (MediaCodec).

## Downloading

Clone with the submodules:

```shell
git clone --recurse-submodules https://github.com/5G-MAG/rt-v3c-decoder-plugin.git
```

## Building

### Dependencies

Before compiling, download the build dependencies with this script:

```shell
./Scripts/dl_deps.sh
```

It downloads files or clones Git repositories into the `./External` folder, and needs a github.com
account:

| Dependency | Version | Source |
|---|---|---|
| ninja | v1.11.1 | [github.com/ninja-build/ninja](https://github.com/ninja-build/ninja) |
| fmt | v11.0.2 | [github.com/fmtlib/fmt](https://github.com/fmtlib/fmt.git) |
| zlib | v1.3.1 | [github.com/madler/zlib](https://github.com/madler/zlib) |
| glfw | v3.4 | [github.com/glfw/glfw](https://github.com/glfw/glfw) |
| tmiv | v23.0 | [gitlab.com/mpeg-i-visual/tmiv](https://gitlab.com/mpeg-i-visual/tmiv.git) |
| tmc2 | v25.0 | [github.com/MPEGGroup/mpeg-pcc-tmc2](https://github.com/MPEGGroup/mpeg-pcc-tmc2.git) |
| mhrm | v5.1 | [github.com/MPEGGroup/HapticReferenceSoftware](https://github.com/MPEGGroup/HapticReferenceSoftware.git) |

Check the downloaded dependencies in `./External`, and their licences, before compiling.

### Compiling for Windows

```shell
./Scripts/build_win64.sh release all
```  

The script should end with `-- Install configuration: "Release"`. Then check that the build files are in the output directory:

```shell
ls ./Output/Windows/Release/x86_64
```

- V3CImmersiveDecoderVideo.dll
- AudioPlugin_V3CImmersiveDecoderAudio.dll

### Compiling for Android

```shell
./Scripts/build_android.sh release all  
```  

Then check that the build files are in the output directory:

```shell
ls ./Output/Android/Release/arm64-v8a
```

- libV3CImmersiveDecoderVideo.so
- libAudioPlugin_V3CImmersiveDecoderAudio.so

The directory should also hold the `.so` files from TMIV and TMC2.

## Contributing

Contributions are welcome. How to raise an issue, fork the repository and open a pull request, and
the Contributor License Agreement required before code can be merged, are described at
<https://www.5g-mag.com/contributing>.

## License

Distributed under the 5G-MAG Public License v1.0. See [License.md](License.md); the licence terms
are also available [online](https://drive.google.com/file/d/1cinCiA778IErENZ3JN52VFW-1ffHpx7Z/view).

The software distributed with this repository as binaries is listed in
[ATTRIBUTION_NOTICE](ATTRIBUTION_NOTICE.txt). The external build dependencies are listed under
[Dependencies](#dependencies).
