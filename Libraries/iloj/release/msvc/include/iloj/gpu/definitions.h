/*
* Copyright (c) 2024 InterDigital R&D France
* Licensed under the License terms of 5GMAG software (the "License").
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at https://www.5g-mag.com/license .
* Unless required by applicable law or agreed to in writing, software distributed under the License is
* distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*/

#pragma once

#include <cstddef>
#include <cstdint>

namespace iloj::gpu
{
////////// GL //////////
// Typedefs
using GLvoid = void;
using GLboolean = unsigned char;
using GLchar = char;
using GLbyte = char;
using GLuchar = unsigned char;
using GLubyte = unsigned char;
using GLint = int;
using GLuint = unsigned;
using GLfloat = float;
using GLclampf = float;
using GLclampd = double;

using GLenum = unsigned;
using GLsizei = int;
using GLbitfield = unsigned;

using GLintptr = std::intptr_t;
using GLsizeiptr = std::intptr_t;

using BOOL = int;

// Error
static constexpr unsigned GL_NO_ERROR = 0;
static constexpr unsigned GL_INVALID_ENUM = 0x0500;
static constexpr unsigned GL_INVALID_VALUE = 0x0501;
static constexpr unsigned GL_INVALID_OPERATION = 0x0502;
static constexpr unsigned GL_STACK_OVERFLOW = 0x0503;
static constexpr unsigned GL_STACK_UNDERFLOW = 0x0504;
static constexpr unsigned GL_OUT_OF_MEMORY = 0x0505;
static constexpr unsigned GL_INVALID_FRAMEBUFFER_OPERATION = 0x0506;
static constexpr unsigned GL_CONTEXT_LOST = 0x0507;
static constexpr unsigned GL_TABLE_TOO_LARGE = 0x8031;
static constexpr unsigned GL_INVALID_INDEX = 0xFFFFFFFF;

// Boolean values
static constexpr unsigned GL_FALSE = 0;
static constexpr unsigned GL_TRUE = 1;

// Internal sized formats
static constexpr unsigned GL_R8 = 0x8229;
static constexpr unsigned GL_R8I = 0x8231;
static constexpr unsigned GL_R8UI = 0x8232;
static constexpr unsigned GL_R16 = 0x822A;
static constexpr unsigned GL_R16I = 0x8233;
static constexpr unsigned GL_R16UI = 0x8234;
static constexpr unsigned GL_R16F = 0x822D;
static constexpr unsigned GL_R32I = 0x8235;
static constexpr unsigned GL_R32UI = 0x8236;
static constexpr unsigned GL_R32F = 0x822E;

static constexpr unsigned GL_RG8 = 0x822B;
static constexpr unsigned GL_RG8I = 0x8237;
static constexpr unsigned GL_RG8UI = 0x8238;
static constexpr unsigned GL_RG16 = 0x822C;
static constexpr unsigned GL_RG16I = 0x8239;
static constexpr unsigned GL_RG16UI = 0x823A;
static constexpr unsigned GL_RG16F = 0x822F;
static constexpr unsigned GL_RG32I = 0x823B;
static constexpr unsigned GL_RG32UI = 0x823C;
static constexpr unsigned GL_RG32F = 0x8230;

static constexpr unsigned GL_RGB8 = 0x8051;
static constexpr unsigned GL_RGB8I = 0x8D8F;
static constexpr unsigned GL_RGB8UI = 0x8D7D;
static constexpr unsigned GL_RGB16 = 0x8054;
static constexpr unsigned GL_RGB16I = 0x8D89;
static constexpr unsigned GL_RGB16UI = 0x8D77;
static constexpr unsigned GL_RGB16F = 0x881B;
static constexpr unsigned GL_RGB32I = 0x8D83;
static constexpr unsigned GL_RGB32UI = 0x8D71;
static constexpr unsigned GL_RGB32F = 0x8815;

static constexpr unsigned GL_RGBA8 = 0x8058;
static constexpr unsigned GL_RGBA8I = 0x8D8E;
static constexpr unsigned GL_RGBA8UI = 0x8D7C;
static constexpr unsigned GL_RGBA16 = 0x805B;
static constexpr unsigned GL_RGBA16I = 0x8D88;
static constexpr unsigned GL_RGBA16UI = 0x8D76;
static constexpr unsigned GL_RGBA16F = 0x881A;
static constexpr unsigned GL_RGBA32I = 0x8D82;
static constexpr unsigned GL_RGBA32UI = 0x8D70;
static constexpr unsigned GL_RGBA32F = 0x8814;

static constexpr unsigned GL_DEPTH_COMPONENT16 = 0x81A5;
static constexpr unsigned GL_DEPTH_COMPONENT24 = 0x81A6;
static constexpr unsigned GL_DEPTH_COMPONENT32F = 0x8CAC;

// Formats
static constexpr unsigned GL_RED = 0x1903;
static constexpr unsigned GL_RED_INTEGER = 0x8D94;
static constexpr unsigned GL_GREEN = 0x1904;
static constexpr unsigned GL_GREEN_INTEGER = 0x8D95;
static constexpr unsigned GL_BLUE = 0x1905;
static constexpr unsigned GL_BLUE_INTEGER = 0x8D96;
static constexpr unsigned GL_RG = 0x8227;
static constexpr unsigned GL_RG_INTEGER = 0x8228;
static constexpr unsigned GL_RGB = 0x1907;
static constexpr unsigned GL_RGB_INTEGER = 0x8D98;
static constexpr unsigned GL_BGR = 0x80E0;
static constexpr unsigned GL_BGR_INTEGER = 0x8D9A;
static constexpr unsigned GL_RGBA = 0x1908;
static constexpr unsigned GL_RGBA_INTEGER = 0x8D99;
static constexpr unsigned GL_BGRA = 0x80E1;
static constexpr unsigned GL_BGRA_INTEGER = 0x8D9B;
static constexpr unsigned GL_DEPTH_COMPONENT = 0x1902;
static constexpr unsigned GL_ALPHA = 0x1906;

// Data types
static constexpr unsigned GL_BYTE = 0x1400;
static constexpr unsigned GL_UNSIGNED_BYTE = 0x1401;
static constexpr unsigned GL_SHORT = 0x1402;
static constexpr unsigned GL_UNSIGNED_SHORT = 0x1403;
static constexpr unsigned GL_INT = 0x1404;
static constexpr unsigned GL_UNSIGNED_INT = 0x1405;
static constexpr unsigned GL_FLOAT = 0x1406;
static constexpr unsigned GL_HALF_FLOAT = 0x140B;

// Buffers
static constexpr unsigned GL_ARRAY_BUFFER = 0x8892;
static constexpr unsigned GL_ELEMENT_ARRAY_BUFFER = 0x8893;
static constexpr unsigned GL_STATIC_DRAW = 0x88E4;
static constexpr unsigned GL_DYNAMIC_DRAW = 0x88E8;
static constexpr unsigned GL_MAP_READ_BIT = 0x0001;
static constexpr unsigned GL_DRAW_INDIRECT_BUFFER = 0x8F3F;
static constexpr unsigned GL_SHADER_STORAGE_BUFFER = 0x90D2;


// Texture
static constexpr unsigned GL_TEXTURE_BUFFER = 0x8C2A;
static constexpr unsigned GL_MAX_TEXTURE_SIZE = 0x0D33;
static constexpr unsigned GL_MAX_TEXTURE_BUFFER_SIZE = 0x8C2B;
static constexpr unsigned GL_MAX_TEXTURE_IMAGE_UNITS = 0x8872;
static constexpr unsigned GL_TEXTURE_1D = 0x0DE0;
static constexpr unsigned GL_TEXTURE_2D = 0x0DE1;
static constexpr unsigned GL_TEXTURE_2D_MULTISAMPLE = 0x9100;
static constexpr unsigned GL_TEXTURE_2D_ARRAY = 0x8C1A;
static constexpr unsigned GL_TEXTURE0 = 0x84C0;
static constexpr unsigned GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = 0x8B4D;
static constexpr unsigned GL_PACK_ALIGNMENT = 0x0D05;
static constexpr unsigned GL_UNPACK_ALIGNMENT = 0x0CF5;
static constexpr unsigned GL_UNPACK_ROW_LENGTH = 0x0CF2;
static constexpr unsigned GL_PACK_ROW_LENGTH = 0x0D02;
static constexpr unsigned GL_UNPACK_IMAGE_HEIGHT = 0x806E;
static constexpr unsigned GL_PACK_IMAGE_HEIGHT = 0x806C;
static constexpr unsigned GL_UNPACK_SWAP_BYTES = 0x0CF0;
static constexpr unsigned GL_TEXTURE_MAG_FILTER = 0x2800;
static constexpr unsigned GL_TEXTURE_MIN_FILTER = 0x2801;
static constexpr unsigned GL_NEAREST = 0x2600;
static constexpr unsigned GL_LINEAR = 0x2601;
static constexpr unsigned GL_NEAREST_MIPMAP_NEAREST = 0x2700;
static constexpr unsigned GL_LINEAR_MIPMAP_NEAREST = 0x2701;
static constexpr unsigned GL_NEAREST_MIPMAP_LINEAR = 0x2702;
static constexpr unsigned GL_LINEAR_MIPMAP_LINEAR = 0x2703;
static constexpr unsigned GL_TEXTURE_WRAP_S = 0x2802;
static constexpr unsigned GL_TEXTURE_WRAP_T = 0x2803;
static constexpr unsigned GL_REPEAT = 0x2901;
static constexpr unsigned GL_CLAMP_TO_EDGE = 0x812F;
static constexpr unsigned GL_MIRRORED_REPEAT = 0x8370;
static constexpr unsigned GL_TEXTURE_BASE_LEVEL = 0x813C;
static constexpr unsigned GL_TEXTURE_MAX_LEVEL = 0x813D;
static constexpr unsigned GL_TEXTURE_WIDTH = 0x1000;
static constexpr unsigned GL_TEXTURE_HEIGHT = 0x1001;

// Framebuffers
static constexpr unsigned GL_NONE = 0;
static constexpr unsigned GL_BACK_LEFT = 0x0402;
static constexpr unsigned GL_BACK_RIGHT = 0x0403;
static constexpr unsigned GL_FRAMEBUFFER_COMPLETE = 0x8CD5;
static constexpr unsigned GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT = 0x8CD6;
static constexpr unsigned GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT = 0x8CD7;
static constexpr unsigned GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS = 0x8CD9;
static constexpr unsigned GL_FRAMEBUFFER_UNSUPPORTED = 0x8CDD;
static constexpr unsigned GL_FRAMEBUFFER = 0x8D40;
static constexpr unsigned GL_READ_FRAMEBUFFER = 0x8CA8;
static constexpr unsigned GL_DRAW_FRAMEBUFFER = 0x8CA9;
static constexpr unsigned GL_RENDERBUFFER = 0x8D41;
static constexpr unsigned GL_COLOR_ATTACHMENT0 = 0x8CE0;
static constexpr unsigned GL_MAX_COLOR_ATTACHMENTS = 0x8CDF;
static constexpr unsigned GL_DEPTH_ATTACHMENT = 0x8D00;

// Clear buffers
static constexpr unsigned GL_COLOR = 0x1800;
static constexpr unsigned GL_DEPTH = 0x1801;
static constexpr unsigned GL_DRAW_BUFFER0 = 0x8825;

// Uniforms
static constexpr unsigned GL_UNIFORM_BUFFER = 0x8A11;
static constexpr unsigned GL_UNIFORM_BLOCK_DATA_SIZE = 0x8A40;

// Programs
static constexpr unsigned GL_COMPILE_STATUS = 0x8B81;
static constexpr unsigned GL_FRAGMENT_SHADER = 0x8B30;
static constexpr unsigned GL_VERTEX_SHADER = 0x8B31;
static constexpr unsigned GL_TESS_EVALUATION_SHADER = 0x8E87;
static constexpr unsigned GL_TESS_CONTROL_SHADER = 0x8E88;
static constexpr unsigned GL_LINK_STATUS = 0x8B82;
static constexpr unsigned GL_INFO_LOG_LENGTH = 0x8B84;
static constexpr unsigned GL_GEOMETRY_SHADER = 0x8DD9;
static constexpr unsigned GL_COMPUTE_SHADER = 0x91B9;

// Atrributes
static constexpr unsigned GL_CURRENT_BIT = 0x00000001;
static constexpr unsigned GL_POINT_BIT = 0x00000002;
static constexpr unsigned GL_LINE_BIT = 0x00000004;
static constexpr unsigned GL_POLYGON_BIT = 0x00000008;
static constexpr unsigned GL_POLYGON_STIPPLE_BIT = 0x00000010;
static constexpr unsigned GL_PIXEL_MODE_BIT = 0x00000020;
static constexpr unsigned GL_LIGHTING_BIT = 0x00000040;
static constexpr unsigned GL_FOG_BIT = 0x00000080;
static constexpr unsigned GL_DEPTH_BUFFER_BIT = 0x00000100;
static constexpr unsigned GL_ACCUM_BUFFER_BIT = 0x00000200;
static constexpr unsigned GL_STENCIL_BUFFER_BIT = 0x00000400;
static constexpr unsigned GL_VIEWPORT_BIT = 0x00000800;
static constexpr unsigned GL_TRANSFORM_BIT = 0x00001000;
static constexpr unsigned GL_ENABLE_BIT = 0x00002000;
static constexpr unsigned GL_COLOR_BUFFER_BIT = 0x00004000;
static constexpr unsigned GL_HINT_BIT = 0x00008000;
static constexpr unsigned GL_EVAL_BIT = 0x00010000;
static constexpr unsigned GL_LIST_BIT = 0x00020000;
static constexpr unsigned GL_TEXTURE_BIT = 0x00040000;
static constexpr unsigned GL_SCISSOR_BIT = 0x00080000;
static constexpr unsigned GL_ALL_ATTRIB_BITS = 0x000FFFFF;

// Scissor box
static constexpr unsigned GL_SCISSOR_TEST = 0x0C11;

// Primitives
static constexpr unsigned GL_POINTS = 0x0000;
static constexpr unsigned GL_LINES = 0x0001;
static constexpr unsigned GL_LINE_LOOP = 0x0002;
static constexpr unsigned GL_LINE_STRIP = 0x0003;
static constexpr unsigned GL_TRIANGLES = 0x0004;
static constexpr unsigned GL_TRIANGLE_STRIP = 0x0005;
static constexpr unsigned GL_TRIANGLE_FAN = 0x0006;
static constexpr unsigned GL_PATCHES = 0x000E;

// Culling
static constexpr unsigned GL_CULL_FACE = 0x0B44;
static constexpr unsigned GL_CW = 0x0900;
static constexpr unsigned GL_CCW = 0x0901;
static constexpr unsigned GL_FRONT = 0x0404;
static constexpr unsigned GL_BACK = 0x0405;
static constexpr unsigned GL_FRONT_AND_BACK = 0x0408;

// Blending
static constexpr unsigned GL_BLEND = 0x0BE2;
static constexpr unsigned GL_BLEND_SRC = 0x0BE1;
static constexpr unsigned GL_BLEND_DST = 0x0BE0;
static constexpr unsigned GL_ZERO = 0;
static constexpr unsigned GL_ONE = 1;
static constexpr unsigned GL_SRC_COLOR = 0x0300;
static constexpr unsigned GL_ONE_MINUS_SRC_COLOR = 0x0301;
static constexpr unsigned GL_SRC_ALPHA = 0x0302;
static constexpr unsigned GL_ONE_MINUS_SRC_ALPHA = 0x0303;
static constexpr unsigned GL_DST_ALPHA = 0x0304;
static constexpr unsigned GL_ONE_MINUS_DST_ALPHA = 0x0305;
static constexpr unsigned GL_DST_COLOR = 0x0306;
static constexpr unsigned GL_ONE_MINUS_DST_COLOR = 0x0307;
static constexpr unsigned GL_SRC_ALPHA_SATURATE = 0x0308;
static constexpr unsigned GL_FUNC_ADD = 0x8006;
static constexpr unsigned GL_MIN = 0x8007;
static constexpr unsigned GL_MAX = 0x8008;

// Depth test
static constexpr unsigned GL_DEPTH_TEST = 0x0B71;
static constexpr unsigned GL_DEPTH_WRITEMASK = 0x0B72;
static constexpr unsigned GL_NEVER = 0x0200;
static constexpr unsigned GL_LESS = 0x0201;
static constexpr unsigned GL_EQUAL = 0x0202;
static constexpr unsigned GL_LEQUAL = 0x0203;
static constexpr unsigned GL_GREATER = 0x0204;
static constexpr unsigned GL_NOTEQUAL = 0x0205;
static constexpr unsigned GL_GEQUAL = 0x0206;
static constexpr unsigned GL_ALWAYS = 0x0207;

// Points attributes
static constexpr unsigned GL_PROGRAM_POINT_SIZE = 0x8642;
static constexpr unsigned GL_POINT_SMOOTH = 0x0B10;

// Line attributes
static constexpr unsigned GL_LINE_SMOOTH = 0x0B20;

// Polygon attributes
static constexpr unsigned GL_POLYGON_SMOOTH = 0x0B41;

// Multisampling
static constexpr unsigned GL_MULTISAMPLE = 0x809D;
static constexpr unsigned GL_SAMPLE_POSITION = 0x8E50;
static constexpr unsigned GL_SAMPLES = 0x80A9;

// 3D
static constexpr unsigned GL_TEXTURE_3D = 0x806F;

// Tesselation
static constexpr unsigned GL_PATCH_VERTICES = 0x8E72;
static constexpr unsigned TESS_GEN_POINT_MODE = 0x8E79;
static constexpr unsigned GL_MAX_TESS_GEN_LEVEL = 0x8E7E;

// Atomic counters
static constexpr unsigned GL_ATOMIC_COUNTER_BUFFER = 0x92C0;

// Color clamp
static constexpr unsigned GL_CLAMP_READ_COLOR = 0x891C;

// Logic
static constexpr unsigned GL_COLOR_LOGIC_OP = 0x0BF2;
static constexpr unsigned GL_NOOP = 0x1505;
static constexpr unsigned GL_AND = 0x1501;
static constexpr unsigned GL_OR = 0x1507;
static constexpr unsigned GL_XOR = 0x1506;

// Compute shader
static constexpr unsigned GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS = 0x91BC;
static constexpr unsigned GL_MAX_COMPUTE_WORK_GROUP_COUNT = 0x91BE;
static constexpr unsigned GL_MAX_COMPUTE_WORK_GROUP_SIZE = 0x91BF;
static constexpr unsigned GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS = 0x90EB;
static constexpr unsigned GL_MAX_COMPUTE_SHARED_MEMORY_SIZE = 0x8262;
static constexpr unsigned GL_TEXTURE_UPDATE_BARRIER_BIT = 0x00000100;
static constexpr unsigned GL_SHADER_IMAGE_ACCESS_BARRIER_BIT = 0x00000020;
static constexpr unsigned GL_ALL_BARRIER_BITS = 0xFFFFFFFF;
static constexpr unsigned GL_READ_ONLY = 0x88B8;
static constexpr unsigned GL_WRITE_ONLY = 0x88B9;
static constexpr unsigned GL_READ_WRITE = 0x88BA;

// Extensions
static constexpr unsigned GL_NUM_EXTENSIONS = 0x821D;
static constexpr unsigned GL_EXTENSIONS = 0x1F03;

static constexpr unsigned CONSERVATIVE_RASTERIZATION_NV = 0x9346;

static constexpr unsigned GL_TEXTURE_EXTERNAL_OES = 0x8D65;

////////// CUDA //////////
// Typedefs
using CUdevice = int;
using CUdeviceptr = std::size_t;
using CUarray = void *;
using CUcontext = void *;
using CUgraphicsResource = struct CUgraphicsResource_st *;
using CUstream = void *;

enum CUmemorytype
{
    CU_MEMORYTYPE_HOST = 0x01,
    CU_MEMORYTYPE_DEVICE = 0x02,
    CU_MEMORYTYPE_ARRAY = 0x03
};

enum CUresult
{
    CUDA_NONE = -1,
    CUDA_SUCCESS = 0
};

struct CUDA_MEMCPY2D
{
    std::size_t srcXInBytes, srcY;
    CUmemorytype srcMemoryType;
    const void *srcHost;
    CUdeviceptr srcDevice;
    CUarray srcArray;
    std::size_t srcPitch;

    std::size_t dstXInBytes, dstY;
    CUmemorytype dstMemoryType;
    void *dstHost;
    CUdeviceptr dstDevice;
    CUarray dstArray;
    std::size_t dstPitch;

    std::size_t WidthInBytes;
    std::size_t Height;
};

static constexpr unsigned CU_GRAPHICS_MAP_RESOURCE_FLAGS_NONE = 0x00;
static constexpr unsigned CU_GRAPHICS_MAP_RESOURCE_FLAGS_READ_ONLY = 0x01;
static constexpr unsigned CU_GRAPHICS_MAP_RESOURCE_FLAGS_WRITE_DISCARD = 0x02;

////////// WGL_NV_DX_interop2 //////////
using HANDLE = void *;

static constexpr unsigned WGL_ACCESS_READ_ONLY_NV = 0x0000;
static constexpr unsigned WGL_ACCESS_READ_WRITE_NV = 0x0001;
static constexpr unsigned WGL_ACCESS_WRITE_DISCARD_NV = 0x0002;

} // namespace iloj::gpu
