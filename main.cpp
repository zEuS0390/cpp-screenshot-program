#include <iostream>
#include <windows.h>
#include <sstream>

// Function to capture the screen and save it as a BMP file
void captureScreen(std::string path)
{
    // Convert the string path to a wide string for Windows API compatibility
    std::wstring wide_string_path = std::wstring(path.begin(), path.end());
    const wchar_t* wPath = wide_string_path.c_str();

    // Structures for bitmap file header, info header, and bitmap info
    BITMAPFILEHEADER bfHeader;
    BITMAPINFOHEADER biHeader;
    BITMAPINFO bInfo;
    HGDIOBJ hTempBitmap;
    HBITMAP hBitmap;
    BITMAP bAllDesktops;
    HDC hDC;
    HDC hMemDC;
    LONG lWidth;
    LONG lHeight;
    BYTE *bBits = NULL; // Pointer for bitmap bits
    HANDLE hHeap = GetProcessHeap(); // Handle to the process heap
    DWORD cbBits; // Size of the bitmap bits
    DWORD dwWritten = 0; // Number of bytes written to file
    HANDLE hFile; // Handle for the output file
    INT x = GetSystemMetrics(SM_XVIRTUALSCREEN); // X coordinate of the virtual screen
    INT y = GetSystemMetrics(SM_YVIRTUALSCREEN); // Y coordinate of the virtual screen

    // Initialize bitmap headers to zero
    ZeroMemory(&bfHeader, sizeof(BITMAPFILEHEADER));
    ZeroMemory(&biHeader, sizeof(BITMAPINFOHEADER));
    ZeroMemory(&bInfo, sizeof(BITMAPINFO));
    ZeroMemory(&bAllDesktops, sizeof(BITMAP));

    // Get the device context for the entire screen
    hDC = GetDC(NULL);
    hTempBitmap = GetCurrentObject(hDC, OBJ_BITMAP); // Get the current bitmap
    GetObjectW(hTempBitmap, sizeof(BITMAP), &bAllDesktops); // Get information about the bitmap

    // Store the width and height of the bitmap
    lWidth = bAllDesktops.bmWidth;
    lHeight = bAllDesktops.bmHeight;

    // Clean up the temporary bitmap object
    DeleteObject(hTempBitmap);

    // Set up the bitmap file header
    bfHeader.bfType = (WORD)('B' | ('M' << 8)); // BMP magic number
    bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); // Offset to pixel data

    // Set up the bitmap info header
    biHeader.biSize = sizeof(BITMAPINFOHEADER);
    biHeader.biBitCount = 24; // 24 bits per pixel
    biHeader.biCompression = BI_RGB; // No compression
    biHeader.biPlanes = 1; // Must be 1
    biHeader.biWidth = lWidth; // Width of the bitmap
    biHeader.biHeight = lHeight; // Height of the bitmap

    bInfo.bmiHeader = biHeader; // Link the info header to the bitmap info structure

    // Calculate the size of the bitmap bits
    cbBits = (((24 * lWidth + 31) & ~31) / 8) * lHeight;

    // Create a compatible DC and bitmap for the screenshot
    hMemDC = CreateCompatibleDC(hDC);
    hBitmap = CreateDIBSection(hDC, &bInfo, DIB_RGB_COLORS, (VOID **)&bBits, NULL, 0);
    SelectObject(hMemDC, hBitmap); // Select the bitmap into the memory DC

    // Copy the screen content to the memory DC
    BitBlt(hMemDC, 0, 0, lWidth, lHeight, hDC, x, y, SRCCOPY);

    // Create the output file for saving the bitmap
    hFile = CreateFileW(wPath, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    WriteFile(hFile, &bfHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL); // Write the file header
    WriteFile(hFile, &biHeader, sizeof(BITMAPINFOHEADER), &dwWritten, NULL); // Write the info header
    WriteFile(hFile, bBits, cbBits, &dwWritten, NULL); // Write the pixel data

    // Close the file handle
    CloseHandle(hFile);

    // Clean up: release the memory DC, release the screen DC, and delete the bitmap
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hDC);
    DeleteObject(hBitmap);
}

int main ()
{
    int n = 0;
    // Loop to take 10 screenshots with a 500 ms interval
    while (n < 10)
    {
        std::cout << "Taking a screenshot." << std::endl;
        std::stringstream ss;
        ss << n; // Convert the loop index to string
        std::string path = ss.str() + ".bmp"; // Create the file name (0.bmp, 1.bmp, ...)
        captureScreen(path); // Capture the screen and save it
        ++n; // Increment the screenshot count
        Sleep(500); // Wait for 500 milliseconds
    }
    return 0; // Exit the program
}

