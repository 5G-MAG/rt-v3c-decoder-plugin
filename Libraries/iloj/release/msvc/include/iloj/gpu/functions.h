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

#include "definitions.h"
#include <string>

namespace iloj::gpu
{
class Functions
{
public:
    // GL
    using GLGETERROR_PROC = GLenum();
    using GLISENABLED_PROC = GLboolean(GLenum);
    using GLFLUSH_PROC = void();
    using GLFINISH_PROC = void();
    using GLCLEAR_PROC = void(GLbitfield);
    using GLCLEARCOLOR_PROC = void(GLclampf, GLclampf, GLclampf, GLclampf);
    using GLCLEARDEPTHF_PROC = void(GLclampf);
    using GLCLEARBUFFERIV_PROC = void(GLenum, GLint, const GLint *);
    using GLCLEARBUFFERUIV_PROC = void(GLenum, GLint, const GLuint *);
    using GLCLEARBUFFERFV_PROC = void(GLenum, GLint, const GLfloat *);
    using GLSCISSOR_PROC = void(GLint, GLint, GLsizei, GLsizei);
    using GLENABLE_PROC = void(GLenum);
    using GLENABLEI_PROC = void(GLenum, GLuint);
    using GLDISABLE_PROC = void(GLenum);
    using GLDISABLEI_PROC = void(GLenum, GLuint);
    using GLVIEWPORT_PROC = void(GLint, GLint, GLsizei, GLsizei);
    using GLGETBOOLEANV_PROC = void(GLenum, GLboolean *);
    using GLGETFLOATV_PROC = void(GLenum, GLfloat *);
    using GLGETINTEGERV_PROC = void(GLenum, GLint *);
    using GLGETINTEGERIV_PROC = void(GLenum, GLuint, GLint *);
    using GLGETSTRINGI_PROC = GLuchar *(GLenum, GLuint);
    using GLGETTEXIMAGE_PROC = void(GLenum, GLint, GLenum, GLenum, GLvoid *);
    using GLGETTEXTURESUBIMAGE_PROC =
        void(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *);
    using GLGENTEXTURES_PROC = void(GLsizei, GLuint *);
    using GLDELETETEXTURES_PROC = void(GLsizei, const GLuint *);
    using GLBINDTEXTURE_PROC = void(GLenum, GLuint);
    using GLPIXELSTOREI_PROC = void(GLenum, GLint);
    using GLTEXBUFFER_PROC = void(GLenum, GLenum, GLuint);
    using GLTEXIMAGE1D_PROC = void(GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid *);
    using GLTEXIMAGE2D_PROC = void(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *);
    using GLTEXIMAGE3D_PROC =
        void(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *);
    using GLTEXTUREVIEW_PROC = void(GLuint, GLenum, GLuint, GLenum, GLuint, GLuint, GLuint, GLuint);
    using GLTEXIMAGE2DMULTISAMPLE_PROC = void(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean);
    using GLTEXSUBIMAGE2D_PROC = void(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
    using GLTEXSUBIMAGE3D_PROC =
        void(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
    using GLTEXSTORAGE3D_PROC = void(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei);
    using GLTEXPARAMETERI_PROC = void(GLenum, GLenum, GLint);

    using GLTEXTURESTORAGE2D_PROC = void(GLuint, GLsizei, GLenum, GLsizei, GLsizei);
    using GLTEXSTORAGE2D_PROC = void(GLenum, GLsizei, GLenum, GLsizei, GLsizei);
    using GLDRAWELEMENTSINDIRECT_PROC = void(GLenum, GLenum, const void*);

    using GLDRAWELEMENTS_PROC = void(GLenum, GLsizei, GLenum, const GLvoid *);
    using GLDRAWELEMENTSINSTANCED_PROC = void(GLenum, GLsizei, GLenum, const GLvoid *, GLsizei);
    using GLDEPTHFUNC_PROC = void(GLenum);
    using GLDEPTHMASK_PROC = void(GLboolean);
    using GLCULLFACE_PROC = void(GLenum);
    using GLFRONTFACE_PROC = void(GLenum);
    using GLBLENDFUNC_PROC = void(GLenum, GLenum);
    using GLBLENDFUNCI_PROC = void(GLuint, GLenum, GLenum);
    using GLBLENDEQUATION_PROC = void(GLenum);
    using GLBLENDEQUATIONI_PROC = void(GLuint, GLenum);
    using GLPOINTSIZE_PROC = void(GLfloat);
    using GLLINEWIDTH_PROC = void(GLfloat);
    using GLACTIVETEXTURE_PROC = void(GLenum);
    using GLMAPBUFFERRANGE_PROC = GLvoid *(GLenum, GLintptr, GLsizeiptr, GLbitfield);
    using GLUNMAPBUFFER_PROC = GLboolean(GLenum);
    using GLBINDBUFFER_PROC = void(GLenum, GLuint);
    using GLREADBUFFER_PROC = void(GLenum);
    using GLDRAWBUFFER_PROC = void(GLenum);
    using GLDRAWBUFFERS_PROC = void(GLsizei, const GLenum *);
    using GLGENBUFFERS_PROC = void(GLsizei, GLuint *);
    using GLDELETEBUFFERS_PROC = void(GLsizei, const GLuint *);
    using GLBUFFERDATA_PROC = void(GLenum, GLsizeiptr, const GLvoid *, GLenum);
    using GLBUFFERSUBDATA_PROC = void(GLenum, GLintptr, GLsizeiptr, const GLvoid *);
    
    using GLGETBUFFERSUBDATA_PROC = void(GLenum, GLintptr, GLsizeiptr, const GLvoid *);
    
    using GLGENERATEMIPMAP_PROC = void(GLenum);
    using GLGENRENDERBUFFERS_PROC = void(GLsizei, GLuint *);
    using GLRENDERBUFFERSTORAGE_PROC = void(GLenum, GLenum, GLsizei, GLsizei);
    using GLRENDERBUFFERSTORAGEMULTISAMPLE_PROC = void(GLenum, GLsizei, GLenum, GLsizei, GLsizei);
    using GLDELETERENDERBUFFERS_PROC = void(GLsizei, const GLuint *);
    using GLBINDRENDERBUFFER_PROC = void(GLenum, GLuint);
    using GLFRAMEBUFFERRENDERBUFFER_PROC = void(GLenum, GLenum, GLenum, GLuint);
    using GLFRAMEBUFFERTEXTURE2D_PROC = void(GLenum, GLenum, GLenum, GLuint, GLint);
    using GLFRAMEBUFFERTEXTURELAYER_PROC = void(GLenum, GLenum, GLuint, GLint, GLint);
    using GLGENFRAMEBUFFERS_PROC = void(GLsizei, GLuint *);
    using GLBINDFRAMEBUFFER_PROC = void(GLenum, GLuint);
    using GLDELETEFRAMEBUFFERS_PROC = void(GLsizei, const GLuint *);
    using GLBLITFRAMEBUFFER_PROC = void(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum);
    using GLCHECKFRAMEBUFFERSTATUS_PROC = GLenum(GLenum);
    using GLGETATTRIBLOCATION_PROC = GLint(GLuint, const GLchar *);
    using GLENABLEVERTEXATTRIBARRAY_PROC = void(GLuint);
    using GLVERTEXATTRIBPOINTER_PROC = void(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *);
    using GLVERTEXATTRIBIPOINTER_PROC = void(GLuint, GLint, GLenum, GLsizei, const GLvoid *);
    using GLGETUNIFORMLOCATION_PROC = GLint(GLuint, const GLchar *);
    using GLUNIFORM1I_PROC = void(GLint, GLint);
    using GLUNIFORM1IV_PROC = void(GLint, GLsizei, const GLint *);
    using GLUNIFORM2IV_PROC = void(GLint, GLsizei, const GLint *);
    using GLUNIFORM3IV_PROC = void(GLint, GLsizei, const GLint *);
    using GLUNIFORM4IV_PROC = void(GLint, GLsizei, const GLint *);
    using GLUNIFORM1UI_PROC = void(GLint, GLuint);
    using GLUNIFORM1UIV_PROC = void(GLint, GLsizei, const GLuint *);
    using GLUNIFORM2UIV_PROC = void(GLint, GLsizei, const GLuint *);
    using GLUNIFORM3UIV_PROC = void(GLint, GLsizei, const GLuint *);
    using GLUNIFORM4UIV_PROC = void(GLint, GLsizei, const GLuint *);
    using GLUNIFORM1F_PROC = void(GLint, GLfloat);
    using GLUNIFORM1FV_PROC = void(GLint, GLsizei, const GLfloat *);
    using GLUNIFORM2FV_PROC = void(GLint, GLsizei, const GLfloat *);
    using GLUNIFORM3FV_PROC = void(GLint, GLsizei, const GLfloat *);
    using GLUNIFORM4FV_PROC = void(GLint, GLsizei, const GLfloat *);
    using GLUNIFORMMATRIX3FV_PROC = void(GLint, GLsizei, GLboolean, const GLfloat *);
    using GLUNIFORMMATRIX4FV_PROC = void(GLint, GLsizei, GLboolean, const GLfloat *);
    using GLGETUNIFORMBLOCKINDEX_PROC = GLuint(GLuint, const GLchar *);
    using GLGETACTIVEUNIFORMBLOCKIV_PROC = void(GLuint, GLuint, GLenum, GLint *);
    using GLBINDBUFFERBASE_PROC = void(GLenum, GLuint, GLuint);
    using GLUNIFORMBLOCKBINDING_PROC = void(GLuint, GLuint, GLuint);
    using GLCOMPILESHADER_PROC = void(GLuint);
    using GLCOPYBUFFERSUBDATA_PROC = void(GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr);
    using GLCREATEPROGRAM_PROC = GLuint();
    using GLCREATESHADER_PROC = GLuint(GLenum);
    using GLGETSHADERIV_PROC = void(GLuint, GLenum, GLint *);
    using GLSHADERSOURCE_PROC = void(GLuint, GLsizei, const GLchar *const *, const GLint *);
    using GLDELETESHADER_PROC = void(GLuint);
    using GLGETSHADERINFOLOG_PROC = void(GLuint, GLsizei, GLsizei *, GLchar *);
    using GLATTACHSHADER_PROC = void(GLuint, GLuint);
    using GLDETACHSHADER_PROC = void(GLuint, GLuint);
    using GLDELETEPROGRAM_PROC = void(GLuint);
    using GLLINKPROGRAM_PROC = void(GLuint);
    using GLGETPROGRAMIV_PROC = void(GLuint, GLenum, GLint *);
    using GLGETPROGRAMINFOLOG_PROC = void(GLuint, GLsizei, GLsizei *, GLchar *);
    using GLUSEPROGRAM_PROC = void(GLuint);
    using GLGENVERTEXARRAYS_PROC = void(GLsizei, GLuint *);
    using GLBINDVERTEXARRAY_PROC = void(GLuint);
    using GLDELETEVERTEXARRAYS_PROC = void(GLsizei, const GLuint *);
    using GLPATCHPARAMETERI_PROC = void(GLenum, GLint);
    using GLCLAMPCOLOR_PROC = void(GLenum, GLenum);
    using GLREADPIXELS_PROC = void(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid *);
    using GLGETMULTISAMPLEFV_PROC = void(GLenum, GLuint, GLfloat *);
    using GLLOGICOP_PROC = void(GLenum);
    using GLCOPYIMAGESUBDATA_PROC = void(GLuint,
                                         GLenum,
                                         GLint,
                                         GLint,
                                         GLint,
                                         GLint,
                                         GLuint,
                                         GLenum,
                                         GLint,
                                         GLint,
                                         GLint,
                                         GLint,
                                         GLsizei,
                                         GLsizei,
                                         GLsizei);
    using GLGETTEXLEVELPARAMETERIV_PROC = void(GLenum, GLint, GLenum, GLint *);
    using GLDISPATCHCOMPUTE_PROC = void(GLuint, GLuint, GLuint);
    using GLBINDIMAGETEXTURE_PROC = void(GLuint, GLuint, GLint, GLboolean, GLint, GLenum, GLenum);
    using GLMEMORYBARRIER_PROC = void(GLbitfield barriers);
    // CUDA
    using CUCTXPUSHCURRENT_PROC = CUresult(CUcontext);
    using CUCTXPOPCURRENT_PROC = CUresult(CUcontext *);
    using CUGRAPHICSGLREGISTERIMAGE_PROC = CUresult(CUgraphicsResource *, GLuint, GLenum, unsigned);
    using CUGRAPHICSUNREGISTERRESOURCE_PROC = CUresult(CUgraphicsResource);
    using CUGRAPHICSMAPRESOURCES_PROC = CUresult(unsigned, CUgraphicsResource *, CUstream);
    using CUGRAPHICSUNMAPRESOURCES_PROC = CUresult(unsigned, CUgraphicsResource *, CUstream);
    using CUGRAPHICSSUBRESOURCEGETMAPPEDARRAY_PROC = CUresult(CUarray *, CUgraphicsResource, unsigned, unsigned);
    using CUMEMCPY2D_PROC = CUresult(const CUDA_MEMCPY2D *);
#if _WIN64
    // WGL_NV_DX_interop2
    using WGLDXOPENDEVICENV_PROC = HANDLE(void *);
    using WGLDXCLOSEDEVICENV_PROC = BOOL(HANDLE);
    using WGLDXREGISTEROBJECTNV_PROC = HANDLE(HANDLE, void *, GLuint, GLenum, GLenum);
    using WGLDXUNREGISTEROBJECTNV_PROC = BOOL(HANDLE, HANDLE);
    using WGLDXOBJECTACCESSNV_PROC = BOOL(HANDLE, GLenum);
    using WGLDXLOCKOBJECTSNV_PROC = BOOL(HANDLE, GLint, HANDLE *);
    using WGLDXUNLOCKOBJECTSNV_PROC = BOOL(HANDLE, GLint, HANDLE *);
#endif
public:
    // GL
    GLGETERROR_PROC *glGetError = nullptr;
    GLISENABLED_PROC *glIsEnabled = nullptr;
    GLFLUSH_PROC *glFlush = nullptr;
    GLFINISH_PROC *glFinish = nullptr;
    GLCLEAR_PROC *glClear = nullptr;
    GLCLEARCOLOR_PROC *glClearColor = nullptr;
    GLCLEARDEPTHF_PROC *glClearDepthf = nullptr;
    GLCLEARBUFFERIV_PROC *glClearBufferiv = nullptr;
    GLCLEARBUFFERUIV_PROC *glClearBufferuiv = nullptr;
    GLCLEARBUFFERFV_PROC *glClearBufferfv = nullptr;
    GLSCISSOR_PROC *glScissor = nullptr;
    GLENABLE_PROC *glEnable = nullptr;
    GLENABLEI_PROC *glEnablei = nullptr;
    GLDISABLE_PROC *glDisable = nullptr;
    GLDISABLEI_PROC *glDisablei = nullptr;
    GLVIEWPORT_PROC *glViewport = nullptr;
    GLGETBOOLEANV_PROC *glGetBooleanv = nullptr;
    GLGETFLOATV_PROC *glGetFloatv = nullptr;
    GLGETINTEGERV_PROC *glGetIntegerv = nullptr;
    GLGETINTEGERIV_PROC *glGetIntegeri_v = nullptr;
    GLGETSTRINGI_PROC *glGetStringi = nullptr;
    GLGETTEXIMAGE_PROC *glGetTexImage = nullptr;
    GLGETTEXTURESUBIMAGE_PROC *glGetTextureSubImage = nullptr;
    GLGENTEXTURES_PROC *glGenTextures = nullptr;
    GLDELETETEXTURES_PROC *glDeleteTextures = nullptr;
    GLBINDTEXTURE_PROC *glBindTexture = nullptr;
    GLPIXELSTOREI_PROC *glPixelStorei = nullptr;
    GLTEXBUFFER_PROC *glTexBuffer = nullptr;
    GLTEXIMAGE1D_PROC *glTexImage1D = nullptr;
    GLTEXIMAGE2D_PROC *glTexImage2D = nullptr;
    GLTEXIMAGE2DMULTISAMPLE_PROC *glTexImage2DMultisample = nullptr;
    GLTEXIMAGE3D_PROC *glTexImage3D = nullptr;
    GLTEXTUREVIEW_PROC *glTextureView = nullptr;
    GLTEXSUBIMAGE2D_PROC *glTexSubImage2D = nullptr;
    GLTEXSUBIMAGE3D_PROC *glTexSubImage3D = nullptr;
    GLTEXSTORAGE3D_PROC *glTexStorage3D = nullptr;
    GLTEXPARAMETERI_PROC *glTexParameteri = nullptr;

    GLTEXTURESTORAGE2D_PROC *glTextureStorage2D = nullptr;
    GLTEXSTORAGE2D_PROC *glTexStorage2D = nullptr;
    GLDRAWELEMENTSINDIRECT_PROC *glDrawElementsIndirect = nullptr;

    GLDRAWELEMENTS_PROC *glDrawElements = nullptr;
    GLDRAWELEMENTSINSTANCED_PROC *glDrawElementsInstanced = nullptr;
    GLDEPTHFUNC_PROC *glDepthFunc = nullptr;
    GLDEPTHMASK_PROC *glDepthMask = nullptr;
    GLCULLFACE_PROC *glCullFace = nullptr;
    GLFRONTFACE_PROC *glFrontFace = nullptr;
    GLBLENDFUNC_PROC *glBlendFunc = nullptr;
    GLBLENDFUNCI_PROC *glBlendFunci = nullptr;
    GLBLENDEQUATION_PROC *glBlendEquation = nullptr;
    GLBLENDEQUATIONI_PROC *glBlendEquationi = nullptr;
    GLPOINTSIZE_PROC *glPointSize = nullptr;
    GLLINEWIDTH_PROC *glLineWidth = nullptr;
    GLACTIVETEXTURE_PROC *glActiveTexture = nullptr;
    GLMAPBUFFERRANGE_PROC *glMapBufferRange = nullptr;
    GLUNMAPBUFFER_PROC *glUnmapBuffer = nullptr;
    GLBINDBUFFER_PROC *glBindBuffer = nullptr;
    GLREADBUFFER_PROC *glReadBuffer = nullptr;
    GLDRAWBUFFER_PROC *glDrawBuffer = nullptr;
    GLDRAWBUFFERS_PROC *glDrawBuffers = nullptr;
    GLGENBUFFERS_PROC *glGenBuffers = nullptr;
    GLDELETEBUFFERS_PROC *glDeleteBuffers = nullptr;
    GLBUFFERDATA_PROC *glBufferData = nullptr;
    GLBUFFERSUBDATA_PROC *glBufferSubData = nullptr;
    
    GLGETBUFFERSUBDATA_PROC *glGetBufferSubData = nullptr;

    GLGENERATEMIPMAP_PROC *glGenerateMipmap = nullptr;
    GLGENRENDERBUFFERS_PROC *glGenRenderbuffers = nullptr;
    GLRENDERBUFFERSTORAGE_PROC *glRenderbufferStorage = nullptr;
    GLRENDERBUFFERSTORAGEMULTISAMPLE_PROC *glRenderbufferStorageMultisample = nullptr;
    GLDELETERENDERBUFFERS_PROC *glDeleteRenderbuffers = nullptr;
    GLBINDRENDERBUFFER_PROC *glBindRenderbuffer = nullptr;
    GLFRAMEBUFFERRENDERBUFFER_PROC *glFramebufferRenderbuffer = nullptr;
    GLFRAMEBUFFERTEXTURE2D_PROC *glFramebufferTexture2D = nullptr;
    GLFRAMEBUFFERTEXTURELAYER_PROC *glFramebufferTextureLayer = nullptr;
    GLGENFRAMEBUFFERS_PROC *glGenFramebuffers = nullptr;
    GLBINDFRAMEBUFFER_PROC *glBindFramebuffer = nullptr;
    GLDELETEFRAMEBUFFERS_PROC *glDeleteFramebuffers = nullptr;
    GLBLITFRAMEBUFFER_PROC *glBlitFramebuffer = nullptr;
    GLCHECKFRAMEBUFFERSTATUS_PROC *glCheckFramebufferStatus = nullptr;
    GLGETATTRIBLOCATION_PROC *glGetAttribLocation = nullptr;
    GLENABLEVERTEXATTRIBARRAY_PROC *glEnableVertexAttribArray = nullptr;
    GLVERTEXATTRIBPOINTER_PROC *glVertexAttribPointer = nullptr;
    GLVERTEXATTRIBIPOINTER_PROC *glVertexAttribIPointer = nullptr;
    GLGETUNIFORMLOCATION_PROC *glGetUniformLocation = nullptr;
    GLUNIFORM1I_PROC *glUniform1i = nullptr;
    GLUNIFORM1IV_PROC *glUniform1iv = nullptr;
    GLUNIFORM2IV_PROC *glUniform2iv = nullptr;
    GLUNIFORM3IV_PROC *glUniform3iv = nullptr;
    GLUNIFORM4IV_PROC *glUniform4iv = nullptr;
    GLUNIFORM1UI_PROC *glUniform1ui = nullptr;
    GLUNIFORM1UIV_PROC *glUniform1uiv = nullptr;
    GLUNIFORM2UIV_PROC *glUniform2uiv = nullptr;
    GLUNIFORM3UIV_PROC *glUniform3uiv = nullptr;
    GLUNIFORM4UIV_PROC *glUniform4uiv = nullptr;
    GLUNIFORM1F_PROC *glUniform1f = nullptr;
    GLUNIFORM1FV_PROC *glUniform1fv = nullptr;
    GLUNIFORM2FV_PROC *glUniform2fv = nullptr;
    GLUNIFORM3FV_PROC *glUniform3fv = nullptr;
    GLUNIFORM4FV_PROC *glUniform4fv = nullptr;
    GLUNIFORMMATRIX3FV_PROC *glUniformMatrix3fv = nullptr;
    GLUNIFORMMATRIX4FV_PROC *glUniformMatrix4fv = nullptr;
    GLGETUNIFORMBLOCKINDEX_PROC *glGetUniformBlockIndex = nullptr;
    GLGETACTIVEUNIFORMBLOCKIV_PROC *glGetActiveUniformBlockiv = nullptr;
    GLBINDBUFFERBASE_PROC *glBindBufferBase = nullptr;
    GLUNIFORMBLOCKBINDING_PROC *glUniformBlockBinding = nullptr;
    GLCOMPILESHADER_PROC *glCompileShader = nullptr;
    GLCOPYBUFFERSUBDATA_PROC *glCopyBufferSubData = nullptr;
    GLCREATEPROGRAM_PROC *glCreateProgram = nullptr;
    GLCREATESHADER_PROC *glCreateShader = nullptr;
    GLGETSHADERIV_PROC *glGetShaderiv = nullptr;
    GLSHADERSOURCE_PROC *glShaderSource = nullptr;
    GLDELETESHADER_PROC *glDeleteShader = nullptr;
    GLGETSHADERINFOLOG_PROC *glGetShaderInfoLog = nullptr;
    GLATTACHSHADER_PROC *glAttachShader = nullptr;
    GLDETACHSHADER_PROC *glDetachShader = nullptr;
    GLDELETEPROGRAM_PROC *glDeleteProgram = nullptr;
    GLLINKPROGRAM_PROC *glLinkProgram = nullptr;
    GLGETPROGRAMIV_PROC *glGetProgramiv = nullptr;
    GLGETPROGRAMINFOLOG_PROC *glGetProgramInfoLog = nullptr;
    GLUSEPROGRAM_PROC *glUseProgram = nullptr;
    GLGENVERTEXARRAYS_PROC *glGenVertexArrays = nullptr;
    GLBINDVERTEXARRAY_PROC *glBindVertexArray = nullptr;
    GLDELETEVERTEXARRAYS_PROC *glDeleteVertexArrays = nullptr;
    GLPATCHPARAMETERI_PROC *glPatchParameteri = nullptr;
    GLCLAMPCOLOR_PROC *glClampColor = nullptr;
    GLREADPIXELS_PROC *glReadPixels = nullptr;
    GLGETMULTISAMPLEFV_PROC *glGetMultisamplefv = nullptr;
    GLLOGICOP_PROC *glLogicOp = nullptr;
    GLCOPYIMAGESUBDATA_PROC *glCopyImageSubData = nullptr;
    GLGETTEXLEVELPARAMETERIV_PROC *glGetTexLevelParameteriv = nullptr;
    GLDISPATCHCOMPUTE_PROC *glDispatchCompute = nullptr;
    GLBINDIMAGETEXTURE_PROC *glBindImageTexture = nullptr;
    GLMEMORYBARRIER_PROC *glMemoryBarrier = nullptr;
    // CUDA
    CUCTXPUSHCURRENT_PROC *cuCtxPushCurrent_v2 = nullptr;
    CUCTXPOPCURRENT_PROC *cuCtxPopCurrent_v2 = nullptr;
    CUGRAPHICSGLREGISTERIMAGE_PROC *cuGraphicsGLRegisterImage = nullptr;
    CUGRAPHICSUNREGISTERRESOURCE_PROC *cuGraphicsUnregisterResource = nullptr;
    CUGRAPHICSMAPRESOURCES_PROC *cuGraphicsMapResources = nullptr;
    CUGRAPHICSUNMAPRESOURCES_PROC *cuGraphicsUnmapResources = nullptr;
    CUGRAPHICSSUBRESOURCEGETMAPPEDARRAY_PROC *cuGraphicsSubResourceGetMappedArray = nullptr;
    CUMEMCPY2D_PROC *cuMemcpy2D_v2 = nullptr;
#if _WIN64
    // WGL_NV_DX_interop2
    WGLDXOPENDEVICENV_PROC *wglDXOpenDeviceNV = nullptr;
    WGLDXCLOSEDEVICENV_PROC *wglDXCloseDeviceNV = nullptr;
    WGLDXREGISTEROBJECTNV_PROC *wglDXRegisterObjectNV = nullptr;
    WGLDXUNREGISTEROBJECTNV_PROC *wglDXUnregisterObjectNV = nullptr;
    WGLDXOBJECTACCESSNV_PROC *wglDXObjectAccessNV = nullptr;
    WGLDXLOCKOBJECTSNV_PROC *wglDXLockObjectsNV = nullptr;
    WGLDXUNLOCKOBJECTSNV_PROC *wglDXUnlockObjectsNV = nullptr;
#endif
public:
    ~Functions() = default;
    Functions(const Functions &) = delete;
    Functions(Functions &&) = default;
    auto operator=(const Functions &) -> Functions & = delete;
    auto operator=(Functions &&) -> Functions & = default;
    static auto getInstance() -> const Functions &;

private:
    Functions();
};

// GL functions
auto getSupportedExtensions() -> std::string;
auto hasExtension(const std::string &extensionName) -> bool;
auto glGetError() -> GLenum;
auto glIsEnabled(GLenum cap) -> GLboolean;
auto glGetInteger(GLenum pname) -> GLint;
void glFlush();
void glFinish();
void glClear(GLbitfield mask);
void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
void glClearDepthf(GLclampd depth);
void glClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint *value);
void glClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint *value);
void glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat *value);
void glScissor(GLint x, GLint y, GLsizei width, GLsizei height);
void glEnable(GLenum cap);
void glEnablei(GLenum cap, GLuint index);
void glDisable(GLenum cap);
void glDisablei(GLenum cap, GLuint index);
void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
void glGetBooleanv(GLenum pname, GLboolean *data);
void glGetFloatv(GLenum pname, GLfloat *data);
void glGetIntegerv(GLenum pname, GLint *params);
void glGetIntegeri_v(GLenum target, GLuint index, GLint *data);
auto glGetStringi(GLenum pname, GLuint index) -> GLuchar *;
void glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
void glGetTextureSubImage(GLuint texture,
                          GLint level,
                          GLint xoffset,
                          GLint yoffset,
                          GLint zoffset,
                          GLsizei width,
                          GLsizei height,
                          GLsizei depth,
                          GLenum format,
                          GLenum type,
                          GLsizei bufSize,
                          void *pixels);
