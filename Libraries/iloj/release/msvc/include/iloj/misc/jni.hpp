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

#include "jni.h"

#define DEFINE_JNI_NONVOID_METHOD(codeName, jniName, returnType)                                                       \
    template<typename... ARGS>                                                                                         \
    auto Object::codeName(const std::string &methodName, ARGS &&... args) const->returnType                            \
    {                                                                                                                  \
        if (auto methodId = getMethodId(methodName.c_str()); methodId != nullptr)                                      \
        {                                                                                                              \
            auto ret =                                                                                                 \
                Context::getInstance().getEnvironment()->jniName(m_instance, methodId, std::forward<ARGS>(args)...);   \
                                                                                                                       \
            if (getError())                                                                                            \
            {                                                                                                          \
                LOG_ERROR(std::string(#jniName), ": ", methodName);                                                    \
            }                                                                                                          \
                                                                                                                       \
            return ret;                                                                                                \
        }                                                                                                              \
                                                                                                                       \
        return {};                                                                                                     \
    }

namespace iloj::misc
{
namespace JNI
{
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename... ARGS>
auto Object::Interface::create(ARGS... args) const -> Object
{
    auto env = Context::getInstance().getEnvironment();

    if (auto clazz = Context::getInstance().findClass(m_name))
    {
        static const std::string methodName = "<init>";
        auto methodId = env->GetMethodID(clazz, methodName.c_str(), getMethodSignature(methodName).c_str());

        if (getError())
        {
            LOG_ERROR("GetMethodID: ", methodName.c_str(), " / ", getMethodSignature(methodName).c_str());
            return {};
        }

        if (auto localRef = env->NewObject(clazz, methodId, std::forward<ARGS>(args)...);
            !getError() && (localRef != nullptr))
        {
            if (auto globalRef = env->NewGlobalRef(localRef); !getError() && (globalRef != nullptr))
            {
                env->DeleteLocalRef(localRef);

                if (!getError())
                {
                    return {*this, globalRef};
                }
                else
                {
                    LOG_ERROR("Failed deleting local reference");
                }
            }
            else
            {
                LOG_ERROR("Failed creating global reference");
            }
        }
        else
        {
            LOG_ERROR("Failed creating object");
        }
    }

    return {};
}

////////////////////////////////////////////////////////////////////////////////////////////////////
DEFINE_JNI_NONVOID_METHOD(callBooleanMethod, CallBooleanMethod, jboolean)
DEFINE_JNI_NONVOID_METHOD(callByteMethod, CallByteMethod, jbyte)
DEFINE_JNI_NONVOID_METHOD(callCharMethod, CallCharMethod, jchar)
DEFINE_JNI_NONVOID_METHOD(callDoubleMethod, CallDoubleMethod, jdouble)
DEFINE_JNI_NONVOID_METHOD(callFloatMethod, CallFloatMethod, jfloat)
DEFINE_JNI_NONVOID_METHOD(callIntMethod, CallIntMethod, jint)
DEFINE_JNI_NONVOID_METHOD(callLongMethod, CallLongMethod, jlong)
DEFINE_JNI_NONVOID_METHOD(callObjectMethod, CallObjectMethod, jobject)
DEFINE_JNI_NONVOID_METHOD(callShortMethod, CallShortMethod, jshort)

DEFINE_JNI_NONVOID_METHOD(callStaticBooleanMethod, CallStaticBooleanMethod, jboolean)
DEFINE_JNI_NONVOID_METHOD(callStaticByteMethod, CallStaticByteMethod, jbyte)
DEFINE_JNI_NONVOID_METHOD(callStaticCharMethod, CallStaticCharMethod, jchar)
DEFINE_JNI_NONVOID_METHOD(callStaticDoubleMethod, CallStaticDoubleMethod, jdouble)
DEFINE_JNI_NONVOID_METHOD(callStaticFloatMethod, CallStaticFloatMethod, jfloat)
DEFINE_JNI_NONVOID_METHOD(callStaticIntMethod, CallStaticIntMethod, jint)
DEFINE_JNI_NONVOID_METHOD(callStaticLongMethod, CallStaticLongMethod, jlong)
DEFINE_JNI_NONVOID_METHOD(callStaticObjectMethod, CallStaticObjectMethod, jobject)
DEFINE_JNI_NONVOID_METHOD(callStaticShortMethod, CallStaticShortMethod, jshort)

template<typename... ARGS>
void Object::callVoidMethod(const std::string &methodName, ARGS &&... args) const
{
    if (auto methodId = getMethodId(methodName.c_str()); methodId != nullptr)
    {
        Context::getInstance().getEnvironment()->CallVoidMethod(m_instance, methodId, std::forward<ARGS>(args)...);

        if (getError())
        {
            LOG_ERROR("CallVoidMethod: ", methodName);
        }
    }
}

} // namespace JNI

} // namespace iloj::misc
