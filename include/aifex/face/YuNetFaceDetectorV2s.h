/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef AIF_YUNET_FACE_DETECTORV2S_H
#define AIF_YUNET_FACE_DETECTORV2S_H

#include <aifex/face/YuNetFaceDetectorV2.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aifex/face/FaceDescriptor.h>

namespace aif {

class YuNetFaceDetectorV2_180_320 : public YuNetFaceDetectorV2 {
    private:
        YuNetFaceDetectorV2_180_320();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~YuNetFaceDetectorV2_180_320() {}

};
DetectorFactoryRegistration<YuNetFaceDetectorV2_180_320, FaceDescriptor>
    face_yunet_180_320("face_yunet_180_320");

class YuNetFaceDetectorV2_180_320_Quant : public YuNetFaceDetectorV2 {
    private:
        YuNetFaceDetectorV2_180_320_Quant();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~YuNetFaceDetectorV2_180_320_Quant() {}
};
DetectorFactoryRegistration<YuNetFaceDetectorV2_180_320_Quant, FaceDescriptor>
    face_yunet_180_320_quant("face_yunet_180_320_quant");


class YuNetFaceDetectorV2_240_320 : public YuNetFaceDetectorV2 {
    private:
        YuNetFaceDetectorV2_240_320();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~YuNetFaceDetectorV2_240_320() {}

};
DetectorFactoryRegistration<YuNetFaceDetectorV2_240_320, FaceDescriptor>
    face_yunet_240_320("face_yunet_240_320");

class YuNetFaceDetectorV2_240_320_Quant : public YuNetFaceDetectorV2 {
    private:
        YuNetFaceDetectorV2_240_320_Quant();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~YuNetFaceDetectorV2_240_320_Quant() {}
};
DetectorFactoryRegistration<YuNetFaceDetectorV2_240_320_Quant, FaceDescriptor>
    face_yunet_240_320_quant("face_yunet_240_320_quant");


class YuNetFaceDetectorV2_320_320 : public YuNetFaceDetectorV2 {
    private:
        YuNetFaceDetectorV2_320_320();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~YuNetFaceDetectorV2_320_320() {}

};
DetectorFactoryRegistration<YuNetFaceDetectorV2_320_320, FaceDescriptor>
    face_yunet_320_320("face_yunet_320_320");

class YuNetFaceDetectorV2_320_320_Quant : public YuNetFaceDetectorV2 {
    private:
        YuNetFaceDetectorV2_320_320_Quant();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~YuNetFaceDetectorV2_320_320_Quant() {}
};
DetectorFactoryRegistration<YuNetFaceDetectorV2_320_320_Quant, FaceDescriptor>
    face_yunet_320_320_quant("face_yunet_320_320_quant");


class YuNetFaceDetectorV2_360_640 : public YuNetFaceDetectorV2 {
    private:
        YuNetFaceDetectorV2_360_640();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~YuNetFaceDetectorV2_360_640() {}

};
DetectorFactoryRegistration<YuNetFaceDetectorV2_360_640, FaceDescriptor>
    face_yunet_360_640("face_yunet_360_640");

class YuNetFaceDetectorV2_360_640_Quant : public YuNetFaceDetectorV2 {
    private:
        YuNetFaceDetectorV2_360_640_Quant();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~YuNetFaceDetectorV2_360_640_Quant() {}
};
DetectorFactoryRegistration<YuNetFaceDetectorV2_360_640_Quant, FaceDescriptor>
    face_yunet_360_640_quant("face_yunet_360_640_quant");

class YuNetFaceDetectorV2_480_640 : public YuNetFaceDetectorV2 {
    private:
        YuNetFaceDetectorV2_480_640();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~YuNetFaceDetectorV2_480_640() {}

};
DetectorFactoryRegistration<YuNetFaceDetectorV2_480_640, FaceDescriptor>
    face_yunet_480_640("face_yunet_480_640");

class YuNetFaceDetectorV2_480_640_Quant : public YuNetFaceDetectorV2 {
    private:
        YuNetFaceDetectorV2_480_640_Quant();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~YuNetFaceDetectorV2_480_640_Quant() {}
};
DetectorFactoryRegistration<YuNetFaceDetectorV2_480_640_Quant, FaceDescriptor>
    face_yunet_480_640_quant("face_yunet_480_640_quant");

class YuNetFaceDetectorV2_640_640 : public YuNetFaceDetectorV2 {
    private:
        YuNetFaceDetectorV2_640_640();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~YuNetFaceDetectorV2_640_640() {}

};
DetectorFactoryRegistration<YuNetFaceDetectorV2_640_640, FaceDescriptor>
    face_yunet_640_640("face_yunet_640_640");

class YuNetFaceDetectorV2_640_640_Quant : public YuNetFaceDetectorV2 {
    private:
        YuNetFaceDetectorV2_640_640_Quant();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~YuNetFaceDetectorV2_640_640_Quant() {}
};
DetectorFactoryRegistration<YuNetFaceDetectorV2_640_640_Quant, FaceDescriptor>
    face_yunet_640_640_quant("face_yunet_640_640_quant");

class YuNetFaceDetectorV2_270_480 : public YuNetFaceDetectorV2 {
    private:
        YuNetFaceDetectorV2_270_480();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~YuNetFaceDetectorV2_270_480() {}

};
DetectorFactoryRegistration<YuNetFaceDetectorV2_270_480, FaceDescriptor>
    face_yunet_270_480("face_yunet_270_480");

class YuNetFaceDetectorV2_270_480_Quant : public YuNetFaceDetectorV2 {
    private:
        YuNetFaceDetectorV2_270_480_Quant();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~YuNetFaceDetectorV2_270_480_Quant() {}
};
DetectorFactoryRegistration<YuNetFaceDetectorV2_270_480_Quant, FaceDescriptor>
    face_yunet_270_480_quant("face_yunet_270_480_quant");
}
#endif // AIF_YUNET_FACE_DETECTORV2S_H