void glGenTextures(GLsizei n, GLuint *textures);
void glDeleteTextures(GLsizei n, const GLuint *textures);
void glBindTexture(GLenum target, GLuint texture);
void glPixelStorei(GLenum pname, GLint param);
void glTexBuffer(GLenum target, GLenum internalformat, GLuint buffer);
void glTexImage1D(GLenum target,
                  GLint level,
                  GLint internalFormat,
                  GLsizei width,
                  GLint border,
                  GLenum format,
                  GLenum type,
                  const GLvoid *data);
void glTexImage2D(GLenum target,
                  GLint level,
                  GLint internalFormat,
                  GLsizei width,
                  GLsizei height,
                  GLint border,
                  GLenum format,
                  GLenum type,
                  const GLvoid *pixels);
void glTexImage2DMultisample(GLenum target,
                             GLsizei samples,
                             GLenum internalformat,
                             GLsizei width,
                             GLsizei height,
                             GLboolean fixedsamplelocations);
void glTexImage3D(GLenum target,
                  GLint level,
                  GLint internalformat,
                  GLsizei width,
                  GLsizei height,
                  GLsizei depth,
                  GLint border,
                  GLenum format,
                  GLenum type,
                  const GLvoid *pixels);
void glTextureView(GLuint texture,
                   GLenum target,
                   GLuint origtexture,
                   GLenum internalformat,
                   GLuint minlevel,
                   GLuint numlevels,
                   GLuint minlayer,
                   GLuint numlayers);
