#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#define SNIPER_TIMER 1
#define ARABIAN_TIMER 2
#define PRESSED(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <windowsx.h>
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <list>
#include <mmsystem.h>
#include "sprite.hpp"
#include "background.hpp"
#include "bullet.hpp"

const int SOLDIER_SPEED = 4;
const int WAIT_TIME = 120;
const int SNIPER_FIRE_TIME = 1500;
const int ARABIAN_RUNNING_TIME = 5000;
const int MAP_SEGMENT_LENGTH = 540;
const int pauseTime = 20;

void DrawAnimation (HDC hdc, RECT * rect);
void UpdateSprites (RECT * rect);
void move_animation(int key, RECT* rect);
void FireBullet(int number_of_bullets, Player);
void Render(HWND);
void PrintText(HDC, std::string, int, int);
void PlaySound(char* filename);
BOOL Initialize(HWND);

std::list<Bullet*> bullets;
std::list<Player*> enemies;
std::vector<int> enemyIdle;
std::map<int,int> terrainMappings;

RECT temp;
Sprite bulletSprite = Sprite("assets/cartouche_droite_bullet_black.bmp","assets/cartouche_droite_bullet_white.bmp", 0, 0, 1, 1);
Sprite scoreSprite = Sprite("assets/score_black.bmp","assets/score_white.bmp", 0, 0, 1, 1);
Sprite livesSprite = Sprite("assets/lives_black.bmp","assets/lives_white.bmp", 0, 0, 1, 1);

Sprite sniperOneSprite = Sprite("assets/sniper_gauche/sniper_gauche_idle_black.bmp","assets/sniper_gauche/sniper_gauche_idle_white.bmp", 0, 0, 3, 1);

Sprite arabianOneIdleSprite = Sprite("assets/arabian/ennemie_arabian_gauche_idle_black.bmp","assets/arabian/ennemie_arabian_gauche_idle_white.bmp", 0, 0, 6, 1);
Sprite arabianOneMoveSprite = Sprite("assets/arabian/ennemie_arabian_gauche_running_black.bmp","assets/arabian/ennemie_arabian_gauche_running_white.bmp", 0, 0, 12, 1);
Sprite arabianOneShootSprite = Sprite("assets/arabian/ennemie_arabian_gauche_attack_black.bmp","assets/arabian/ennemie_arabian_gauche_attack_white.bmp", 0, 0, 4, 1);
Sprite arabianOneDeathSprite = Sprite("assets/arabian/ennemie_arabian_gauche_death_black.bmp","assets/arabian/ennemie_arabian_gauche_death_white.bmp", 0, 0, 20, 1);

Sprite soldierMoveSprite = Sprite("assets/player/move_right_black.bmp", "assets/player/move_right_white.bmp", 0, 0, 6, 1);
Sprite soldierIdleSprite = Sprite("assets/player/idle_right_black.bmp", "assets/player/idle_right_white.bmp", 0, 0, 4, 1);
Sprite soldierJumpSprite = Sprite("assets/player/jump_right_black.bmp", "assets/player/jump_right_white.bmp", 0, 0, 4, 3);
Sprite soldierShootSprite = Sprite("assets/player/shoot_black.bmp", "assets/player/shoot_white.bmp", 0, 0, 4, 1);

Background backgroundSprite = Background("assets/stage1.bmp",0,0);

Player sniperThree = Player(230, 190, 0, 0, temp, false, 3);
Player sniperTwo = Player(430, 190, 0, 0, temp, false, 3);
Player sniperOne = Player(330, 190, 0, 0, temp, false, 3);
Player soldier = Player(0, 200, 0, 0, temp, true, 1);
Player arabianOne = Player(100, 200, 0, 0, temp, false, 1);


