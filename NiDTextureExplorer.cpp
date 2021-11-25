// NiDTextureExplorer.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "FileProcessor.h"
#include "NiDTextureExplorer.h"
#include <stdio.h>


#define OPEN_ID 123
#define SAVE_ID 124
#define LOAD_ID 125
#define OFFSET_ID 126
#define PALETTE_ID 127
#define ADVANCE_ID 128
#define NIBBLE_ID 129
#define BYTE_ID 130
#define PREV_PALETTE_ID 131
#define NEXT_PALETTE_ID 132

#define MAX_LOADSTRING 100


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void intToHexString(int value, wchar_t* hexCharArray);
int hexStringToInt(wchar_t hexStr[], int strlen);
void recalcGrid(FileProcessor* fp, int mode, int width, int height, int offset, int paletteOffset);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_NIDTEXTUREEXPLORER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NIDTEXTUREEXPLORER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NIDTEXTUREEXPLORER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_NIDTEXTUREEXPLORER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

PWSTR SaveFile() {
    PWSTR pszFilePath = NULL;
    HRESULT hr = CoInitializeEx(NULL, COINITBASE_MULTITHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileSaveDialog* pFileSave;
        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
            IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));
        if (SUCCEEDED(hr))
        {
            // Set default extension
            hr = pFileSave->SetDefaultExtension(L"bmp");
            if (SUCCEEDED(hr))
            {
                // Show the Open dialog box.
                hr = pFileSave->Show(NULL);

                // Get the file name from the dialog box.
                if (SUCCEEDED(hr))
                {
                    IShellItem* pItem;
                    hr = pFileSave->GetResult(&pItem);
                    if (SUCCEEDED(hr))
                    {
                        
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                        
                        pItem->Release();
                    }
                }
                pFileSave->Release();
            }
        }
        CoUninitialize();
    }
    return pszFilePath;
}

