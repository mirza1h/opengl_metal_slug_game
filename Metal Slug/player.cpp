#include "player.hpp"
#include <iostream>
#include <iostream>
Player::Player(int x, int y, int w, int h, RECT moveRect, bool dir, int _numLives, enum type newType)
{
    xPos=  x;
    yPos= y;
    width= w;
    height= h;
    moveRectangle= moveRect;
    direction = dir;
    numLives = _numLives;
    playerType = newType;
}


void Player::moveLeft(std::size_t n)
{
    xPos -= n;
}

void Player::moveRight(std::size_t n)
{
    xPos += n;
}

void Player::moveUp(std::size_t n)
{
    yPos -= n;
}



void Player::moveDown(std::size_t n)
{
    yPos += n;
}


void Player::playerJump()
{
    if(falling)
    {
        moveDown(25);
    }
    else
    {
        moveUp(25);
    }
}



bool Player::isHit(const Player& target)
{

        return (xPos >= target.getX() && xPos <= target.getX() + target.getWidth())
             && (yPos >= target.getY() || yPos <= target.getY() + target.getHeight())
             && target.getHumanPlayer();

}
