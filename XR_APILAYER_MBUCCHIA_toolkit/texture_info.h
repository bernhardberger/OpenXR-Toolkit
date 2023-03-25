#pragma once

#include <cstdint>

struct TextureInfo {
    std::uint32_t width;
    std::uint32_t height;
    std::uint32_t format;
    std::uint32_t sampleCount;
    std::uint32_t usageFlags;
    std::uint32_t faceCount;
    std::uint32_t arraySize;
    std::uint32_t mipCount;

    bool operator==(const TextureInfo& other) const {
        return width == other.width && height == other.height && format == other.format &&
               sampleCount == other.sampleCount && usageFlags == other.usageFlags && faceCount == other.faceCount &&
               arraySize == other.arraySize && mipCount == other.mipCount;
    }
};
