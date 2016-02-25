/***
* ==++==
*
* Copyright (c) Microsoft Corporation. All rights reserved.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
**/
#include "stdafx.h"

#if defined(__ANDROID__)
#include <android/log.h>
#include <jni.h>
#endif

namespace crossplat
{

static std::once_flag g_shared_threadpool_init;
static std::unique_ptr<threadpool> g_shared_threadpool;

static void init_threadpool(int threads) {
    g_shared_threadpool.reset(new threadpool(threads));
}

#if defined(__ANDROID__)
// This pointer will be 0-initialized by default (at load time).
std::atomic<JavaVM*> JVM;

static void abort_if_no_jvm()
{
    if (JVM == nullptr)
    {
        __android_log_print(ANDROID_LOG_ERROR, "CPPRESTSDK", "%s", "The CppREST SDK must be initialized before first use on android: https://github.com/Microsoft/cpprestsdk/wiki/How-to-build-for-Android");
        std::abort();
    }
}

JNIEnv* get_jvm_env()
{
    abort_if_no_jvm();
    JNIEnv* env = nullptr;
    auto result = JVM.load()->AttachCurrentThread(&env, nullptr);
    if (result != JNI_OK)
    {
        throw std::runtime_error("Could not attach to JVM");
    }

    return env;
}

threadpool& threadpool::shared_instance()
{
    abort_if_no_jvm();
    std::call_once(crossplat::g_shared_threadpool_init, crossplat::init_threadpool, 20);
    return *crossplat::g_shared_threadpool;
}

#else

// initialize the static shared threadpool
threadpool& threadpool::shared_instance()
{
    std::call_once(crossplat::g_shared_threadpool_init, crossplat::init_threadpool, 20);
    return *crossplat::g_shared_threadpool;
}

#endif

}

#if defined(__ANDROID__)
void cpprest_init(JavaVM* vm, int default_threadpool_size) {
    if (crossplat::g_shared_threadpool != nullptr) {
        // Must call cpprest_init before any other cpprest functionality
        std::abort();
    }
    std::call_once(crossplat::g_shared_threadpool_init, crossplat::init_threadpool, default_threadpool_size);

    crossplat::JVM = vm;
}
#else
void cpprest_init(int default_threadpool_size) {
    if (crossplat::g_shared_threadpool != nullptr) {
        // Must call cpprest_init before any other cpprest functionality
        std::abort();
    }

    std::call_once(crossplat::g_shared_threadpool_init, crossplat::init_threadpool, default_threadpool_size);
}
#endif

