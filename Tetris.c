#include<stdlib.h>
#include<windows.h>
#include<time.h>
#define ID_TIMER  1
#define NUM 200

struct
{
    int iStyle;
    TCHAR * szText;
}

button[]=
{
    BS_PUSHBUTTON,    TEXT("Single Player"),
    BS_PUSHBUTTON,    TEXT("Multy Players"),
    BS_PUSHBUTTON,    TEXT("Quit Game"),
};

const int SquareSize=20;
const int GameSizeX=300;
const int GameSizeY=50;
const int GameSizeX_Player1=100;
const int GameSizeY_Player1=50;
const int GameSizeX_Player2=600;
const int GameSizeY_Player2=50;
const int GameRegionWidth=10;
const int GameRegionHeight=20;
const RECT rect_Player1= {100,50,500,500};
const RECT rect_Player2= {600,50,1000,500};
BOOL Stop=FALSE;
BOOL Start=TRUE;
int Players;
BOOL invalidatePlayer1=TRUE;
BOOL invalidatePlayer2=TRUE;
BOOL FirstDraw=TRUE;

struct GameMapStates
{
    int states;
    int shapes;
} GameMapStatesInfo[10][20],GameMapStatesInfo_Player1[10][20],GameMapStatesInfo_Player2[10][20]; //游戏区每一个小块的状态

int nx[4],ny[4]; //每一个小矩形的坐标
int nx_Player1[4],ny_Player1[4]; //每一个小矩形的坐标
int nx_Player2[4],ny_Player2[4]; //每一个小矩形的坐标

int count=0;
int score=0;
int level=0;
int speed=500;
int speed2=350;
int szshape=0;
int szshape_Player1=1;
int szshape_Player2=2;

struct CurrentTerics
{
    int shape,dir,x,y;
} CurrentTericsInfo,CurrentTericsInfo_Player1,CurrentTericsInfo_Player2;

const COLORREF color[7] =
{
    RGB(255,0,0),
    RGB(240,100,5),
    RGB(150,250,60),
    RGB(27,229,50),
    RGB(10,125,145),
    RGB(20,12,184),
    RGB(116,34,156)

};

const POINT Terics[7][4][4] =
{
    {

        0,0,1,0,0,1,-1,1,  //   **
        //  **

        0,0,0,1,1,1,1,2,   //  *
        //  **
        //   *
        0,0,1,0,0,1,-1,1,  //同一

        0,0,0,1,1,1,1,2    //同二

    },
    {

        0,0,1,0,1,1,2,1,  //   **
        //    **

        0,0,0,1,-1,1,-1,2,  //   *
        //  **
        //  *

        0,0,1,0,1,1,2,1,    //同一

        0,0,0,1,-1,1,-1,2   //同二

    },
    {

        0,0,0,1,0,2,1,2,    //  *
        //   *
        //   **

        0,0,0,1,-1,1,-2,1,  //    *
        //  ***

        0,0,1,0,1,1,1,2,    //  **
        //    *
        //    *

        0,0,0,1,1,0,2,0     //    ***
        //    *

    },
    {

        0,0,0,1,0,2,-1,2,   //    *
        //    *
        //   **

        0,0,1,0,2,0,2,1,    //   ***
        //     *

        0,0,1,0,0,1,0,2,    //   **
        //   *
        //   *

        0,0,0,1,1,1,2,1     //   *
        //   ***

    },
    {

        0,0,0,1,0,2,0,3,     //  *
        //  *
        //  *
        //  *

        0,0,1,0,2,0,3,0,     //  ****

        0,0,0,1,0,2,0,3,     //  *
        //  *
        //  *
        //  *

        0,0,1,0,2,0,3,0      //  ****

    },
    {

        0,0,1,0,0,1,1,1,      //   **
        //   **

        0,0,1,0,0,1,1,1,      //   **
        //   **

        0,0,1,0,0,1,1,1,

        0,0,1,0,0,1,1,1

    },
    {

        0,0,1,0,2,0,1,1,       //   ***
        //    *

        0,0,0,1,0,2,1,1,       //    *
        //    **
        //    *
        0,0,0,1,-1,1,1,1,      //     *
        //    ***

        0,0,0,1,0,2,-1,1       //    *
        //   **
        //    *

    }
};

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
void DrawGameRegion(HDC hdc);//绘制游戏区
void DrawGameRegionPlayer1(HDC hdc);
void DrawGameRegionPlayer2(HDC hdc);

void EraseGameRegion(HDC hdc);//擦除游戏区
void EraseGameRegionPlayer1(HDC hdc);
void EraseGameRegionPlayer2(HDC hdc);

void DrawRectangle(HDC hdc,int shape,int x,int y);//绘制矩形，x,y表示相对于游戏区边界的偏移量，单位是一个方格的大小
void DrawRectanglePlayer1(HDC hdc,int shape,int x,int y);
void DrawRectanglePlayer2(HDC hdc,int shape,int x,int y);

void DrawTerics(HDC hdc,int shape,int dir,int x,int y);//绘制俄罗斯方块
void DrawTericsPlayer1(HDC hdc,int shape,int dir,int x,int y);
void DrawTericsPlayer2(HDC hdc,int shape,int dir,int x,int y);

BOOL IsOutOfRegion(int shape,int dir,int x,int y);//判断图形是否出界
BOOL IsOutOfRegionPlayer1(int shape,int dir,int x,int y);
BOOL IsOutOfRegionPlayer2(int shape,int dir,int x,int y);

void InitTericsInfo();//初始化俄罗斯方块
void InitTericsInfoPlayer1();
void InitTericsInfoPlayer2();

