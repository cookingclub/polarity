#ifndef __POLARITY__COLOR__HPP_
#define __POLARITY__COLOR__HPP_
struct SDL_Color;
namespace Polarity {
struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    Color():r(0), g(0), b(0), a(0){}
    Color(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char a_):r(r_), g(g_), b(b_), a(a_){}
    Color(const SDL_Color& c);
    int asInt() const {
        int retval = a;
        retval <<=8;
        retval |= b;
        retval <<=8;
        retval |=g;
        retval <<=8;
        retval |=r;
        return retval;
    }
    bool operator == (const Color&oth)const {
        return r == oth.r && g == oth.g && b == oth.b && a == oth.a;
    }
};

}
#endif
