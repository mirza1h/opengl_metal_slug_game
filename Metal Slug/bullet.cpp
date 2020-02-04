#include "bullet.hpp"



bool Bullet::update()
{
    if(direction == true) {
            xPos += BULLET_SPEED;
            return true;
    } else {
            xPos -= BULLET_SPEED;
            return true;
    }
    return false;
}



void Bullet::render(HDC hdc)
{
  projectile.draw(hdc, xPos, yPos, 0, 0, false);
}


bool Bullet::isHit(const Player& target)
{

    if(direction) {
      return (xPos >= target.getX() && xPos <= target.getX() + target.getWidth())
             && (yPos >= target.getY() && yPos <= target.getY() + target.getHeight());
    } else {
        return (xPos >= target.getX() && xPos <= target.getX() + target.getWidth())
             && (yPos >= target.getY() && yPos <= target.getY() + target.getHeight())
             && target.getHumanPlayer();
    }
}

