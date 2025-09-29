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


UINT DoCalc_Find(LPVOID pParam);
UINT DoCalc_1(LPVOID pParam);
void ReadCalcParam(char* in_Src, char* in_Mask, SPos& data, double defValue);
void ReadCalcParam(char* in_Src, char* in_Mask, int& data, int defValue);
void ReadCalcParam(char* in_Src, char* in_Mask, double& data, double defValue);
void SaveBitmap(char* in_FileName, HDC in_hdc, HBITMAP in_hBMP);
void PaintTraceToCDC(CDC* DCMem, SP_Calc* mData, int in_DrawFlags, SPos &delta);
void SaveBitmap(char* in_FileName, HDC in_hdc, HBITMAP in_hBMP);

#endif //PCH_H


