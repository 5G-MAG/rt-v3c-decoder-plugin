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

#include "logger.h"
#include <jni.h>
#include <map>

#define CHECK_JNI(cmd)                                                                                                 \
    cmd;                                                                                                               \
    if (getError())                                                                                                    \
    {                                                                                                                  \
        LOG_ERROR("JNI exception");                                                                                    \
    }

namespace iloj::misc
{
namespace JNI
{
////////////////////////////////////////////////////////////////////////////////////////////////////
auto getError() -> bool;

////////////////////////////////////////////////////////////////////////////////////////////////////
class Object
{
public:
    class Interface
    {
    public:
        class Factory
        {
        private:
            std::map<std::string, Interface> m_interfaceMap;

        public:
            void registerInterface(Interface interface);
            auto getInterface(const std::string &name) const -> const Interface &;
        };

    private:
        std::string m_name;
        std::map<std::string, std::string> m_methodMap;

    public:
        Interface() = default;
        ~Interface() = default;
        Interface(std::string name, const std::vector<std::pair<std::string, std::string>> &methodList);
        Interface(const Interface &) = default;
        Interface(Interface &&other) = default;
        auto operator=(const Interface &) -> Interface & = default;
        auto operator=(Interface &&other) -> Interface & = default;
        auto getClassName() const -> const std::string & { return m_name; }
        auto getMethodSignature(const std::string &methodName) const -> const std::string &;
        template<typename... ARGS>
        auto create(ARGS... args) const -> Object;
    };

private:
    Reference<const Interface> m_interface;
    jobject m_instance = nullptr;

public:
    Object() = default;
    Object(const Interface &interface, jobject instance): m_interface{interface}, m_instance{instance} {}
    ~Object();
    Object(const Object &) = delete;
    Object(Object &&other);
    auto operator=(const Object &) -> Object & = delete;
    auto operator=(Object &&other) -> Object &;
    auto getInterface() const -> const Interface & { return m_interface.get(); }
    auto getNativeObject() const -> jobject { return m_instance; }
    auto getMethodId(const std::string &methodName) const -> jmethodID;
    void registerNatives(const std::vector<JNINativeMethod> &nativeMethods) const;
    template<typename... ARGS>
    auto callBooleanMethod(const std::string &methodName, ARGS &&... args) const -> jboolean;
    template<typename... ARGS>
    auto callByteMethod(const std::string &methodName, ARGS &&... args) const -> jbyte;
    template<typename... ARGS>
    auto callCharMethod(const std::string &methodName, ARGS &&... args) const -> jchar;
    template<typename... ARGS>
    auto callDoubleMethod(const std::string &methodName, ARGS &&... args) const -> jdouble;
    template<typename... ARGS>
    auto callFloatMethod(const std::string &methodName, ARGS &&... args) const -> jfloat;
    template<typename... ARGS>
    auto callIntMethod(const std::string &methodName, ARGS &&... args) const -> jint;
    template<typename... ARGS>
    auto callLongMethod(const std::string &methodName, ARGS &&... args) const -> jlong;
    template<typename... ARGS>
    auto callObjectMethod(const std::string &methodName, ARGS &&... args) const -> jobject;
    template<typename... ARGS>
    auto callShortMethod(const std::string &methodName, ARGS &&... args) const -> jshort;
    template<typename... ARGS>
    auto callStaticBooleanMethod(const std::string &methodName, ARGS &&... args) const -> jboolean;
    template<typename... ARGS>
    auto callStaticByteMethod(const std::string &methodName, ARGS &&... args) const -> jbyte;
    template<typename... ARGS>
    auto callStaticCharMethod(const std::string &methodName, ARGS &&... args) const -> jchar;
    template<typename... ARGS>
    auto callStaticDoubleMethod(const std::string &methodName, ARGS &&... args) const -> jdouble;
    template<typename... ARGS>
    auto callStaticFloatMethod(const std::string &methodName, ARGS &&... args) const -> jfloat;
    template<typename... ARGS>
    auto callStaticIntMethod(const std::string &methodName, ARGS &&... args) const -> jint;
    template<typename... ARGS>
    auto callStaticLongMethod(const std::string &methodName, ARGS &&... args) const -> jlong;
    template<typename... ARGS>
    auto callStaticObjectMethod(const std::string &methodName, ARGS &&... args) const -> jobject;
    template<typename... ARGS>
    auto callStaticShortMethod(const std::string &methodName, ARGS &&... args) const -> jshort;
    template<typename... ARGS>
    void callVoidMethod(const std::string &methodName, ARGS &&... args) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
class Context
{
public:
    class Proxy
    {
    private:
        static SpinLock m_locker;

    public:
        Proxy();
        ~Proxy();
    };

private:
    JavaVM *m_javaVM = nullptr;
    JNIEnv *m_jniEnv = nullptr;
    Object m_loaderHelper;
    Object::Interface::Factory m_factory;

public:
    static auto getInstance() -> Context &;
    void setJavaVirtualMachine(JavaVM *javaVM);
    auto getJavaVirtualMachine() -> JavaVM * { return m_javaVM; }
    auto getEnvironment() -> JNIEnv * { return m_jniEnv; }
    auto getFactory() -> Object::Interface::Factory & { return m_factory; }
    void clear();
    auto attach() -> JNIEnv *;
    void detach();
    auto findClass(const std::string &name) -> jclass;

private:
    Context() = default;
    ~Context() = default;
    Context(const Context &) = delete;
    Context(Context &&) = delete;
    auto operator=(const Context &) -> Context & = delete;
    auto operator=(Context &&) -> Context & = delete;

private:
    void createLoaderHelper();
};

} // namespace JNI

} // namespace iloj::misc

#include "jni.hpp"
