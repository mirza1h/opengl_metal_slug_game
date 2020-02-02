#include "projectile.hpp"



// void ProjectileType::draw(HDC bufferHDC, int xPos, int yPos) const
// {
//   HDC tmpHDC = CreateCompatibleDC(bufferHDC);
//   HBITMAP oldBITMAP = (HBITMAP)SelectObject(tmpHDC, rotatedBitmask);
//   BitBlt(bufferHDC, xPos, yPos, imgInfo.bmWidth, imgInfo.bmHeight, tmpHDC, 0, 0, SRCAND);

//   SelectObject(tmpHDC, rotatedImg);
//   BitBlt(bufferHDC, xPos, yPos, imgInfo.bmWidth, imgInfo.bmHeight, tmpHDC, 0, 0, SRCPAINT);

//   SelectObject(tmpHDC, oldBITMAP);
//   DeleteObject(rotatedBitmask);
//   DeleteObject(rotatedImg);
//   DeleteDC(tmpHDC);
// }



// ProjectileType::ProjectileType(const std::string& imgFilename,
//                        const std::string& imgBitmaskFilename,
//                        int width, int height):
//   imgBitmask{ (HBITMAP)LoadImage(NULL, imgBitmaskFilename.c_str(), IMAGE_BITMAP, width, height, LR_LOADFROMFILE) }
// {
//     img = (HBITMAP)LoadImage(NULL, imgFilename.c_str(), IMAGE_BITMAP, width, height, LR_LOADFROMFILE);
//       if(img == NULL)
//         throw std::invalid_argument("ProjectileType(): image " + imgFilename + " not found");

//       if(imgBitmask == NULL)
//          throw std::invalid_argument("ProjectileType(): image " + imgBitmaskFilename + " not found");

//       BITMAP imgBitmaskInfo;
//       GetObject(imgBitmask, sizeof(imgBitmaskInfo), &imgBitmaskInfo);
//       GetObject(img, sizeof(imgInfo), &imgInfo);

//       if(imgBitmaskInfo.bmWidth != imgInfo.bmWidth || imgBitmaskInfo.bmHeight != imgInfo.bmHeight)
//         throw std::invalid_argument("ProjectileType(): incompatibile dimensions of img and bitmaskImg");
// }

bool Bullet::update(RECT * rect)
{
    if(direction) {
     if(this->xPos < rect->right + this->projectile.getWidth())
        {
            bullet->x += 20;
            return true;
        }
    } else {
        if(this->xPos < rect->left - this->projectile.getWidth())
        {
            bullet->x -= 20;
            return true;
        }
    }
    return false;
}



void Bullet::render(HDC hdc)
{
  projectile.draw(hdc, xPos, yPos);
}


void Bullet::checkInput() {}



bool Bullet::isHit(const Player& target)
{
    if(direction) {
      return (xPos >= target.getX() && xPos <= target.getX() + target.getWidth()) 
             && (yPos >= target.getY() && ypos <= taget.getY() + taget.getHeight());
    } else {
        return (xPos >= target.getX() && xPos <= target.getX() + target.getWidth()) 
             && (yPos >= target.getY() && ypos <= taget.getY() + taget.getHeight())
             && target.humanPlayer();
    }
}