int animationCounter = 0;
int idleAnimationCounter = 0;
int jumpCounterX = 0;
int jumpCounterY = 0;
int shootCounter = 0;
int mapSegementCount = 1;
int arabianMovingCounter = 0;
int arabianAttackCounter = 0;
int arabianDeadCounter = 0;
bool move_right = false;
bool reachedTarget = false;

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               0,       /* Title Text */
               WS_POPUP, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               544,                 /* The programs width */
               302,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);
    // Gameloop
    if(Initialize(hwnd))
    {
        while(TRUE)
        {
            DWORD startTime;

            if(PeekMessage(&messages, NULL, 0, 0, PM_REMOVE))
            {
                if(messages.message == WM_QUIT)
                {
                    break;
                }
                TranslateMessage(&messages);
                DispatchMessage(&messages);
            }
            startTime = GetTickCount();
            Render(hwnd);
            while((GetTickCount() - startTime) < pauseTime)
            {
                Sleep(60);
            }
        }
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

BOOL Initialize(HWND hwnd)
{
    SetTimer(hwnd, SNIPER_TIMER, SNIPER_FIRE_TIME, NULL);
    SetTimer(hwnd, ARABIAN_TIMER, ARABIAN_RUNNING_TIME, NULL);

    sniperOne.setIdle(sniperOneSprite);
    sniperTwo.setIdle(sniperOneSprite);
    sniperThree.setIdle(sniperOneSprite);

    soldier.setIdle(soldierIdleSprite);
    soldier.setMoving(soldierMoveSprite);
    soldier.setJump(soldierJumpSprite);
    soldier.setShoot(soldierShootSprite);

    arabianOne.setIdle(arabianOneIdleSprite);
    arabianOne.setMoving(arabianOneMoveSprite);
    arabianOne.setShoot(arabianOneShootSprite);
    arabianOne.setDeath(arabianOneDeathSprite);


    enemies.push_back(&sniperOne);
    enemies.push_back(&sniperTwo);
    enemies.push_back(&sniperThree);
    enemies.push_back(&arabianOne);

    //PlaySound("sounds/theme.wav");
    return true;
}
/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
    static int width, height;
    int mouseX, mouseY;
    switch (message)                  /* handle the messages */
    {
    case WM_LBUTTONDOWN:
        mouseX = GET_X_LPARAM(lParam);
        mouseY = GET_Y_LPARAM(lParam);
        printf("x: %d\n", mouseX);
        printf("y: %d\n", mouseY);
        break;
    case WM_TIMER:
        switch (wParam)
        {
        case SNIPER_TIMER:
            if(!sniperOne.getPlayerState() == Dead)
                FireBullet(1, sniperOne);
            return 0;
        case ARABIAN_TIMER:
            arabianOne.setPlayerState(Moving);
            return 0;
        }
    case WM_SIZE:
        width = LOWORD(lParam);
        height = HIWORD(lParam);
        break;
    case WM_KEYDOWN:
    {
        GetClientRect(hwnd,&rect);
        if(wParam == VK_SPACE)
        {
            soldier.setPlayerState(Shooting);
            FireBullet(1, soldier);
        }
        break;
    }
    case WM_KEYUP:
        switch(wParam)
        {
        case VK_RIGHT:
            move_right = false;
            soldier.setPlayerState(Idle);
            break;
        case VK_LEFT:
            soldier.setPlayerState(Idle);
            break;
        default:
            break;
        }
        break;
    case WM_DESTROY:
        KillTimer(hwnd, SNIPER_TIMER);
        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

void Render(HWND hwnd)
{
    RECT rect;
    HDC hdc = GetDC(hwnd);
    GetClientRect(hwnd, &rect);
    UpdateSprites(&rect);
    DrawAnimation (hdc, &rect);
    ReleaseDC(hwnd,hdc);
}

void FireBullet(int number_of_bullets, Player shooter)
{
    PlaySound("sounds/gun.wav");
    for(int i = 0; i < number_of_bullets; ++i)
    {

        Bullet* bulletObj = new Bullet(shooter, bulletSprite, shooter.getDirection());
        bullets.push_back(bulletObj);
    }
}

void PlaySound(char* filename)
{
    static long id = 1;
    char cmd[300];
    char name[20];
    sprintf(name, "mydev%d", ++id);
    sprintf(cmd, "open %s type waveaudio alias %s",filename, name);
    mciSendString(cmd, NULL, NULL, NULL);
    sprintf(cmd, "play %s notify", name);
    mciSendString(cmd, NULL, NULL, NULL);
}

void PrintText(HDC hdc,std::string text, int x, int y)
{
    HFONT hFont,hTmp;
    hFont=CreateFont(28,0,0,0,FW_BOLD,0,0,0,0,0,0,NONANTIALIASED_QUALITY,FF_SCRIPT,"SYSTEM_FIXED_FONT");
    hTmp=(HFONT)SelectObject(hdc,hFont);
    SIZE dim;
    SetBkMode(hdc, TRANSPARENT);
    GetTextExtentPoint32(hdc, text.c_str(), text.size(), &dim);
    TextOut(hdc,x,y,text.c_str(),text.size());
    DeleteObject(SelectObject(hdc,hTmp));
}

void UpdateSprites(RECT * rect)
{

    if(PRESSED(VK_RIGHT) && (soldier.getPlayerState() != Shooting))
    {
        move_animation(VK_RIGHT, rect);
    }
    if(PRESSED(VK_LEFT) && (soldier.getPlayerState() != Shooting))
    {
        move_animation(VK_LEFT, rect);
    }
    if (PRESSED(VK_UP) && (soldier.getPlayerState() != Shooting))
    {
        move_animation(VK_UP, rect);
    }

    if(soldier.getPlayerState() == Jumping || soldier.getPlayerState() == MoveAndJump)
    {

        if(PRESSED(VK_RIGHT))
        {
            soldier.setPlayerState(MoveAndJump);
            move_animation(VK_RIGHT, rect);
        }


        if(jumpCounterX == 1 && jumpCounterY == 1)
        {
            soldier.setFalling();
        }
        soldier.playerJump();
    }

    for(auto it = bullets.begin(); it != bullets.end(); ++it)
    {
        if((*it)->xPos < rect->right)
        {
            (*it)->update();
        }
    }

    // Move map as soldier moves
    if(soldier.getX() >= MAP_SEGMENT_LENGTH)
    {
        backgroundSprite.setX(-MAP_SEGMENT_LENGTH * mapSegementCount);
        soldier.setX(0);
        ++mapSegementCount;
    }
    // If there's a terrain change, update y position
    if(terrainMappings.count(soldier.getX()) == 1 && (soldier.getPlayerState() != Jumping) && soldier.getX()<= terrainMappings[soldier.getX()])
        soldier.setY(terrainMappings[soldier.getX()]);

    if(arabianOne.getPlayerState() == Moving)
    {
        arabianOne.moveLeft(5);
    }
    if(arabianOne.getX() <= 20 && !reachedTarget)
    {
        reachedTarget = true;
        arabianOne.setPlayerState(Shooting);
    }

}

void move_animation(int key, RECT* rect)
{
    switch(key)
    {
    case VK_LEFT:
        if(soldier.getX() > rect->left - soldier.getWidth() )
        {
            soldier.setPlayerState(Moving);
            soldier.moveLeft(SOLDIER_SPEED);
        }
        break;
    case VK_UP:
        if(soldier.getY() > rect->top - soldier.getHeight() && (soldier.getPlayerState() != Jumping) )
        {
            soldier.setPlayerState(Jumping);
        }
        break;
    case VK_RIGHT:
        if(soldier.getX() < rect->right + soldier.getWidth())
        {
            soldier.setPlayerState(Moving);
            soldier.moveRight(SOLDIER_SPEED);
        }
        break;
    }
}

void DrawAnimation (HDC hdc, RECT * rect)
{
    HDC hdcBuffer = CreateCompatibleDC(hdc);
    HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, rect->right, rect->bottom);
    HBITMAP hbmOldBuffer = (HBITMAP)SelectObject(hdcBuffer,hbmBuffer);
    HDC hdcMem = CreateCompatibleDC(hdc);

    backgroundSprite.draw(hdcBuffer);

    scoreSprite.draw(hdcBuffer, 400, 22, 1, 0, false);
    livesSprite.draw(hdcBuffer, 10, 10, 1, 0, false);

    for(auto it = enemies.begin(); it != enemies.end(); it++)
    {
        (*it)->getIdle().draw(hdcBuffer, (*it)->getX(), (*it)->getY(), 0, 0, false);
    }

    if(soldier.getPlayerState() == Idle)
    {
        soldier.getIdle().draw(hdcBuffer, soldier.getX(), soldier.getY(), idleAnimationCounter, 0, false);
    }
    else if (soldier.getPlayerState() == Shooting)
    {
        soldier.getShoot().draw(hdcBuffer, soldier.getX(), soldier.getY(), shootCounter, 0, false);
    }
    else if(soldier.getPlayerState() == Jumping || soldier.getPlayerState() == MoveAndJump)
    {
        soldier.getJump().draw(hdcBuffer, soldier.getX(), soldier.getY(), jumpCounterX, jumpCounterY, false);
    }
    else
    {
        soldier.getMoving().draw(hdcBuffer, soldier.getX(), soldier.getY(), animationCounter, 0, false);
    }

    if(arabianOne.getPlayerState() == Moving)
    {
        arabianOne.getMoving().draw(hdcBuffer, arabianOne.getX(), arabianOne.getY(), arabianMovingCounter, 0, false);
    }
    else if(arabianOne.getPlayerState() == Shooting){
        arabianOne.getShoot().draw(hdcBuffer, arabianOne.getX(), arabianOne.getY(), arabianAttackCounter, 0, false);
    }
    else if(arabianOne.getPlayerState() == Dead){
        arabianOne.getDeath().draw(hdcBuffer, arabianOne.getX(), arabianOne.getY(), arabianDeadCounter, 0, false);
    }

    if(arabianOne.getPlayerState() == Moving)
    {
        ++arabianMovingCounter;
        if(arabianMovingCounter == 11)
            arabianMovingCounter = 0;
    }
    else if(arabianOne.getPlayerState() == Shooting){
        ++arabianAttackCounter;
        if(arabianAttackCounter == 3){
            arabianAttackCounter = 0;
            arabianOne.setPlayerState(Idle);
        }
    }
    else if(arabianOne.getPlayerState() == Dead){
        ++arabianDeadCounter;
        if(arabianDeadCounter == 19)
            arabianDeadCounter = 0;
    }

    if(soldier.getPlayerState() == Idle)
    {
        ++idleAnimationCounter;
        if( idleAnimationCounter == 3)
        {
            idleAnimationCounter = 0;
        }

    }
    else if(soldier.getPlayerState() == Shooting)
    {
        ++shootCounter;
        if( shootCounter == 3)
        {
            shootCounter = 0;
            soldier.setPlayerState(Idle);
        }

    }
    else if(soldier.getPlayerState() == Jumping || soldier.getPlayerState() == MoveAndJump)
    {
        ++jumpCounterX;
        if( jumpCounterX == 4)
        {
            ++jumpCounterY;
            jumpCounterX = 0;
        }

        if(jumpCounterX == 2 && jumpCounterY == 2)
        {
            soldier.resetFalling();
            soldier.setPlayerState(Idle);
            jumpCounterX = 0;
            jumpCounterY = 0;
        }
    }
    else
    {

        ++animationCounter;
        if(animationCounter == 6)
        {
            animationCounter = 0;
        }
    }

    for(auto bullet = bullets.begin(); bullet != bullets.end(); ++bullet)
    {
        (*bullet)->render(hdcBuffer);
        for(auto it = enemies.begin(); it != enemies.end(); it++)
        {
            if((*bullet)->isHit(**it))
            {
                bullet = bullets.erase(bullet);
                (*it)->decreaseNumLives();
                if((*it)->getLives() == 0)
                {
                    (*it)->setPlayerState(Dead);
                    it = enemies.erase(it);
                }
            }
        }

    }

    DeleteDC(hdcMem);

    BitBlt(hdc, 0, 0, rect -> right, rect -> bottom, hdcBuffer, 0, 0, SRCCOPY);
    PrintText(hdc, "2", 78, 20);
    PrintText(hdc, "0", 500, 20);
    SelectObject(hdcBuffer, hbmOldBuffer);
    DeleteDC(hdcBuffer);
    DeleteObject(hbmBuffer);
}