void glTexSubImage2D(GLenum target,
                     GLint level,
                     GLint xoffset,
                     GLint yoffset,
                     GLsizei width,
                     GLsizei height,
                     GLenum format,
                     GLenum type,
                     const GLvoid *pixels);
void glTexSubImage3D(GLenum target,
                     GLint level,
                     GLint xoffset,
                     GLint yoffset,
                     GLint zoffset,
                     GLsizei width,
                     GLsizei height,
                     GLsizei depth,
                     GLenum format,
                     GLenum type,
                     const GLvoid *pixels);
void glTexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
void glTexParameteri(GLenum target, GLenum pname, GLint param);

void glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
void glTexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
void glDrawElementsIndirect(GLenum mode, GLenum type, const void* indirect);

void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
void glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei instanceCount);
void glDepthFunc(GLenum func);
void glDepthMask(GLboolean flag);
void glCullFace(GLenum mode);
void glFrontFace(GLenum mode);
void glBlendEquation(GLenum mode);
void glBlendEquationi(GLuint buf, GLenum mode);
void glBlendFunc(GLenum sfactor, GLenum dfactor);
void glBlendFunci(GLuint buf, GLenum sfactor, GLenum dfactor);
void glPointSize(GLfloat size);
void glLineWidth(GLfloat size);
void glActiveTexture(GLenum texture);
auto glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access) -> GLvoid *;
auto glUnmapBuffer(GLenum target) -> GLboolean;
void glBindBuffer(GLenum target, GLuint buffer);
void glReadBuffer(GLenum buffer);
void glDrawBuffer(GLenum buffer);
void glDrawBuffers(GLsizei n, const GLenum *buffer);
void glGenBuffers(GLsizei n, GLuint *buffers);
void glDeleteBuffers(GLsizei n, const GLuint *buffers);
void glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
void glGetBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
void glGenerateMipmap(GLenum target);
void glGenRenderbuffers(GLsizei n, GLuint *renderbuffers);
void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
void glRenderbufferStorageMultisample(GLenum target,
                                      GLsizei samples,
                                      GLenum internalformat,
                                      GLsizei width,
                                      GLsizei height);
void glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers);
void glBindRenderbuffer(GLenum target, GLuint renderbuffer);
void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
void glFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
void glGenFramebuffers(GLsizei n, GLuint *framebuffers);
void glBindFramebuffer(GLenum target, GLuint framebuffer);
void glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers);
void glBlitFramebuffer(GLint srcX0,
                       GLint srcY0,
                       GLint srcX1,
                       GLint srcY1,
                       GLint dstX0,
                       GLint dstY0,
                       GLint dstX1,
                       GLint dstY1,
                       GLbitfield mask,
                       GLenum filter);
auto glCheckFramebufferStatus(GLenum target) -> GLenum;
auto glGetAttribLocation(GLuint program, const GLchar *name) -> GLint;
void glEnableVertexAttribArray(GLuint index);
void glVertexAttribPointer(GLuint index,
                           GLint size,
                           GLenum type,
                           GLboolean normalized,
                           GLsizei stride,
                           const GLvoid *pointer);
void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
auto glGetUniformLocation(GLuint program, const GLchar *name) -> GLint;
void glUniform1i(GLint location, GLint v0);
void glUniform1iv(GLint location, GLsizei count, const GLint *value);
void glUniform2iv(GLint location, GLsizei count, const GLint *value);
void glUniform3iv(GLint location, GLsizei count, const GLint *value);
void glUniform4iv(GLint location, GLsizei count, const GLint *value);
void glUniform1ui(GLint location, GLuint v0);
void glUniform1uiv(GLint location, GLsizei count, const GLuint *value);
void glUniform2uiv(GLint location, GLsizei count, const GLuint *value);
void glUniform3uiv(GLint location, GLsizei count, const GLuint *value);
void glUniform4uiv(GLint location, GLsizei count, const GLuint *value);
void glUniform1f(GLint location, GLfloat v0);
void glUniform1fv(GLint location, GLsizei count, const GLfloat *value);
void glUniform2fv(GLint location, GLsizei count, const GLfloat *value);
void glUniform3fv(GLint location, GLsizei count, const GLfloat *value);
void glUniform4fv(GLint location, GLsizei count, const GLfloat *value);
void glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
auto glGetUniformBlockIndex(GLuint program, const GLchar *uniformBlockName) -> GLuint;
void glGetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
void glBindBufferBase(GLenum target, GLuint index, GLuint buffer);
void glUniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
void glCompileShader(GLuint shader);
void glCopyBufferSubData(GLenum readTarget,
                         GLenum writeTarget,
                         GLintptr readOffset,
                         GLintptr writeOffset,
                         GLsizeiptr size);
