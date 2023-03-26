#ifndef CAS_UPSCALER_H
#define CAS_UPSCALER_H

#include <memory>
#include <vector>
#include <array>
#include <optional>

#include "interfaces.h"

namespace toolkit {
    namespace graphics {
        struct CASConstants {
            uint32_t Const0[4];
            uint32_t Const1[4];
        };

        class CASUpscaler : public IImageProcessor {
        public:
            CASUpscaler(std::shared_ptr<config::IConfigManager> configManager,
                        std::shared_ptr<IDevice> graphicsDevice,
                        int settingScaling,
                        int settingAnamorphic);

            void reload() override;
            void update() override;
            void process(std::shared_ptr<ITexture> input,
                         std::shared_ptr<ITexture> output,
                         std::vector<std::shared_ptr<ITexture>>& textures,
                         std::array<uint8_t, 1024>& blob,
                         std::optional<utilities::Eye> eye = std::nullopt) override;

        protected:
            void initializeUpscaler();
            virtual float getSharpnessSetting() const;
            const std::shared_ptr<config::IConfigManager> m_configManager;
            const std::shared_ptr<IDevice> m_device;

        private:
            const bool m_isSharpenOnly;

            std::shared_ptr<IComputeShader> m_shaderCAS;
            std::shared_ptr<IShaderBuffer> m_configBuffer;
        };
    } // namespace graphics
}     // namespace toolkit

#endif // CAS_UPSCALER_H
