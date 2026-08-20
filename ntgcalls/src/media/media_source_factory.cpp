//
// Created by Laky64 on 07/10/24.
//

#include <ntgcalls/devices/media_device.hpp>
#include <ntgcalls/io/file_reader.hpp>
#include <ntgcalls/io/audio_file_writer.hpp>
#include <ntgcalls/io/audio_shell_writer.hpp>
#include <ntgcalls/io/shell_reader.hpp>
#include <ntgcalls/media/media_source_factory.hpp>
#include <rtc_base/logging.h>

#define BOOST_THROW RTC_LOG(LS_ERROR) << "Shell execution is not yet supported on your OS/Architecture"; \
    throw ShellError("Shell execution is not yet supported on your OS/Architecture");

namespace ntgcalls {

    std::unique_ptr<BaseReader> MediaSourceFactory::fromInput(const BaseMediaDescription& desc, BaseSink *sink) {
        if (const auto* video = dynamic_cast<const VideoDescription*>(&desc)) {
            if (video->width <= 0 || video->height <= 0 || video->fps == 0) {
                RTC_LOG(LS_ERROR) << "Invalid video resolution or fps";
                throw InvalidParams("Invalid video resolution or fps");
            }
        }
        
        std::unique_ptr<BaseReader> reader;
        
        // SUPPORTED INPUT MODES
        switch (desc.mediaSource) {
        case BaseMediaDescription::MediaSource::File:
            RTC_LOG(LS_INFO) << "Using file reader for " << desc.input;
            reader = std::make_unique<FileReader>(desc.input, sink);
            break;
        case BaseMediaDescription::MediaSource::Shell:
#ifdef BOOST_ENABLED
            RTC_LOG(LS_INFO) << "Using shell reader for " << desc.input;
            reader = std::make_unique<ShellReader>(desc.input, sink);
            break;
#else
            BOOST_THROW
#endif
        case BaseMediaDescription::MediaSource::Device:
            if (const auto* video = dynamic_cast<const VideoDescription*>(&desc)) {
                reader = MediaDevice::CreateCameraCapture(*video, sink);
            } else {
                reader = MediaDevice::CreateDevice<BaseReader>(desc, sink, true);
            }
            break;
        case BaseMediaDescription::MediaSource::FFmpeg:
            RTC_LOG(LS_ERROR) << "FFmpeg encoder is not yet supported";
            throw FFmpegError("FFmpeg encoder is not yet supported");
        case BaseMediaDescription::MediaSource::Desktop:
            if (const auto* video = dynamic_cast<const VideoDescription*>(&desc)) {
                reader = MediaDevice::CreateDesktopCapture(*video, sink);
                break;
            }
            throw InvalidParams("Invalid media type");
        default:
            RTC_LOG(LS_ERROR) << "Invalid input mode";
            throw InvalidParams("Invalid input mode");
        }
        
        return reader;
    }

    std::unique_ptr<AudioWriter> MediaSourceFactory::fromAudioOutput(const BaseMediaDescription& desc, BaseSink *sink) {
        // SUPPORTED OUTPUT AUDIO MODES
        switch (desc.mediaSource) {
        case BaseMediaDescription::MediaSource::File:
            RTC_LOG(LS_INFO) << "Using file writer for " << desc.input;
            return std::make_unique<AudioFileWriter>(desc.input, sink);
        case BaseMediaDescription::MediaSource::Shell:
#ifdef BOOST_ENABLED
            RTC_LOG(LS_INFO) << "Using shell writer for " << desc.input;
            return std::make_unique<AudioShellWriter>(desc.input, sink);
#else
            BOOST_THROW
#endif
        case BaseMediaDescription::MediaSource::Device:
            return MediaDevice::CreateDevice<AudioWriter>(desc, sink, false);
        case BaseMediaDescription::MediaSource::FFmpeg:
            RTC_LOG(LS_ERROR) << "FFmpeg encoder is not yet supported";
            throw FFmpegError("FFmpeg encoder is not yet supported");
        default:
            RTC_LOG(LS_ERROR) << "Invalid input mode";
            throw InvalidParams("Invalid input mode");
        }
    }

} // ntgcalls
