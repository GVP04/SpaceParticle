// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include "CSpParticle.h"

#define READPARAMS_NOT_USE_DEFS		0x1
#define READPARAMS_USE_DEFS			0x0


UINT DoCalc_Find(LPVOID pParam);
UINT DoCalc_1(LPVOID pParam);

void SaveBitmap(char* in_FileName, HDC in_hdc, HBITMAP in_hBMP);
void PaintTraceToCDC(CDC* DCMem, SP_Calc* mData, int in_DrawFlags, SPos& delta);
void SaveBitmap(char* in_FileName, HDC in_hdc, HBITMAP in_hBMP);

#endif //PCH_H


