#ifndef POLARITY_GRAPHICS_FONT_MANAGER_HPP__
#define POLARITY_GRAPHICS_FONT_MANAGER_HPP__

#include "graphics/font_renderer.hpp"

#include "util/lru_map.hpp"

#include <unordered_map>
#include <functional>

namespace Polarity {

class FontManager {

    class FontKey {
    public:

        class Hash {
        public:
            size_t operator () (const FontKey &key) const {
                return (std::hash<std::string>()(key.fontName) * 53) ^
                       (std::hash<int>()(key.ptSize) * 47);
            }
        };

        FontKey(std::string fn, int pt)
            : fontName(fn), ptSize(pt) {
        }

        bool operator==(const FontKey &oth) const {
            return (ptSize == oth.ptSize && fontName == oth.fontName);
        }
    private:
        std::string fontName;
        int ptSize;
    };

    class TextKey {
    public:

        class Hash {
        public:
            size_t operator () (const TextKey &key) const {
                return FontKey::Hash()(key.fontKey) ^
                       (std::hash<std::string>()(key.message) * 31) ^
                       (std::hash<int>()(key.colorUnion.colorVal) * 37);
            }
        };

        TextKey(FontKey &fk, SDL_Color c, std::string msg)
            : fontKey(fk), message(msg) {
            colorUnion.color = c;
        }

        bool operator==(const TextKey &oth) const {
            return (colorUnion.colorVal == oth.colorUnion.colorVal &&
                    fontKey == oth.fontKey && message == oth.message);
        }
    private:
        FontKey fontKey;
        union {
            SDL_Color color;
            int colorVal;
        } colorUnion;
        std::string message;
    };

public:

    FontManager(size_t fontCacheSize, size_t textCacheSize)
        : fontCache(fontCacheSize), textCache(textCacheSize) {
    }

    void drawText(Canvas *canvas, Rect rect, const std::string &fontName,
                  int ptSize, SDL_Color color, const std::string &message,
                  bool cacheText=true) {
        FontKey fcKey(fontName, ptSize);
        auto fontRendererPtr = fontCache.get(fcKey);
        FontRenderer *fontRenderer;
        if (fontRendererPtr == nullptr) {
            fontRenderer = new FontRenderer(fontName, ptSize);
            fontCache.insert(fcKey, std::move(std::unique_ptr<FontRenderer>(fontRenderer)));
        } else {
            fontRenderer = fontRendererPtr->get();
        }
        TextKey tcKey(fcKey, color, message);
        auto imagePtr = textCache.get(tcKey);
        std::unique_ptr<Image> throwaway;
        Image *image;
        if (imagePtr == nullptr) {
            image = fontRenderer->render(canvas, color, message);
            if (!image) {
                return;
            }
            if (cacheText) {
                textCache.insert(tcKey, std::move(std::unique_ptr<Image>(image)));
            } else {
                throwaway.reset(image);
            }
        } else {
            image = imagePtr->get();
        }
        canvas->drawSprite(image,
                           rect.left() + image->width() / 2,
                           rect.top() + image->height() / 2,
                           image->width(), image->height(), 0);
    }

    void clearTextCache() {
        textCache.clear();
    }

private:

    LRUMap<FontKey, std::unique_ptr<FontRenderer>, FontKey::Hash > fontCache;
    LRUMap<TextKey, std::unique_ptr<Image>, TextKey::Hash > textCache;
};

}
#endif
