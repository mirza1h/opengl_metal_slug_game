#include "background.hpp"

void Background::draw(HDC bufferHDC)
{
    HDC tmpHDC = CreateCompatibleDC(bufferHDC);
    HBITMAP tmpOldBitmap = (HBITMAP)SelectObject(tmpHDC, backgroundBitmap);

    BitBlt(bufferHDC, this->x, 0, getWidth(), getHeight(), tmpHDC, 0, 0, SRCCOPY);

    SelectObject(tmpHDC, tmpOldBitmap);
    DeleteDC(tmpHDC);
}

Background::Background(const std::string& filename, int width, int height) :
    backgroundBitmap{ (HBITMAP)LoadImage(NULL, filename.c_str(), IMAGE_BITMAP, width, height, LR_LOADFROMFILE) }
{
    if( backgroundBitmap == NULL )
        throw std::invalid_argument("Background(): invalid file name");

    GetObject(backgroundBitmap, sizeof(backgroundInfo), &backgroundInfo);
}


RECT Background::getDimensions() const
{
  RECT returnRECT;

  returnRECT.left = returnRECT.top = 0;
  returnRECT.right = getWidth();
  returnRECT.bottom = getHeight();

  return returnRECT;
}


 bool Background::isPointBelongToBackground(POINT pt) const
 {
     return pt.x >= 0 && pt.y >= 0 && pt.x < backgroundInfo.bmWidth && pt.y < backgroundInfo.bmHeight;
 }

