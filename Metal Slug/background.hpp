#ifndef BACKGROUND_HPP_INCLUDED
#define BACKGROUND_HPP_INCLUDED

#include <windows.h>
#include <string>
#include <stdexcept>
#include <iostream>

class Background{
    public:
        Background(const std::string& filename, int width, int height);

        int getWidth() const { return backgroundInfo.bmWidth; }
        int getHeight() const { return backgroundInfo.bmHeight; }
        RECT getDimensions() const;
        bool isPointBelongToBackground(POINT pt) const ;
        void setX(int xPos) {this->x = xPos;}
        void draw(HDC bufferHDC);

    private:
        HBITMAP backgroundBitmap;
        BITMAP backgroundInfo;
        int x = 0;

};




#endif // BACKGROUND_HPP_INCLUDED
