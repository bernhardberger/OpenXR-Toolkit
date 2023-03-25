#ifndef TEXTUREPOOL_H
#define TEXTUREPOOL_H

#include <vector>
#include <memory>
#include "interfaces.h"

namespace toolkit::textures {
    using namespace toolkit::graphics;

    inline bool operator==(const XrSwapchainCreateInfo a, const XrSwapchainCreateInfo b) {
        return a.width == b.width && a.height == b.height && a.format == b.format && a.sampleCount == b.sampleCount &&
               a.usageFlags == b.usageFlags && a.faceCount == b.faceCount && a.arraySize == b.arraySize &&
               a.mipCount == b.mipCount;
    }

    class TexturePool {
      public:
        static TexturePool& getInstance(std::shared_ptr<IDevice> graphicsDevice) {
            static TexturePool instance(graphicsDevice);
            return instance;
        }

        TexturePool(const TexturePool&) = delete;
        TexturePool& operator=(const TexturePool&) = delete;
        TexturePool(TexturePool&&) = delete;
        TexturePool& operator=(TexturePool&&) = delete;

        std::shared_ptr<ITexture> acquire(const XrSwapchainCreateInfo createInfo,
                                          const std::string& debugName);
        void release(std::shared_ptr<ITexture> texture);

      private:
        TexturePool(std::shared_ptr<IDevice> graphicsDevice);

        struct TextureEntry {
            std::shared_ptr<ITexture> texture;
            uint64_t lastUsedFrame = -1;
            bool inUse = false;
        };
        uint64_t m_frameCount = 0;
        int m_textureLifetime = 60; // in frames

        void cleanup();
        std::shared_ptr<ITexture> createTexture(XrSwapchainCreateInfo createInfo,
                                                const std::string& debugName) const;

        // std::vector<std::shared_ptr<ITexture>> m_textures;
        std::vector<TextureEntry> m_textures;
        std::shared_ptr<IDevice> m_device;
    };

#endif // TEXTUREPOOL_H
}