void SaveStateOfTerics(int shape, int dir, int x, int y);//保存游戏区状态
void SaveStateOfTericsPlayer1(int shape, int dir, int x, int y);
void SaveStateOfTericsPlayer2(int shape, int dir, int x, int y);

void LoadCurrentTerics(HDC hdc);//加载已有的俄罗斯方块
void LoadCurrentTericsPlayer1(HDC hdc);
void LoadCurrentTericsPlayer2(HDC hdc);

void PrintInfo(HDC hdc,int shape);//输出函数
void PrintInfoPlayer1(HDC hdc,int shape);
void PrintInfoPlayer2(HDC hdc,int shape);

BOOL IsGameOver(int shape,int dir,int x,int y);//游戏结束
BOOL IsGameOverPlayer1(int shape,int dir,int x,int y);
BOOL IsGameOverPlayer2(int shape,int dir,int x,int y);


//******************************入口函数******************************
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR szCmdLine,int iCmdShow)
{
    static TCHAR szAppName[]=TEXT("MyTerics") ;
    HWND hwnd ;
    MSG msg ;
    WNDCLASS wndclass ;
//注册窗口类
    wndclass.style=CS_HREDRAW | CS_VREDRAW ;
    wndclass.lpfnWndProc=WndProc;   //窗口过程，见下窗口过程函数
    wndclass.cbClsExtra=0 ;
    wndclass.cbWndExtra=0 ;
    wndclass.hInstance=hInstance ;
    wndclass.hIcon=LoadIcon (NULL, IDI_APPLICATION) ;
    wndclass.hCursor=LoadCursor(NULL, IDC_ARROW) ;
    wndclass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName=NULL ;
    wndclass.lpszClassName=szAppName ;
    if(!RegisterClass(&wndclass))
    {
        MessageBox(NULL,TEXT("Program requires Windows NT!"),

                   szAppName, MB_ICONERROR) ;

        return 0 ;
    }
//窗口的创建
    hwnd=CreateWindow(szAppName,
                      TEXT("MyTerics"),
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      NULL,
                      NULL,
                      hInstance,
                      NULL) ;
//窗口的显示
    ShowWindow(hwnd,iCmdShow) ;
//窗口客户区重绘
    UpdateWindow(hwnd) ;
//消息循环
    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam ;

}

