/*
* Copyright (c) 2025 InterDigital CE Patent Holdings SASU
* Licensed under the License terms of 5GMAG software (the "License").
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at https://www.5g-mag.com/license .
* Unless required by applicable law or agreed to in writing, software distributed under the License is
* distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*/

// Unity Native Plugin API copyright © 2015 Unity Technologies ApS
//
// Licensed under the Unity Companion License for Unity - dependent projects--see[Unity Companion
// License](http://www.unity3d.com/legal/licenses/Unity_Companion_License).
//
// Unless expressly provided otherwise, the Software under this license is made available strictly on an “AS IS” BASIS
// WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.Please review the license for details on these and other terms and
// conditions.

#pragma once

#include "IUnityInterface.h"
#include <d3d11.h>

// Should only be used on the rendering thread unless noted otherwise.
UNITY_DECLARE_INTERFACE(IUnityGraphicsD3D11)
{
    ID3D11Device *(UNITY_INTERFACE_API * GetDevice)();

    ID3D11Resource *(UNITY_INTERFACE_API * TextureFromRenderBuffer)(UnityRenderBuffer buffer);
    ID3D11Resource *(UNITY_INTERFACE_API * TextureFromNativeTexture)(UnityTextureID texture);

    ID3D11RenderTargetView *(UNITY_INTERFACE_API * RTVFromRenderBuffer)(UnityRenderBuffer surface);
    ID3D11ShaderResourceView *(UNITY_INTERFACE_API * SRVFromNativeTexture)(UnityTextureID texture);
};

UNITY_REGISTER_INTERFACE_GUID(0xAAB37EF87A87D748ULL, 0xBF76967F07EFB177ULL, IUnityGraphicsD3D11)
