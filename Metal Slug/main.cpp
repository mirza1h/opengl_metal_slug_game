#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#define SNIPER_TIMER 1
#define ARABIAN_TIMER 2
#define MUSIC_TIMER 3

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
#include "player.hpp"

void setupSniper(Player & sniper);
void setupArabian(Player& arabian);
void setupTerrain();
const int SOLDIER_SPEED = 4;
const int WAIT_TIME = 120;
const int SNIPER_FIRE_TIME = 1500;
const int ARABIAN_RUNNING_TIME = 1000;
const int MAP_SEGMENT_LENGTH = 544;
const int pauseTime = 20;
const int MUSIC_LENGTH = 50000;
bool gameStarted = false;
bool gameOver = false;

void DrawAnimation (HDC hdc, RECT * rect);
void UpdateSprites (RECT * rect);
void move_animation(int key, RECT* rect);
void FireBullet(int number_of_bullets, Player);
void Render(HWND);
void PrintText(HDC, int, int, int);
void PlaySound(char* filename);
BOOL Initialize(HWND);
void SniperFire();
void ArabianAttacks();
void setupBoss(Player & boss);

std::map<int,std::list<Player*>> enemies;
std::list<Bullet*> bullets;
std::list<Player*> currentEnemies;
std::list<Player*> currentMeleeEnemies;
std::vector<int> enemyIdle(3);
std::vector<int> enemyDeathCounter(3);
std::vector<int> enemyMovingCounter(3);
std::vector<int> enemyShootingCounter(3);
std::map<int,std::map<int, int>> terrainMappings;
std::map<int, int> currentTerrainMappings;

RECT temp;
Sprite bulletSprite = Sprite("assets/cartouche_droite_bullet_black.bmp","assets/cartouche_droite_bullet_white.bmp", 0, 0, 1, 1);
Sprite scoreSprite = Sprite("assets/score_black.bmp","assets/score_white.bmp", 0, 0, 1, 1);
Sprite livesSprite = Sprite("assets/lives_black.bmp","assets/lives_white.bmp", 0, 0, 1, 1);
Sprite bossBulletSprite = Sprite("assets/cartouche_boss2_bullet_black.bmp", "assets/cartouche_boss2_bullet_white.bmp", 0, 0, 1, 1);

Sprite sniperOneSprite = Sprite("assets/sniper_gauche/sniper_idle_black.bmp","assets/sniper_gauche/sniper_idle_white.bmp", 0, 0, 1, 1);
Sprite sniperDeath = Sprite("assets/sniper_gauche/sniper_death_black.bmp","assets/sniper_gauche/sniper_death_white.bmp", 0, 0, 5, 1);

Sprite bossIdleSprite = Sprite("assets/boss/boss_idle_black.bmp", "assets/boss/boss_idle_white.bmp", 0, 0, 1, 1);
Sprite bossShootSprite = Sprite("assets/boss/boss_shoot_black.bmp", "assets/boss/boss_shoot_white.bmp", 0, 0, 4, 1);
Sprite bossDeathSprite = Sprite("assets/boss/boss_death_black.bmp", "assets/boss/boss_death_white.bmp", 0, 0, 6, 1);

Sprite arabianOneIdleSprite = Sprite("assets/arabian/ennemie_arabian_gauche_idle_black.bmp","assets/arabian/ennemie_arabian_gauche_idle_white.bmp", 0, 0, 6, 1);
Sprite arabianOneMoveSprite = Sprite("assets/arabian/ennemie_arabian_gauche_running_black.bmp","assets/arabian/ennemie_arabian_gauche_running_white.bmp", 0, 0, 12, 1);
Sprite arabianOneShootSprite = Sprite("assets/arabian/ennemie_arabian_gauche_attack_black.bmp","assets/arabian/ennemie_arabian_gauche_attack_white.bmp", 0, 0, 2, 1);
Sprite arabianOneDeathSprite = Sprite("assets/arabian/ennemie_arabian_gauche_death_black.bmp","assets/arabian/ennemie_arabian_gauche_death_white.bmp", 0, 0, 5, 1);

Sprite soldierMoveSprite = Sprite("assets/player/move_right_black.bmp", "assets/player/move_right_white.bmp", 0, 0, 6, 1);
Sprite soldierIdleSprite = Sprite("assets/player/idle_right_black.bmp", "assets/player/idle_right_white.bmp", 0, 0, 4, 1);
Sprite soldierJumpSprite = Sprite("assets/player/jump_right_black.bmp", "assets/player/jump_right_white.bmp", 0, 0, 4, 3);
Sprite soldierShootSprite = Sprite("assets/player/shoot_black.bmp", "assets/player/shoot_white.bmp", 0, 0, 4, 1);
Sprite soldierDeathSprite = Sprite("assets/player/death_black.bmp", "assets/player/death_white.bmp", 0, 0, 4, 1);