//****************************窗口过程函数************************************
LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)//hwnd 表示接收消息的窗口句柄
{
    HDC hdc ;
    PAINTSTRUCT ps ;
    RECT rect;
    HBRUSH brush;
    static HWND hwndButton[3];
    static int cxChar,cyChar;
    int i,ID;

    switch (message)
    {
//处理WM_CREATE消息，处理CreateWindow函数时接收该消息，
    case WM_CREATE:
        //MessageBox(hwnd,TEXT("Have a good time!"),TEXT("Welcome"),MB_OK);
        cxChar=LOWORD(GetDialogBaseUnits());
        cyChar=HIWORD(GetDialogBaseUnits());
        for(i=0; i<3; i++)
            hwndButton[i]=CreateWindow(TEXT("button"),button[i].szText,WS_CHILD|WS_VISIBLE|button[i].iStyle,cxChar*(1+52*i),cyChar,
                                       20*cxChar,7*cyChar/4,hwnd,(HMENU)i,((LPCREATESTRUCT)lParam)->hInstance,NULL);
        return 0;
//处理WM_TIMER消息
    case WM_TIMER:
        hdc=GetDC(hwnd);
        if(Players==1)
        {
            if(!IsOutOfRegion(CurrentTericsInfo.shape,CurrentTericsInfo.dir,
                              CurrentTericsInfo.x,(CurrentTericsInfo.y+1)))
            {
                ++CurrentTericsInfo.y;
                EraseGameRegion(hdc);
                DrawGameRegion(hdc);
                LoadCurrentTerics(hdc);
                DrawTerics(hdc,CurrentTericsInfo.shape,CurrentTericsInfo.dir,
                           CurrentTericsInfo.x,CurrentTericsInfo.y);
            }
            else
            {
                SaveStateOfTerics(CurrentTericsInfo.shape,CurrentTericsInfo.dir,
                                  CurrentTericsInfo.x,CurrentTericsInfo.y);
                InvalidateRect(hwnd,NULL,FALSE);
            }
        }
        else
        {
            if(!IsOutOfRegionPlayer1(CurrentTericsInfo_Player1.shape,CurrentTericsInfo_Player1.dir,
                                     CurrentTericsInfo_Player1.x,(CurrentTericsInfo_Player1.y+1)))
            {
                ++CurrentTericsInfo_Player1.y;
                EraseGameRegionPlayer1(hdc);
                DrawGameRegionPlayer1(hdc);
                LoadCurrentTericsPlayer1(hdc);
                DrawTericsPlayer1(hdc,CurrentTericsInfo_Player1.shape,CurrentTericsInfo_Player1.dir,
                                  CurrentTericsInfo_Player1.x,CurrentTericsInfo_Player1.y);
            }
            else
            {
                SaveStateOfTericsPlayer1(CurrentTericsInfo_Player1.shape,CurrentTericsInfo_Player1.dir,
                                         CurrentTericsInfo_Player1.x,CurrentTericsInfo_Player1.y);
                invalidatePlayer1=TRUE;
            }
            if(!IsOutOfRegionPlayer2(CurrentTericsInfo_Player2.shape,CurrentTericsInfo_Player2.dir,
                                     CurrentTericsInfo_Player2.x,(CurrentTericsInfo_Player2.y+1)))
            {
                ++CurrentTericsInfo_Player2.y;
                EraseGameRegionPlayer2(hdc);
                DrawGameRegionPlayer2(hdc);
                LoadCurrentTericsPlayer2(hdc);
                DrawTericsPlayer2(hdc,CurrentTericsInfo_Player2.shape,CurrentTericsInfo_Player2.dir,
                                  CurrentTericsInfo_Player2.x,CurrentTericsInfo_Player2.y);
            }
            else
            {
                SaveStateOfTericsPlayer2(CurrentTericsInfo_Player2.shape,CurrentTericsInfo_Player2.dir,
                                         CurrentTericsInfo_Player2.x,CurrentTericsInfo_Player2.y);
                invalidatePlayer2=TRUE;
            }
            if(invalidatePlayer2&&!invalidatePlayer1)
            {
                InvalidateRect(hwnd,&rect_Player2,FALSE);
            }
            if(invalidatePlayer1&&!invalidatePlayer2)
                InvalidateRect(hwnd,&rect_Player1,FALSE);
            if(invalidatePlayer1||invalidatePlayer2)
            {
                InvalidateRect(hwnd,&rect_Player1,FALSE);
                InvalidateRect(hwnd,&rect_Player2,FALSE);
            }
        }
        ReleaseDC (hwnd, hdc) ;
        return 0;
//处理WM_PAINT消息
    case WM_PAINT:
        hdc=BeginPaint(hwnd,&ps);
        if(Start==TRUE)
        {
            int i;
            brush=CreateSolidBrush(RGB(0,0,0));
            GetClientRect(hwnd,&rect) ;
            FillRect(hdc,&rect,brush);
            for(i=0; i<7; i++)
                DrawTerics(hdc,i,0,-10+10*i,10);
            Start=FALSE;
        }
        else
        {
            if(Players==1)
            {
                brush=CreateSolidBrush(RGB(165,180,255));
                GetClientRect(hwnd,&rect);
                FillRect(hdc,&rect,brush);
                EraseGameRegion(hdc);
                DrawGameRegion(hdc);
                InitTericsInfo();
                PrintInfo(hdc,szshape);
                if(level!=0)
                {
                    KillTimer(hwnd,ID_TIMER);
                    SetTimer(hwnd,ID_TIMER,speed-100*level,NULL);
                }
                LoadCurrentTerics(hdc);
                if(!(IsGameOver(CurrentTericsInfo.shape,CurrentTericsInfo.dir,
                                CurrentTericsInfo.x,CurrentTericsInfo.y)))
                {
                    DrawTerics(hdc,CurrentTericsInfo.shape,CurrentTericsInfo.dir,
                               CurrentTericsInfo.x,CurrentTericsInfo.y);
                }
                else
                {
                    char szscore[200];
                    wsprintf(szscore,"Somplete %d score!",score);
                    KillTimer(hwnd,ID_TIMER);
                    MessageBox(hwnd,szscore,TEXT("GameOver"),MB_OK);
                }
            }
            else
            {
                if(FirstDraw==TRUE)
                {
                    brush=CreateSolidBrush(RGB(165,180,255));
                    GetClientRect(hwnd,&rect);
                    FillRect(hdc,&rect,brush);
                    FirstDraw=FALSE;
                }
                if(invalidatePlayer1)
                {
                    brush=CreateSolidBrush(RGB(165,180,255));
                    FillRect(hdc,&rect_Player1,brush);
                    EraseGameRegionPlayer1(hdc);
                    DrawGameRegionPlayer1(hdc);
                    InitTericsInfoPlayer1();
                    PrintInfoPlayer1(hdc,szshape_Player1);
                    LoadCurrentTericsPlayer1(hdc);
                    if(!(IsGameOverPlayer1(CurrentTericsInfo_Player1.shape,CurrentTericsInfo_Player1.dir,
                                           CurrentTericsInfo_Player1.x,CurrentTericsInfo_Player1.y)))
                    {
                        DrawTericsPlayer1(hdc,CurrentTericsInfo_Player1.shape,CurrentTericsInfo_Player1.dir,
                                          CurrentTericsInfo_Player1.x,CurrentTericsInfo_Player1.y);
                    }
                    else
                    {
                        KillTimer(hwnd,ID_TIMER);
                        MessageBox(hwnd,TEXT("Player2 is the winner! Congratulations!"),TEXT("GameOver"),MB_OK);
                    }
                    invalidatePlayer1=FALSE;
                }
                if(invalidatePlayer2)
                {
                    brush=CreateSolidBrush(RGB(165,180,255));
                    FillRect(hdc,&rect_Player2,brush);
                    EraseGameRegionPlayer2(hdc);
                    DrawGameRegionPlayer2(hdc);
                    InitTericsInfoPlayer2();
                    PrintInfoPlayer2(hdc,szshape_Player2);
                    LoadCurrentTericsPlayer2(hdc);
                    if(!(IsGameOverPlayer2(CurrentTericsInfo_Player2.shape,CurrentTericsInfo_Player2.dir,
                                           CurrentTericsInfo_Player2.x,CurrentTericsInfo_Player2.y)))
                    {
                        DrawTericsPlayer2(hdc,CurrentTericsInfo_Player2.shape,CurrentTericsInfo_Player2.dir,
                                          CurrentTericsInfo_Player2.x,CurrentTericsInfo_Player2.y);
                    }
                    else
                    {
                        KillTimer(hwnd,ID_TIMER);
                        MessageBox(hwnd,TEXT("Player1 is the winner! Congratulations!"),TEXT("GameOver"),MB_OK);
                    }
                    invalidatePlayer2=FALSE;
                }
            }
        }
        EndPaint(hwnd,&ps);
        return 0 ;
    //处理键盘消息
    case WM_KEYDOWN:
        hdc=GetDC(hwnd);
        switch(wParam)
        {
        case VK_UP:
            if(Players==1)
            {
                if(Stop==FALSE)
                {
                    if(!IsOutOfRegion(CurrentTericsInfo.shape,(CurrentTericsInfo.dir+1)%4,
                                      CurrentTericsInfo.x,CurrentTericsInfo.y))
                    {
                        CurrentTericsInfo.dir=++CurrentTericsInfo.dir%4;
                        EraseGameRegion(hdc);
                        DrawGameRegion(hdc);
                        LoadCurrentTerics(hdc);
                        DrawTerics(hdc,CurrentTericsInfo.shape,CurrentTericsInfo.dir,
                                   CurrentTericsInfo.x,CurrentTericsInfo.y);
                    }
                }
            }
            else
            {
                if(Stop==FALSE)
                {
                    if(!IsOutOfRegionPlayer2(CurrentTericsInfo_Player2.shape,(CurrentTericsInfo_Player2.dir+1)%4,
                                             CurrentTericsInfo_Player2.x,CurrentTericsInfo_Player2.y))
                    {
                        CurrentTericsInfo_Player2.dir=++CurrentTericsInfo_Player2.dir%4;
                        EraseGameRegionPlayer2(hdc);
                        DrawGameRegionPlayer2(hdc);
                        LoadCurrentTericsPlayer2(hdc);
                        DrawTericsPlayer2(hdc,CurrentTericsInfo_Player2.shape,CurrentTericsInfo_Player2.dir,
                                          CurrentTericsInfo_Player2.x,CurrentTericsInfo_Player2.y);
                    }
                }
            }
            return 0;
        case VK_DOWN:
            if(Players==1)
            {
                if(Stop==FALSE)
                {
                    if(!IsOutOfRegion(CurrentTericsInfo.shape,CurrentTericsInfo.dir,
                                      CurrentTericsInfo.x,(CurrentTericsInfo.y+1)))
                    {
                        ++CurrentTericsInfo.y;
                        EraseGameRegion(hdc);
                        DrawGameRegion(hdc);
                        LoadCurrentTerics(hdc);
                        DrawTerics(hdc,CurrentTericsInfo.shape,CurrentTericsInfo.dir,
                                   CurrentTericsInfo.x,CurrentTericsInfo.y);
                    }
                    else
                    {
                        SaveStateOfTerics(CurrentTericsInfo.shape,CurrentTericsInfo.dir,
                                          CurrentTericsInfo.x,CurrentTericsInfo.y);
                        InvalidateRect(hwnd,NULL,FALSE);
                    }
                }
            }
            else
            {
                if(Stop==FALSE)
                {
                    if(!IsOutOfRegionPlayer2(CurrentTericsInfo_Player2.shape,CurrentTericsInfo_Player2.dir,
                                             CurrentTericsInfo_Player2.x,(CurrentTericsInfo_Player2.y+1)))
                    {
                        ++CurrentTericsInfo_Player2.y;
                        EraseGameRegionPlayer2(hdc);
                        DrawGameRegionPlayer2(hdc);
                        LoadCurrentTericsPlayer2(hdc);
                        DrawTericsPlayer2(hdc,CurrentTericsInfo_Player2.shape,CurrentTericsInfo_Player2.dir,
                                          CurrentTericsInfo_Player2.x,CurrentTericsInfo_Player2.y);
                    }
                    else
                    {
                        SaveStateOfTericsPlayer2(CurrentTericsInfo_Player2.shape,CurrentTericsInfo_Player2.dir,
                                                 CurrentTericsInfo_Player2.x,CurrentTericsInfo_Player2.y);
                        invalidatePlayer2=TRUE;
                        InvalidateRect(hwnd,&rect_Player2,FALSE);
                    }
                }
            }
            return 0;
        case VK_LEFT:
            if(Players==1)
            {
                if(Stop==FALSE)
                {
                    if(!IsOutOfRegion(CurrentTericsInfo.shape,CurrentTericsInfo.dir,
                                      (CurrentTericsInfo.x-1),CurrentTericsInfo.y))
                    {
                        --CurrentTericsInfo.x;
                        EraseGameRegion(hdc);
                        DrawGameRegion(hdc);
                        LoadCurrentTerics(hdc);
                        DrawTerics(hdc,CurrentTericsInfo.shape,CurrentTericsInfo.dir,
                                   CurrentTericsInfo.x,CurrentTericsInfo.y);
                    }
                }
            }
            else
            {
                if(Stop==FALSE)
                {
                    if(!IsOutOfRegionPlayer2(CurrentTericsInfo_Player2.shape,CurrentTericsInfo_Player2.dir,
                                             (CurrentTericsInfo_Player2.x-1),CurrentTericsInfo_Player2.y))
                    {
                        --CurrentTericsInfo_Player2.x;
                        EraseGameRegionPlayer2(hdc);
                        DrawGameRegionPlayer2(hdc);
                        LoadCurrentTericsPlayer2(hdc);
                        DrawTericsPlayer2(hdc,CurrentTericsInfo_Player2.shape,CurrentTericsInfo_Player2.dir,
                                          CurrentTericsInfo_Player2.x,CurrentTericsInfo_Player2.y);
                    }
                }
            }
            return 0;
        case VK_RIGHT:
            if(Players==1)
            {
                if(Stop==FALSE)
                {
                    if(!IsOutOfRegion(CurrentTericsInfo.shape,CurrentTericsInfo.dir,
                                      (CurrentTericsInfo.x+1),CurrentTericsInfo.y))
                    {
                        ++CurrentTericsInfo.x;
                        EraseGameRegion(hdc);
                        DrawGameRegion(hdc);
                        LoadCurrentTerics(hdc);
                        DrawTerics(hdc,CurrentTericsInfo.shape,CurrentTericsInfo.dir,
                                   CurrentTericsInfo.x,CurrentTericsInfo.y);
                    }
                }
            }
            else
            {
                if(Stop==FALSE)
                {
                    if(!IsOutOfRegionPlayer2(CurrentTericsInfo_Player2.shape,CurrentTericsInfo_Player2.dir,
                                             (CurrentTericsInfo_Player2.x+1),CurrentTericsInfo_Player2.y))
                    {
                        ++CurrentTericsInfo_Player2.x;
                        EraseGameRegionPlayer2(hdc);
                        DrawGameRegionPlayer2(hdc);
                        LoadCurrentTericsPlayer2(hdc);
                        DrawTericsPlayer2(hdc,CurrentTericsInfo_Player2.shape,CurrentTericsInfo_Player2.dir,
                                          CurrentTericsInfo_Player2.x,CurrentTericsInfo_Player2.y);
                    }
                }
            }
            return 0;
        case VK_SPACE:
            if(Stop==FALSE)
            {
                KillTimer(hwnd,ID_TIMER);
                Stop=TRUE;
            }
            else
            {
                SetTimer(hwnd,ID_TIMER,500,NULL);
                Stop=FALSE;
            }

        }
        ReleaseDC(hwnd,hdc);
        return 0;

//处理键盘消息
    case WM_CHAR:
        hdc=GetDC(hwnd);
        switch(wParam)
        {
        case 'w':
            if(Stop==FALSE)
            {
                if(!IsOutOfRegionPlayer1(CurrentTericsInfo_Player1.shape,(CurrentTericsInfo_Player1.dir+1)%4,
                                         CurrentTericsInfo_Player1.x,CurrentTericsInfo_Player1.y))
                {
                    CurrentTericsInfo_Player1.dir=++CurrentTericsInfo_Player1.dir%4;
                    EraseGameRegionPlayer1(hdc);
                    DrawGameRegionPlayer1(hdc);
                    LoadCurrentTericsPlayer1(hdc);
                    DrawTericsPlayer1(hdc,CurrentTericsInfo_Player1.shape,CurrentTericsInfo_Player1.dir,
                                      CurrentTericsInfo_Player1.x,CurrentTericsInfo_Player1.y);
                }
            }
            return 0;
        case 's':
            if(Stop==FALSE)
            {
                if(!IsOutOfRegionPlayer1(CurrentTericsInfo_Player1.shape,CurrentTericsInfo_Player1.dir,
                                         CurrentTericsInfo_Player1.x,(CurrentTericsInfo_Player1.y+1)))
                {
                    ++CurrentTericsInfo_Player1.y;
                    EraseGameRegionPlayer1(hdc);
                    DrawGameRegionPlayer1(hdc);
                    LoadCurrentTericsPlayer1(hdc);
                    DrawTericsPlayer1(hdc,CurrentTericsInfo_Player1.shape,CurrentTericsInfo_Player1.dir,
                                      CurrentTericsInfo_Player1.x,CurrentTericsInfo_Player1.y);
                }
                else
                {
                    SaveStateOfTericsPlayer1(CurrentTericsInfo_Player1.shape,CurrentTericsInfo_Player1.dir,
                                             CurrentTericsInfo_Player1.x,CurrentTericsInfo_Player1.y);
                    invalidatePlayer1=TRUE;
                    InvalidateRect(hwnd,&rect_Player1,FALSE);
                }
            }
            return 0;
        case 'a':
            if(Stop==FALSE)
            {
                if(!IsOutOfRegionPlayer1(CurrentTericsInfo_Player1.shape,CurrentTericsInfo_Player1.dir,
                                         (CurrentTericsInfo_Player1.x-1),CurrentTericsInfo_Player1.y))
                {
                    --CurrentTericsInfo_Player1.x;
                    EraseGameRegionPlayer1(hdc);
                    DrawGameRegionPlayer1(hdc);
                    LoadCurrentTericsPlayer1(hdc);
                    DrawTericsPlayer1(hdc,CurrentTericsInfo_Player1.shape,CurrentTericsInfo_Player1.dir,
                                      CurrentTericsInfo_Player1.x,CurrentTericsInfo_Player1.y);
                }
            }
            return 0;
        case 'd':
            if(Stop==FALSE)
            {
                if(!IsOutOfRegionPlayer1(CurrentTericsInfo_Player1.shape,CurrentTericsInfo_Player1.dir,
                                         (CurrentTericsInfo_Player1.x+1),CurrentTericsInfo_Player1.y))
                {
                    ++CurrentTericsInfo_Player1.x;
                    EraseGameRegionPlayer1(hdc);
                    DrawGameRegionPlayer1(hdc);
                    LoadCurrentTericsPlayer1(hdc);
                    DrawTericsPlayer1(hdc,CurrentTericsInfo_Player1.shape,CurrentTericsInfo_Player1.dir,
                                      CurrentTericsInfo_Player1.x,CurrentTericsInfo_Player1.y);
                }
            }
            return 0;
        }


//处理按钮的消息
    case WM_COMMAND:
        ID=LOWORD(wParam);
        if(!(IsGameOver(CurrentTericsInfo.shape,CurrentTericsInfo.dir,
                        CurrentTericsInfo.x,CurrentTericsInfo.y)))
        {
            if(ID==0)
            {
                Players=1;
                InvalidateRect(hwnd,NULL,FALSE);
                SetTimer(hwnd,ID_TIMER,speed,NULL);    //设置计时器
                for(i=0; i<2; i++)
                    DestroyWindow(hwndButton[i]);
            }
        }
        if(!(IsGameOver(CurrentTericsInfo.shape,CurrentTericsInfo.dir,
                        CurrentTericsInfo.x,CurrentTericsInfo.y)))
        {
            if(ID==1)
            {
                Players=2;
                InvalidateRect(hwnd,NULL,FALSE);
                SetTimer(hwnd,ID_TIMER,speed2,NULL);    //设置计时器
                for(i=0; i<2; i++)
                    DestroyWindow(hwndButton[i]);
            }
        }
        if(ID==2)
        {
            DestroyWindow(hwnd);
        }
        return 0;

//销毁窗口
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc (hwnd, message, wParam, lParam);
        return 0;
    }
}

