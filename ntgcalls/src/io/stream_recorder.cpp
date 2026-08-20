#include <ntgcalls/io/stream_recorder.hpp>
#include <ntgcalls/exceptions.hpp>
#include <rtc_base/logging.h>

namespace ntgcalls {
    StreamRecorder::StreamRecorder() {}
    StreamRecorder::~StreamRecorder() {
        stop();
    }

    void StreamRecorder::start(const std::string& path) {
        std::lock_guard lock(mutex);
        if (is_recording) {
            throw ConnectionError("Already recording");
        }
        output.open(path, std::ios::binary);
        if (!output.is_open()) {
            throw ConnectionError("Failed to open file for recording");
        }
        is_recording = true;
        RTC_LOG(LS_INFO) << "Recording started" << path;
    }

    void StreamRecorder::stop() {
        std::lock_guard lock(mutex);
        if (!is_recording) return;
        is_recording = false;
        if (output.is_open()) {
            output.close();
        }
        RTC_LOG(LS_INFO) << "Recording stopped";
    }

    bool StreamRecorder::isRecording() const {
        std::lock_guard lock(mutex);
        return is_recording;
    }

    void StreamRecorder::feedFrame(const bytes::unique_binary& data, size_t size) {
        std::lock_guard lock(mutex);
        if (!is_recording || !output.is_open()) return;
        try {
            output.write(reinterpret_cast<const char*>(data.get()), size);
        } catch (const std::exception& e) {
            RTC_LOG(LS_ERROR) << "Recording fialed" << e.what();
            is_recording = false;
            output.close();
        }
    }

}
