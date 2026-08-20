//
// Created by Laky64 on 07/10/24.
//

#pragma once

#include <ntgcalls/io/audio_writer.hpp>
#include <ntgcalls/media/base_sink.hpp>

namespace ntgcalls {

    class AudioMixer: public AudioWriter {
    protected:
        std::function<void(const bytes::unique_binary&, size_t)> mixedCallback;
        virtual void onData(bytes::unique_binary data) = 0;

    public:
        explicit AudioMixer(BaseSink* sink);

        void onMixedData(const std::function<void(const bytes::unique_binary&, size_t)>& callback);

        void sendFrames(const std::map<uint32_t, std::pair<bytes::unique_binary, size_t>>& frames) override;
    };

} // ntgcalls
