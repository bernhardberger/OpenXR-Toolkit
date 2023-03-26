// MIT License
//
// Copyright(c) 2023 Bernhard Berger <bernhard.berger@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "pch.h"
#include "cas.h"
#include "layer.h"
#include "shader_utilities.h"

namespace toolkit {
    namespace graphics {

        class CASSharpenerPostProcessor: public CASUpscaler, public IPostProcessor {
          public:
            CASSharpenerPostProcessor(std::shared_ptr<config::IConfigManager> configManager,
                            std::shared_ptr<IDevice> graphicsDevice)
                : CASUpscaler(configManager, graphicsDevice, 100, 0) {
                createRenderResources();
            }

            bool isEnabled() override {
                return m_configManager->getEnumValue<config::PostProcessSharpenerType>(
                           config::SettingPostSharpenerType) == config::PostProcessSharpenerType::CAS;
            }

            void reload() {
                CASUpscaler::reload();
                createRenderResources();
            }

            void update() {
                CASUpscaler::update();
            }

            void process(std::shared_ptr<ITexture> input,
                         std::shared_ptr<ITexture> output,
                         std::vector<std::shared_ptr<ITexture>>& textures,
                         std::array<uint8_t, 1024>& blob,
                         std::optional<utilities::Eye> eye = std::nullopt) override {
                const auto outputInfo = output->getInfo();

                // create intermediate texture we can render to
                if (textures.empty() || textures[0]->getInfo().width != outputInfo.width ||
                    textures[0]->getInfo().height != outputInfo.height ||
                    textures[0]->getInfo().format != outputInfo.format ||
                    textures[0]->getInfo().arraySize != outputInfo.arraySize) {
                    textures.clear();
                    auto createInfo = outputInfo;

                     if (m_device->isTextureFormatSRGB(outputInfo.format)) {
                        // Good balance between visuals and performance.
                        createInfo.format = m_device->getTextureFormat(graphics::TextureFormat::R10G10B10A2_UNORM);
                    }

                    createInfo.usageFlags |= XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_UNORDERED_ACCESS_BIT;
                    textures.push_back(m_device->createTexture(createInfo, "PostProcessor Intermediate Texture"));
                }

                // Apply the parent CASUpscaler's process method
                CASUpscaler::process(input, textures[0], textures, blob, eye);
                textures[0]->copyTo(output);
            }

        protected:
            float getSharpnessSetting() const override {
                return m_configManager->getValue(config::SettingPostSharpness) / 100.f;
            }

        private:
            void createRenderResources() {
                // const auto shadersDir = dllHome / "shaders";
                // const auto shaderFile = shadersDir / "postprocess.hlsl";
                //
                // utilities::shader::Defines defines;
                // defines.add("PASS_THROUGH_USE_GAINS", false);
                // m_shader = m_device->createQuadShader(shaderFile, "mainPassThrough", "Sample CAS Post Sharpen", defines.get());
            }

            // std::shared_ptr<IQuadShader> m_shader;
        };

    } // namespace graphics
} // namespace toolkit

namespace toolkit::graphics {
    std::shared_ptr<IPostProcessor> CreateCASSharpenerPostProcessor(std::shared_ptr<config::IConfigManager> configManager,
                                                       std::shared_ptr<IDevice> graphicsDevice) {
        return std::make_shared<CASSharpenerPostProcessor>(configManager, graphicsDevice);
    }
} // namespace toolkit::graphics
