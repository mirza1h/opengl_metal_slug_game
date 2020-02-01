#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#define TIMER 1
#define PRESSED(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <vector>

const int SOLDIER_SPEED = 10;
const int BULLET_SPEED = 30;
const int WAIT_TIME = 120;
int between_rand(int high, int low);
void DrawAnimation (HDC hdc, RECT * rect);
void UpdateSprites (RECT * rect);
void move_animation(int key, RECT* rect);
void Snow (RECT * rect);
void Bullet (int number_of_bullets = 1);

typedef struct ObjectInfo
{
    int x;
    int y;
    int width;
    int height;
    int dx;
    int dy;
} Object;


Object background;
Object bullet;
Object soldier, soldierJump;
Object soldierIdle;
Object snow ;

std::vector<Object *> snow_sprites;
std::vector<Object *> bullet_sprites;

HBITMAP hbmbackground;
HBITMAP hbmsoldier, hbmsoldierMask;
HBITMAP hbmsnow,hbmsnowMask;
HBITMAP hbmbullet,hbmbulletMask;
HBITMAP hbmsoldierIdle, hbmsoldierIdleMask;
HBITMAP hbmsoldierJump, hbmsoldierJumpMask;
int animationCounter = 0;
int idleAnimationCounter = 0;
int jumpCounterX = 0;
int jumpCounterY = 0;
bool idle = true;
bool jump = false;
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
               _T("CodeBlocksWindowsApp"),       /* Title Text */
               WS_OVERLAPPEDWINDOW, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               1024,                 /* The programs width */
               768,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
    static int width, height;
    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
    {
        BITMAP bitmap;
        SetTimer(hwnd, TIMER, WAIT_TIME, NULL);

        hbmbackground = (HBITMAP)LoadImage(NULL, "background.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hbmsoldierMask = (HBITMAP)LoadImage(NULL, "move_right_white.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hbmsoldier = (HBITMAP)LoadImage(NULL, "move_right_black.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hbmsoldierIdleMask = (HBITMAP)LoadImage(NULL, "idle_right_white.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hbmsoldierIdle = (HBITMAP)LoadImage(NULL, "idle_right_black.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hbmsoldierJumpMask = (HBITMAP)LoadImage(NULL, "jump_right_white.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hbmsoldierJump = (HBITMAP)LoadImage(NULL, "jump_right_black.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hbmbullet = (HBITMAP)LoadImage(NULL, "bullet.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hbmbulletMask = (HBITMAP)LoadImage(NULL, "bullet_mask.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hbmsnow = (HBITMAP)LoadImage(NULL,"snow.bmp",IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hbmsnowMask = (HBITMAP)LoadImage(NULL,"snow_mask.bmp",IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

        GetObject(hbmbackground,sizeof(BITMAP),&bitmap);
        background.width = bitmap.bmWidth;
        background.height = bitmap.bmHeight;

        GetObject(hbmsoldierJumpMask,sizeof(BITMAP),&bitmap);
        soldierJump.width = bitmap.bmWidth/4;
        soldierJump.height = bitmap.bmHeight/3;
                std::cout<< bitmap.bmWidth << std::endl;

        GetObject(hbmsoldierIdleMask,sizeof(BITMAP),&bitmap);
        soldierIdle.width = bitmap.bmWidth/4;
        soldierIdle.height = bitmap.bmHeight;

        GetObject(hbmsoldierMask, sizeof(BITMAP),&bitmap);
        soldier.width = bitmap.bmWidth/6;
        soldier.height = bitmap.bmHeight;
        soldier.dx = SOLDIER_SPEED;
        soldier.dy = SOLDIER_SPEED;
        soldier.x = 0;
        GetObject(hbmsnow,sizeof(BITMAP),&bitmap);
        snow.width = bitmap.bmWidth;
        snow.height = bitmap.bmHeight;
        snow.y = -snow.height;

        GetObject(hbmbullet, sizeof(BITMAP), &bitmap);
        bullet.width = bitmap.bmWidth;
        bullet.height = bitmap.bmHeight;


        break;
    }
    case WM_SIZE:
        width = LOWORD(lParam);
        height = HIWORD(lParam);
        soldier.y = height - 150;
        break;
    case WM_TIMER:
    {
        HDC hdc = GetDC(hwnd);
        GetClientRect(hwnd, &rect);
        UpdateSprites(&rect);
        DrawAnimation (hdc, &rect);
        ReleaseDC(hwnd,hdc);
        break;
    }
    case WM_RBUTTONDOWN:
        Bullet(2);
        break;
    case WM_LBUTTONDOWN:
        Bullet();
        break;
    case WM_KEYDOWN:
    {
        GetClientRect(hwnd,&rect);
        if(wParam == VK_SPACE) {
            if(PRESSED(VK_LSHIFT)) {
                Bullet(2);
            } else {
                Bullet();
            }
            if(PRESSED(VK_UP)) {
                move_animation(VK_UP, &rect);
            } else if(PRESSED(VK_DOWN)) {
                move_animation(VK_DOWN, &rect);
            } else if(PRESSED(VK_LEFT)) {
                move_animation(VK_LEFT, &rect);
            } else if(PRESSED(VK_RIGHT)){
                move_animation(VK_RIGHT, &rect);
            }
        } else {
            move_animation(wParam, &rect);
        }
    break;
    }
    case WM_KEYUP:
            switch(wParam) {
                case VK_DOWN:
                case VK_LEFT:
                case VK_RIGHT:
                    idle = true;
                break;
                default:
                break;
            }
        break;
    case WM_DESTROY:
        KillTimer(hwnd, TIMER);
        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}


void Bullet(int number_of_bullets)
{
    for(int i = 0; i < number_of_bullets; ++i) {
        Object* bullet_sprite = new Object();
        bullet_sprite->height = bullet.height;
        bullet_sprite->width = bullet.width;
        bullet_sprite->x = soldier.x + soldier.width - 10;
        bullet_sprite->y = soldier.y + soldier.height/4 + 15*i;
        bullet_sprite->dx = BULLET_SPEED;
        bullet_sprites.push_back(bullet_sprite);
    }


}

void UpdateSprites(RECT * rect)
{
     static bool falling = false;
     static int i = 0;
    if(jump) {
                std::cout <<jump<< std::endl;

        if(jumpCounterX == 1 && jumpCounterY == 1) {
            falling =true;
        }
        if(!falling) {
            soldier.y -= 25;
            i+=25;
        } else {
            soldier.y += 25;
            i-=25;
        }
    } else  {
        falling = false;
    }
    for(auto bullet : bullet_sprites){
        if(bullet->x < rect->right + bullet->width){
            bullet->x += BULLET_SPEED;
        }
    }
}
int between_rand(int high, int low){
    return rand()%(high - low + 1) + low;
}

void move_animation(int key, RECT* rect){
    switch(key) {
        case VK_LEFT:
            if(soldier.x > rect->left - soldier.width ) {
               idle = false;
               soldier.x -= SOLDIER_SPEED;
            }
            break;
        case VK_UP:
            if(soldier.y > rect->top - soldier.height && !jump ) {
                jump = true;
            }
            break;
        case VK_RIGHT:
             if(soldier.x < rect->right + soldier.width) {
                idle = false;
                soldier.x += SOLDIER_SPEED;
             }
            break;
        case VK_DOWN:
            if(soldier.y < rect->bottom + soldier.height) {
                idle = false;
                soldier.y += SOLDIER_SPEED;
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
    HBITMAP hbmOld = (HBITMAP) SelectObject( hdcMem, hbmbackground);
    BitBlt(hdcBuffer, background.x, background.y, background.width, background.height, hdcMem, 0, 0, SRCCOPY);

    if(idle && !jump) {
          SelectObject(hdcMem, hbmsoldierIdleMask);
        BitBlt(hdcBuffer, soldier.x, soldier.y, soldierIdle.width, soldierIdle.height, hdcMem, idleAnimationCounter*soldierIdle.width, 0 ,SRCAND);

        SelectObject(hdcMem, hbmsoldierIdle);
        BitBlt(hdcBuffer, soldier.x, soldier.y, soldierIdle.width, soldierIdle.height, hdcMem, idleAnimationCounter*soldierIdle.width, 0, SRCPAINT);

    } else if(jump) {
        SelectObject(hdcMem, hbmsoldierJumpMask);
        BitBlt(hdcBuffer, soldier.x, soldier.y, soldierJump.width, soldierJump.height, hdcMem, jumpCounterX*soldierJump.width, jumpCounterY*soldierJump.height ,SRCAND);
        SelectObject(hdcMem, hbmsoldierJump);
        BitBlt(hdcBuffer, soldier.x, soldier.y, soldierJump.width, soldierJump.height, hdcMem, jumpCounterX*soldierJump.width, jumpCounterY*soldierJump.height, SRCPAINT);
    } else {
      SelectObject(hdcMem, hbmsoldierMask);
        BitBlt(hdcBuffer, soldier.x, soldier.y, soldier.width, soldier.height, hdcMem, animationCounter*soldier.width, 0 ,SRCAND);

        SelectObject(hdcMem, hbmsoldier);
        BitBlt(hdcBuffer, soldier.x, soldier.y, soldier.width, soldier.height, hdcMem, animationCounter*soldier.width, 0, SRCPAINT);
    }


    if(idle && !jump) {
         ++idleAnimationCounter;
        if( idleAnimationCounter == 3) {
            idleAnimationCounter = 0;
        }

    } else if(jump) {
        ++jumpCounterX;
        if( jumpCounterX == 4) {
            ++jumpCounterY;
            jumpCounterX = 0;
        }

        if(jumpCounterX == 2 && jumpCounterY == 2) {
            jump = false;
            jumpCounterX = 0;
            jumpCounterY = 0;
        }
    }
    else {
       ++animationCounter;
        if(animationCounter == 6) {
            animationCounter = 0;
        }
    }

    if(mm) {
        idle = true;
    }

    for(auto bullet : bullet_sprites)
    {
        SelectObject(hdcMem, hbmbulletMask);
        BitBlt(hdcBuffer, bullet->x, bullet->y, bullet->width, bullet->height, hdcMem, 0, 0, SRCAND);
        SelectObject(hdcMem, hbmbullet);
        BitBlt(hdcBuffer, bullet->x, bullet->y, bullet->width, bullet->height, hdcMem, 0, 0, SRCPAINT);
    }


    SelectObject(hdcMem, hbmOld);
    DeleteDC(hdcMem);

    BitBlt(hdc, 0, 0, rect -> right, rect -> bottom, hdcBuffer, 0, 0, SRCCOPY);

    SelectObject(hdcBuffer, hbmOldBuffer);
    DeleteDC(hdcBuffer);
    DeleteObject(hbmBuffer);
}