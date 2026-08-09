/*
 * Copyright (C) 2023 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "libbinder.OS_android"

#include "OS.h"

#include <android-base/threads.h>
#include <binder/Common.h>
#include <cutils/trace.h>
#include <utils/misc.h>

namespace android::binder {
namespace os {

uint64_t GetThreadId() {
#ifdef BINDER_RPC_SINGLE_THREADED
    return 0;
#else
    return base::GetThreadId();
#endif
}

bool report_sysprop_change() {
    android::report_sysprop_change();
    return true;
}

void trace_begin(uint64_t tag, const char* name) {
    atrace_begin(tag, name);
}

void trace_end(uint64_t tag) {
    atrace_end(tag);
}

void trace_int(uint64_t tag, const char* name, int32_t value) {
    atrace_int(tag, name, value);
}

uint64_t get_trace_enabled_tags() {
    return atrace_enabled_tags;
}

} // namespace os

// Legacy trace symbol. To be removed once all of downstream rebuilds.
//
// arcfox: these two MUST be exported. libbinder builds with -fvisibility=hidden
// (see BUILDING_LIBBINDER in Android.bp), so without an export macro these legacy
// shims are compiled but invisible, and every prebuilt vendor blob still linked
// against the old android::binder::atrace_begin fails to load:
//
//   CANNOT LINK EXECUTABLE "/vendor/bin/qms": cannot locate symbol
//     "_ZN7android6binder12atrace_beginEmPKc" (@LIBBINDER)
//
// That took out qms, qcc-vendor, xtra-daemon and cnd (via libcne.so) -- four
// services in permanent restart loops. The symbols the blobs want are exactly
// these; AOSP moved the real implementation into binder::os:: and left these as
// the compatibility shims, which is precisely the "downstream has not rebuilt"
// case they were written for. LIBBINDER_INTERNAL_EXPORTED is the macro AOSP
// provides for symbols that are exported but tracked for eventual removal, so it
// is the right one here: when upstream deletes these shims this patch dies with
// them.
LIBBINDER_INTERNAL_EXPORTED void atrace_begin(uint64_t tag, const char* name) {
    os::trace_begin(tag, name);
}

// Legacy trace symbol. To be removed once all of downstream rebuilds.
LIBBINDER_INTERNAL_EXPORTED void atrace_end(uint64_t tag) {
    os::trace_end(tag);
}

} // namespace android::binder
