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

    void draw(HDC bufferHDC, int xPos, int yPos, std::size_t spriteCountX, std::size_t spriteCountY, bool inverted);


  private:
    HBITMAP img;
    HBITMAP imgBitmask;
    BITMAP imgInfo;
    int columns = 0;
    int rows = 0;
};

enum state {Idle, Moving, Shooting, Dead};

class Player{
  public:
    Player() = default;
    Player(int x, int y, int w, int h, RECT moveRect, bool dir, int numLives);
    void playerJump();
    void moveLeft(std::size_t n);
    void moveRight(std::size_t n);
    void moveUp(std::size_t n);
    void moveDown(std::size_t n);
    void moveToPoint(POINT pt);
    void setX(int x) {xPos = x;}
    void setY(int y) {yPos = y;}
    int getX() const { return xPos; }
    int getY() const { return yPos; }
    int getWidth() const { return idle.getWidth(); }
    int getHeight() const { return idle.getHeight(); }
    bool isHit(const Player& target);
    POINT getCurrentPosition() const;
    bool getDirection() const { return direction; }

    Sprite& getIdle() { return idle; }
    void setIdle(Sprite& idleSprite) { idle = idleSprite;}
    Sprite& getMoving() { return moving; }
    void setMoving(Sprite& moveSprite) { moving = moveSprite;}
    void setInverseMove(Sprite& inverseMoveSprite) { inverse_move = inverseMoveSprite;}
    Sprite& getInverseMove() { return inverse_move; }
    void setDeath(Sprite& deathSprite) { death = deathSprite; }
    Sprite& getDeath() { return death; }
    void setJump(Sprite& jumpSprite) { jump = jumpSprite; }
    Sprite& getJump() { return jump; }
    void setShoot(Sprite& shootSprite) { shoot = shootSprite;}
    Sprite& getShoot() { return shoot; }
    void decreaseNumLives()  {  --numLives;}
    void setHumanPlayer() { humanPlayer = true; }
    bool getHumanPlayer() const { return humanPlayer; }
    int getLives() const { return numLives; }
    void setLives() { numLives = 2; }
    void flipDirection() { direction = !direction; }
    void setFalling() { falling = true;}
    void resetFalling() { falling = false; }
    bool getFalling() const { return falling; }
    void setJumping() { jumping = true;}
    void resetJumping() { jumping = false; }
    bool getJumping() const { return jumping; }

    enum state getPlayerState() { return playerState;}
    void setPlayerState(enum state newState) {playerState = newState;}

  private:
    int xPos;
    int yPos;
    int width;
    int height;
    int numLives;
    RECT moveRectangle;
    Sprite idle;
    Sprite moving;
    Sprite inverse_move;
    Sprite death;
    Sprite shoot;
    Sprite jump;
    bool direction = true;
    bool humanPlayer = false;
    bool falling = false;
    bool jumping = false;
    enum state playerState = Idle;
};





#endif // SPRITE_HPP_INCLUDED
