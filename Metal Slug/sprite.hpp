#ifndef SPRITE_HPP_INCLUDED
#define SPRITE_HPP_INCLUDED

#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <windows.h>

class Sprite{
  public:
    Sprite(const std::string& filenameImg,
                const std::string& filenameBitmaskImg,
                int width,
                int height,
                int c,
                int r);

    Sprite() = default;
    Sprite(const Sprite&);

    int getWidth() const { return imgInfo.bmWidth / columns; }
    int getHeight() const { return imgInfo.bmHeight / rows; }

    int getColumnCount() const { return columns; }
    int getRowsCount() const { return rows; }

    void draw(HDC bufferHDC, int xPos, int yPos, std::size_t spriteCountX, std::size_t spriteCountY);


  private:
    HBITMAP img;
    HBITMAP imgBitmask;
    BITMAP imgInfo;
    int columns = 0;
    int rows = 0;
};





#endif // SPRITE_HPP_INCLUDED
