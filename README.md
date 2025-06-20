<h1 align="center">V3C Immersive Platform - Decoder Plugin</h1>
<p align="center">
  <img src="https://img.shields.io/badge/Status-Under_Development-yellow" alt="Under Development">
  <img src="https://img.shields.io/badge/License-5G--MAG%20Public%20License%20(v1.0)-blue" alt="License">
</p>

# Introduction

This project contains the entrypoints for decoding and viewing MPEG Immersive Video (MIV) and Video Point Cloud Compression (V-PCC) related to the V3C Immersive Platform.

It contains source code of V3C native decoder (in `./Sources` folder) and compiled libraries (in `./Libraries` folder) as illustrated in the figure below.

![V3CImmersivePlatform components](docs/images/v3c-immersive-player-component.png)

Libraries components are:

- V3C DASH Streamer Client, a DASH client to receive V3C DASH segments from a remote DASH server
  - V3C Dash Streamer (lib name v3c_dash_streamer, ownership InterDigital R&D France)
  - V3C Extractor (lib name v3c_extractor, ownership InterDigital R&D France)
- V-PCC Synthesizer (lib name V3CImmersiveSynthesizerVPCC, ownership InterDigital R&D France)
- MIV MPI Synthesizer (lib name V3CImmersiveSynthesizerMPI, ownership InterDigital R&D France)
- MIV MVD Synthesizer (lib name V3CImmersiveSynthesizerMES, ownership Philips)
- Haptic Synthesizer (lib name V3CImmersiveSynthesizerHaptic, ownership InterDigital R&D France)
- ILOJ Toolkit (lib name iloj[j], ownership InterDigital R&D France)
  - ILOJ AVCodec Wrapper (lib name iloj_avcodec_hwdec, ownership InterDigital R&D France)

A related Unity project ([V3C Unity Player](https://github.com/5G-MAG/rt-v3c-unity-player)) allows MIV and V-PCC data view with a simple user interface and Windows/Android support.

V3C test contents (MIV and V-PCC) are provided in [V3C content](https://github.com/5G-MAG/rt-v3c-content) project.

## Cloning

To clone the project, use this command line:  

```shell
git clone --recurse-submodules https://github.com/5G-MAG/rt-v3c-decoder-plugin.git
```

## Dependencies

### Install dependencies

Before compiling the project, you need to install dependencies. You can use the following script:  

```shell
cd rt-v3c-decoder-plugin
./Scripts/dl_deps.sh
```

This script will download files or clone git repositories into `./External` folder:

- ninja, v1.11.1, [github.com/ninja-build/ninja](https://github.com/ninja-build/ninja)
- fmt, v11.0.2, [github.com/fmtlib/fmt](https://github.com/fmtlib/fmt.git)
- zlib, v1.3.1, [github.com/madler/zlib](https://github.com/madler/zlib)
- glfw, v3.4, [github.com/glfw/glfw](https://github.com/glfw/glfw)
- tmiv, v23.0, [gitlab.com/mpeg-i-visual/tmiv](https://gitlab.com/mpeg-i-visual/tmiv.git)
- tmc2, v25.0, [github.com/MPEGGroup/mpeg-pcc-tmc2](https://github.com/MPEGGroup/mpeg-pcc-tmc2.git)
- mhrm, v5.1, [github.com/MPEGGroup/HapticReferenceSoftware](https://github.com/MPEGGroup/HapticReferenceSoftware.git)

Note: This script requests to get an account to github.com.

You need to install additional dependencies before compiling:

- Examples of Audio/Video decoder libraries such as FFmpeg (7.1) are available in [rt-common-shared](https://github.com/5G-MAG/rt-common-shared). Should you want to use another Audio/Video decoder libraries, please reach out to us.

Please check the downloaded dependencies and associated licences before compiling in : ./External

## Compiling

You will need following softwares installed on your machine for the build process:  

- Visual Studio Professional 2022 (17.14.4)
- CMake (for example 3.30.4)

Use the script according to the platform you want to use.

### Compiling for Windows

Use the following script to run the compilation:  

```shell
./Scripts/build_win64.sh release all
```  

The script should end with :
 -- Install configuration: "Release"

Then you can check the presence of the build files in the output directory

```shell
ls ./Output/Windows/Release/x86_64
```

- V3CImmersiveDecoderVideo.dll  
- AudioPlugin_V3CImmersiveDecoderAudio.dll

### Compiling for Android

You will need following software installed:

- Android Studio with NDK r27c (27.2.12479018), and API 35

Use the following script to run the compilation:  

```shell
./Scripts/build_android.sh release all  
```  

Then you can check the presence of the build files in the output directory

```shell
ls ./Output/Android/Release/arm64-v8a
```

- libV3CImmersiveDecoderVideo.so  
- libAudioPlugin_V3CImmersiveDecoderAudio.so

In addition you should also have .so from TMIV and TMC2.

### Hardware Requirement

On Windows, a device with an NVIDIA GPU card supporting hardware HEVC decoding (CUVID) and OpenGL Core 4.5 is expected.
On Android, a device supporting OpenGL ES 3.2 and HEVC hardware decoding (MediaCodec) is expected.

## FAQ

If you have any questions on the setup or the usage of the V3C Immersive Platform, please consult the [FAQ page from the Unity Player repository](https://github.com/5G-MAG/rt-v3c-unity-player)

## Licenses

This project is provided under 5G-MAG's Public License. For the full license terms, please see the LICENSE file distributed along with the repository or retrieve it from [here](https://drive.google.com/file/d/1cinCiA778IErENZ3JN52VFW-1ffHpx7Z/view).

Distributed with the software (binaries) as listed in: [ATTRIBUTION_NOTICE](ATTRIBUTION_NOTICE.txt).

External dependencies are listed in [dependencies](#dependencies).
