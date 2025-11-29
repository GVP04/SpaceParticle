char * GetDblStr(char *oStr, char *oStr2, BYTE *Arr, UINT Len);
char * GetDecStr(char *oStr, BYTE *Arr,   UINT Len);
char * GetOctStr(char *oStr, BYTE *Arr,   UINT Len);
char * GetHexStr(char *oStr, BYTE *Arr,   UINT Len);
BYTE * GetDblArr(BYTE *Arr,  const char *oStr, const char *oStr2);
BYTE * GetDecArr(BYTE *Arr,  const char *oStr);
BYTE * GetOctArr(BYTE *Arr,  const char *oStr);
BYTE * GetHexArr(BYTE *Arr,  const char *oStr);
BYTE * convHexToByteArr(const char *iDataSrc, BYTE *ArrDataDst);

const char *GetFileName(const char *fname);
//UINT SetObjStlCheck(CCheckListBox *m_ListUnitStl, UINT stl, int nn,const char **lpstl,const UINT *uistl,const UINT *uimask,UINT add = 0);
UINT GPRecalcWndRect(CRect *QurentRect, CRect *EtalonRect,CRect *InitialRect, CRect *CompareRect,char *SizingStyle);
UINT ShadowColor(UINT & incolor);
void OptimColor(CDC *dc, UINT & iFRG, UINT & iBKG);
void qsCharLine(const char **Item, int left, int right);
void qsCharILine(const char **Item, int left, int right);
void qsCharLineAndDWORD(const char **Item, DWORD *dwItem, int left, int right);
void qsCharILineAndDWORD(const char **Item, DWORD *dwItem, int left, int right);
void qsBSTRLine(BSTR *Item, int left, int right);
void qsBSTRILine(BSTR *Item, int left, int right);
void qsVARIANTI(VARIANT *Item, int left, int right);
void qsVARIANT(VARIANT *Item, int left, int right);
int CompareIntASC(const void *arg1, const void *arg2);
int CompareIntDES(const void *arg1, const void *arg2);

void qsRectXByNum(CRect *RectArr,UINT *NUMs, int left, int right);
void qsRectYByNum(CRect *RectArr,UINT *NUMs, int left, int right);

DWORD FindInSortArrayIC(const char *ccFindWhat, const char **ccArray, DWORD dwItems);
DWORD FindInSortArray(const char *ccFindWhat, const char **ccArray, DWORD dwItems);

////////////////////////////*********************************
char * ReplaceChar(char *Str, char cOld, char cNew);
char * ReplaceChar(char *Str, char cOld, char cNew, char cStop);
char* TrimChar(char* inout_Str, char in_CharToTrim);
long GetIndexSubString(const CString & iString, const CString & SubString, long Occurence);
long CountOccurence(const CString & iString, const CString & OccurString);

char* StrUpr_my(char* inout_str);
char* StrLwr_my(char* inout_str);

wchar_t *ExtractField( wchar_t * destStr, int iMaxLen, const wchar_t * iStr, int FieldNum, const wchar_t  delim );
wchar_t *ExtractField( wchar_t * destStr, int iMaxLen, const wchar_t * iStr, int FieldNum, const wchar_t * delim );
wchar_t *ExtractField( wchar_t * destStr, int iMaxLen, const char * iStr, int FieldNum, const char delim );
const wchar_t *FindField( const wchar_t * iStr, int FieldNum, const wchar_t delim );
const wchar_t *FindField( const wchar_t * iStr, int FieldNum, const wchar_t * delim );
const wchar_t *FindFieldNC( const wchar_t * iStr, int FieldNum, const wchar_t delim );
const wchar_t *FindFieldNC( const wchar_t * iStr, int FieldNum, const wchar_t * delim );
const wchar_t *FindFieldEx( const wchar_t * iStr, int FieldNum, const wchar_t * delim );

