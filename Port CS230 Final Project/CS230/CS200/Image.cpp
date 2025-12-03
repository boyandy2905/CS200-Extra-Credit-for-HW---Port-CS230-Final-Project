/**
 * \file
 * \author Rudy Castan
 * \author Sehoon Kim
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Image.hpp"

#include "Engine/Error.hpp"
#include "Engine/Path.hpp"

#include <stb_image.h>
#include <utility>

namespace CS200
{
    Image::Image(const std::filesystem::path& image_path, bool flip_vertical)
    {
        stbi_set_flip_vertically_on_load(flip_vertical ? 1 : 0);

        int width = 0, height = 0, channels = 0;

        auto full_path = assets::locate_asset(image_path);

        unsigned char* data = stbi_load(full_path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

        if(!data)
        {
            throw std::runtime_error("Failed to load image: " + full_path.string());
        }

        size   = Math::ivec2(width, height);
        pixels = reinterpret_cast<RGBA*>(data);
    }

    Image::Image(Image&& temporary) noexcept : pixels(temporary.pixels), size(temporary.size)
    {
        temporary.pixels = nullptr;
        temporary.size   = {0, 0};
    }

    Image& Image::operator=(Image&& temporary) noexcept
    {
        if(this != &temporary)
        {
            std::swap(pixels, temporary.pixels);
            std::swap(size, temporary.size);
        }
        return *this;
    }

    Image::~Image()
    {
        if(pixels)
        {
            stbi_image_free(pixels);
            pixels = nullptr;
        }
    }

    const RGBA* Image::data() const noexcept
    {
        return pixels;
    }

    RGBA* Image::data() noexcept
    {
        return pixels;
    }

    Math::ivec2 Image::GetSize() const noexcept
    {
        return size;
    }
}
