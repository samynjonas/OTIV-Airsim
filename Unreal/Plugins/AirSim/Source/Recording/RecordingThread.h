#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"

#include "AirBlueprintLib.h"
#include "api/VehicleSimApiBase.hpp"
#include "Recording/RecordingFile.h"
#include "physics/Kinematics.hpp"
#include <memory>
#include "common/ClockFactory.hpp"
#include "common/AirSimSettings.hpp"
#include "common/WorkerThread.hpp"

#include <atomic>
#include <mutex>
#include <condition_variable>

class FRecordingThread : public FRunnable
{
public:
    typedef msr::airlib::AirSimSettings::RecordingSetting RecordingSetting;
    typedef msr::airlib::VehicleSimApiBase VehicleSimApiBase;
    typedef msr::airlib::ImageCaptureBase ImageCaptureBase;

public:
    FRecordingThread();
    virtual ~FRecordingThread();

    static void init();
    static void startRecording(const RecordingSetting& settings,
                               const common_utils::UniqueValueMap<std::string, VehicleSimApiBase*>& vehicle_sim_apis,
                               const bool repeatUntillStopped = true);

    static void stopRecording();
    static void killRecording();
    static bool isRecording();

protected:
    virtual bool Init() override;
    virtual uint32 Run() override;

    void Pause();
    void Resume();
    bool isPaused() const;

    virtual void Stop() override;
    virtual void Exit() override;

private:
    FThreadSafeCounter stop_task_counter_;

    //static ASegmentationSorter* _pSegmentationSorter;

    static std::unique_ptr<FRecordingThread> running_instance_;
    static std::unique_ptr<FRecordingThread> finishing_instance_;
    static msr::airlib::WorkerThreadSignal finishing_signal_;
    static bool first_;

    static std::unique_ptr<FRecordingThread> instance_;

    std::unique_ptr<FRunnableThread> thread_;

    RecordingSetting settings_;
    std::unique_ptr<RecordingFile> recording_file_;
    common_utils::UniqueValueMap<std::string, VehicleSimApiBase*> vehicle_sim_apis_;
    std::unordered_map<std::string, const ImageCaptureBase*> image_captures_;
    std::unordered_map<std::string, msr::airlib::Pose> last_poses_;

    msr::airlib::TTimePoint last_screenshot_on_;
    int image_Count_{};

    bool is_ready_;
    bool run_untill_stopped_;

    //Add pause functionality
    std::atomic<bool> bPaused{ true };

    std::mutex pause_mutex_;
    std::condition_variable pauseCondition;
};