//********************************************************************************************
void DrawGameRegion(HDC hdc)
{
    HBRUSH brush;
    int LeftX,LeftY,RightX,RightY;
    brush=CreateSolidBrush(RGB(0,0,0));
    SelectObject(hdc,brush);
    LeftX=GameSizeX;
    LeftY=GameSizeY;
    RightX=LeftX+GameRegionWidth*SquareSize;
    RightY=LeftY+GameRegionHeight*SquareSize;
    Rectangle(hdc,LeftX,LeftY,RightX,RightY);
    DeleteObject(brush);
}

void DrawGameRegionPlayer1(HDC hdc)
{
    HBRUSH brush;
    int LeftX,LeftY,RightX,RightY;
    brush=CreateSolidBrush(RGB(0,0,0));
    SelectObject(hdc,brush);
    LeftX=GameSizeX_Player1;
    LeftY=GameSizeY_Player1;
    RightX=LeftX+GameRegionWidth*SquareSize;
    RightY=LeftY+GameRegionHeight*SquareSize;
    Rectangle(hdc,LeftX,LeftY,RightX,RightY);
    DeleteObject(brush);
}

void DrawGameRegionPlayer2(HDC hdc)
{
    HBRUSH brush;
    int LeftX,LeftY,RightX,RightY;
    brush=CreateSolidBrush(RGB(0,0,0));
    SelectObject(hdc,brush);
    LeftX=GameSizeX_Player2;
    LeftY=GameSizeY_Player2;
    RightX=LeftX+GameRegionWidth*SquareSize;
    RightY=LeftY+GameRegionHeight*SquareSize;
    Rectangle(hdc,LeftX,LeftY,RightX,RightY);
    DeleteObject(brush);
}