char *ExtractField( char * destStr, int iMaxLen, const char * iStr, int FieldNum, const char  delim );
char *ExtractField( char * destStr, int iMaxLen, const char * iStr, int FieldNum, const char * delim );
const char *FindField( const char * iStr, int FieldNum, const char delim );
const char *FindField( const char * iStr, int FieldNum, const char * delim );
const char *FindFieldNC( const char * iStr, int FieldNum, const char delim );
const char *FindFieldNC( const char * iStr, int FieldNum, const char * delim );
const char *FindFieldEx( const char * iStr, int FieldNum, const char * delim );

wchar_t *wStrReplace(wchar_t *dstStr, int iMaxLen, wchar_t *StrFind, wchar_t *StrReplace );
wchar_t *wStrReplace( wchar_t *dstStr, wchar_t DestChar, wchar_t SrcChar );
wchar_t *wIntToStr( wchar_t *dstStr, char in_Value );
wchar_t *wIntToStr( wchar_t *dstStr, char in_Value, int in_nDigits );
wchar_t *wIntToStr( wchar_t *dstStr, WORD in_Value );
wchar_t *wIntToStr( wchar_t *dstStr, WORD in_Value, int in_nDigits );
wchar_t *wIntToStr( wchar_t *dstStr, DWORD in_Value );
wchar_t *wIntToStr( wchar_t *dstStr, DWORD in_Value, int in_nDigits );
wchar_t *wIntToStr( wchar_t *dstStr, int in_Value );
wchar_t *wIntToStr( wchar_t *dstStr, int in_Value, int in_nDigits );
wchar_t *wHexToStr( wchar_t *dstStr, DWORD in_Value );
wchar_t *wHexToStr( wchar_t *dstStr, DWORD in_Value, int in_nDigits );
wchar_t *wDblToStr( wchar_t *dstStr, double in_Value, int in_nAfter );
wchar_t *wStrToStr( wchar_t *dstStr, const wchar_t *srcStr );
wchar_t *wStrToStr( wchar_t *dstStr, const wchar_t *srcStr, int in_Len );
wchar_t *wStrToStr( wchar_t *dstStr, const char *srcStr );
wchar_t *wStrToStr( wchar_t *dstStr, const char *srcStr, int in_Len );
wchar_t *wStrToStrEx( wchar_t *dstStr, const char *srcStr );


char * wStrReplaceDbl( char *dstStr );

char *wStrReplace( char *dstStr, int iMaxLen, char *StrFind, char *StrReplace );
char *wStrReplace(char *dstStr, char DestChar, char SrcChar);
char *wIntToStr( char *dstStr, char in_Value );
char *wIntToStr( char *dstStr, char in_Value, int in_nDigits );
char *wIntToStr( char *dstStr, WORD in_Value );
char *wIntToStr( char *dstStr, WORD in_Value, int in_nDigits );
char *wIntToStr( char *dstStr, DWORD in_Value );
char *wIntToStr( char *dstStr, DWORD in_Value, int in_nDigits );
char *wIntToStr( char *dstStr, int in_Value );
char *wIntToStr( char *dstStr, int in_Value, int in_nDigits );
char *wHexToStr( char *dstStr, DWORD in_Value );
char *wHexToStr( char *dstStr, DWORD in_Value, int in_nDigits );
char *wDblToStr( char *dstStr, double in_Value, int in_nAfter );
char *wStrToStr( char *dstStr, const char *srcStr );
char *wStrToStr( char *dstStr, const char *srcStr, int in_Len );
char *wStrToStr( char *dstStr, const wchar_t *srcStr );
char *wStrToStrEx( char *dstStr, const char *srcStr );

int wStrCompare( const wchar_t *srcStr1, const wchar_t *srcStr2 );
int wStrCompare( const char *srcStr1, const wchar_t *srcStr2 );
int wStrCompare( const wchar_t *srcStr1, const char *srcStr2 );
int wStrCompare( const char *srcStr1, const char *srcStr2 );


