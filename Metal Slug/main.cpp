#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#define SNIPER_TIMER 1
#define PRESSED(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <windowsx.h>
#include <iostream>
#include <vector>
#include <map>
#include "sprite.hpp"
#include "background.hpp"

const int SOLDIER_SPEED = 4;
const int BULLET_SPEED = 30;
const int WAIT_TIME = 120;
const int SNIPER_FIRE_TIME = 1500;
const int MAP_SEGMENT_LENGTH = 540;
const int pauseTime = 20;
int between_rand(int high, int low);
void DrawAnimation (HDC hdc, RECT * rect);
void UpdateSprites (RECT * rect);
void move_animation(int key, RECT* rect);
void Bullet (int number_of_bullets = 1);
void Render(HWND);
void PrintText(HDC, std::string, int, int);
BOOL Initialize(HWND);
typedef struct ObjectInfo
{
    int x;
    int y;
    int width;
    int height;
    int dx;
    int dy;
} Object;

Object soldier, soldierIdle, soldierJump, soldierShoot, soldierMoveShoot, soldierDeath;
Object bullet;
std::vector<Object *> bullet_sprites;
std::map<int,int> terrainMappings;
HBITMAP hbmsoldier, hbmsoldierMask;
HBITMAP hbmsoldierIdle, hbmsoldierIdleMask;
HBITMAP hbmsoldierJump, hbmsoldierJumpMask;
HBITMAP hbmsoldierDeah, hbmsoldierDeathMask;
HBITMAP hbmsoldierShoot, hbmsoldierShootMask;
HBITMAP hbmsoldierMoveShoot, hbmsoldierMoveShootMask;

Sprite bulletSprite = Sprite("assets/cartouche_droite_bullet_black.bmp","assets/cartouche_droite_bullet_white.bmp", 0, 0, 1, 1);
Sprite scoreSprite = Sprite("assets/score_black.bmp","assets/score_white.bmp", 0, 0, 1, 1);
Sprite livesSprite = Sprite("assets/lives_black.bmp","assets/lives_white.bmp", 0, 0, 1, 1);
Sprite sniperOneSprite = Sprite("assets/sniper_gauche/sniper_gauche_idle_black.bmp","assets/sniper_gauche/sniper_gauche_idle_white.bmp", 0, 0, 3, 1);
Background backgroundSprite = Background("assets/stage1.bmp",0,0);