Background backgroundSprite = Background("assets/stage1.bmp",0,0);
Background gameSplashScreen = Background("assets/splash_screen.bmp",0,0);
Background gameOverScreen = Background("assets/game_over.bmp",0,0);

Player sniperOne = Player(290, 200, 0, 0, temp, false, 3, Sniper);
Player sniperTwo = Player(430, 190, 0, 0, temp, false, 3, Sniper);
Player sniperThree = Player(460, 175, 0, 0, temp, false, 3, Sniper);
Player sniperFour = Player(300, 190, 0, 0, temp, false, 3, Sniper);
Player sniperFive = Player(430, 240, 0, 0, temp, false, 3, Sniper);

Player boss = Player(400, 205, 0, 0, temp, false, 10, Boss);

Player soldier = Player(0, 200, 0, 0, temp, true, 3, Soldier);

Player arabianOne = Player(100, 200, 0, 0, temp, false, 1, Arabian);
Player arabianTwo = Player(110, 165, 0, 0, temp, false, 1, Arabian);
Player arabianThree = Player(360, 200, 0, 0, temp, false, 1, Arabian);
Player arabianFour = Player(220, 175, 0, 0, temp, false, 1, Arabian);

int animationCounter = 0;
int idleAnimationCounter = 0;
int jumpCounterX = 0;
int jumpCounterY = 0;
int shootCounter = 0;
int deathCounter = 0;
int mapSegementCount = 1;
bool move_right = false;
bool reachedTarget = false;
size_t playerScore = 0;

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
               290,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );
    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);
    // Gameloop
    PlaySound("sounds/theme.wav");
    HDC hdc = GetDC(hwnd);
    gameSplashScreen.draw(hdc);
    while(TRUE)
    {
        DWORD startTime;
        if(gameOver == true)
        {
            gameOverScreen.draw(hdc);
            KillTimer(hwnd, SNIPER_TIMER);
            KillTimer(hwnd, ARABIAN_TIMER);
        }
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
        if(gameStarted && !gameOver)
            Render(hwnd);
        while((GetTickCount() - startTime) < pauseTime)
        {
            Sleep(60);
        }
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

BOOL Initialize(HWND hwnd)
{
    SetTimer(hwnd, SNIPER_TIMER, SNIPER_FIRE_TIME, NULL);
    SetTimer(hwnd, ARABIAN_TIMER, ARABIAN_RUNNING_TIME, NULL);
    SetTimer(hwnd, MUSIC_TIMER, MUSIC_LENGTH, NULL);

    setupTerrain();
    setupSniper(sniperOne);
    setupSniper(sniperTwo);
    setupSniper(sniperThree);
    setupSniper(sniperFour);
    setupSniper(sniperFive);
    setupBoss(boss);
    setupArabian(arabianOne);
    setupArabian(arabianTwo);
    setupArabian(arabianThree);
    setupArabian(arabianFour);
    soldier.setIdle(soldierIdleSprite);
    soldier.setMoving(soldierMoveSprite);
    soldier.setJump(soldierJumpSprite);
    soldier.setShoot(soldierShootSprite);
    soldier.setDeath(soldierDeathSprite);
    soldier.setHumanPlayer();
    for(int i = 1; i < 6; ++i)
    {
        enemies[i] = std::list<Player*>();
    }
    enemies[1].push_back(&sniperOne);
    enemies[1].push_back(&sniperTwo);
    enemies[1].push_back(&arabianOne);

    enemies[2].push_back(&sniperThree);
    enemies[2].push_back(&arabianTwo);
    enemies[2].push_back(&arabianThree);

    enemies[3].push_back(&arabianFour);
    enemies[3].push_back(&sniperFour);

    enemies[4].push_back(&sniperFive);

    enemies[5].push_back(&boss);
    currentEnemies = enemies[1];
    return true;
}
/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
    switch (message)                  /* handle the messages */
    {
        break;
    case WM_TIMER:
        switch (wParam)
        {
        case SNIPER_TIMER:
            SniperFire();
            return 0;
        case ARABIAN_TIMER:
            ArabianAttacks();
            return 0;
        case MUSIC_TIMER:
            PlaySound("sounds/theme.wav");
        }
    case WM_KEYDOWN:
    {
        GetClientRect(hwnd,&rect);
        if(wParam == VK_SPACE)
        {
            if(soldier.getPlayerState() !=Dead)
            {
                soldier.setPlayerState(Shooting);
                FireBullet(1, soldier);
            }
        }
        if(wParam == VK_RETURN)
        {
            gameStarted = Initialize(hwnd);
        }
        if(wParam == VK_ESCAPE)
        {
            ExitProcess(1);
        }
    }
    break;
    case WM_KEYUP:
        switch(wParam)
        {
        case VK_RIGHT:
            move_right = false;
            if(soldier.getPlayerState() != Dead)
            {
                soldier.setPlayerState(Idle);
            }
            break;
        case VK_LEFT:
            if(soldier.getPlayerState() != Dead)
            {
                soldier.setPlayerState(Idle);
            }
            break;
        default:
            break;
        }
        break;
    case WM_DESTROY:
        KillTimer(hwnd, SNIPER_TIMER);
        KillTimer(hwnd, ARABIAN_TIMER);
        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

void SniperFire()
{
    if(mapSegementCount == 1)
    {
        if(sniperOne.getPlayerState() != Dead)
        {
            FireBullet(1, sniperOne);
        }
        else
        {
            if(sniperTwo.getPlayerState() != Dead)
                FireBullet(1, sniperTwo);
        }
    }
    if(mapSegementCount == 2)
    {
        if(sniperThree.getPlayerState() != Dead)
        {
            FireBullet(1, sniperThree);
        }
    }
    if(mapSegementCount == 3)
    {
        if(sniperFour.getPlayerState() != Dead)
        {
            FireBullet(1, sniperFour);
        }
    }
    if(mapSegementCount == 4)
    {
        if(sniperFive.getPlayerState() != Dead)
            FireBullet(1, sniperFive);
    }
    if(mapSegementCount == 5)
    {
        if(boss.getPlayerState() != Dead)
        {
            FireBullet(1, boss);
            boss.setPlayerState(Shooting);
        }
    }
}

void ArabianAttacks()
{
    if(mapSegementCount == 1)
    {
        if(arabianOne.getPlayerState() != Dead && arabianOne.getPlayerState() != Shooting)
            arabianOne.setPlayerState(Moving);
    }
    if(mapSegementCount == 2)
    {
        if(arabianTwo.getPlayerState() != Dead && arabianTwo.getPlayerState() != Shooting)
        {
            arabianTwo.setPlayerState(Moving);
        }
        else
        {
            if(arabianThree.getPlayerState() != Dead && arabianThree.getPlayerState() != Shooting && soldier.getX() >= 250)
                arabianThree.setPlayerState(Moving);
        }
    }
    if(mapSegementCount == 3)
    {
        if(arabianFour.getPlayerState() != Dead && arabianThree.getPlayerState() != Shooting)
            arabianFour.setPlayerState(Moving);
    }
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
    Sprite temp = bulletSprite;
    if (shooter.getPlayerType() == Boss)
        temp = bossBulletSprite;
    for(int i = 0; i < number_of_bullets; ++i)
    {
        PlaySound("sounds/gun.wav");
        Bullet* bulletObj = new Bullet(shooter, temp, shooter.getDirection());
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

void PrintText(HDC hdc,int number, int x, int y)
{
    HFONT hFont,hTmp;
    hFont=CreateFont(28,0,0,0,FW_BOLD,0,0,0,0,0,0,NONANTIALIASED_QUALITY,FF_SCRIPT,"SYSTEM_FIXED_FONT");
    hTmp=(HFONT)SelectObject(hdc,hFont);
    SIZE dim;
    SetBkMode(hdc, TRANSPARENT);
    number = number < 0 ? 0 : number;
    char text[5];
    sprintf(text, "%d", number);
    GetTextExtentPoint32(hdc, text, strlen(text), &dim);
    TextOut(hdc,x,y,text,strlen(text));
    DeleteObject(SelectObject(hdc,hTmp));
}

void UpdateSprites(RECT * rect)
{
    if(soldier.getY() >= 280)
    {
        soldier.setPlayerState(Dead);
    }

    if(PRESSED(VK_RIGHT) && (soldier.getPlayerState() != Shooting) && (soldier.getPlayerState() != Dead))
    {
        move_animation(VK_RIGHT, rect);
    }
    if(PRESSED(VK_LEFT) && (soldier.getPlayerState() != Shooting) && (soldier.getPlayerState() != Dead))
    {
        move_animation(VK_LEFT, rect);
    }
    if (PRESSED(VK_UP) && (soldier.getPlayerState() != Shooting) && (soldier.getPlayerState() != Dead))
    {
        move_animation(VK_UP, rect);
    }

    if(soldier.getJumping())
    {

        if(PRESSED(VK_RIGHT) && (soldier.getPlayerState() != Dead))
        {
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
        currentEnemies = enemies[mapSegementCount];
        currentTerrainMappings = terrainMappings[mapSegementCount];
    }
    // If there's a terrain change, update y position
    if(currentTerrainMappings.count(soldier.getX()) == 1 && (soldier.getJumping() == false))
    {
        soldier.setY(currentTerrainMappings[soldier.getX()]);
    }

    for(auto it = currentEnemies.begin(); it != currentEnemies.end(); ++it)
    {
        if((*it)->getPlayerState() == Moving )
        {
            (*it)->moveLeft(5);
            if(currentTerrainMappings.count((*it)->getX()) == 1)
            {
                (*it)->setY(currentTerrainMappings[(*it)->getX()]);
            }
        }
    }

    for(auto it = currentEnemies.begin(); it != currentEnemies.end(); ++it)
    {
        if((*it)->getShoot().getColumnCount() != 0 && (*it)->getPlayerState() != Dead  && soldier.getPlayerState() != Dead && (*it)->isHit(soldier))
        {
            (*it)->setPlayerState(Shooting);
            soldier.decreaseNumLives();
            if(soldier.getLives() == 0)
            {
                soldier.setPlayerState(Dead);
                gameOver = true;
            }
        }
        else if((*it)->getPlayerState() != Moving && (*it)->getPlayerState() != Dead && mapSegementCount != 5)
        {
            (*it)->setPlayerState(Idle);
        }
    }

}

void move_animation(int key, RECT* rect)
{
    switch(key)
    {
    case VK_LEFT:
        if(soldier.getX() > 0)
        {
            soldier.setPlayerState(Moving);
            soldier.moveLeft(SOLDIER_SPEED);
        }
        break;
    case VK_UP:
        if(soldier.getY() > rect->top - soldier.getHeight() && (soldier.getJumping() == false) )
        {
            soldier.setJumping();
        }
        break;
    case VK_RIGHT:
        if(mapSegementCount <= 5)
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
    int i = 0;
    for(auto it = currentEnemies.begin(); it != currentEnemies.end(); it++)
    {
        if((*it)->getPlayerState() == Dead)
        {
            (*it)->getDeath().draw(hdcBuffer, (*it)->getX(), (*it)->getY(), enemyDeathCounter[i]++, 0, false);
            if(enemyDeathCounter[i] == (*it)->getDeath().getColumnCount())
            {
                it = currentEnemies.erase(it);
                ++playerScore;
                enemyDeathCounter[i] = 0;
            }
        }
        else if((*it)->getPlayerState() == Moving)
        {
            (*it)->getMoving().draw(hdcBuffer, (*it)->getX(), (*it)->getY(), enemyMovingCounter[i]++, 0, false);
            if(enemyMovingCounter[i] == (*it)->getMoving().getColumnCount())
            {
                enemyMovingCounter[i] = 0;
            }
        }
        else if((*it)->getPlayerState() == Shooting)
        {
            (*it)->getShoot().draw(hdcBuffer, (*it)->getX(), (*it)->getY(), enemyShootingCounter[i]++, 0, false);
            if(enemyShootingCounter[i] == (*it)->getShoot().getColumnCount())
            {
                enemyShootingCounter[i] = 0;
            }
        }
        else
        {
            (*it)->getIdle().draw(hdcBuffer, (*it)->getX(), (*it)->getY(), enemyIdle[i]++, 0, false);
            if(enemyIdle[i] == (*it)->getIdle().getColumnCount())
            {
                enemyIdle[i] = 0;
            }
        }
        ++i;
    }
    if(soldier.getJumping())
    {
        soldier.getJump().draw(hdcBuffer, soldier.getX(), soldier.getY(), jumpCounterX, jumpCounterY, false);
    }
    else if(soldier.getPlayerState() == Dead)
    {
        soldier.getDeath().draw(hdcBuffer, soldier.getX(), soldier.getY(), deathCounter, 0, false);
    }
    else if (soldier.getPlayerState() == Shooting)
    {
        soldier.getShoot().draw(hdcBuffer, soldier.getX(), soldier.getY(), shootCounter, 0, false);
    }
    else if(soldier.getPlayerState() == Idle)
    {
        soldier.getIdle().draw(hdcBuffer, soldier.getX(), soldier.getY(), idleAnimationCounter, 0, false);
    }
    else
    {
        soldier.getMoving().draw(hdcBuffer, soldier.getX(), soldier.getY(), animationCounter, 0, false);
    }

    if(soldier.getJumping())
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
            soldier.resetJumping();
            jumpCounterX = 0;
            jumpCounterY = 0;
        }
    }
    else if(soldier.getPlayerState() == Dead)
    {
        deathCounter++;
        if(deathCounter == 4)
        {
            gameOver = true;
            soldier.setLives();
            soldier.setX(0);
            soldier.setY(currentTerrainMappings[0]);
            soldier.setPlayerState(Idle);
            deathCounter = 0;
        }
    }
    else if(soldier.getPlayerState() == Idle)
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
        if((*bullet)->isHit(soldier))
        {
            soldier.decreaseNumLives();
            if(soldier.getLives() == 0)
            {
                soldier.setPlayerState(Dead);
            }
        }
        for(auto it = currentEnemies.begin(); it != currentEnemies.end(); it++)
        {
            if((*bullet)->isHit(**it))
            {
                bullet = bullets.erase(bullet);
                (*it)->decreaseNumLives();

                if((*it)->getLives() == 0)
                {
                    PlaySound("sounds/die.wav");
                    (*it)->setPlayerState(Dead);
                }
            }
        }

    }

    DeleteDC(hdcMem);

    BitBlt(hdc, 0, 0, rect -> right, rect -> bottom, hdcBuffer, 0, 0, SRCCOPY);
    PrintText(hdc, soldier.getLives(), 78, 20);
    PrintText(hdc, playerScore, 500, 20);
    SelectObject(hdcBuffer, hbmOldBuffer);
    DeleteDC(hdcBuffer);
    DeleteObject(hbmBuffer);
}

void setupArabian(Player& arabian)
{
    arabian.setIdle(arabianOneIdleSprite);
    arabian.setMoving(arabianOneMoveSprite);
    arabian.setShoot(arabianOneShootSprite);
    arabian.setDeath(arabianOneDeathSprite);

}

void setupSniper(Player & sniper)
{
    sniper.setIdle(sniperOneSprite);
    sniper.setDeath(sniperDeath);
}

void setupBoss(Player & boss)
{
    boss.setIdle(bossIdleSprite);
    boss.setShoot(bossShootSprite);
    boss.setDeath(bossDeathSprite);
}

void terrainOne()
{
    for( int i = 0; i < 151; i++)
    {
        terrainMappings[1][i] = 200;
    }
    int berg = 195;
    int bergInc = 5;
    int bergInc2 = 2;
    for(int i = 151; i <= 540; i++)
    {
        terrainMappings[1][i] = berg;
        if(i%10 == 0 && i < 180)
        {
            berg-=bergInc;
        }

        if(i%10 == 0 && i > 230 && i < 280)
        {
            berg+=bergInc;
        }

        if(i%10 == 0 && ((i > 280 && i < 400) || (i >= 476 && i <= 540)))
        {
            berg-=bergInc2;
        }


    }

}

void terrainTwo()
{
    int currentPlace = 170;
    int inc = 5;
    for( int i = 0; i <= MAP_SEGMENT_LENGTH; ++i)
    {
        if(i < 180)
            terrainMappings[2][i] = terrainMappings[1][540];
        else
        {
            if(i >= 180 && i < 250)
            {
                if(i%10 == 0)
                {
                    currentPlace+=inc;
                }
            }

            if(i > 384 && i <= 450 && i % 10 == 0)
            {
                currentPlace -=3;
            }

            terrainMappings[2][i] = currentPlace;
        }


    }
}

void terrainThree()
{
    int currentPlace = 180;
    int inc = 5;
    for( int i = 0; i <= MAP_SEGMENT_LENGTH; ++i)
    {
        if(i < 280)
            terrainMappings[3][i] = terrainMappings[2][540];
        else
        {

            if(i%10 == 0)
            {
                currentPlace+=2;
            }


            terrainMappings[3][i] = currentPlace;
        }


    }

}

void terrainFour()
{
    for(int i = 0; i < terrainMappings[3].size() ; ++i)
    {
        if((i >= 116 && i <= 130 )
                ||(i >= 212 && i <= 228)
                || (i >= 376 && i <= 396)
                || (i >= 476 && i <= 496) )
        {
            terrainMappings[4][i]=290;

        }
        else
            terrainMappings[4][i] = terrainMappings[3][540];
    }
}

void terrainFive()
{
    for(int i = 0; i < terrainMappings[3].size() ; ++i)
    {
        terrainMappings[5][i] = terrainMappings[3][540];
    }
}

void setupTerrain()
{
    terrainOne();
    terrainTwo();
    terrainThree();
    terrainFour();
    terrainFive();
    currentTerrainMappings = terrainMappings[1];
}