auto glCreateProgram() -> GLuint;
auto glCreateShader(GLenum type) -> GLuint;
void glGetShaderiv(GLuint shader, GLenum pname, GLint *params);
void glShaderSource(GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length);
void glDeleteShader(GLuint shader);
void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
void glAttachShader(GLuint program, GLuint shader);
void glDetachShader(GLuint program, GLuint shader);
void glDeleteProgram(GLuint program);
void glLinkProgram(GLuint program);
void glGetProgramiv(GLuint program, GLenum pname, GLint *params);
void glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
void glUseProgram(GLuint program);
void glGenVertexArrays(GLsizei n, GLuint *arrays);
void glBindVertexArray(GLuint array);
void glDeleteVertexArrays(GLsizei n, const GLuint *arrays);
void glPatchParameteri(GLenum pname, GLint value);
void glClampColor(GLenum target, GLenum clamp);
void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *data);
void glGetMultisamplefv(GLenum pname, GLuint index, GLfloat *val);
auto isDepthMaskEnabled() -> bool;
void glLogicOp(GLenum opcode);
void glCopyImageSubData(GLuint srcName,
                        GLenum srcTarget,
                        GLint srcLevel,
                        GLint srcX,
                        GLint srcY,
                        GLint srcZ,
                        GLuint dstName,
                        GLenum dstTarget,
                        GLint dstLevel,
                        GLint dstX,
                        GLint dstY,
                        GLint dstZ,
                        GLsizei srcWidth,
                        GLsizei srcHeight,
                        GLsizei srcDepth);
void glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params);
auto glGetErrorString(GLenum errorCode) -> const char *;
void glLogAllError();
void glDispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
void glBindImageTexture(GLuint unit,
                        GLuint texture,
                        GLint level,
                        GLboolean layered,
                        GLint layer,
                        GLenum access,
                        GLenum format);
void glMemoryBarrier(GLbitfield barriers);
// CUDA functions
auto cuCtxPushCurrent(CUcontext ctx) -> CUresult;
auto cuCtxPopCurrent(CUcontext *pctx) -> CUresult;
auto cuGraphicsGLRegisterImage(CUgraphicsResource *pCudaResource, GLuint image, GLenum target, unsigned Flags)
    -> CUresult;
auto cuGraphicsUnregisterResource(CUgraphicsResource resource) -> CUresult;
auto cuGraphicsMapResources(unsigned count, CUgraphicsResource *resources, CUstream hStream) -> CUresult;
auto cuGraphicsUnmapResources(unsigned count, CUgraphicsResource *resources, CUstream hStream) -> CUresult;
auto cuGraphicsSubResourceGetMappedArray(CUarray *pArray,
                                         CUgraphicsResource resource,
                                         unsigned arrayIndex,
                                         unsigned mipLevel) -> CUresult;
auto cuMemcpy2D(const CUDA_MEMCPY2D *pCopy) -> CUresult;
#if _WIN64
// WGL_NV_DX_interop2
auto wglDXOpenDeviceNV(HANDLE dxDevice) -> HANDLE;
auto wglDXCloseDeviceNV(HANDLE hDevice) -> BOOL;
auto wglDXRegisterObjectNV(HANDLE hDevice, HANDLE dxObject, GLuint name, GLenum type, GLenum access) -> HANDLE;
auto wglDXUnregisterObjectNV(HANDLE hDevice, HANDLE hObject) -> BOOL;
auto wglDXObjectAccessNV(HANDLE hObject, GLenum access) -> BOOL;
auto wglDXLockObjectsNV(HANDLE hDevice, GLint count, HANDLE *hObjects) -> BOOL;
auto wglDXUnlockObjectsNV(HANDLE hDevice, GLint count, HANDLE *hObjects) -> BOOL;
#endif
} // namespace iloj::gpu
