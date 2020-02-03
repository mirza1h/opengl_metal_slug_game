#ifndef BULLET_HPP_INCLUDED
#define BULLET_HPP_INCLUDED

#include <windows.h>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cmath>
#include "sprite.hpp"
#include "background.hpp"
#include <wingdi.h>

using std::min;
using std::max;
const int BULLET_SPEED = 30;


class Bullet {
    public:
        Bullet(const Player& p, const Sprite& s, bool d): source(p)
        {
            projectile = s;
            direction = d;
            if(!d) {

            this->xPos= p.getX() - s.getWidth() - 10;
            this->yPos = p.getY() + s.getHeight()/4 ;

            } else {
                this->xPos= p.getX() + s.getWidth() - 10;
                this->yPos = p.getY() + s.getHeight()/4 ;
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

    public:
        const Player source;
        Sprite projectile;
        int xPos;
        int yPos;
        bool direction;
};






#endif // BULLET_HPP_INCLUDED
