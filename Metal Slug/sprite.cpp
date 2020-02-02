#include "sprite.hpp"



Player::Player(int x, int y, int w, int h, RECT moveRect, bool dir, int numLives = 3)
{
  xPos=  x;
  yPos= y;
  width= w;
  height= h;
  moveRectangle= moveRect;
  direction = dir;
  numLives = numLives;

  if( xPos - width/2 < moveRectangle.left || xPos + width/2 > moveRectangle.right )
  {
    std::cout << "xPos: " << xPos << std::endl;
    std::cout << "width/2: " << width/2 << std::endl;
    std::cout << "left: " << moveRect.left << std::endl;
    std::cout << "right: " << moveRect.right << std::endl;
    throw std::invalid_argument("Player(): invalid player x position: ");
  }

  if( yPos - height/2 < moveRectangle.top || yPos + height/2 > moveRectangle.bottom )
    throw std::invalid_argument("Player(): invalid player y position: ");
}


void Player::moveLeft(std::size_t n){
  xPos -= n;
  if(xPos - width/2 < moveRectangle.left)
    xPos = moveRectangle.left + width/2;
  direction = false;
}

void Player::moveRight(std::size_t n){
  xPos += n;
 if(xPos + width/2 > moveRectangle.right)
    xPos = moveRectangle.right - width/2;
  direction = true;
}

void Player::moveUp(std::size_t n){
  yPos -= n;
  // if(yPos - height/2 < moveRectangle.top)
  //   yPos = moveRectangle.top + height/2;
}



void Player::moveDown(std::size_t n){
  yPos += n;
  // if(yPos + height/2 > moveRectangle.bottom)
  //   yPos = moveRectangle.bottom - height/2;
}


void Player::playerJump() {
   if(falling)
   {
      moveDown(25);
   }
   else
   {
     moveUp(25);
   }
}


void Sprite::draw(HDC bufferHDC, int xPos, int yPos, std::size_t spriteCount, bool inverted)
{
  spriteCount = spriteCount % (columns * rows);

  int posX = (spriteCount) % columns * getWidth();
  int posY = (spriteCount) / columns * getHeight();

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



void Player::moveToPoint(POINT pt)
{
    if(pt.x > xPos)
        direction = true;
    else if(pt.x < xPos)
        direction = false;

    xPos = pt.x;
    yPos = pt.y;
}

POINT Player::getCurrentPosition() const
{
    POINT pt;
    pt.x = xPos;
    pt.y = yPos;

    return pt;
}
