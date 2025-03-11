#pragma once
#include <id3DevicesLib.h>
#include <id3FingerCaptureLib.h>
#include <id3Finger/id3FingerImage.h>
#include <string>

class FingerImage {
public:
    FingerImage() {
        int sdk_err{};
        CHECK_ID3_ERROR(sdk_err, id3FingerImage_Initialize(&hImage_));
    }
    virtual ~FingerImage() {
        id3FingerImage_Dispose(&hImage_);
    }
    operator ID3_FINGER_IMAGE () { return hImage_; }

    virtual int width() {
        int value{};
        auto sdk_err = id3FingerImage_GetWidth(hImage_, &value);
        if (sdk_err == 0) {
            return value;
        }
        return sdk_err;
    }
    virtual int height() {
        int value{};
        auto sdk_err = id3FingerImage_GetHeight(hImage_, &value);
        if (sdk_err == 0) {
            return value;
        }
        return sdk_err;
    }
    virtual int stride() {
        int value{};
        auto sdk_err = id3FingerImage_GetStride(hImage_, &value);
        if (sdk_err == 0) {
            return value;
        }
        return sdk_err;
    }
    virtual void *getPixels() {
        void *pixels{};
        auto sdk_err = id3FingerImage_GetPixels(hImage_, &pixels);
        if (sdk_err == 0) {
            return pixels;
        }
        return nullptr;
    }
    virtual int save(std::string &filepath, float compressionLevel) {
        return id3FingerImage_ToFile(hImage_, filepath.c_str(), compressionLevel);
    }
    virtual int getFingerCount() {
        int fingerCount{};
        auto sdk_err = id3FingerImage_GetFingerCount(hImage_, &fingerCount);
        if (sdk_err == 0) {
            return fingerCount;
        }
        return 0;
    }
    virtual int getTemplateRecord(ID3_FINGER_TEMPLATE_RECORD hTemplateRecord) {
        return id3FingerImage_GetTemplateRecord(hImage_, hTemplateRecord);
    }

protected:
    ID3_FINGER_IMAGE hImage_{};
};

class FingerExtractor {
public:
    FingerExtractor() {
        int sdk_err{};
        CHECK_ID3_ERROR(sdk_err, id3FingerExtractor_Initialize(&hExtractor_));
    }
    virtual ~FingerExtractor() {
        id3FingerExtractor_Dispose(&hExtractor_);
    }
    operator ID3_FINGER_EXTRACTOR () { return hExtractor_; }

    virtual int initialize() {
        return id3FingerExtractor_Initialize(&hExtractor_);
    }
    virtual int setMinutiaDetectorModel(id3FingerModel eMinutiaDetectorModel) {
        return id3FingerExtractor_SetMinutiaDetectorModel(hExtractor_, eMinutiaDetectorModel);
    }
    virtual int setThreadCount(int threadCount) {
        return id3FingerExtractor_SetThreadCount(hExtractor_, threadCount);
    }

protected:
    ID3_FINGER_EXTRACTOR hExtractor_{};
};

class FingerScanner {
public:
    FingerScanner(bool initialize) {
        if (initialize) {
            int sdk_err{};
            CHECK_ID3_ERROR(sdk_err, id3FingerCaptureFingerScanner_Initialize(&hFingerCapture_));
        }
    }
    virtual ~FingerScanner() {
        id3FingerCaptureFingerScanner_Dispose(&hFingerCapture_);
    }
    operator ID3_FINGER_CAPTURE_FINGER_SCANNER () { return hFingerCapture_; }

    virtual int initialize() {
        return id3FingerCaptureFingerScanner_Initialize(&hFingerCapture_);
    }
    virtual int setDeviceAddedCallback(id3DevicesPlugAndPlayCallback deviceAddedCallback, void *context = nullptr) {
        return id3FingerCaptureFingerScanner_SetDeviceAddedCallback(hFingerCapture_, deviceAddedCallback, context);
    }
    virtual int setDeviceRemovedCallback(id3DevicesPlugAndPlayCallback deviceRemovedCallback, void *context = nullptr) {
        return id3FingerCaptureFingerScanner_SetDeviceRemovedCallback(hFingerCapture_, deviceRemovedCallback, context);
    }
    virtual int setDeviceStatusChangedCallback(id3FingerCaptureStatusCallback deviceStatusChangedCallback, void *context = nullptr) {
        return id3FingerCaptureFingerScanner_SetDeviceStatusChangedCallback(hFingerCapture_, deviceStatusChangedCallback, context);
    }
    virtual int setImageCapturedCallback(id3DevicesCaptureCallback captureCallback, void *context = nullptr) {
        return id3FingerCaptureFingerScanner_SetImageCapturedCallback(hFingerCapture_, captureCallback, context);
    }
    virtual int setImagePreviewCallback(id3DevicesCaptureCallback takeSnapshotCallback, void *context = nullptr) {
        return id3FingerCaptureFingerScanner_SetImagePreviewCallback(hFingerCapture_, takeSnapshotCallback, context);
    }