int ReadCalcParam(char* in_Src, char* in_Mask, SPos& data, double defValu, int inFlags = READPARAMS_USE_DEFS);
int ReadCalcParam(char* in_Src, char* in_Mask, int& data, int defValue, int inFlags = READPARAMS_USE_DEFS);
int ReadCalcParam(char* in_Src, char* in_Mask, double& data, double defValue, int inFlags = READPARAMS_USE_DEFS);
void DrawArrow(HDC hDC, int posSX, int posSY, int posEX, int posEY, int in_ArrLen);


struct CMD_Param_File
{
    char FileName[256];
    char Mode[10];
    FILE *pPile;

    CMD_Param_File()
    {
        CLEAR();
    };

    void CLEAR() {
        *FileName = 0;
        *Mode = 0;
        pPile = NULL;
    };

    void CHECK()
    {
        if (*Mode == 0) strcpy_s(Mode, 3, "wt");
        StrLwr_my(Mode);
    }

    int OPEN()
    {
       int ret = -2;
       if (*FileName)
       {
           CHECK();
           ret = fopen_s(&pPile, FileName, Mode);
       }
       return ret;
    }

    int CLOSE()
    {
        int ret = -1;
        if (pPile)
            ret = fclose(pPile);

        return ret;
    }

    int ReadParams(char* in_String)
    {
        int ret = 0;

        if (in_String)
        {
            const char* pos = FindField(in_String, 2, "FILENAME");
            if (*pos)
            {
                ExtractField(FileName, 250, pos, 2, '"');
                TrimChar(FileName, ' ');
                if (*FileName) ret++;
            }
            pos = FindField(in_String, 2, "FILEMODE");
            if (*pos)
            {
                ExtractField(Mode, 9, pos, 2, '"');
                TrimChar(Mode, ' ');
                if (*Mode) ret++;
            }
            CHECK();

        }
        return ret;
    }
};



struct CMD_Param_DataType
{
    //Data
    int Trace;
    int GroupData;
    int Relation;

    //For for
    int StartId;
    int EndId;
    int StepId;

    CMD_Param_DataType()
    {
        CLEAR();
    };

    void CLEAR()
    {
        Trace = 0;
        GroupData = 0;
        Relation = 0;
        StartId = 0;
        EndId = 0;
        StepId = 0;
    };

    void SET_ALL_ON()
    {
        Trace = 1;
        Relation = 1;
        GroupData = 1;
        StartId = 0;
        EndId = 0x4FFFFFFF;
        StepId = 0x4FFFFFFF;
    };

    void CHECK(int in_MaxId)
    {
        //Trace;      Relation;   GroupData;
        if (StartId < 0) StartId = 0;
        if (StartId > in_MaxId) StartId = in_MaxId;

        if (EndId < 0) EndId = 0x4FFFFFFF;
        if (EndId > in_MaxId && in_MaxId > StartId) EndId = in_MaxId;
        if (EndId < StartId) EndId = StartId;

        if (StepId < 1 || StepId >(EndId - StartId)) StepId = EndId - StartId;
        if (StepId < 1 ) StepId = 1;
    };

    int ReadParams(char* in_String)
    {
        int ret = 0;

        if (in_String)
        {
            const char* pos;
            
            if (*FindField(in_String, 2, "TRACE")) { ret++; Trace = 1; }
            if (*FindField(in_String, 2, "GROUPDATA")) { ret++; GroupData = 1; }
            if (*FindField(in_String, 2, "RELATION")) { ret++; Relation = 1; }

            if ((pos = FindField(in_String, 2, "STARTID")))
            {
                ret++;
                sscanf_s(FindField(pos, 2, "="), "%i", &StartId);
                if (StartId < 0) StartId = 0;
            }

            if ((pos = FindField(in_String, 2, "ENDID")))
            {
                ret++;
                sscanf_s(FindField(pos, 2, "="), "%i", &EndId);
                if (EndId < 0) EndId = 0x4FFFFFFF;
            }

            if ((pos = FindField(in_String, 2, "STEPID")))
            {
                ret++;
                sscanf_s(FindField(pos, 2, "="), "%i", &StepId);
                if (StepId < 0) EndId = 0x4FFFFFFF;;
            }
        }
        return ret;
    }
};
