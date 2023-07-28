/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <dlfcn.h>
#include <stdio.h>
#include <string>
#include <opencv2/opencv.hpp>

void *handle = NULL;
bool (*edgeai_isStarted)() = NULL;
bool (*edgeai_startup)(const std::string& basePth) = NULL;
bool (*edgeai_shutdown)() = NULL;
bool (*edgeai_createDetector)(int type, const std::string& option) = NULL;
bool (*edgeai_deleteDetector)(int type) = NULL;
bool (*edgeai_detect)(int type, const cv::Mat &input, std::string &output) = NULL;
bool (*edgeai_detectFromFile)(int type, const std::string &inputPath, std::string &output) = NULL;
bool (*edgeai_detectFromBase64)(int type, const std::string &input, std::string &output) = NULL;

bool init()
{
    handle = dlopen("/usr/lib/libedgeai-vision.so.1", RTLD_LAZY);
    if(handle == NULL) {
        printf("error %s\n", dlerror());
        return false;
    }

    const char* funcIsStarted = "edgeai_isStarted";
    edgeai_isStarted = (bool (*)()) dlsym(handle, funcIsStarted);
    if(dlerror() != NULL) {
        printf("dlsym error: %s (%s)\n", funcIsStarted, dlerror());
        dlclose(handle);
        handle = NULL;
        return false;
    }

    const char* funcStartup = "edgeai_startup";
    edgeai_startup = (bool (*)(const std::string& basePath)) dlsym(handle, funcStartup);
    if(dlerror() != NULL) {
        printf("dlsym error: %s (%s)\n", funcStartup, dlerror());
        dlclose(handle);
        handle = NULL;
        return false;
    }

    const char* funcShutdown = "edgeai_shutdown";
    edgeai_shutdown= (bool (*)()) dlsym(handle, funcShutdown);
    if(dlerror() != NULL) {
        printf("dlsym error: %s (%s)\n", funcShutdown, dlerror());
        dlclose(handle);
        handle = NULL;
        return false;
    }

    const char* funcCreateDetector = "edgeai_createDetector";
    edgeai_createDetector = (bool (*)(int, const std::string&)) dlsym(handle, funcCreateDetector);
    if(dlerror() != NULL) {
        printf("dlsym error: %s (%s)\n", funcCreateDetector, dlerror());
        dlclose(handle);
        handle = NULL;
        return false;
    }

    const char* funcDeleteDetector = "edgeai_deleteDetector";
    edgeai_deleteDetector = (bool (*)(int)) dlsym(handle, funcDeleteDetector);
    if(dlerror() != NULL) {
        printf("dlsym error: %s (%s)\n", funcDeleteDetector, dlerror());
        dlclose(handle);
        handle = NULL;
        return false;
    }

    const char* funcDetect = "edgeai_detect";
    edgeai_detect = (bool (*)(int, const cv::Mat&, std::string&)) dlsym(handle, funcDetect);
    if(dlerror() != NULL) {
        printf("dlsym error: %s (%s)\n", funcDetect, dlerror());
        dlclose(handle);
        handle = NULL;
        return false;
    }

    const char* funcDetectFromFile = "edgeai_detectFromFile";
    edgeai_detectFromFile = (bool (*)(int, const std::string&, std::string&)) dlsym(handle, funcDetectFromFile);
    if(dlerror() != NULL) {
        printf("dlsym error: %s (%s)\n", funcDetect, dlerror());
        dlclose(handle);
        handle = NULL;
        return false;
    }

    const char* funcDetectFromBase64 = "edgeai_detectFromBase64";
    edgeai_detectFromBase64= (bool (*)(int, const std::string&, std::string&)) dlsym(handle, funcDetectFromBase64);
    if(dlerror() != NULL) {
        printf("dlsym error: %s (%s)\n", funcDetectFromBase64, dlerror());
        dlclose(handle);
        handle = NULL;
        return false;
    }


    return true;
}

bool deinit() {
    if (handle) {
        dlclose(handle);
        handle = NULL;
    }
    return true;
}

int main(int argc, char **argv)
{
    if (!init()) {
        printf("failed to init libedgeai-vision.so.1\n");
        return 0;
    }
    cv::Mat input = cv::imread("/usr/share/aif/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;
    int type = 0; // DetectorType::FACE

    if (!edgeai_startup("")) {
        printf("failed to startup\n");
        return 0;
    }
    if (!edgeai_createDetector(type, "")) {
        printf("failed to createDetector\n");
        return 0;
    }
    if (!edgeai_detect(type, input, output)) {
        printf("failed to detect\n");
        return 0;
    }
    if (!edgeai_deleteDetector(type)) {
        printf("failed to deleteDetector\n");
        return 0;
    }
    if (!edgeai_shutdown()) {
        printf("failed to shutdown\n");
        return 0;
    }

    printf("output: %s\n", output.c_str());

    if (!deinit()) {
        printf("failed to deinit\n");
    }
    return 0;
}
