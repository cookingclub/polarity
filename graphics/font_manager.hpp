#ifndef POLARITY_GRAPHICS_FONT_MANAGER_HPP__
#define POLARITY_GRAPHICS_FONT_MANAGER_HPP__

#include "graphics/font_renderer.hpp"
#include "color.hpp"
#include "util/lru_map.hpp"

#include <unordered_map>
#include <functional>

namespace Polarity {

class POLARITYGFX_EXPORT FontManager {

    class POLARITYGFX_EXPORT FontKey {
    public:

        class POLARITYGFX_EXPORT Hash {
        public:
            size_t operator () (const FontKey &key) const;
        };
        FontKey(std::string fn, int pt);

        bool operator==(const FontKey &oth) const;
        const std::string & name() const{ return fontName; }
        int pointSize() const{ return ptSize; }
        
    private:
        std::string fontName;
        int ptSize;
    };

    class POLARITYGFX_EXPORT TextKey {
    public:

        class POLARITYGFX_EXPORT Hash {
        public:
            size_t operator () (const TextKey &key) const;
        };

        TextKey(const FontKey &fk, Color c, std::string msg);

        bool operator==(const TextKey &oth) const;
    private:
        FontKey fontKey;
        Color color;
        std::string message;
    };
    FontRenderer* getFontRenderer(const FontKey &fcKey);
public:

    FontManager(size_t fontCacheSize, size_t textCacheSize);


    Rect textSize(const std::string&fontName, int ptSize, const std::string &message);

    void drawText(Canvas *canvas, int rect_left, int rect_top, const std::string &fontName,
                  int ptSize, Color color, const std::string &message,
                  bool cacheText=true);

    void clearTextCache();

private:

    LRUMap<FontKey, std::unique_ptr<FontRenderer>, FontKey::Hash > fontCache;
    LRUMap<TextKey, std::unique_ptr<Image>, TextKey::Hash > textCache;
};

}
#endif
