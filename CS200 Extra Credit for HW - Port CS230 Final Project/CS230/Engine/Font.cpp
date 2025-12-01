/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Sehoon Kim
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Font.hpp"
#include "Window.hpp"
#include "CS200/Image.hpp"
#include "Engine.hpp"
#include "Error.hpp"
#include "Matrix.hpp"
#include "TextureManager.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstddef>

namespace CS230
{
    namespace {
        std::string make_cache_key(const std::string& text, CS200::RGBA color)
        {
            std::ostringstream oss;
            oss << text << "_0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << static_cast<uint32_t>(color);
            return oss.str();
        }
    }

    Font::Font(const std::filesystem::path& file_name) :
    atlas(file_name)
    {
        CS200::Image img(file_name, false);
        const auto sz = img.GetSize();
        glyph_height = sz.y - 1;

        if(sz.y < 2 || !img.data())
        {
            throw std::runtime_error("Invalid font image: height too small or no data");
        }

        const CS200::RGBA* pixels = img.data();

        if(pixels[0] != CS200::WHITE)
        {
            throw std::runtime_error("Invalid font image: first pixel must be white");
        }

        std::vector<int> cuts;
        cuts.reserve(256);
        cuts.push_back(0);
        for(int x = 1; x < sz.x; ++x)
        {
            if(pixels[x] != pixels[x-1])
            {
                cuts.push_back(x);
            }
        }
        cuts.push_back(sz.x);

        const int available = static_cast<int>(cuts.size()) - 1;
        const int needed    = GlyphCount;
        const int count     = std::min(available, needed);

        for(std::size_t i = 0; i < static_cast<std::size_t>(count); ++i)
        {
            const int left  = cuts[i];
            const int right = cuts[i + 1];
            glyphs[i].texel_pos = { left, 1 };
            glyphs[i].size      = { right - left - 1, glyph_height };
        }
    }

    int Font::charToIndex(char c) const noexcept
    {
        if(c < FirstChar || c > LastChar)
        {
            return -1;
        }
        return c - FirstChar;
    }

    Math::ivec2 Font::measureText(const std::string& text) const noexcept
    {
        int w = 0;
        for(char c : text)
        {
            const int idx = charToIndex(c);
            if(idx >= 0)
            {
                w += glyphs[static_cast<std::size_t>(idx)].size.x;
            }
        }
        return { w, glyph_height };
    }

    std::shared_ptr<Texture> Font::PrintToTexture(const std::string& text, CS200::RGBA color)
    {
        const auto& env = Engine::GetWindowEnvironment();
        const std::string key = make_cache_key(text, color);

        if(auto it = cache.find(key); it != cache.end() && it->second.texture)
        {
            it->second.last_used = env.FrameCount;
            return it->second.texture;
        }

        for(auto it = cache.begin(); it != cache.end(); )
        {
            if(it->second.texture && it->second.texture.use_count() == 1 && env.FrameCount - it->second.last_used > 60)
            {
                it = cache.erase(it);
            }
            else
            {
                ++it;
            }
        }

        const Math::ivec2 size = measureText(text);
        if(size.x <= 0 || size.y <= 0)
        {
            if(Engine::GetWindow().IsHeadless())
            {
                auto empty = std::shared_ptr<CS230::Texture>(new CS230::Texture(static_cast<OpenGL::TextureHandle>(0), Math::ivec2{1,1}));
                cache[key] = { empty, env.FrameCount };
                return empty;
            }
            CS230::TextureManager::StartRenderTextureMode(1, 1);
            auto empty = CS230::TextureManager::EndRenderTextureMode();
            cache[key] = { empty, env.FrameCount };
            return empty;
        }

        if(Engine::GetWindow().IsHeadless())
        {
            // In headless mode we cannot create GPU-backed render textures. Return a placeholder texture
            auto placeholder = std::shared_ptr<CS230::Texture>(new CS230::Texture(static_cast<OpenGL::TextureHandle>(0), size));
            cache[key] = { placeholder, env.FrameCount };
            return placeholder;
        }

        CS230::TextureManager::StartRenderTextureMode(size.x, size.y);

        double pen_x = 0.0;
        for(char c : text)
        {
            const int idx = charToIndex(c);
            if(idx < 0)
            {
                continue;
            }
            const Glyph& g = glyphs[static_cast<std::size_t>(idx)];

            Math::TransformationMatrix T = Math::TranslationMatrix(Math::vec2{ pen_x, 0.0 });
            atlas.Draw(T, g.texel_pos, g.size, static_cast<unsigned int>(color));

            pen_x += static_cast<double>(g.size.x);
        }

        auto result = CS230::TextureManager::EndRenderTextureMode();
        cache[key] = { result, env.FrameCount };
        return result;
    }
}

