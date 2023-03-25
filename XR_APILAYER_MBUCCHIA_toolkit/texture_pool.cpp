#include "pch.h"
#include "texture_pool.h"
#include "interfaces.h"
#include "log.h"

namespace toolkit::textures {
    using namespace toolkit::textures;

    std::shared_ptr<ITexture> TexturePool::acquire(const XrSwapchainCreateInfo createInfo,
                                                   const std::string& debugName) {
        // Find an unused texture that matches the requested info
        for (auto& textureEntry : m_textures) {
            const auto textureInfo = textureEntry.texture->getInfo();
            if (textureInfo == createInfo && !textureEntry.inUse) {
                textureEntry.lastUsedFrame = m_frameCount;
                textureEntry.inUse = true;
                cleanup();
                log::Log("\nRE-USE, pool size: %s, fc: %s",
                         std::to_string(m_textures.size()).c_str(),
                         std::to_string(m_frameCount).c_str());
                return textureEntry.texture;
            }
        }

        // If no matching texture is found, create a new one
        auto newTexture = createTexture(createInfo, debugName);
        const auto newTextureEntry = TextureEntry{newTexture, m_frameCount, true};

        m_textures.push_back(newTextureEntry);
        return newTexture;
    }

    void TexturePool::release(std::shared_ptr<ITexture> texture) {
        for (auto& textureEntry : m_textures) {
            if (textureEntry.texture == texture) {
                textureEntry.lastUsedFrame = m_frameCount;
                textureEntry.inUse = false;
                log::Log("\nRELEASE");
                break;
            }
        }
        cleanup();
    }

    void TexturePool::cleanup() {
        // not sure we actually need to clean up after ourselves here in real life.
        // if we want to, we'd need a way to properly advance a framecount or sth like that
        return;

        // Remove any unused textures from the texture pool that have not been used for m_textureLifetime frames
        // for (auto it = m_textures.begin(); it != m_textures.end();) {
        //     // if (m_frameCount - it->lastUsedFrame > m_textureLifetime && it->texture.use_count() == 1) {
        //     if (it->texture.use_count() == 1) {
        //         log::Log("\nERASE, pool size: %s, fc: %s",
        //                  std::to_string(m_textures.size()).c_str(),
        //                  std::to_string(m_frameCount).c_str());
        //         it = m_textures.erase(it);
        //     } else {
        //         ++it;
        //     }
        // }
    }

    TexturePool::TexturePool(std::shared_ptr<IDevice> graphicsDevice)
        : m_device(graphicsDevice) {
    }

    std::shared_ptr<ITexture> TexturePool::createTexture(XrSwapchainCreateInfo createInfo,
                                                         const std::string& debugName) const {
        log::Log("\ncreate tex, fc: %s", std::to_string(m_frameCount).c_str());

        return m_device->createTexture(createInfo, debugName);
    }
} // namespace toolkit::textures