PWSTR LoadFile() {
    IFileOpenDialog* pFileOpen;
    PWSTR pszFilePath = NULL;

    // Create the FileOpenDialog object.
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
        IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
    if (SUCCEEDED(hr))
    {
        hr = pFileOpen->Show(NULL);

        // Get the file name from the dialog box.
        if (SUCCEEDED(hr))
        {
            IShellItem* pItem;
            hr = pFileOpen->GetResult(&pItem);
            if (SUCCEEDED(hr))
            {
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                pItem->Release();
            }
        }
        pFileOpen->Release();
    }
    return pszFilePath;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
HWND openButton;
HWND saveButton;
HWND loadImgButton;
HWND widthSlider;
HWND heightSlider;
HWND fileNameEntry;
HWND widthText;
HWND heightText;
HWND offsetEntry;
HWND paletteEntry;
HWND offsetWarning;
HWND paletteWarning;
HWND offsetNextButton;
HWND fourBit;
HWND eightBit;
HWND prevPalette;
HWND nextPalette;
HWND offsetMove1;
HWND offsetMove4;
HWND offsetMove16;
RECT gridCoords{};
FileProcessor* fp = NULL;

bool moveSliders = false;

RECT getGridCoords(HWND hWnd) {
    RECT windowDims{};
    GetClientRect(hWnd, &windowDims);
    windowDims.left = 250;
    return windowDims;
}

void drawGrid(HDC hdc, HWND hWnd, int width, int height, unsigned char* colorArray, int mode, unsigned char* palette) {
    
    double gridRatio = (double)width / (double)height;
    int padding = 32;
    RECT windowDims{ 0,0,0,0 };
    GetClientRect(hWnd, &windowDims);
    int winWidth = windowDims.right;
    int winHeight = windowDims.bottom;
    int leftPadding = 250;
    double winRatio = 0.0;
    RECT toolBar{ 0,0,leftPadding,winHeight };
    RECT toolBar2{ winWidth - leftPadding, 0, winWidth,winHeight };
    HBRUSH fillBrush = CreateSolidBrush(RGB(127, 127, 127));
    FillRect(hdc, &toolBar, fillBrush);
    FillRect(hdc, &toolBar2, fillBrush);
    DeleteObject(fillBrush);
    winRatio = (double)(winWidth - padding * 2 - leftPadding * 2) / (double)(winHeight);
    if (moveSliders) {
        SetWindowPos(paletteWarning, HWND_TOP, winWidth - 200, 25, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
        SetWindowPos(paletteEntry, HWND_TOP, winWidth - 150, 50, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
        SetWindowPos(prevPalette, HWND_TOP, winWidth - 150, 100, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
        SetWindowPos(nextPalette, HWND_TOP, winWidth - 100, 100, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
    }
    wchar_t messageString[100];
    swprintf(messageString, 100, L"Width: %d pixels", width);
    SetWindowText(widthText, messageString);
    swprintf(messageString, 100, L"Height: %d pixels", height);
    SetWindowText(heightText, messageString);

    int gridSquareSize = 8 * 250 / 10 / mode;
    int gridStartLeft = winWidth - 9 * 250 / 10;
    int gridStartTop = 300;

    if (palette != NULL) {
        for (int i = 0; i < mode; i++) {
            for (int j = 0; j < mode; j++) {
                RECT newRect{ gridStartLeft + j * gridSquareSize,
                gridStartTop + i * gridSquareSize,
                gridStartLeft + (j + 1) * gridSquareSize,
                gridStartTop + (i + 1) * gridSquareSize };
                if (colorArray != NULL) {
                    unsigned char* color = (palette + (i * mode * 3) + j * 3);
                    HBRUSH fillBrush = CreateSolidBrush(RGB(color[2], color[1], color[0]));
                    FillRect(hdc, &newRect, fillBrush);
                    DeleteObject(fillBrush);
                }
            }
        }
    }

    if (winRatio > gridRatio) {
        gridSquareSize = (winHeight - padding * 2) / height;
        gridStartLeft = winWidth / 2 - width * gridSquareSize /2 ;
        gridStartTop = padding;
    }
    else {
        gridSquareSize = (winWidth - padding * 2 - leftPadding * 2) / width;
        gridStartLeft = padding + leftPadding;
        gridStartTop = winHeight / 2 - height * gridSquareSize / 2;
    }
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            RECT newRect{ gridStartLeft + j * gridSquareSize,
            gridStartTop + i * gridSquareSize,
            gridStartLeft + (j + 1) * gridSquareSize,
            gridStartTop + (i + 1) * gridSquareSize };
            if (colorArray != NULL) {
                unsigned char* color = (colorArray + (i * width + j) * 3);
                HBRUSH fillBrush = CreateSolidBrush(RGB(color[2], color[1], color[0]));
                FillRect(hdc, &newRect, fillBrush);
                DeleteObject(fillBrush);
            }
        }
    }
}
int width = 16;
int height = 16;
int mode = FOUR_BIT;
bool warnings[] = { SW_HIDE,SW_HIDE };
int offset = 0;
int paletteOffset = 0;
int paletteIndex = 0;
wchar_t savePath[100] = L"";
unsigned char* colorArray = NULL;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    
    HDC hdc = NULL;
    switch (message)
    {
    case WM_CREATE: 
    {
        PAINTSTRUCT ps;
        hdc = BeginPaint(hWnd, &ps);
        openButton = CreateWindow(
            L"BUTTON", L"OPEN FILE", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            25, 25, 150, 50, hWnd, (HMENU)OPEN_ID, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        saveButton = CreateWindow(
            L"BUTTON", L"SAVE FILE", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            25, 75, 150, 50, hWnd, (HMENU)SAVE_ID, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        loadImgButton = CreateWindow(
            L"BUTTON", L"LOAD IMAGE", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            25, 125, 150, 50, hWnd, (HMENU)LOAD_ID, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        widthSlider = CreateWindowEx(
            0, TRACKBAR_CLASS, L"Trackbar Control", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
            25, 200, 200, 30, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        heightSlider = CreateWindowEx(
            0, TRACKBAR_CLASS, L"Trackbar Control", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
            25, 300, 200, 30, hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        SendMessage(widthSlider, TBM_SETRANGE,
            (WPARAM)TRUE,                   // redraw flag 
            (LPARAM)MAKELONG(1, 24));
        SendMessage(heightSlider, TBM_SETRANGE,
            (WPARAM)TRUE,                   // redraw flag 
            (LPARAM)MAKELONG(1, 24));
        offsetEntry = CreateWindow(
            L"EDIT", L"0", WS_TABSTOP | WS_VISIBLE | WS_CHILD, 25, 400, 100, 20, hWnd, (HMENU)OFFSET_ID,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        paletteEntry = CreateWindow(
            L"EDIT", L"0", WS_TABSTOP | WS_VISIBLE | WS_CHILD, 25, 500, 100, 20, hWnd, (HMENU)PALETTE_ID,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        widthText = CreateWindow(
            L"STATIC", L"OK", WS_TABSTOP | WS_VISIBLE | WS_CHILD, 25, 175, 150, 20, hWnd,
            NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        heightText = CreateWindow(
            L"STATIC", L"OK", WS_TABSTOP | WS_VISIBLE | WS_CHILD, 25, 275, 150, 20, hWnd,
            NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        offsetWarning = CreateWindow(
            L"STATIC", L"Offset (hex)", WS_TABSTOP | WS_VISIBLE | WS_CHILD, 25, 375, 150, 20, hWnd,
            NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        paletteWarning = CreateWindow(
            L"STATIC", L"Palette (hex)", WS_TABSTOP | WS_VISIBLE | WS_CHILD, 25, 475, 150, 20, hWnd,
            NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        offsetNextButton = CreateWindow(
            L"BUTTON", L"Advance Offset", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            25, 425, 100, 50, hWnd, (HMENU)ADVANCE_ID, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        fourBit = CreateWindow(
            L"BUTTON", L"4-bit", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            25, 575, 50, 50, hWnd, (HMENU)NIBBLE_ID, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        eightBit = CreateWindow(
            L"BUTTON", L"8-bit", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            75, 575, 50, 50, hWnd, (HMENU)BYTE_ID, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        prevPalette = CreateWindow(
            L"BUTTON", L"Prev", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            25, 525, 50, 50, hWnd, (HMENU)PREV_PALETTE_ID, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        nextPalette = CreateWindow(
            L"BUTTON", L"Next", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            75, 525, 50, 50, hWnd, (HMENU)NEXT_PALETTE_ID, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        EndPaint(hWnd, &ps); 

    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case OPEN_ID: 
            {
                PWSTR filePath = LoadFile();
                InvalidateRect(hWnd, NULL, true);
                if (filePath != NULL) {
                    fp = new FileProcessor(filePath);
                    wcscpy_s(savePath, filePath);
                    fp->FindPaletteCandidates();
                    paletteIndex = 0;
                    wchar_t paletteText[9];
                    int paletteTextOffset = 0;
                    intToHexString(fp->getPalette(paletteIndex), paletteText);
                    for (int i = 0; i < 8; i++) {
                        if (paletteText[i] != L'0') {
                            paletteTextOffset = i;
                            break;
                        }
                    }
                    SetDlgItemTextW(hWnd, PALETTE_ID, paletteText + paletteTextOffset);
                }
               
            }
            break;
            case SAVE_ID:
            {
                PWSTR filePath = SaveFile();
                if (filePath != NULL) {
                    recalcGrid(fp, mode, width, height, offset, paletteOffset);
                    if (fp != NULL) {
                        fp->writeImgFile(filePath);
                    }
                    wcscpy_s(savePath, filePath);
                }
            }
            break;
            case LOAD_ID:
            {
                if (fp != NULL) {
                    int temp = 0;
                    wchar_t messageString[100];
                    GetDlgItemTextW(hWnd, OFFSET_ID, messageString, 100);
                    temp = hexStringToInt(messageString, _tcslen(messageString));
                    if (temp != -1) {
                        offset = temp;
                        warnings[0] = SW_HIDE;
                    }
                    else {
                        warnings[0] = SW_NORMAL;
                    }
                    GetDlgItemTextW(hWnd, PALETTE_ID, messageString, 100);
                    temp = hexStringToInt(messageString, _tcslen(messageString));
                    if (temp != -1) {
                        paletteOffset = temp;
                        warnings[1] = SW_HIDE;
                    }
                    else {
                        warnings[1] = SW_NORMAL;
                    }
                    recalcGrid(fp, mode, width, height, offset, paletteOffset);
                    InvalidateRect(hWnd, &gridCoords, true);
                }
            }
            break;
            case ADVANCE_ID:
            {
                if (fp != NULL) {
                    offset += width * height * (mode / 8.0);
                    wchar_t offsetText[9];
                    int offsetTextOffset = 0;
                    intToHexString(offset, offsetText);
                    for (int i = 0; i < 8; i++) {
                        if (offsetText[i] != L'0') {
                            offsetTextOffset = i;
                            break;
                        }
                    }
                    SetDlgItemTextW(hWnd, OFFSET_ID, offsetText + offsetTextOffset);
                    recalcGrid(fp, mode, width, height, offset, paletteOffset);
                    RECT fixText{};
                    GetWindowRect(offsetEntry, &fixText);
                    InvalidateRect(hWnd, &fixText, true);
                    InvalidateRect(hWnd, &gridCoords, true);
                }
                break;
            }
            case NIBBLE_ID:
            {
                mode = FOUR_BIT;
                recalcGrid(fp, mode, width, height, offset, paletteOffset);
                if (fp != NULL) {
                    fp->FindPaletteCandidates();
                }
                InvalidateRect(hWnd, &gridCoords, true);
                break;
            }
            case BYTE_ID:
            {
                mode = EIGHT_BIT;
                recalcGrid(fp, mode, width, height, offset, paletteOffset);
                if (fp != NULL) {
                    fp->FindPaletteCandidates();
                }
                InvalidateRect(hWnd, &gridCoords, true);
                break;
            }
            case PREV_PALETTE_ID:
            {
                paletteIndex += 1;
                wchar_t paletteText[9];
                int paletteTextOffset = 0;
                wchar_t messageString[100];
                GetDlgItemTextW(hWnd, PALETTE_ID, messageString, 100);
                paletteOffset = fp->getPrevPalette(hexStringToInt(messageString, _tcslen(messageString)));
                intToHexString(paletteOffset, paletteText);
                for (int i = 0; i < 8; i++) {
                    if (paletteText[i] != L'0') {
                        paletteTextOffset = i;
                        break;
                    }
                }
                SetDlgItemTextW(hWnd, PALETTE_ID, paletteText + paletteTextOffset);
                recalcGrid(fp, mode, width, height, offset, paletteOffset);
                RECT fixText{};
                GetWindowRect(paletteEntry, &fixText);
                InvalidateRect(hWnd, &fixText, true);
                InvalidateRect(hWnd, &gridCoords, true);
                break;
            }
            case NEXT_PALETTE_ID:
            {
                paletteIndex += 1;
                wchar_t paletteText[9];
                int paletteTextOffset = 0;
                wchar_t messageString[100];
                GetDlgItemTextW(hWnd, PALETTE_ID, messageString, 100);
                paletteOffset = fp->getNextPalette(hexStringToInt(messageString,_tcslen(messageString)));
                intToHexString(paletteOffset, paletteText);
                for (int i = 0; i < 8; i++) {
                    if (paletteText[i] != L'0') {
                        paletteTextOffset = i;
                        break;
                    }
                }
                SetDlgItemTextW(hWnd, PALETTE_ID, paletteText + paletteTextOffset);
                recalcGrid(fp, mode, width, height, offset, paletteOffset);
                RECT fixText{};
                GetWindowRect(paletteEntry, &fixText);
                InvalidateRect(hWnd, &fixText, true);
                InvalidateRect(hWnd, &gridCoords, true);
                break;
            }
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            hdc = BeginPaint(hWnd, &ps);
            unsigned char* palette = NULL;
            if (fp != NULL) {
                palette = fp->getPaletteArray();
            }
            drawGrid(hdc, hWnd, width, height, colorArray, mode, palette);
            
            //ShowWindow(offsetWarning, warnings[0]);
            //ShowWindow(paletteWarning, warnings[1]);
            ReleaseDC(hWnd, hdc);
            EndPaint(hWnd, &ps);
            moveSliders = false;
        }
        break;
    case WM_HSCROLL:
    {
        int wmId = lParam;
        bool regenPixelArray = false;
        int newVal = HIWORD(wParam);
        if ((HWND)lParam == widthSlider) {
            if (LOWORD(wParam) == TB_THUMBPOSITION || LOWORD(wParam) == TB_THUMBTRACK) {
                recalcGrid(fp, mode, width, height, offset, paletteOffset);
                width = newVal * 4;
                InvalidateRect(hWnd, &gridCoords, true);
            }
            else if (LOWORD(wParam) == SB_LINELEFT) {
                if (width > 4) {
                    width -= 4;
                    RECT fixText{};
                    GetWindowRect(widthText, &fixText);
                    InvalidateRect(hWnd, &fixText, true);
                    InvalidateRect(hWnd, &gridCoords, true);
                }
            }
            else if (LOWORD(wParam) == SB_LINERIGHT) {
                if (width < 4*24) {
                    width += 4;
                    RECT fixText{};
                    GetWindowRect(widthText, &fixText);
                    recalcGrid(fp, mode, width, height, offset, paletteOffset);
                    InvalidateRect(hWnd, &fixText, true);
                    InvalidateRect(hWnd, &gridCoords, true);
                }
            }
        }
        if ((HWND)lParam == heightSlider) {
            if (LOWORD(wParam) == TB_THUMBPOSITION || LOWORD(wParam) == TB_THUMBTRACK) {
                recalcGrid(fp, mode, width, height, offset, paletteOffset);
                height = newVal * 4;
                InvalidateRect(hWnd, &gridCoords, true);
            }
            else if (LOWORD(wParam) == SB_LINELEFT) {
                if (height > 4) {
                    height -= 4;
                    RECT fixText{};
                    GetWindowRect(heightText, &fixText);
                    InvalidateRect(hWnd, &fixText, true);
                    InvalidateRect(hWnd, &gridCoords, true);
                }
            }
            else if (LOWORD(wParam) == SB_LINERIGHT) {
                if (height < 4 * 24) {
                    height += 4;
                    RECT fixText{};
                    GetWindowRect(heightText, &fixText);
                    recalcGrid(fp, mode, width, height, offset, paletteOffset);
                    InvalidateRect(hWnd, &fixText, true);
                    InvalidateRect(hWnd, &gridCoords, true);
                }
            }
        }
        
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SIZE:
    {
        moveSliders = true;
        RECT updateRect{ 0,0,200,200 };
        gridCoords = getGridCoords(hWnd);
        InvalidateRect(hWnd, &updateRect, true);
    }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void recalcGrid(FileProcessor* fp, int mode, int width, int height, int offset, int paletteOffset) {
    if (fp != NULL) {
        fp->setDims(width, height, mode);
        fp->setupImage(offset, paletteOffset);
        fp->genPixelArray();
        colorArray = fp->getPixelArray();
    }
}

void intToHexString(int value, wchar_t* hexCharArray) {
    wchar_t hexChar = L'\0';
    for (int i = 0; i < 8; i++) {
        switch (value % 16) {
        case 0:
            hexChar = L'0';
            break;
        case 1:
            hexChar = L'1';
            break;
        case 2:
            hexChar = L'2';
            break;
        case 3:
            hexChar = L'3';
            break;
        case 4:
            hexChar = L'4';
            break;
        case 5:
            hexChar = L'5';
            break;
        case 6:
            hexChar = L'6';
            break;
        case 7:
            hexChar = L'7';
            break;
        case 8:
            hexChar = L'8';
            break;
        case 9:
            hexChar = L'9';
            break;
        case 10:
            hexChar = L'a';
            break;
        case 11:
            hexChar = L'b';
            break;
        case 12:
            hexChar = L'c';
            break;
        case 13:
            hexChar = L'd';
            break;
        case 14:
            hexChar = L'e';
            break;
        case 15:
            hexChar = L'f';
            break;
        }
        hexCharArray[7 - i] = hexChar;
        value /= 16;
    }
    hexCharArray[8] = L'\0';
}


int hexStringToInt(wchar_t hexStr[], int strlen) {
    int value = 0;
    int tempVal = 0;
    for (int i = 0; i < strlen; i++) {
        switch (hexStr[strlen - 1 - i]) {
        case L'0':
            tempVal = 0;
            break;
        case L'1':
            tempVal = 1;
            break;
        case L'2':
            tempVal = 2;
            break;
        case L'3':
            tempVal = 3;
            break;
        case L'4':
            tempVal = 4;
            break;
        case L'5':
            tempVal = 5;
            break;
        case L'6':
            tempVal = 6;
            break;
        case L'7':
            tempVal = 7;
            break;
        case L'8':
            tempVal = 8;
            break;
        case L'9':
            tempVal = 9;
            break;
        case L'A':
        case L'a':
            tempVal = 10;
            break;
        case L'B':
        case L'b':
            tempVal = 11;
            break;
        case L'C':
        case L'c':
            tempVal = 12;
            break;
        case L'D':
        case L'd':
            tempVal = 13;
            break;
        case L'E':
        case L'e':
            tempVal = 14;
            break;
        case L'F':
        case L'f':
            tempVal = 15;
            break;
        default:
            value = -1;
            break;
        }
        if (value == -1) {
            break;
        }
        value += tempVal * pow(16, i);
    }
    return value;
}