void DrawRectangle(HDC hdc,int shape,int x,int y)  //x,y相对于绘制区的左上角坐标，下同
{
    HBRUSH brush;
    int leftx=GameSizeX+SquareSize*x;
    int lefty=GameSizeY+SquareSize*y;
    int rightx=leftx+SquareSize;
    int righty=lefty+SquareSize;
    brush=CreateSolidBrush(color[shape]);
    SelectObject(hdc,brush);
    Rectangle(hdc,leftx,lefty,rightx,righty);
    DeleteObject(brush);
}

void DrawRectanglePlayer1(HDC hdc,int shape,int x,int y)  //x,y相对于绘制区的左上角坐标，下同
{
    HBRUSH brush;
    int leftx=GameSizeX_Player1+SquareSize*x;
    int lefty=GameSizeY_Player1+SquareSize*y;
    int rightx=leftx+SquareSize;
    int righty=lefty+SquareSize;
    brush=CreateSolidBrush(color[shape]);
    SelectObject(hdc,brush);
    Rectangle(hdc,leftx,lefty,rightx,righty);
    DeleteObject(brush);
}

void DrawRectanglePlayer2(HDC hdc,int shape,int x,int y)  //x,y相对于绘制区的左上角坐标，下同
{
    HBRUSH brush;
    int leftx=GameSizeX_Player2+SquareSize*x;
    int lefty=GameSizeY_Player2+SquareSize*y;
    int rightx=leftx+SquareSize;
    int righty=lefty+SquareSize;
    brush=CreateSolidBrush(color[shape]);
    SelectObject(hdc,brush);
    Rectangle(hdc,leftx,lefty,rightx,righty);
    DeleteObject(brush);
}

