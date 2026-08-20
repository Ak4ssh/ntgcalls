#ifndef BASE_VIDEO_FACTORY_HPP
#define BASE_VIDEO_FACTORY_HPP

#include <vector>
#include <memory>
#include <api/video_codecs/sdp_video_format.h>
#include <api/environment/environment.h>

namespace wrtc {
    template <typename ConfigType, typename CodecType>
    class BaseVideoFactory {
    protected:
        std::vector<ConfigType> codecs_;
        mutable std::vector<std::vector<webrtc::SdpVideoFormat>> formats_;
    public:
        explicit BaseVideoFactory(std::vector<ConfigType> codecs) : codecs_(std::move(codecs)) {}
        std::unique_ptr<CodecType> Create(const webrtc::Environment& e, const webrtc::SdpVideoFormat& f) {
            int n = 0;
            for (const auto& c : codecs_) {
                for (auto sf = formats_[n++]; const auto& f2 : sf) {
                    if (f2.IsSameCodec(f)) {
                        return c.CreateVideoCodec(e, f);
                    }
                }
            }
            return nullptr;
        }
        std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const {
            formats_.clear();
            std::vector<webrtc::SdpVideoFormat> r;
            for (const auto& c : codecs_) {
                auto sf = c.GetSupportedFormats();
                r.insert(r.end(), sf.begin(), sf.end());
                formats_.push_back(sf);
            }
            return r;
        }
    };
}

#endif
