#ifndef BULLET_HPP_INCLUDED
#define BULLET_HPP_INCLUDED

#include <windows.h>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cmath>
#include "sprite.hpp"
#include "background.hpp"
#include "player.hpp"
#include <wingdi.h>

using std::min;
using std::max;
const int BULLET_SPEED = 30;


class Bullet
{
public:
    Bullet(Player& p, const Sprite& s, bool d): source(p)
    {
        projectile = s;
        direction = d;
        int xOffset = p.getWidth() - 10;
        int yOffset = p.getHeight()/4 ;
        if(p.getPlayerType() == Sniper){
            xOffset = 10;
            yOffset = 4;
        }
        if(p.getPlayerType() == Boss){
            xOffset = 15;
            yOffset = 10;
        }
        if(!d)
        {
            this->xPos = p.getX() + xOffset;
            this->yPos = p.getY() + yOffset;
        }
        else
        {
            this->xPos= p.getX() + xOffset;
            this->yPos = p.getY() + yOffset;
        }

    }

    Bullet(const Bullet& ps): source(ps.source)
    {
        projectile = ps.projectile;
        xPos = ps.xPos;
        yPos = ps.yPos;
    }

    bool update();
    void render(HDC hdc);
    void checkInput();
    bool isHit(const Player& target);
    int getX() const { return xPos; }

public:
    const Player source;
    Sprite projectile;
    int xPos;
    int yPos;
    bool direction;
};






#endif // BULLET_HPP_INCLUDED