    virtual int setAutoCapture(bool value) {
        return id3FingerCaptureFingerScanner_SetAutoCapture(hFingerCapture_, value);
    }
    virtual int setAutoDetect(bool value) {
        return id3FingerCaptureFingerScanner_SetAutoDetect(hFingerCapture_, value);
    }
    virtual int setAutoProcess(bool value) {
        return id3FingerCaptureFingerScanner_SetAutoProcess(hFingerCapture_, value);
    }
    virtual int setFingerExtractor(ID3_FINGER_EXTRACTOR hFingerExtractor) {
        return id3FingerCaptureFingerScanner_SetFingerExtractor(hFingerCapture_, hFingerExtractor);
    }

    virtual int deviceId() {
        int deviceId{};
        auto sdk_err = id3FingerCaptureFingerScanner_GetDeviceId(hFingerCapture_, &deviceId);
        if (sdk_err == 0) {
            return deviceId;
        }
        return sdk_err;
    }

    virtual id3DevicesDeviceState state() {
        id3DevicesDeviceState state{};
        auto sdk_err = id3FingerCaptureFingerScanner_GetDeviceState(hFingerCapture_, &state);
        if (sdk_err == 0) {
            return state;
        }
        return id3DevicesDeviceState_NoDevice;
    }

    virtual bool isCapturing() {
        bool value{};
        auto sdk_err = id3FingerCaptureFingerScanner_GetIsCapturing(hFingerCapture_, &value);
        if (sdk_err == 0) {
            return value;
        }
        return false;
    }

    virtual std::string getDeviceModelName() {
        std::string str;
        int deviceId = 0;
        id3FingerCaptureFingerScanner_GetDeviceId(hFingerCapture_, &deviceId);
        if (deviceId > 0) {
            DeviceInfo device(deviceId);
            str = device.getModel();
        }
        return str;
    }

    virtual std::string getDeviceName() {
        std::string str;
        int deviceId = 0;
        id3FingerCaptureFingerScanner_GetDeviceId(hFingerCapture_, &deviceId);
        if (deviceId > 0) {
            DeviceInfo device(deviceId);
            str = device.getName();
        }
        return str;
    }

    virtual int setParameter(const char *parameterName, int value) {
        return id3FingerCaptureFingerScanner_SetParameterValueInt(hFingerCapture_, parameterName, value);
    }

    virtual int openDevice(int deviceId) {
        return id3FingerCaptureFingerScanner_OpenDevice(hFingerCapture_, deviceId);
    }

    virtual int startCapture(id3FingerCaptureFingerPositionFlags eRequiredFingers) {
        return id3FingerCaptureFingerScanner_StartCapture(hFingerCapture_, eRequiredFingers);
    }

    virtual int stopCapture() {
        return id3FingerCaptureFingerScanner_StopCapture(hFingerCapture_);
    }

    virtual bool waitForCapture(int timeout, bool doEvent) {
        bool available{};
        auto sdk_err = id3FingerCaptureFingerScanner_WaitForCapture(hFingerCapture_, timeout, doEvent, &available);
        if (sdk_err == 0) {
            return available;
        }
        return false;
    }

    virtual bool getCurrentFrame(CapturedImage &image) {
        bool available{};
        auto sdk_err = id3FingerCaptureFingerScanner_GetCurrentFrame(hFingerCapture_, image, &available);
        if (sdk_err == 0) {
            return available;
        }
        return false;
    }

    virtual int getCapturedImage(FingerImage &image) {
        return id3FingerCaptureFingerScanner_GetCapturedImage(hFingerCapture_, image);
    }

protected:
    ID3_FINGER_CAPTURE_FINGER_SCANNER hFingerCapture_{};
};