int animationCounter = 0;
int idleAnimationCounter = 0;
int jumpCounterX = 0;
int jumpCounterY = 0;
int shootCounter = 0;
int mapSegementCount = 1;
bool idle = true;
bool jump = false;
bool falling = false;
bool shoot = false;
bool death = false;
bool move_right = false;
bool mm = false;

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

    //SetWindowLong(hwnd, GWL_STYLE, 0);
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
    BITMAP bitmap;
    hbmsoldierMask = (HBITMAP)LoadImage(NULL, "assets/player/move_right_white.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hbmsoldier = (HBITMAP)LoadImage(NULL, "assets/player/move_right_black.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hbmsoldierIdleMask = (HBITMAP)LoadImage(NULL, "assets/player/idle_right_white.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hbmsoldierIdle = (HBITMAP)LoadImage(NULL, "assets/player/idle_right_black.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hbmsoldierJumpMask = (HBITMAP)LoadImage(NULL, "assets/player/jump_right_white.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hbmsoldierJump = (HBITMAP)LoadImage(NULL, "assets/player/jump_right_black.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hbmsoldierShoot = (HBITMAP)LoadImage(NULL, "assets/player/shoot_black.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hbmsoldierShootMask = (HBITMAP)LoadImage(NULL, "assets/player/shoot_white.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hbmsoldierMoveShoot = (HBITMAP)LoadImage(NULL, "assets/player/move_and_shoot_black.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hbmsoldierMoveShootMask = (HBITMAP)LoadImage(NULL, "assets/player/move_and_shoot_white.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    GetObject(hbmsoldierJumpMask,sizeof(BITMAP),&bitmap);
    soldierJump.width = bitmap.bmWidth/4;
    soldierJump.height = bitmap.bmHeight/3;

    GetObject(hbmsoldierMoveShoot,sizeof(BITMAP),&bitmap);
    soldierMoveShoot.width = bitmap.bmWidth/4;
    soldierMoveShoot.height = bitmap.bmHeight;

    GetObject(hbmsoldierIdleMask,sizeof(BITMAP),&bitmap);
    soldierIdle.width = bitmap.bmWidth/4;
    soldierIdle.height = bitmap.bmHeight;

    GetObject(hbmsoldierShootMask,sizeof(BITMAP),&bitmap);
    soldierShoot.width = bitmap.bmWidth/4;
    soldierShoot.height = bitmap.bmHeight;


    GetObject(hbmsoldierMask, sizeof(BITMAP),&bitmap);
    soldier.width = bitmap.bmWidth/6;
    soldier.height = bitmap.bmHeight;
    soldier.dx = SOLDIER_SPEED;
    soldier.dy = SOLDIER_SPEED;
    soldier.x = 0;
    soldier.y = 200;

//    terrainMappings[16] = 195;
//    terrainMappings[18] = 190;
//    terrainMappings[20] = 180;
//    terrainMappings[22] = 180;
//    terrainMappings[24] = 185;
//    terrainMappings[26] = 195;
//    terrainMappings[28] = 200;
//    terrainMappings[36] = 200;
//    terrainMappings[38] = 190;
//    terrainMappings[54] = 185;
//    terrainMappings[56] = 170;
//    terrainMappings[82] = 200;
//    terrainMappings[160] = 220;
//    terrainMappings[170] = 230;
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
            //Bullet();
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
            shoot = true;
            Bullet();

            if(PRESSED(VK_UP))
            {
                move_animation(VK_UP, &rect);
            }
            else if(PRESSED(VK_DOWN))
            {
                move_animation(VK_DOWN, &rect);
            }
            else if(PRESSED(VK_LEFT))
            {
                if(soldier.x > 0)
                    move_animation(VK_LEFT, &rect);
            }
            else if(PRESSED(VK_RIGHT))
            {
                if(soldier.x < 320)
                    move_animation(VK_RIGHT, &rect);
            }
        }
        else
        {
            move_animation(wParam, &rect);
        }
        break;
    }
    case WM_KEYUP:
        switch(wParam)
        {
        case VK_RIGHT:
            move_right = false;
            idle = true;
            break;
        case VK_LEFT:
            idle = true;
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

void Bullet(int number_of_bullets)
{
    for(int i = 0; i < number_of_bullets; ++i)
    {
        Object* bullet_sprite = new Object();
        bullet_sprite->height = bullet.height;
        bullet_sprite->width = bullet.width;
        bullet_sprite->x = soldier.x + soldier.width - 10;
        bullet_sprite->y = soldier.y + soldier.height/4 + 15*i;
        bullet_sprite->dx = BULLET_SPEED;
        bullet_sprites.push_back(bullet_sprite);
    }
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
    if(jump)
    {

        if(PRESSED(VK_RIGHT))
        {
            move_animation(VK_RIGHT, rect);
        }

        if(jumpCounterX == 1 && jumpCounterY == 1)
        {
            falling = true;
        }
        if(!falling)
        {
            soldier.y -= 25;
        }
        else
        {
            soldier.y += 25;
        }
    }

    for(auto bullet : bullet_sprites)
    {
        if(bullet->x < rect->right + bullet->width)
        {
            bullet->x += BULLET_SPEED;
        }
    }
    // Move map as soldier moves
    if(soldier.x == MAP_SEGMENT_LENGTH)
    {
        backgroundSprite.setX(-MAP_SEGMENT_LENGTH * mapSegementCount);
        soldier.x = 0;
        ++mapSegementCount;
    }
    // If there's a terrain change, update y position
    if(terrainMappings.count(soldier.x) == 1 && !jump && soldier.x <= terrainMappings[soldier.x])
        soldier.y = terrainMappings[soldier.x];
}

void move_animation(int key, RECT* rect)
{
    switch(key)
    {
    case VK_LEFT:
        if(soldier.x > rect->left - soldier.width )
        {
            idle = false;
            soldier.x -= SOLDIER_SPEED;
        }
        break;
    case VK_UP:
        if(soldier.y > rect->top - soldier.height && !jump )
        {
            jump = true;
        }
        break;
    case VK_RIGHT:
        if(soldier.x < rect->right + soldier.width)
        {
            idle = false;
            move_right = true;
            soldier.x += SOLDIER_SPEED;
        }
        break;
    case VK_DOWN:
        if(soldier.y < rect->bottom + soldier.height)
        {
            idle = false;
            //soldier.y += SOLDIER_SPEED;
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

    scoreSprite.draw(hdcBuffer, 400, 22, 1, false);
    livesSprite.draw(hdcBuffer, 10, 10, 1, false);
    sniperOneSprite.draw(hdcBuffer, 430, 190, 3, false);
    if(idle && !jump && !shoot)
    {
        SelectObject(hdcMem, hbmsoldierIdleMask);
        BitBlt(hdcBuffer, soldier.x, soldier.y, soldierIdle.width, soldierIdle.height, hdcMem, idleAnimationCounter*soldierIdle.width, 0,SRCAND);

        SelectObject(hdcMem, hbmsoldierIdle);
        BitBlt(hdcBuffer, soldier.x, soldier.y, soldierIdle.width, soldierIdle.height, hdcMem, idleAnimationCounter*soldierIdle.width, 0, SRCPAINT);

    }
    else if (shoot && !jump)
    {
        SelectObject(hdcMem, hbmsoldierShootMask);
        BitBlt(hdcBuffer, soldier.x, soldier.y, soldierShoot.width, soldierShoot.height, hdcMem, shootCounter*soldierShoot.width, 0,SRCAND);

        SelectObject(hdcMem, hbmsoldierShoot);
        BitBlt(hdcBuffer, soldier.x, soldier.y, soldierShoot.width, soldierShoot.height, hdcMem, shootCounter*soldierShoot.width, 0, SRCPAINT);
    }
    else if(jump)
    {
        SelectObject(hdcMem, hbmsoldierJumpMask);
        BitBlt(hdcBuffer, soldier.x, soldier.y, soldierJump.width, soldierJump.height, hdcMem, jumpCounterX*soldierJump.width, jumpCounterY*soldierJump.height,SRCAND);
        SelectObject(hdcMem, hbmsoldierJump);
        BitBlt(hdcBuffer, soldier.x, soldier.y, soldierJump.width, soldierJump.height, hdcMem, jumpCounterX*soldierJump.width, jumpCounterY*soldierJump.height, SRCPAINT);
    }
    else
    {
        SelectObject(hdcMem, hbmsoldierMask);
        BitBlt(hdcBuffer, soldier.x, soldier.y, soldier.width, soldier.height, hdcMem, animationCounter*soldier.width, 0,SRCAND);

        SelectObject(hdcMem, hbmsoldier);
        BitBlt(hdcBuffer, soldier.x, soldier.y, soldier.width, soldier.height, hdcMem, animationCounter*soldier.width, 0, SRCPAINT);
    }


    if(idle && !jump && !shoot)
    {
        ++idleAnimationCounter;
        if( idleAnimationCounter == 3)
        {
            idleAnimationCounter = 0;
        }

    }
    else if(shoot && !jump)
    {
        ++shootCounter;
        if( shootCounter == 3)
        {
            shootCounter = 0;
            shoot = false;
        }

    }
    else if(jump)
    {
        ++jumpCounterX;
        if( jumpCounterX == 4)
        {
            ++jumpCounterY;
            jumpCounterX = 0;
        }

        if(jumpCounterX == 2 && jumpCounterY == 2)
        {
            jump = false;
            falling = false;
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

    for(auto bullet : bullet_sprites)
    {
        bulletSprite.draw(hdcBuffer, bullet->x, bullet->y, 1, false);
    }

    DeleteDC(hdcMem);

    BitBlt(hdc, 0, 0, rect -> right, rect -> bottom, hdcBuffer, 0, 0, SRCCOPY);
    PrintText(hdc, "2", 78, 20);
    PrintText(hdc, "0", 500, 20);
    SelectObject(hdcBuffer, hbmOldBuffer);
    DeleteDC(hdcBuffer);
    DeleteObject(hbmBuffer);
}

