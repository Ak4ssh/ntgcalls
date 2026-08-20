
#pragma once
#include <mutex>
#include <fstream>
#include <ntgcalls/media/base_sink.hpp>
#include <wrtc/utils/binary.hpp>

namespace ntgcalls {
    class StreamRecorder {
    public:
        StreamRecorder();
        ~StreamRecorder();
        void start(const std::string& path);
        void stop();
        [[nodiscard]] bool isRecording() const;
        void feedFrame(const bytes::unique_binary& data, size_t size);
    private:
        bool is_recording = false;
        std::ofstream output;
        mutable std::mutex mutex;
    };
}
