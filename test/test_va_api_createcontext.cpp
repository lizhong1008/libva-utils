/*
 * Copyright (C) 2016 Intel Corporation. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "test_va_api_createcontext.h"

namespace VAAPI {
VAAPICreateContext::VAAPICreateContext() { m_vaDisplay = doInitialize(); }

VAAPICreateContext::~VAAPICreateContext() { doTerminate(); }

VAAPICreateContextToFail::VAAPICreateContextToFail()
{
    m_vaDisplay = doInitialize();
}

VAAPICreateContextToFail::~VAAPICreateContextToFail() { doTerminate(); }

TEST_F(VAAPICreateContextToFail, CreateContextWithNoConfig)
{
    std::pair<uint32_t, uint32_t> currentResolution
        = std::make_pair(1920, 1080);
    // There's no need to test all inputs for this to be a good test
    // as long as there's no config the expected error should be
    // returned

    doCreateContext(currentResolution, VA_STATUS_ERROR_INVALID_CONFIG);
}

TEST_P(VAAPICreateContext, CreateContext)
{

    VAProfile currentProfile = ::testing::get<0>(GetParam());
    VAEntrypoint currentEntrypoint = ::testing::get<1>(GetParam());
    std::pair<uint32_t, uint32_t> currentResolution
        = ::testing::get<2>(GetParam());
    std::pair<uint32_t, uint32_t> maxResolution;

    // vaCreateContext requires a valida VAConfigID, vaCreateConfig requires a
    // supported profile and entrypoint

    doGetMaxValues();

    doQueryConfigProfiles();

    if (doFindProfileInList(currentProfile)) {

        doQueryConfigEntrypoints(currentProfile);
        if (doFindEntrypointInList(currentEntrypoint)) {
            // profile and entrypoint are supported

            doCreateConfig(currentProfile, currentEntrypoint);
            doGetMaxSurfaceResolution(currentProfile, currentEntrypoint, maxResolution);
            if (currentResolution.first > maxResolution.first
                || currentResolution.second > maxResolution.second) {
                doCreateContext(currentResolution,
                                VA_STATUS_ERROR_RESOLUTION_NOT_SUPPORTED);
                doDestroyContext(VA_STATUS_ERROR_INVALID_CONTEXT);
            }
            else {
                doCreateContext(currentResolution);
                doDestroyContext();
            }
            doDestroyConfig();
        }
    }
}

INSTANTIATE_TEST_CASE_P(
    CreateContext, VAAPICreateContext,
    ::testing::Combine(::testing::ValuesIn(m_vaProfiles),
                       ::testing::ValuesIn(m_vaEntrypoints),
                       ::testing::ValuesIn(m_vaResolutions)));
} // VAAPI
