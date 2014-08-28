#ifndef POLARITY_GRAPHICS_RECT_HPP__
#define POLARITY_GRAPHICS_RECT_HPP__

class Rect {
public:
    float x, y;
    float w, h;

    explicit Rect() {
        x = 0;
        y = 0;
        w = 0;
        h = 0;
    }

    Rect(float x, float y, float w, float h)
            : x(x), y(y), w(w), h(h) {
        if (w < 0) {
            w = 0;
        }
        if (h < 0) {
            h = 0;
        }
    }

    static Rect fromBounds(float x1, float y1, float x2, float y2) {
        return Rect(x1, y1, x2 - x1, y2 - y1);
    }

    float top() const {
        return y;
    }
    float bottom() const {
        return y + h;
    }
    float left() const {
        return x;
    }
    float right() const {
        return x + w;
    }

    float width() const {
        return w;
    }
    float height() const {
        return h;
    }

    operator bool() {
        return w > 0 && h > 0;
    }
    Rect unionize(const Rect& other) const {
        return Rect::fromBounds(
                left() < other.left() ? left() : other.left(),
                top() < other.top() ? top() : other.top(),
                right() > other.right() ? right() : other.right(),
                bottom() > other.bottom() ? bottom() : other.bottom());
    }
    Rect intersect(const Rect& other) const {
        return Rect::fromBounds(
                left() > other.left() ? left() : other.left(),
                top() > other.top() ? top() : other.top(),
                right() < other.right() ? right() : other.right(),
                bottom() < other.bottom() ? bottom() : other.bottom());
    }
};

#endif
