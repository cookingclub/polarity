#include <memory>
#include "font_manager.hpp"
#include "canvas.hpp"
namespace Polarity {
size_t FontManager::FontKey::Hash::operator () (const FontKey &key) const {
    return (std::hash<std::string>()(key.fontName) * 53) ^
        (std::hash<int>()(key.ptSize) * 47);
}
FontManager::FontKey::FontKey(std::string fn, int pt)
            : fontName(fn), ptSize(pt) {
}

bool FontManager::FontKey::operator==(const FontKey &oth) const {
    return (ptSize == oth.ptSize && fontName == oth.fontName);
}

size_t FontManager::TextKey::Hash::operator () (const TextKey &key) const {
    return FontKey::Hash()(key.fontKey) ^
        (std::hash<std::string>()(key.message) * 31) ^
        (std::hash<int>()(key.color.asInt()) * 37);
}
FontManager::TextKey::TextKey(const FontManager::FontKey &fk, Color c, std::string msg)
            : fontKey(fk), message(msg) {
    color = c;
}

bool FontManager::TextKey::operator==(const TextKey &oth) const {
    return (color == oth.color &&
            fontKey == oth.fontKey && message == oth.message);
}
FontRenderer* FontManager::getFontRenderer(const FontKey &fcKey) {
    auto fontRendererPtr = fontCache.get(fcKey);
    FontRenderer *fontRenderer = nullptr;
    if (fontRendererPtr == nullptr) {
        fontRenderer = new FontRenderer(fcKey.name(), fcKey.pointSize());
        fontCache.insert(fcKey, std::move(std::unique_ptr<FontRenderer>(fontRenderer)));
    } else {
        fontRenderer = fontRendererPtr->get();
    }
    return fontRenderer;
}
FontManager::FontManager(size_t fontCacheSize, size_t textCacheSize)
    : fontCache(fontCacheSize), textCache(textCacheSize) {
}


Rect FontManager::textSize(const std::string&fontName, int ptSize, const std::string &message) {
    FontRenderer *fontRenderer = getFontRenderer(FontKey(fontName, ptSize));
    return fontRenderer->textSize(message);
}

void FontManager::drawText(Canvas *canvas, int rect_left, int rect_top, const std::string &fontName,
                           int ptSize, Color color, const std::string &message,
                           bool cacheText) {
    color.a = 255;
    FontKey fcKey(fontName, ptSize);
    FontRenderer *fontRenderer = getFontRenderer(fcKey);
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
                       rect_left + image->width() / 2,
                       rect_top + image->height() / 2,
                       image->width(), image->height(), 0, 1);
}

void FontManager::clearTextCache() {
    textCache.clear();
}


}
