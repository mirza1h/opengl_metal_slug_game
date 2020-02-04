#include "sprite.hpp"
#include "player.hpp"
#include <iostream>

void Sprite::draw(HDC bufferHDC, int xPos, int yPos, std::size_t spriteCountX, std::size_t spriteCountY = 0, bool inverted = false)
{
    spriteCountX = spriteCountX % (columns * rows);

    int posX = (spriteCountX) % columns * getWidth();
    int posY = (spriteCountY) * getHeight();
    HDC tmpHDC = CreateCompatibleDC(bufferHDC);
    HBITMAP oldBITMAP = (HBITMAP)SelectObject(tmpHDC, imgBitmask);

    if(inverted)
        StretchBlt(bufferHDC, xPos, yPos, getWidth(), getHeight(), tmpHDC, posX + getWidth() - 1, posY, -getWidth(), getHeight(), SRCAND);
    else
        BitBlt(bufferHDC, xPos, yPos, getWidth(), getHeight(), tmpHDC, posX, posY, SRCAND);

    SelectObject(tmpHDC, img);

    if(inverted)
        StretchBlt(bufferHDC, xPos, yPos, getWidth(), getHeight(), tmpHDC, posX + getWidth() - 1, posY, -getWidth(), getHeight(), SRCPAINT);
    else
        BitBlt(bufferHDC, xPos, yPos, getWidth(), getHeight(), tmpHDC, posX, posY, SRCPAINT);


    SelectObject(tmpHDC, oldBITMAP);
    DeleteDC(tmpHDC);
}



Sprite::Sprite(const std::string& filenameImg,
               const std::string& filenameBitmaskImg,
               int width,
               int height,
               int c,
               int r):
    img{ (HBITMAP)LoadImage(NULL, filenameImg.c_str(), IMAGE_BITMAP, width, height, LR_LOADFROMFILE) },
    imgBitmask{ (HBITMAP)LoadImage(NULL, filenameBitmaskImg.c_str(), IMAGE_BITMAP, width, height, LR_LOADFROMFILE) },
    columns{ c },
    rows{ r }
{
    if(img == NULL)
        throw std::invalid_argument("Sprite(): image " + filenameImg + " not found");

    if(imgBitmask == NULL)
        throw std::invalid_argument("Sprite(): image " + filenameBitmaskImg + " not found");

    BITMAP imgBitmaskInfo;
    GetObject(imgBitmask, sizeof(imgBitmaskInfo), &imgBitmaskInfo);
    GetObject(img, sizeof(imgInfo), &imgInfo);

    if(imgBitmaskInfo.bmWidth != imgInfo.bmWidth || imgBitmaskInfo.bmHeight != imgInfo.bmHeight)
        throw std::invalid_argument("Sprite(): incompatibile dimensions of img and bitmaskImg");
}

Sprite::Sprite(const Sprite& s)
{
    img = s.img;
    imgBitmask = s.imgBitmask;
    columns = s.columns;
    rows = s.rows;
    imgInfo = s.imgInfo;
}
