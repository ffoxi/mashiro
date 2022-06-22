// copyright ffoxi 2022

#include <stdint.h>
#include <stdio.h>

#include <windows.h>
#include <tchar.h>

#include "SkString.h"
#include "SkSurface.h"
#include "SkCanvas.h"
#include "SkFont.h"
#include "SkTypeface.h"

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main(int argc, char **argv) {
    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        WndProc,
        0L,
        0L,
        GetModuleHandle(nullptr),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        _T("mashiro window class"),
        nullptr
    };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(
        wc.lpszClassName,
        _T("mashiro"),
        WS_OVERLAPPEDWINDOW,
        100,
        100,
        800,
        600,
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr
    );
    if (hwnd == nullptr) {
        return 0;
    }
    ShowWindow(hwnd, SW_SHOW);
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

// Draw
void Draw(SkCanvas* canvas, int w, int h) {
    SkPaint textpaint;
    textpaint.reset();
    textpaint.setColor(SkColor(0xff0000ff));
    textpaint.setAntiAlias(true);
    SkString string("你好世界");
    canvas->drawString(string, 0, 20, SkFont(
        SkTypeface::MakeFromFile("Chusung-220206.ttf"), 20), textpaint);
    canvas->flush();
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        {
            PAINTSTRUCT paint_struct;
            HDC hdc = BeginPaint(hwnd, &paint_struct);
            RECT rect;
            GetClientRect(hwnd, &rect);

            int height = rect.bottom - rect.top;
            int width = rect.right - rect.left;

            const size_t bitmap_size = sizeof(BITMAPINFOHEADER) +
                width * height * sizeof(uint32_t);
            BITMAPINFO* bitmap_info = reinterpret_cast<BITMAPINFO*>(
                new BYTE[bitmap_size]());
            bitmap_info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bitmap_info->bmiHeader.biWidth = width;
            bitmap_info->bmiHeader.biHeight = -height;
            bitmap_info->bmiHeader.biPlanes = 1;
            bitmap_info->bmiHeader.biBitCount = 32;
            bitmap_info->bmiHeader.biCompression = BI_RGB;
            void *pixels = bitmap_info->bmiColors;

            // use skia
            SkImageInfo skimageinfo = SkImageInfo::Make(
                width, height, kBGRA_8888_SkColorType, kPremul_SkAlphaType);
            sk_sp<SkSurface> sksurface = SkSurface::MakeRasterDirect(
                skimageinfo, pixels, width * sizeof(uint32_t));
            SkCanvas *skcanvas = sksurface->getCanvas();
            skcanvas->clear(SK_ColorWHITE);
            Draw(skcanvas, width, height); 

            StretchDIBits(hdc,
                0, 0, width, height,
                0, 0, width, height,
                pixels, bitmap_info,
                DIB_RGB_COLORS, SRCCOPY);
            
            delete[] bitmap_info;
            EndPaint(hwnd, &paint_struct);
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

extern "C" {
#ifdef _WIN64
  PROC WINAPI __imp_wglGetProcAddress(LPCSTR) {
    abort();
    return nullptr;
  }
  HGLRC WINAPI  __imp_wglGetCurrentContext() {
    abort();
    return nullptr;
  }
#else
  PROC WINAPI _imp__wglGetProcAddress(LPCSTR) {
    abort();
    return nullptr;
  }
  HGLRC WINAPI _imp__wglGetCurrentContext() {
    abort();
    return nullptr;
  }
#endif
}