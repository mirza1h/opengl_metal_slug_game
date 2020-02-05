#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <windows.h>
#include "sprite.hpp"

enum state {Idle, Moving, Shooting, Dead};
enum type {Soldier, Sniper, Arabian, Boss};
class Player{
  public:
    Player() = default;
    Player(int x, int y, int w, int h, RECT moveRect, bool dir, int numLives, enum type newType);
    void playerJump();
    void moveLeft(std::size_t n);
    void moveRight(std::size_t n);
    void moveUp(std::size_t n);
    void moveDown(std::size_t n);
    void setX(int x) {xPos = x;}
    void setY(int y) {yPos = y;}
    int getX() const { return xPos; }
    int getY() const { return yPos; }
    int getWidth() const { return idle.getWidth(); }
    int getHeight() const { return idle.getHeight(); }
    bool isHit(const Player& target);
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

    enum type getPlayerType() { return playerType;}
    void setPlayerType(enum type newType) {playerType = newType;}

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
    enum type playerType = Soldier;
    enum state playerState = Idle;
};

#endif