void DrawTerics(HDC hdc,int shape,int dir,int x,int y)
{
    int index;
    for(index=0; index<4; index++)
    {
        nx[index]=Terics[shape][dir][index].x+x;
        ny[index]=Terics[shape][dir][index].y+y;
        DrawRectangle(hdc,shape,nx[index],ny[index]);
    }
}

void DrawTericsPlayer1(HDC hdc,int shape,int dir,int x,int y)
{
    int index;
    for(index=0; index<4; index++)
    {
        nx_Player1[index]=Terics[shape][dir][index].x+x;
        ny_Player1[index]=Terics[shape][dir][index].y+y;
        DrawRectanglePlayer1(hdc,shape,nx_Player1[index],ny_Player1[index]);
    }
}

void DrawTericsPlayer2(HDC hdc,int shape,int dir,int x,int y)
{
    int index;
    for(index=0; index<4; index++)
    {
        nx_Player2[index]=Terics[shape][dir][index].x+x;
        ny_Player2[index]=Terics[shape][dir][index].y+y;
        DrawRectanglePlayer2(hdc,shape,nx_Player2[index],ny_Player2[index]);
    }
}

BOOL IsOutOfRegion(int shape,int dir,int x,int y)
{
    int index;
    int nx,ny;
    for(index=0; index<4; index++)
    {
        nx=Terics[shape][dir][index].x+x;
        ny=Terics[shape][dir][index].y+y;
        if(nx<0||nx>=GameRegionWidth||ny<0||ny>=GameRegionHeight||GameMapStatesInfo[nx][ny].states!=0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL IsOutOfRegionPlayer1(int shape,int dir,int x,int y)
{
    int index;
    int nx,ny;
    for(index=0; index<4; index++)
    {
        nx=Terics[shape][dir][index].x+x;
        ny=Terics[shape][dir][index].y+y;
        if(nx<0||nx>=GameRegionWidth||ny<0||ny>=GameRegionHeight||GameMapStatesInfo_Player1[nx][ny].states!=0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL IsOutOfRegionPlayer2(int shape,int dir,int x,int y)
{
    int index;
    int nx,ny;
    for(index=0; index<4; index++)
    {
        nx=Terics[shape][dir][index].x+x;
        ny=Terics[shape][dir][index].y+y;
        if(nx<0||nx>=GameRegionWidth||ny<0||ny>=GameRegionHeight||GameMapStatesInfo_Player2[nx][ny].states!=0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

void InitTericsInfo()
{
    srand((unsigned)time(NULL));
    CurrentTericsInfo.shape=szshape;
    CurrentTericsInfo.dir=0;
    CurrentTericsInfo.x=4;
    CurrentTericsInfo.y=0;
    szshape=rand()%7;
}

void InitTericsInfoPlayer1()
{
    srand((unsigned)time(NULL));
    CurrentTericsInfo_Player1.shape=szshape_Player1;
    CurrentTericsInfo_Player1.dir=0;
    CurrentTericsInfo_Player1.x=4;
    CurrentTericsInfo_Player1.y=0;
    szshape_Player1=rand()%7;
}

void InitTericsInfoPlayer2()
{
    srand((unsigned)time(NULL));
    CurrentTericsInfo_Player2.shape=szshape_Player2;
    CurrentTericsInfo_Player2.dir=0;
    CurrentTericsInfo_Player2.x=4;
    CurrentTericsInfo_Player2.y=0;
    szshape_Player2=(rand()+2)%7;
}

void EraseGameRegion(HDC hdc)
{
    RECT rect;
    HBRUSH brush;
    rect.left=GameSizeX;
    rect.top=GameSizeY;
    rect.right=GameSizeX+GameRegionWidth*SquareSize;
    rect.bottom=GameSizeY+GameRegionHeight*SquareSize;
    brush=CreateSolidBrush(RGB(255,255,255));
    FillRect(hdc,&rect,brush);
    DeleteObject(brush);
}

void EraseGameRegionPlayer1(HDC hdc)
{
    RECT rect;
    HBRUSH brush;
    rect.left=GameSizeX_Player1;
    rect.top=GameSizeY_Player1;
    rect.right=GameSizeX_Player1+GameRegionWidth*SquareSize;
    rect.bottom=GameSizeY_Player1+GameRegionHeight*SquareSize;
    brush=CreateSolidBrush(RGB(255,255,255));
    FillRect(hdc,&rect,brush);
    DeleteObject(brush);
}

void EraseGameRegionPlayer2(HDC hdc)
{
    RECT rect;
    HBRUSH brush;
    rect.left=GameSizeX_Player2;
    rect.top=GameSizeY_Player2;
    rect.right=GameSizeX_Player2+GameRegionWidth*SquareSize;
    rect.bottom=GameSizeY_Player2+GameRegionHeight*SquareSize;
    brush=CreateSolidBrush(RGB(255,255,255));
    FillRect(hdc,&rect,brush);
    DeleteObject(brush);
}

//可以消行
void SaveStateOfTerics(int shape, int dir, int x, int y)
{
    int index;
    int nx,ny;
    int indexX,indexY;
    BOOL flag;
    int m,n;
    for(index=0; index<4; ++index)
    {
        nx=Terics[shape][dir][index].x+x;
        ny=Terics[shape][dir][index].y+y;
        GameMapStatesInfo[nx][ny].states=1;
        GameMapStatesInfo[nx][ny].shapes=shape;
    }
    for(indexX=0; indexX<GameRegionHeight; indexX++)
    {
        flag=TRUE;
        for(indexY=0; indexY<GameRegionWidth; indexY++)
        {
            if(GameMapStatesInfo[indexY][indexX].states!=1)
            {
                flag=FALSE;
            }
        }
        if(flag)
        {
            for(m=indexX; m>=1; --m)
            {
                for(n=0; n<GameRegionWidth; ++n)
                {
                    GameMapStatesInfo[n][m].states=GameMapStatesInfo[n][m-1].states;
                    GameMapStatesInfo[n][m].shapes=GameMapStatesInfo[n][m-1].shapes;
                }
            }
            ++count;
        }
    }
}

void SaveStateOfTericsPlayer1(int shape, int dir, int x, int y)
{
    int index;
    int nx,ny;
    int indexX,indexY;
    BOOL flag;
    int m,n;
    for(index=0; index<4; ++index)
    {
        nx=Terics[shape][dir][index].x+x;
        ny=Terics[shape][dir][index].y+y;
        GameMapStatesInfo_Player1[nx][ny].states=1;
        GameMapStatesInfo_Player1[nx][ny].shapes=shape;
    }
    for(indexX=0; indexX<GameRegionHeight; indexX++)
    {
        flag=TRUE;
        for(indexY=0; indexY<GameRegionWidth; indexY++)
        {
            if(GameMapStatesInfo_Player1[indexY][indexX].states!=1)
            {
                flag=FALSE;
            }
        }
        if(flag)
        {
            for(m=indexX; m>=1; --m)
            {
                for(n=0; n<GameRegionWidth; ++n)
                {
                    GameMapStatesInfo_Player1[n][m].states=GameMapStatesInfo_Player1[n][m-1].states;
                    GameMapStatesInfo_Player1[n][m].shapes=GameMapStatesInfo_Player1[n][m-1].shapes;
                }
            }
        }
    }
}

void SaveStateOfTericsPlayer2(int shape, int dir, int x, int y)
{
    int index;
    int nx,ny;
    int indexX,indexY;
    BOOL flag;
    int m,n;
    for(index=0; index<4; ++index)
    {
        nx=Terics[shape][dir][index].x+x;
        ny=Terics[shape][dir][index].y+y;
        GameMapStatesInfo_Player2[nx][ny].states=1;
        GameMapStatesInfo_Player2[nx][ny].shapes=shape;
    }
    for(indexX=0; indexX<GameRegionHeight; indexX++)
    {
        flag=TRUE;
        for(indexY=0; indexY<GameRegionWidth; indexY++)
        {
            if(GameMapStatesInfo_Player2[indexY][indexX].states!=1)
            {
                flag=FALSE;
            }
        }
        if(flag)
        {
            for(m=indexX; m>=1; --m)
            {
                for(n=0; n<GameRegionWidth; ++n)
                {
                    GameMapStatesInfo_Player2[n][m].states=GameMapStatesInfo_Player2[n][m-1].states;
                    GameMapStatesInfo_Player2[n][m].shapes=GameMapStatesInfo_Player2[n][m-1].shapes;
                }
            }
        }
    }
}

//绘制当前的状态
void LoadCurrentTerics(HDC hdc)
{
    int indexX,indexY;
    for(indexX=0; indexX<GameRegionWidth; indexX++)
    {
        for(indexY=0; indexY<GameRegionHeight; indexY++)
        {
            if(GameMapStatesInfo[indexX][indexY].states==1)
            {
                DrawRectangle(hdc,GameMapStatesInfo[indexX][indexY].shapes,indexX,indexY);
            }
        }
    }
}

void LoadCurrentTericsPlayer1(HDC hdc)
{
    int indexX,indexY;
    for(indexX=0; indexX<GameRegionWidth; indexX++)
    {
        for(indexY=0; indexY<GameRegionHeight; indexY++)
        {
            if(GameMapStatesInfo_Player1[indexX][indexY].states==1)
            {
                DrawRectanglePlayer1(hdc,GameMapStatesInfo_Player1[indexX][indexY].shapes,indexX,indexY);
            }
        }
    }
}

void LoadCurrentTericsPlayer2(HDC hdc)
{
    int indexX,indexY;
    for(indexX=0; indexX<GameRegionWidth; indexX++)
    {
        for(indexY=0; indexY<GameRegionHeight; indexY++)
        {
            if(GameMapStatesInfo_Player2[indexX][indexY].states==1)
            {
                DrawRectanglePlayer2(hdc,GameMapStatesInfo_Player2[indexX][indexY].shapes,indexX,indexY);
            }
        }
    }
}

void PrintInfo(HDC hdc,int shape)
{
    char szscore[200]="Score : ";
    char strscore[20];
    score=count*10;
    itoa(score,strscore,10);
    strcat(szscore,strscore);
    SetBkColor(hdc,RGB(165,180,255));
    TextOut(hdc,600,100,szscore,strlen(szscore));
    level=score/20;
    strcpy(szscore,"Level : ");
    itoa(level,strscore,10);
    strcat(szscore,strscore);
    SetBkColor(hdc,RGB(165,180,255));
    TextOut(hdc,600,125,szscore,strlen(szscore));
    DrawTerics(hdc,shape,0,15,5);
}

void PrintInfoPlayer1(HDC hdc,int shape)
{
    char text[20]="Player1";
    SetBkColor(hdc,RGB(165,180,255));
    TextOut(hdc,350,100,text,strlen(text));
    DrawTericsPlayer1(hdc,shape,0,13,5);
}

void PrintInfoPlayer2(HDC hdc,int shape)
{
    char text[20]="Player2";
    SetBkColor(hdc,RGB(165,180,255));
    TextOut(hdc,850,100,text,strlen(text));
    DrawTericsPlayer2(hdc,shape,0,13,5);
}

BOOL IsGameOver(int shape,int dir,int x,int y)
{
    int index;
    int nx,ny;
    for(index=0; index<4; index++)
    {
        nx=Terics[shape][dir][index].x+x;
        ny=Terics[shape][dir][index].y+y;
        if(ny==0 && GameMapStatesInfo[nx][ny].states==1)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL IsGameOverPlayer1(int shape,int dir,int x,int y)
{
    int index;
    int nx,ny;
    for(index=0; index<4; index++)
    {
        nx=Terics[shape][dir][index].x+x;
        ny=Terics[shape][dir][index].y+y;
        if(ny==0 && GameMapStatesInfo_Player1[nx][ny].states==1)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL IsGameOverPlayer2(int shape,int dir,int x,int y)
{
    int index;
    int nx,ny;
    for(index=0; index<4; index++)
    {
        nx=Terics[shape][dir][index].x+x;
        ny=Terics[shape][dir][index].y+y;
        if(ny==0 && GameMapStatesInfo_Player2[nx][ny].states==1)
        {
            return TRUE;
        }
    }
    return FALSE;
}
