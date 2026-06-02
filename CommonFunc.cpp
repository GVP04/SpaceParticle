// GP_term.cpp : Defines the class behaviors for the application.
//


#include "pch.h"
#include "CommonFunc.h"


int mRGB[16] = {
	RGB(200, 0, 0),RGB(0, 200, 0),RGB(0, 0, 200),RGB(200, 200, 0),RGB(200, 0, 200),RGB(0, 200, 200),
	RGB(200, 100, 0),RGB(200, 0, 100),RGB(200, 100, 100),RGB(100, 200, 0),RGB(0, 200, 100),RGB(100, 0, 200),
	RGB(0, 100, 200),RGB(0, 100, 100),RGB(100, 0, 100),RGB(100, 100, 0),
};

int mRGBDark[16] = {
	RGB(111, 0, 0),RGB(0, 111, 0),RGB(0, 0, 111),RGB(111, 111, 0),RGB(111, 0, 111),RGB(0, 111, 111),
	RGB(111, 51, 0),RGB(111, 0, 51),RGB(111, 51, 51),RGB(51, 111, 0),RGB(0, 111, 51),RGB(51, 0, 111),
	RGB(0, 51, 111),RGB(0, 51, 51),RGB(51, 0, 51),RGB(51, 51, 0),
};

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////


char* StrUpr_my(char* inout_str)
{
	char* ret = inout_str;
	if (inout_str)
		while (*inout_str)
		{
			if (*inout_str >= 'a' && *inout_str <= 'z')
				*inout_str -= 'a' - 'A';
			else
				if (*inout_str >= 'а' && *inout_str <= 'я')
					*inout_str -= 'а' - 'А';

			inout_str++;
		}
	return ret;
}


char* StrLwr_my(char* inout_str)
{
	char* ret = inout_str;
	if (inout_str)
		while (*inout_str)
		{
			if (*inout_str >= 'A' && *inout_str <= 'Z')
				*inout_str += 'a' - 'A';
			else
				if (*inout_str >= 'А' && *inout_str <= 'Я')
					*inout_str += 'а' - 'А';

			inout_str++;
		}
	return ret;
}


const char *GetFileName( const char *fname )
{
	const char *pos = fname + (int)strlen( fname );
	while ( pos != fname && *pos != '\\' ) pos--;
	if ( *pos == '\\' ) pos++;
	return pos;
}

UINT ShadowColor( UINT & incolor )
{
	incolor /= 2;
	incolor &= RGB( 127, 127, 127 );
	incolor ^= RGB( 7, 7, 7 );
	return incolor;
}

char * ReplaceChar( char *Str, char cOld, char cNew )
{
	char *pos = Str;
	if ( pos )
		while (*pos)
		{
			if (*pos == cOld) *pos = cNew;
			pos++;
		}

	return Str;
}

char * TrimChar(char *inout_Str, char in_CharToTrim)
{
	char *posSrc = inout_Str;
	char *posDst = posSrc;
	if (posSrc && *posSrc)
	{

		while (*posSrc && *posSrc == in_CharToTrim) 
			posSrc++;

		while (*posSrc)
		{
			while (*posSrc && *posSrc != in_CharToTrim)
				*(posDst++) = *(posSrc++);

			if (*posSrc)
				*(posDst++) = *(posSrc++);

			while (*posSrc && *posSrc == in_CharToTrim)
				posSrc++;
		}

		do { *posDst = 0; }
		while (posDst != inout_Str && *(--posDst) == in_CharToTrim);
	}

	return inout_Str;
}

char * ReplaceChar( char *Str, char cOld, char cNew, char cStop )
{
	char *pos = Str;
	if ( pos )
		while ( *pos != 0 && *pos != cStop )
			if ( *( pos++ ) == cOld )
			{
				*( --pos ) = cNew;
				pos++;
			}

	return Str;
}


UINT GPRecalcWndRect( CRect *QurentRect, CRect *EtalonRect, CRect *InitialRect, CRect *CompareRect, char *SizingStyle )
{
	UINT ret = 0;
	int tmp = 0;

	if ( CompareRect->IsRectEmpty( ) == FALSE )
	{
		if ( EtalonRect->IsRectEmpty( ) == TRUE ) *EtalonRect = *CompareRect;
		else
		{
			switch ( SizingStyle[ 0 ] )
			{
				case 'P':
					tmp = CompareRect->left + ( ( InitialRect->left - EtalonRect->left )*( CompareRect->Width( ) + 1 ) ) / ( EtalonRect->Width( ) + 1 );
					if ( QurentRect->left != tmp ) { ret++; QurentRect->left = tmp; }
					break;
				case 'S':
					tmp = CompareRect->left - EtalonRect->left + InitialRect->left;
					if ( QurentRect->left != tmp ) { ret++; QurentRect->left = tmp; }
					break;
				case 'E':
					tmp = CompareRect->right - EtalonRect->right + InitialRect->left;
					if ( QurentRect->left != tmp ) { ret++; QurentRect->left = tmp; }
					break;
					//			case 'N':
				default:
					QurentRect->left = InitialRect->left;
					break;
			}

			switch ( SizingStyle[ 1 ] )
			{
				case 'P':
					tmp = CompareRect->top + ( ( InitialRect->top - EtalonRect->top )*( CompareRect->Height( ) + 1 ) ) / ( EtalonRect->Height( ) + 1 );
					if ( QurentRect->top != tmp ) { ret++; QurentRect->top = tmp; }
					break;
				case 'S':
					tmp = CompareRect->top - EtalonRect->top + InitialRect->top;
					if ( QurentRect->top != tmp ) { ret++; QurentRect->top = tmp; }
					break;
				case 'E':
					tmp = CompareRect->bottom - EtalonRect->bottom + InitialRect->top;
					if ( QurentRect->top != tmp ) { ret++; QurentRect->top = tmp; }
					break;
					//			case 'N':
				default:
					QurentRect->top = InitialRect->top;
					break;
			}

			switch ( SizingStyle[ 2 ] )
			{
				case 'P':
					tmp = CompareRect->right + ( ( InitialRect->right - EtalonRect->right )*( CompareRect->Width( ) + 1 ) ) / ( EtalonRect->Width( ) + 1 );
					if ( QurentRect->right != tmp ) { ret++; QurentRect->right = tmp; }
					break;
				case 'S':
					tmp = CompareRect->left - EtalonRect->left + InitialRect->right;
					if ( QurentRect->right != tmp ) { ret++; QurentRect->right = tmp; }
					break;
				case 'E':
					tmp = CompareRect->right - EtalonRect->right + InitialRect->right;
					if ( QurentRect->right != tmp ) { ret++; QurentRect->right = tmp; }
					break;
					//			case 'N':
				default:
					QurentRect->right = InitialRect->right;
					break;
			}

			switch ( SizingStyle[ 3 ] )
			{
				case 'P':
					tmp = CompareRect->bottom + ( ( InitialRect->bottom - EtalonRect->bottom )*( CompareRect->Height( ) + 1 ) ) / ( EtalonRect->Height( ) + 1 );
					if ( QurentRect->bottom != tmp ) { ret++; QurentRect->bottom = tmp; }
					break;
				case 'S':
					tmp = CompareRect->top - EtalonRect->top + InitialRect->bottom;
					if ( QurentRect->bottom != tmp ) { ret++; QurentRect->bottom = tmp; }
					break;
				case 'E':
					tmp = CompareRect->bottom - EtalonRect->bottom + InitialRect->bottom;
					if ( QurentRect->bottom != tmp ) { ret++; QurentRect->bottom = tmp; }
					break;
					//			case 'N':
				default:
					QurentRect->bottom = InitialRect->bottom;
					break;
			}
		}
	}
	return ret;
}

long CountOccurence( const CString & iString, const CString & OccurString )
{
	long ret = 0, pos = 0;

	while ( ( pos = iString.Find( OccurString, pos ) ) >= 0 )
	{
		pos++;	ret++;
	}

	return ret;
}

long GetIndexSubString( const CString & iString, const CString & SubString, long Occurence )
{
	long count = 1, pos = 0;

	while ( ( pos = iString.Find( SubString, pos ) ) >= 0 && count < Occurence )
	{
		pos++;	count++;
	}

	return pos;
}

void qsCharLine( const char **Item, int left, int right )
{
	int i, j;
	const char *x, *y;

	i = left;
	j = right;
	x = Item[ ( left + right ) / 2 ];

	do
	{
		while ( strcmp( Item[ i ], x ) < 0 && i < right ) i++;
		while ( strcmp( x, Item[ j ] ) < 0 && j > left ) j--;

		if ( i <= j )
		{
			y = Item[ i ];		Item[ i ] = Item[ j ];		Item[ j ] = y;
			i++;			j--;
		}
	} while ( i <= j );
	if ( left < j ) qsCharLine( Item, left, j );
	if ( i < right ) qsCharLine( Item, i, right );
}


void qsCharILine( const char **Item, int left, int right )
{
	int i, j;
	const char *x, *y;

	i = left;
	j = right;
	x = Item[ ( left + right ) / 2 ];

	do
	{
		while ( _stricmp( Item[ i ], x ) < 0 && i < right ) i++;
		while ( _stricmp( x, Item[ j ] ) < 0 && j > left ) j--;


		if ( i <= j )
		{
			y = Item[ i ];		Item[ i ] = Item[ j ];		Item[ j ] = y;
			i++;			j--;
		}
	} while ( i <= j );
	if ( left < j ) qsCharILine( Item, left, j );
	if ( i < right ) qsCharILine( Item, i, right );
}

void qsCharLineAndDWORD( const char **Item, DWORD *dwItem, int left, int right )
{
	int i, j;
	const char *x, *y;
	DWORD dwTmp;

	i = left;
	j = right;
	x = Item[ ( left + right ) / 2 ];

	do
	{
		while ( strcmp( Item[ i ], x ) < 0 && i < right ) i++;
		while ( strcmp( x, Item[ j ] ) < 0 && j > left ) j--;

		if ( i <= j )
		{
			y = Item[ i ];			Item[ i ] = Item[ j ];		Item[ j ] = y;
			dwTmp = dwItem[ i ];	dwItem[ i ] = dwItem[ j ];	dwItem[ j ] = dwTmp;
			i++;			j--;
		}
	} while ( i <= j );
	if ( left < j ) qsCharLineAndDWORD( Item, dwItem, left, j );
	if ( i < right ) qsCharLineAndDWORD( Item, dwItem, i, right );
}

void qsCharILineAndDWORD( const char **Item, DWORD *dwItem, int left, int right )
{
	int i, j;
	const char *x, *y;
	DWORD dwTmp;

	i = left;
	j = right;
	x = Item[ ( left + right ) / 2 ];

	do
	{
		while ( _stricmp( Item[ i ], x ) < 0 && i < right ) i++;
		while ( _stricmp( x, Item[ j ] ) < 0 && j > left ) j--;

		if ( i <= j )
		{
			y = Item[ i ];			Item[ i ] = Item[ j ];		Item[ j ] = y;
			dwTmp = dwItem[ i ];	dwItem[ i ] = dwItem[ j ];	dwItem[ j ] = dwTmp;
			i++;			j--;
		}
	} while ( i <= j );
	if ( left < j ) qsCharILineAndDWORD( Item, dwItem, left, j );
	if ( i < right ) qsCharILineAndDWORD( Item, dwItem, i, right );
}


void qsBSTRLine( BSTR *Item, int left, int right )
{
	int i, j;
	BSTR x, y;

	i = left;
	j = right;
	x = Item[ ( left + right ) / 2 ];

	do
	{
		while ( wcscmp( Item[ i ], x ) < 0 && i < right ) i++;
		while ( wcscmp( x, Item[ j ] ) < 0 && j > left ) j--;

		if ( i <= j )
		{
			y = Item[ i ];		Item[ i ] = Item[ j ];		Item[ j ] = y;
			i++;			j--;
		}
	} while ( i <= j );
	if ( left < j ) qsBSTRLine( Item, left, j );
	if ( i < right ) qsBSTRLine( Item, i, right );
}

void qsBSTRILine( BSTR *Item, int left, int right )
{
	int i, j;
	BSTR x, y;

	i = left;
	j = right;
	x = Item[ ( left + right ) / 2 ];

	do
	{
		while ( _wcsicmp( Item[ i ], x ) < 0 && i < right ) i++;
		while ( _wcsicmp( x, Item[ j ] ) < 0 && j > left ) j--;


		if ( i <= j )
		{
			y = Item[ i ];		Item[ i ] = Item[ j ];		Item[ j ] = y;
			i++;			j--;
		}
	} while ( i <= j );
	if ( left < j ) qsBSTRILine( Item, left, j );
	if ( i < right ) qsBSTRILine( Item, i, right );
}

void qsVARIANT( VARIANT *Item, int left, int right )
{
	int i, j;
	VARIANT x, y;
	VariantInit( &x );
	VariantInit( &y );

	i = left;
	j = right;

	if ( V_VT( Item ) == VT_BSTR )
	{
		HRESULT hr = VariantCopy( &x, Item + ( left + right ) / 2 );
		do
		{
			while ( wcscmp( Item[ i ].bstrVal, x.bstrVal ) < 0 && i < right )
				i++;
			while ( wcscmp( x.bstrVal, Item[ j ].bstrVal ) < 0 && j > left )
				j--;

			if ( i <= j )
			{
				hr = VariantCopy( &y, Item + i );
				hr = VariantCopy( Item + i, Item + j );	hr = VariantCopy( Item + j, &y );
				i++;				j--;
			}
		} while ( i <= j );
		VariantClear( &x );
		VariantClear( &y );
	}
	else
	{
		x = Item[ ( left + right ) / 2 ];
		do
		{
			while ( Item[ i ].dblVal < x.dblVal && i < right ) i++;
			while ( x.dblVal<Item[ j ].dblVal && j>left ) j--;

			if ( i <= j )
			{
				y = Item[ i ];			Item[ i ] = Item[ j ];			Item[ j ] = y;
				i++;				j--;
			}
		} while ( i <= j );
	}

	if ( left < j ) qsVARIANT( Item, left, j );
	if ( i < right ) qsVARIANT( Item, i, right );
}

void qsVARIANTI( VARIANT *Item, int left, int right )
{
	int i, j;
	VARIANT x, y;

	i = left;
	j = right;
	x = Item[ ( left + right ) / 2 ];

	if ( V_VT( Item ) == VT_BSTR )
	{
		do
		{
			while ( _wcsicmp( Item[ i ].bstrVal, x.bstrVal ) < 0 && i < right ) i++;
			while ( _wcsicmp( x.bstrVal, Item[ j ].bstrVal ) < 0 && j > left ) j--;

			if ( i <= j )
			{
				y = Item[ i ];			Item[ i ] = Item[ j ];			Item[ j ] = y;
				i++;				j--;
			}
		} while ( i <= j );
	}
	else
	{
		do
		{
			while ( Item[ i ].dblVal < x.dblVal && i < right ) i++;
			while ( x.dblVal<Item[ j ].dblVal && j>left ) j--;

			if ( i <= j )
			{
				y = Item[ i ];			Item[ i ] = Item[ j ];			Item[ j ] = y;
				i++;				j--;
			}
		} while ( i <= j );
	}

	if ( left < j ) qsVARIANTI( Item, left, j );
	if ( i < right ) qsVARIANTI( Item, i, right );
}

int CompareIntASC( const void *arg1, const void *arg2 )
{
	return *(int *)arg1 - *(int *)arg2;
}

int CompareIntDES( const void *arg1, const void *arg2 )
{
	return *(int *)arg2 - *(int *)arg1; 
}


BOOL GPDeleteDir( const wchar_t * iDir )
{
	CFileFind ff;
	BOOL ret;
	CString sDir = iDir;
	sDir += L"\\*.*";

	ret = ff.FindFile( sDir, 0 );

	while ( ret )
	{
		ret = ff.FindNextFile( );
		if ( !ff.IsDots( ) )
		{
			if ( ff.IsDirectory( ) )	GPDeleteDir( ff.GetFilePath( ) );
			else					DeleteFile( ff.GetFilePath( ) );
		}
	}
	ff.Close( );

	return _wrmdir( iDir );
}


BOOL GPCopyDir( const wchar_t * sDir, const wchar_t * dDir )
{
	CFileFind ff;
	BOOL ret = FALSE;
	CString tmpsDir = sDir;
	tmpsDir += L"\\*.*";

	if ( _wmkdir( dDir ) == 0 )
	{
		ret = ff.FindFile( tmpsDir, 0 );
		while ( ret )
		{
			ret = ff.FindNextFile( );
			tmpsDir = dDir;
			tmpsDir += L"\\" + ff.GetFileName( );

			if ( !ff.IsDots( ) )
			{
				if ( ff.IsDirectory( ) )	GPCopyDir( ff.GetFilePath( ), tmpsDir );
				else					CopyFile( ff.GetFilePath( ), tmpsDir, FALSE );
			}
		}
		ff.Close( );
	}

	return ret;
}

wchar_t *ExtractField( wchar_t * destStr, int iMaxLen, const wchar_t * iStr, int FieldNum, const wchar_t delim )
{
	if ( destStr )
	{
		wchar_t *pOut = destStr;
		*pOut = 0;
		if ( !iStr ) iStr = L"";
		const wchar_t *pos = iStr;

		while ( FieldNum > 1 && *pos )
			if ( *( pos++ ) == delim ) FieldNum--;

		while ( *pos && *pos != delim && --iMaxLen > 0 ) *( pOut++ ) = *( pos++ );

		*pOut = 0;
	}
	return destStr;
}

char *ExtractField( char * destStr, int iMaxLen, const char * iStr, int FieldNum, const char delim )
{
	if ( destStr )
	{
		char *pOut = destStr;
		*pOut = 0;
		if ( !iStr ) iStr = "";
		const char *pos = iStr;

		while ( FieldNum > 1 && *pos )
			if ( *( pos++ ) == delim ) FieldNum--;

		while ( *pos && *pos != delim && --iMaxLen > 0 ) *( pOut++ ) = *( pos++ );

		*pOut = 0;
	}
	return destStr;
}

wchar_t *ExtractField( wchar_t * destStr, int iMaxLen, const char * iStr, int FieldNum, const char delim )
{
	if ( destStr )
	{
		wchar_t *pOut = destStr;
		*pOut = 0;
		if ( !iStr ) iStr = "";
		const char *pos = iStr;

		while ( FieldNum > 1 && *pos )
			if ( *( pos++ ) == delim ) FieldNum--;

		while ( *pos && *pos != delim && --iMaxLen > 0 ) *( pOut++ ) = *( pos++ );

		*pOut = 0;
	}
	return destStr;
}


wchar_t *ExtractField( wchar_t * destStr, int iMaxLen, const wchar_t * iStr, int FieldNum, const wchar_t * delim )
{
	if ( destStr && delim  && *delim )
		if ( delim[ 1 ] == 0 ) ExtractField( destStr, iMaxLen, iStr, FieldNum, *delim );
		else
		{
			wchar_t *pOut = destStr;
			*pOut = 0;
			if ( !iStr ) iStr = L"";
			const wchar_t *pos = iStr;

			while ( FieldNum > 1 && *pos )
				if ( *( pos++ ) == *delim )
				{
					const wchar_t *posDelim = delim;
					const wchar_t *posPos = pos - 1;
					while ( *( ++posDelim ) && *( ++posPos ) );

					if ( *posDelim == 0 ) { FieldNum--; pos = posPos; }
				}

			while ( *pos && --iMaxLen > 0 )
				if ( *( pos ) != *delim ) *( pOut++ ) = *( pos++ );
				else
				{
					const wchar_t *posDelim = delim;
					const wchar_t *posPos = pos;
					while ( *( ++posDelim ) && *( ++posPos ) && *posDelim == *posPos );

					if ( *posDelim == 0 ) iMaxLen = 0;
					else *( pOut++ ) = *( pos++ );
				}
			*pOut = 0;
		}
	return destStr;
}

char *ExtractField( char * destStr, int iMaxLen, const char * iStr, int FieldNum, const char * delim )
{
	if ( destStr && delim  && *delim )
		if ( delim[ 1 ] == 0 ) ExtractField( destStr, iMaxLen, iStr, FieldNum, *delim );
		else
		{
			char *pOut = destStr;
			*pOut = 0;
			if ( !iStr ) iStr = "";
			const char *pos = iStr;

			while ( FieldNum > 1 && *pos )
				if ( *( pos++ ) == *delim )
				{
					const char *posDelim = delim;
					const char *posPos = pos - 1;
					while ( *( ++posDelim ) && *( ++posPos ) );

					if ( *posDelim == 0 ) { FieldNum--; pos = posPos; }
				}

			while ( *pos && --iMaxLen > 0 )
				if ( *( pos ) != *delim ) *( pOut++ ) = *( pos++ );
				else
				{
					const char *posDelim = delim;
					const char *posPos = pos;
					while ( *( ++posDelim ) && *( ++posPos ) && *posDelim == *posPos );

					if ( *posDelim == 0 ) iMaxLen = 0;
					else *( pOut++ ) = *( pos++ );
				}
			*pOut = 0;
		}
	return destStr;
}

const wchar_t *FindField( const wchar_t *iStr, int FieldNum, const wchar_t delim )
{
	const wchar_t *pOut = iStr;
	if ( !pOut ) pOut = L"";

	while ( FieldNum > 1 && *pOut )
		if ( *( pOut++ ) == delim ) FieldNum--;

	return pOut;
}

const wchar_t *FindFieldNC( const wchar_t *iStr, int FieldNum, const wchar_t delim )
{
	const wchar_t *pOut = iStr;
	if ( !pOut ) pOut = L"";
	
	wchar_t delimNC = __ascii_towlower( delim );

	while ( FieldNum > 1 && *pOut )
		if ( __ascii_towlower( *( pOut++ )) == delimNC ) FieldNum--;

	return pOut;
}

const char *FindField( const char *iStr, int FieldNum, const char delim )
{
	const char *pOut = iStr;
	if ( !pOut ) pOut = "";

	while ( FieldNum > 1 && *pOut )
		if ( *( pOut++ ) == delim ) FieldNum--;

	return pOut;
}


const char *FindFieldNC( const char *iStr, int FieldNum, const char delim )
{
	int iDelta = 'a' - 'A';
	const char *pOut = iStr;
	if ( !pOut ) pOut = "";

	while ( FieldNum > 1 && *pOut )
	{
		if ( *pOut == delim || ( *pOut > delim  &&  *pOut == delim + iDelta ) || ( *pOut < delim  &&  *pOut == delim - iDelta ) ) FieldNum--;

		pOut++;
	}
	return pOut;
}


const wchar_t *FindField( const wchar_t *iStr, int FieldNum, const wchar_t * delim )
{
	const wchar_t *pOut = iStr;
	if ( !pOut ) pOut = L"";

	if ( delim  && *delim )
		while ( FieldNum > 1 && *pOut )
			if ( *( pOut++ ) == *delim )
			{
				const wchar_t *posDelim = delim;
				const wchar_t *posPos = pOut - 1;
				while ( *( ++posDelim ) && *( ++posPos ) && *posDelim == *posPos );

				if ( *posDelim == 0 ) { FieldNum--; pOut = ++posPos; }
			}

	return pOut;
}

const wchar_t *FindFieldNC( const wchar_t *iStr, int FieldNum, const wchar_t * delim )
{
	const wchar_t *pOut = iStr;
	if ( !pOut ) pOut = L"";

	if ( delim  && *delim )
		while ( FieldNum > 1 && *pOut )
			if ( *( pOut++ ) == *delim )
			{
				const wchar_t *posDelim = delim;
				const wchar_t *posPos = pOut - 1;
				while ( *( ++posDelim ) && *( ++posPos ) && __ascii_towlower( *posDelim) == __ascii_towlower( *posPos ) );

				if ( *posDelim == 0 ) { FieldNum--; pOut = ++posPos; }
			}

	return pOut;
}

const char *FindField( const char *iStr, int FieldNum, const char * delim )
{
	const char *pOut = iStr;
	if ( !pOut ) pOut = "";

	if ( delim  && *delim )
		while ( FieldNum > 1 && *pOut )
			if ( *( pOut++ ) == *delim )
			{
				const char *posDelim = delim;
				const char *posPos = pOut - 1;
				while ( *( ++posDelim ) && *( ++posPos ) && *posDelim == *posPos );

				if ( *posDelim == 0 ) { FieldNum--; pOut = ++posPos; }
			}

	return pOut;
}

const char *FindFieldNC( const char *iStr, int FieldNum, const char * delim )
{
	int iDelta = 'a' - 'A';
	const char *pOut = iStr;
	if ( !pOut ) pOut = "";

	if ( delim  && *delim )
		while ( FieldNum > 1 && *pOut )
			if ( *pOut == *delim || ( *pOut > *delim  &&  *pOut == *delim + iDelta ) || ( *pOut < *delim  &&  *pOut == *delim - iDelta ) )
			{
				pOut++;
				const char *posDelim = delim;
				const char *posPos = pOut - 1;
				while ( *( ++posDelim ) && *( ++posPos ) && ( *posPos == *posDelim || ( *posPos > *posDelim  &&  *posPos == *posDelim + iDelta ) || ( *posPos < *posDelim  &&  *posPos == *posDelim - iDelta ) ) );

				if ( *posDelim == 0 ) { FieldNum--; pOut = ++posPos; }
			}
			else
			{
				pOut++;
			}

	return pOut;
}

const wchar_t *FindFieldEx( const wchar_t * iStr, int FieldNum, const wchar_t * delim )
{
	const wchar_t *pos;
	int lendelim = (int)wcslen( delim );
	int i;
	for ( i = 1; i < FieldNum; i++ )
	{
		pos = iStr;
		int nLevel = 0;
		while ( *pos && ( nLevel || ( !nLevel && wcsncmp( pos, delim, lendelim ) ) ) )
		{
			if ( *pos == '(' ) nLevel++;
			else
				if ( *pos == ')' && nLevel > 0 ) nLevel--;
			pos++;
		}
		if ( !*pos )	iStr = L"";
		else { iStr = pos + lendelim; }
	}

	return iStr;
}

const char *FindFieldEx( const char * iStr, int FieldNum, const char * delim )
{
	const char *pos;
	int lendelim = (int)strlen( delim );
	int i;
	for ( i = 1; i < FieldNum; i++ )
	{
		pos = iStr;
		int nLevel = 0;
		while ( *pos && ( nLevel || ( !nLevel && strncmp( pos, delim, lendelim ) ) ) )
		{
			if ( *pos == '(' ) nLevel++;
			else
				if ( *pos == ')' && nLevel > 0 ) nLevel--;
			pos++;
		}
		if ( !*pos )	iStr = "";
		else { iStr = pos + lendelim; }
	}

	return iStr;
}
void qsRectYByNum( CRect *RectArr, UINT *NUMs, int left, int right )
{
	int i, j;
	UINT x, y;

	i = left;
	j = right;
	x = NUMs[ ( left + right ) / 2 ];

	do
	{
		while ( ( RectArr[ NUMs[ i ] ].top < RectArr[ x ].top || ( RectArr[ NUMs[ i ] ].top == RectArr[ x ].top && RectArr[ NUMs[ i ] ].left < RectArr[ x ].left ) ) && i < right ) i++;
		while ( ( RectArr[ x ].top < RectArr[ NUMs[ j ] ].top || ( RectArr[ x ].top == RectArr[ NUMs[ j ] ].top && RectArr[ x ].left < RectArr[ NUMs[ j ] ].left ) ) && j > left ) j--;

		if ( i <= j )
		{
			y = NUMs[ i ];		NUMs[ i ] = NUMs[ j ];		NUMs[ j ] = y;
			i++;			j--;
		}
	} while ( i <= j );
	if ( left < j ) qsRectYByNum( RectArr, NUMs, left, j );
	if ( i < right ) qsRectYByNum( RectArr, NUMs, i, right );
}

void qsRectXByNum( CRect *RectArr, UINT *NUMs, int left, int right )
{
	int i, j;
	UINT x, y;

	i = left;
	j = right;
	x = NUMs[ ( left + right ) / 2 ];

	do
	{
		while ( ( RectArr[ NUMs[ i ] ].left < RectArr[ x ].left || ( RectArr[ NUMs[ i ] ].left == RectArr[ x ].left && RectArr[ NUMs[ i ] ].top < RectArr[ x ].top ) ) && i < right ) i++;
		while ( ( RectArr[ x ].left < RectArr[ NUMs[ j ] ].left || ( RectArr[ x ].left == RectArr[ NUMs[ j ] ].left && RectArr[ x ].top < RectArr[ NUMs[ j ] ].top ) ) && j > left ) j--;

		if ( i <= j )
		{
			y = NUMs[ i ];		NUMs[ i ] = NUMs[ j ];		NUMs[ j ] = y;
			i++;			j--;
		}
	} while ( i <= j );
	if ( left < j ) qsRectXByNum( RectArr, NUMs, left, j );
	if ( i < right ) qsRectXByNum( RectArr, NUMs, i, right );
}


char * GetHexStr( char *oStr, BYTE *Arr, UINT Len )
{
	char mask[ 16 ] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F', };
	char *ret = oStr;
	if ( oStr && Arr && Len )
	{
		while ( Len-- )
		{
			*( oStr++ ) = mask[ ( *Arr ) / 16 ];
			*( oStr++ ) = mask[ ( *( Arr++ ) ) & 0x0F ];
		}
		*oStr = 0;
	}

	return ret;
}

char * GetOctStr( char *oStr, BYTE *Arr, UINT Len )
{
	char mask[ 16 ] = { '0','1','2','3','4','5','6','7', };
	char *ret = oStr;
	if ( oStr && Arr && Len )
	{
		while ( Len-- )
		{
			*( oStr++ ) = mask[ ( *Arr ) / 64 ];
			*( oStr++ ) = mask[ ( ( *Arr ) / 8 ) & 0x07 ];
			*( oStr++ ) = mask[ ( *( Arr++ ) ) & 0x07 ];
		}
		*oStr = 0;
	}

	return ret;
}

char * GetDecStr( char *oStr, BYTE *Arr, UINT Len )
{
	char *ret = oStr;
	if ( oStr && Arr && Len )
	{
		while ( Len-- )
		{
			int c = *( Arr++ );
			*( oStr++ ) = (char)( '0' + c / 100 );
			*( oStr++ ) = (char)( '0' + ( ( c / 10 ) % 10 ) );
			*( oStr++ ) = (char)( '0' + ( c % 10 ) );
		}
		*oStr = 0;
	}
	return ret;
}

char * GetDblStr( char *oStr, char *oStr2, BYTE *Arr, UINT Len )
{
	char *ret = oStr;
	int c;
	if ( oStr && oStr2 && Arr && Len )
	{
		while ( Len-- )
		{
			if ( ( c = *( Arr++ ) ) > 0xFB || c == 0x7F )
			{
				*( oStr++ ) = (char)( c - 0x20 );
				*( oStr2++ ) = '1';
			}
			else
				if ( c < 0x20 )
				{
					*( oStr++ ) = (char)( c + 0x20 );
					*( oStr2++ ) = '2';
				}
				else
				{
					*( oStr++ ) = (char)c;
					*( oStr2++ ) = '0';
				}
		}
		*oStr = 0;
		*oStr2 = 0;
	}
	return ret;
}

BYTE * GetDblArr( BYTE *Arr, const char *oStr, const char *oStr2 )
{
	BYTE *ret = Arr;
	if ( Arr && oStr-- )
	{
		if ( oStr2-- )
			while ( *( ++oStr ) && *( ++oStr2 ) )
			{
				if ( *oStr2 == '0' )
					*( Arr++ ) = *oStr;
				else
					if ( *oStr2 == '1' )
						*( Arr++ ) = (BYTE)( *oStr + 0x20 );
					else
						if ( *oStr2 == '2' )
							*( Arr++ ) = (BYTE)( *oStr - 0x20 );
			}
		else
			while ( (*( Arr++ ) = *( ++oStr )) );

	}
	return ret;
}

BYTE * GetDecArr( BYTE *Arr, const char *oStr )
{
	BYTE *ret = Arr;
	if ( Arr && oStr )
		while ( *( oStr ) )
		{
			*Arr = (BYTE)( ( *( oStr++ ) - '0' ) * 100 );
			if ( *oStr ) *Arr = (BYTE)( *Arr + ( *( oStr++ ) - '0' ) * 10 );
			if ( *oStr ) *Arr = (BYTE)( *Arr + *( oStr++ ) - '0' );
			Arr++;
		}
	return ret;
}

BYTE * GetOctArr( BYTE *Arr, const char *oStr )
{
	BYTE *ret = Arr;
	if ( Arr && oStr )
		while ( *( oStr ) )
		{
			*Arr = (BYTE)( ( *( oStr++ ) - '0' ) * 64 );
			if ( *oStr ) *Arr = (BYTE)( *Arr + ( *( oStr++ ) - '0' ) * 8 );
			if ( *oStr ) *Arr = (BYTE)( *Arr + *( oStr++ ) - '0' );
			Arr++;
		}
	return ret;
}

BYTE * GetHexArr( BYTE *Arr, const char *oStr )
{
	BYTE *ret = Arr;
	BYTE tmp1;
	BYTE tmp2;
	if ( Arr && oStr )
		while ( *( oStr ) && oStr[ 1 ] )
		{
			if ( ( tmp1 = *( oStr++ ) ) > '9' )
			{
				if ( ( tmp2 = *( oStr++ ) ) > '9' )
					*( Arr++ ) = (BYTE)( ( tmp1 - 'A' + 10 ) * 0x10 + tmp2 - 'A' + 10 );
				else
					*( Arr++ ) = (BYTE)( ( tmp1 - 'A' + 10 ) * 0x10 + tmp2 - '0' );
			}
			else
				if ( ( tmp2 = *( oStr++ ) ) > '9' )
					*( Arr++ ) = (BYTE)( ( tmp1 - '0' ) * 0x10 + tmp2 - 'A' + 10 );
				else
					*( Arr++ ) = (BYTE)( ( tmp1 - '0' ) * 0x10 + tmp2 - '0' );
		}
	return ret;
}

BYTE * convHexToByteArr( const char *iDataSrc, BYTE *ArrDataDst )
{
	BYTE *dest = ArrDataDst;
	const char *src = iDataSrc;
	int cc;

	while ( *src )
	{
		cc = *( src++ );
		if ( cc >= '0' && cc <= '9' ) *dest = (char)( ( cc - '0' ) * 16 );
		else
			if ( cc >= 'A' && cc <= 'F' ) *dest = (char)( ( cc - 'A' + 10 ) * 16 );
			else
				if ( cc >= 'a' && cc <= 'f' ) *dest = (char)( ( cc - 'a' + 10 ) * 16 );
				else { *dest = (char)cc; continue; }

				if ( *src )
				{
					cc = *( src++ );
					if ( cc >= '0' && cc <= '9' ) *dest = (char)( *dest + (char)( cc - '0' ) );
					else
						if ( cc >= 'A' && cc <= 'F' ) *dest = (char)( *dest + (char)( cc - 'A' + 10 ) );
						else
							if ( cc >= 'a' && cc <= 'f' ) *dest = (char)( *dest + (char)( cc - 'a' + 10 ) );
					src++;
				}
	}
	return ArrDataDst;
}


DWORD FindInSortArray( const char *ccFindWhat, const char **ccArray, DWORD dwItems )
{
	DWORD ret = dwItems;

	switch ( dwItems )
	{
		case 1: if ( strcmp( *ccArray, ccFindWhat ) == 0 ) ret = 0;
		case 0: break;
		default:
			{
				int low = 0, high = dwItems - 1, mid;
				while ( low <= high )
				{
					mid = ( low + high ) / 2;
					if ( strcmp( ccFindWhat, ccArray[ mid ] ) < 0 ) high = mid - 1;
					else
						if ( strcmp( ccFindWhat, ccArray[ mid ] ) > 0 ) low = mid + 1;
						else { ret = mid; break; }
				}
			}
	}
	return ret;
}

DWORD FindInSortArrayIC( const char *ccFindWhat, const char **ccArray, DWORD dwItems )
{
	DWORD ret = dwItems;

	switch ( dwItems )
	{
		case 1: if ( _stricmp( *ccArray, ccFindWhat ) == 0 ) ret = 0;
		case 0: break;
		default:
			{
				int low = 0, high = dwItems == 0 ? 0 : dwItems - 1, mid;
				while ( low <= high )
				{
					mid = ( low + high ) / 2;
					if ( _stricmp( ccFindWhat, ccArray[ mid ] ) < 0 ) high = mid - 1;
					else
						if ( _stricmp( ccFindWhat, ccArray[ mid ] ) > 0 ) low = mid + 1;
						else { ret = mid; break; }
				}
			}
	}
	return ret;
}


wchar_t * wStrReplace( wchar_t *dstStr, wchar_t DestChar, wchar_t SrcChar )
{
	wchar_t * ret = dstStr;
	if ( dstStr )
		if ( SrcChar == 0 )
		{
			wchar_t *srcStr = dstStr;
			while ( *srcStr )
				if ( *srcStr == DestChar )
					srcStr++;
				else
					*( dstStr++ ) = *( srcStr++ );

			if ( srcStr != dstStr ) *dstStr = 0;
		}
		else
			while ( *dstStr )
				if ( *dstStr == DestChar )
					*( dstStr++ ) = SrcChar;
				else
					dstStr++;

	return ret;
}

int iintMltpl[ 15 ];
DWORD dwMltpl[ 15 ];
char cMltpl[ 15 ];
WORD wMltpl[ 15 ];

wchar_t *wIntToStr( wchar_t *dstStr, char in_Value )
{
	if ( dstStr )
	{
		if ( in_Value < 0 ) { *( dstStr++ ) = '-'; in_Value = -in_Value; }
		*cMltpl = in_Value;
		char *p1 = cMltpl;
		while ( *p1 > 9 )
			p1[ 1 ] = *( p1++ ) / 10;

		*( dstStr++ ) = '0' + *( p1 );

		while ( p1-- != cMltpl )
			*( dstStr++ ) = '0' + *p1 % 10;

		*dstStr = 0;
	}
	return dstStr;
}

wchar_t *wIntToStr( wchar_t *dstStr, char in_Value, int in_nDigits )
{
	if ( dstStr )
	{
		if ( in_Value < 0 ) { *( dstStr++ ) = '-'; in_Value = -in_Value; }
		*cMltpl = in_Value;
		char *p1 = cMltpl;
		while ( --in_nDigits > 0 || *p1 > 9 )
			p1[ 1 ] = *( p1++ ) / 10;

		*( dstStr++ ) = '0' + *( p1 );

		while ( p1-- != cMltpl )
			*( dstStr++ ) = '0' + *p1 % 10;

		*dstStr = 0;
	}
	return dstStr;
}

wchar_t *wIntToStr( wchar_t *dstStr, WORD in_Value )
{
	if ( dstStr )
	{
		*wMltpl = in_Value;
		WORD *p1 = wMltpl;
		while ( *p1 > 9 )
			p1[ 1 ] = *( p1++ ) / 10;

		*( dstStr++ ) = '0' + *( p1 );

		while ( p1-- != wMltpl )
			*( dstStr++ ) = '0' + *p1 % 10;

		*dstStr = 0;
	}
	return dstStr;
}

wchar_t *wIntToStr( wchar_t *dstStr, WORD in_Value, int in_nDigits )
{
	if ( dstStr )
	{
		*wMltpl = in_Value;
		WORD *p1 = wMltpl;
		while ( --in_nDigits > 0 || *p1 > 9 )
			p1[ 1 ] = *( p1++ ) / 10;

		*( dstStr++ ) = '0' + *( p1 );

		while ( p1-- != wMltpl )
			*( dstStr++ ) = '0' + *p1 % 10;

		*dstStr = 0;
	}
	return dstStr;
}

wchar_t *wIntToStr( wchar_t *dstStr, DWORD in_Value )
{
	if ( dstStr )
	{
		*dwMltpl = in_Value;
		DWORD *p1 = dwMltpl;
		while ( *p1 > 9 )
			p1[ 1 ] = *( p1++ ) / 10;

		*( dstStr++ ) = (wchar_t)( '0' + *( p1 ) );

		while ( p1-- != dwMltpl )
			*( dstStr++ ) = '0' + *p1 % 10;

		*dstStr = 0;
	}
	return dstStr;
}

wchar_t *wIntToStr( wchar_t *dstStr, DWORD in_Value, int in_nDigits )
{
	if ( dstStr )
	{
		*dwMltpl = in_Value;
		DWORD *p1 = dwMltpl;
		while ( --in_nDigits > 0 || *p1 > 9 )
			p1[ 1 ] = *( p1++ ) / 10;

		*( dstStr++ ) = (wchar_t)( '0' + *( p1 ) );

		while ( p1-- != dwMltpl )
			*( dstStr++ ) = '0' + *p1 % 10;

		*dstStr = 0;
	}
	return dstStr;
}

wchar_t *wIntToStr( wchar_t *dstStr, int in_Value )
{
	if ( dstStr )
	{
		if ( in_Value < 0 ) { *( dstStr++ ) = '-'; in_Value = -in_Value; }
		*iintMltpl = in_Value;
		int *p1 = iintMltpl;
		while ( *p1 > 9 )
			p1[ 1 ] = *( p1++ ) / 10;

		*( dstStr++ ) = '0' + *( p1 );

		while ( p1-- != iintMltpl )
			*( dstStr++ ) = '0' + *p1 % 10;

		*dstStr = 0;
	}
	return dstStr;
}

wchar_t *wIntToStr( wchar_t *dstStr, int in_Value, int in_nDigits )
{
	if ( dstStr )
	{
		if ( in_Value < 0 ) { *( dstStr++ ) = '-'; in_Value = -in_Value; }
		*iintMltpl = in_Value;
		int *p1 = iintMltpl;
		while ( --in_nDigits > 0 || *p1 > 9 )
			p1[ 1 ] = *( p1++ ) / 10;

		*( dstStr++ ) = '0' + *( p1 );

		while ( p1-- != iintMltpl )
			*( dstStr++ ) = '0' + *p1 % 10;

		*dstStr = 0;
	}
	return dstStr;
}

wchar_t *wDblToStr( wchar_t *dstStr, double in_Value, int in_nAfter )
{
	if ( dstStr )
	{
		char tmpChar[ 1000 ]; *tmpChar = 0;
		int nFr, sign;
		_fcvt_s( tmpChar, 998, in_Value, in_nAfter, &nFr, &sign );
		tmpChar[ 999 ] = 0;
		if ( *tmpChar )
		{
			char *pC = tmpChar;
			if ( sign > 0 )  *( dstStr++ ) = '-';
			if ( nFr <= 0 )  *( dstStr++ ) = '0';
			while ( nFr > 0 ) { *( dstStr++ ) = *( pC++ ); nFr--; }
			if ( in_nAfter > 0 ) *( dstStr++ ) = '.';
			while ( nFr < 0 ) { *( dstStr++ ) = '0'; nFr++; }
			while ( in_nAfter-- > 0 ) *( dstStr++ ) = *( pC++ );
		}
		else { *( dstStr++ ) = '0'; }
		*dstStr = 0;
	}
	return dstStr;
}

wchar_t *wStrToStr( wchar_t *dstStr, const wchar_t *srcStr )
{
	if ( dstStr )
	{
		if ( srcStr )
		{
			while ( ( *( dstStr++ ) = *( srcStr++ ) ) );
			dstStr--;
		}
		*dstStr = 0;
	}

	return dstStr;
}

wchar_t *wStrToStr( wchar_t *dstStr, const char *srcStr )
{
	if ( dstStr )
	{
		if ( srcStr )
		{
			while ( ( *( dstStr++ ) = *( srcStr++ ) ) );
			dstStr--;
		}
		*dstStr = 0;
	}

	return dstStr;
}

char *wStrToStr( char *dstStr, const wchar_t *srcStr )
{
	if ( dstStr )
	{
		if ( srcStr )
		{
			while ( ( *( dstStr++ ) = ( char ) *( srcStr++ ) ) );
			dstStr--;
		}
		*dstStr = 0;
	}

	return dstStr;
}

wchar_t *wStrToStr( wchar_t *dstStr, const wchar_t *srcStr, int in_Len )
{
	if ( dstStr )
	{
		if ( srcStr && in_Len > 0 )
		{
			while ( ( *( dstStr++ ) = *( srcStr++ ) ) && in_Len-- );

			dstStr--;
		}

		*dstStr = 0;
	}

	return dstStr;
}

wchar_t *wStrToStr( wchar_t *dstStr, const char *srcStr, int in_Len )
{
	if ( dstStr )
	{
		if ( srcStr && in_Len > 0 )
		{
			while ( ( *( dstStr++ ) = *( srcStr++ ) ) && in_Len-- );

			dstStr--;
		}

		*dstStr = 0;
	}

	return dstStr;
}

char *wStrToStr( char *dstStr, const wchar_t *srcStr, int in_Len )
{
	if ( dstStr )
	{
		if ( srcStr && in_Len > 0 )
		{
			while ( ( *( dstStr++ ) = ( char )*( srcStr++ ) ) && in_Len-- );

			dstStr--;
		}

		*dstStr = 0;
	}

	return dstStr;
}


char * wStrReplace( char *dstStr, char DestChar, char SrcChar )
{
	char * ret = dstStr;
	if ( dstStr )
		if ( SrcChar == 0 )
		{
			char *srcStr = dstStr;
			while ( *srcStr )
				if ( *srcStr == DestChar )
					srcStr++;
				else
					*( dstStr++ ) = *( srcStr++ );

			if ( srcStr != dstStr ) *dstStr = 0;
		}
		else
			while ( *dstStr )
				if ( *dstStr == DestChar )
					*( dstStr++ ) = SrcChar;
				else
					dstStr++;

	return ret;
}


char * wStrReplace(char *dstStr, int iMaxLen, char *StrFind, char *StrReplace)
{
	char * ret = dstStr;
	if (dstStr && StrFind && *StrFind)
	{
		if (!StrReplace) StrReplace = "";

		char *tmps = new char [iMaxLen];
		char *posCopy = tmps, *tmpsPos = nullptr;
		char *StrFindTmp, *dstStrCopy = dstStr;
		int iStrFindLen = 0;

		StrFindTmp = StrFind;
		while (*(StrFindTmp++)) iStrFindLen++;

		char *StopPos = tmps + iMaxLen;
		BOOL bNeedCopy = FALSE;

		while (*dstStr && posCopy < StopPos)
		{
			while (*dstStr && *dstStr != *StrFind && posCopy < StopPos)  *(posCopy++) = *(dstStr++);

			if (*dstStr && posCopy < StopPos)
			{
				StrFindTmp = StrFind;
				tmpsPos = dstStr;

				while (*StrFindTmp && *tmpsPos == *StrFindTmp) { tmpsPos++; StrFindTmp++; }

				if (!*StrFindTmp)
				{
					dstStr += iStrFindLen;
					bNeedCopy = TRUE;
					StrFindTmp = StrReplace;
					while (*StrFindTmp  && posCopy < StopPos)  *(posCopy++) = *(StrFindTmp++);
				}
				else 
					*(posCopy++) = *(dstStr++);
			}
		}


		if (bNeedCopy)
		{
			*posCopy = 0;
			posCopy = tmps;
			while (*posCopy) *(dstStrCopy++)= *(posCopy++);
			*dstStrCopy = 0;

		}
		delete []tmps;
	}

	return ret;
}


wchar_t * wStrReplace(wchar_t *dstStr, int iMaxLen, wchar_t *StrFind, wchar_t *StrReplace)
{
	wchar_t * ret = dstStr;
	if (dstStr && StrFind && *StrFind)
	{
		if (!StrReplace) StrReplace = L"";

		wchar_t *tmps = new wchar_t[iMaxLen];
		wchar_t *posCopy = tmps, *tmpsPos = nullptr;
		wchar_t *StrFindTmp, *dstStrCopy = dstStr;
		int iStrFindLen = 0;

		StrFindTmp = StrFind;
		while (*(StrFindTmp++)) iStrFindLen++;

		wchar_t *StopPos = tmps + iMaxLen;
		BOOL bNeedCopy = FALSE;

		while (*dstStr && posCopy < StopPos)
		{
			while (*dstStr && *dstStr != *StrFind && posCopy < StopPos)  *(posCopy++) = *(dstStr++);

			if (*dstStr && posCopy < StopPos)
			{
				StrFindTmp = StrFind;
				tmpsPos = dstStr;

				while (*StrFindTmp && *tmpsPos == *StrFindTmp) { tmpsPos++; StrFindTmp++; }

				if (!*StrFindTmp)
				{
					dstStr += iStrFindLen;
					bNeedCopy = TRUE;
					StrFindTmp = StrReplace;
					while (*StrFindTmp  && posCopy < StopPos)  *(posCopy++) = *(StrFindTmp++);
				}
				else 
					*(posCopy++) = *(dstStr++);
			}
		}


		if (bNeedCopy)
		{
			*posCopy = 0;
			posCopy = tmps;
			while (*posCopy) *(dstStrCopy++)= *(posCopy++);
			*dstStrCopy = 0;

		}
		delete []tmps;
	}

	return ret;
}

char * wStrReplaceDbl( char *dstStr )
{
	char *ret = dstStr;

	if ( dstStr && *dstStr )
		while ( *( ++dstStr ) )
			if ( *dstStr == ',' && *( dstStr - 1 ) <= '9'  && *( dstStr + 1 ) <= '9'  && *( dstStr - 1 ) >= '0'  && *( dstStr + 1 ) >= '0' )
				*dstStr = '.';

	return ret;
}


char *wIntToStr( char *dstStr, char in_Value )
{
	if ( dstStr )
	{
		if ( in_Value < 0 ) { *( dstStr++ ) = '-'; in_Value = -in_Value; }
		*cMltpl = in_Value;
		char *p1 = cMltpl;
		while ( *p1 > 9 )
			p1[ 1 ] = *( p1++ ) / 10;

		*( dstStr++ ) = '0' + *( p1 );

		while ( p1-- != cMltpl )
			*( dstStr++ ) = '0' + *p1 % 10;

		*dstStr = 0;
	}
	return dstStr;
}

char *wIntToStr( char *dstStr, char in_Value, int in_nDigits )
{
	if ( dstStr )
	{
		if ( in_Value < 0 ) { *( dstStr++ ) = '-'; in_Value = -in_Value; }
		*cMltpl = in_Value;
		char *p1 = cMltpl;
		while ( --in_nDigits > 0 || *p1 > 9 )
			p1[ 1 ] = *( p1++ ) / 10;

		*( dstStr++ ) = '0' + *( p1 );

		while ( p1-- != cMltpl )
			*( dstStr++ ) = '0' + *p1 % 10;

		*dstStr = 0;
	}
	return dstStr;
}

char *wIntToStr( char *dstStr, WORD in_Value )
{
	if ( dstStr )
	{
		*wMltpl = in_Value;
		WORD *p1 = wMltpl;
		while ( *p1 > 9 )
			p1[ 1 ] = *( p1++ ) / 10;

		*( dstStr++ ) = '0' + *( p1 );

		while ( p1-- != wMltpl )
			*( dstStr++ ) = '0' + *p1 % 10;

		*dstStr = 0;
	}
	return dstStr;
}

char *wIntToStr( char *dstStr, WORD in_Value, int in_nDigits )
{
	if ( dstStr )
	{
		*wMltpl = in_Value;
		WORD *p1 = wMltpl;
		while ( --in_nDigits > 0 || *p1 > 9 )
			p1[ 1 ] = *( p1++ ) / 10;

		*( dstStr++ ) = '0' + *( p1 );

		while ( p1-- != wMltpl )
			*( dstStr++ ) = '0' + *p1 % 10;

		*dstStr = 0;
	}
	return dstStr;
}

char *wIntToStr( char *dstStr, DWORD in_Value )
{
	if ( dstStr )
	{
		*dwMltpl = in_Value;
		DWORD *p1 = dwMltpl;
		while ( *p1 > 9 )
			p1[ 1 ] = *( p1++ ) / 10;

		*( dstStr++ ) = (char)( '0' + *( p1 ) );

		while ( p1-- != dwMltpl )
			*( dstStr++ ) = '0' + *p1 % 10;

		*dstStr = 0;
	}
	return dstStr;
}

char *wIntToStr( char *dstStr, DWORD in_Value, int in_nDigits )
{
	if ( dstStr )
	{
		*dwMltpl = in_Value;
		DWORD *p1 = dwMltpl;
		while ( --in_nDigits > 0 || *p1 > 9 )
			p1[ 1 ] = *( p1++ ) / 10;

		*( dstStr++ ) = (char)( '0' + *( p1 ) );

		while ( p1-- != dwMltpl )
			*( dstStr++ ) = '0' + *p1 % 10;

		*dstStr = 0;
	}
	return dstStr;
}

char *wIntToStr( char *dstStr, int in_Value )
{
	if ( dstStr )
	{
		if ( in_Value < 0 ) { *( dstStr++ ) = '-'; in_Value = -in_Value; }
		*iintMltpl = in_Value;
		int *p1 = iintMltpl;
		while ( *p1 > 9 )
			p1[ 1 ] = *( p1++ ) / 10;

		*( dstStr++ ) = '0' + *( p1 );

		while ( p1-- != iintMltpl )
			*( dstStr++ ) = '0' + *p1 % 10;

		*dstStr = 0;
	}
	return dstStr;
}

char *wIntToStr( char *dstStr, int in_Value, int in_nDigits )
{
	if ( dstStr )
	{
		if ( in_Value < 0 ) { *( dstStr++ ) = '-'; in_Value = -in_Value; }
		*iintMltpl = in_Value;
		int *p1 = iintMltpl;
		while ( --in_nDigits > 0 || *p1 > 9 )
			p1[ 1 ] = *( p1++ ) / 10;

		*( dstStr++ ) = '0' + *( p1 );

		while ( p1-- != iintMltpl )
			*( dstStr++ ) = '0' + *p1 % 10;

		*dstStr = 0;
	}
	return dstStr;
}

const wchar_t cwHexCharsArr[ 16 ] = { '0', '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  'A',  'B',  'C',  'D',  'E',  'F' };
const char ccHexCharsArr[ 16 ] = { '0', '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  'A',  'B',  'C',  'D',  'E',  'F' };

wchar_t *wHexToStr( wchar_t *dstStr, DWORD in_Value )
{
	if ( dstStr )
	{
		if ( in_Value == 0 )
		{
			*( dstStr++ ) = '0';
			*dstStr = 0;
		}
		else
			if ( in_Value < 16 )
			{
				*( dstStr++ ) = cwHexCharsArr[ in_Value ];
				*dstStr = 0;
			}
			else
			{
				*dwMltpl = in_Value;
				DWORD *p1 = dwMltpl;
				while ( *p1 > 15 )
					p1[ 1 ] = *( p1++ ) / 16;

				*( dstStr++ ) = cwHexCharsArr[ *p1 ];

				while ( p1-- != dwMltpl )
					*( dstStr++ ) = cwHexCharsArr[ *p1 & 15 ];

				*dstStr = 0;
			}
	}
	return dstStr;
}

wchar_t *wHexToStr( wchar_t *dstStr, DWORD in_Value, int in_nDigits )
{
	if ( dstStr )
	{
		*dwMltpl = in_Value;
		DWORD *p1 = dwMltpl;
		while ( --in_nDigits > 0 || *p1 > 15 )
			p1[ 1 ] = *( p1++ ) / 16;

		*( dstStr++ ) = cwHexCharsArr[ *p1 ];

		while ( p1-- != dwMltpl )
			*( dstStr++ ) = cwHexCharsArr[ *p1 & 15 ];

		*dstStr = 0;
	}
	return dstStr;
}


char *wHexToStr( char *dstStr, DWORD in_Value )
{
	if ( dstStr )
	{
		if ( in_Value == 0 )
		{
			*( dstStr++ ) = '0';
			*dstStr = 0;
		}
		else
			if ( in_Value < 16 )
			{
				*( dstStr++ ) = ccHexCharsArr[ in_Value ];
				*dstStr = 0;
			}
			else
			{
				*dwMltpl = in_Value;
				DWORD *p1 = dwMltpl;
				while ( *p1 > 15 )
					p1[ 1 ] = *( p1++ ) / 16;

				*( dstStr++ ) = ccHexCharsArr[ *p1 ];

				while ( p1-- != dwMltpl )
					*( dstStr++ ) = ccHexCharsArr[ *p1 & 15 ];

				*dstStr = 0;
			}
	}
	return dstStr;
}

char *wHexToStr( char *dstStr, DWORD in_Value, int in_nDigits )
{
	if ( dstStr )
	{
		*dwMltpl = in_Value;
		DWORD *p1 = dwMltpl;
		while ( --in_nDigits > 0 || *p1 > 15 )
			p1[ 1 ] = *( p1++ ) / 16;

		*( dstStr++ ) = ccHexCharsArr[ *p1 ];

		while ( p1-- != dwMltpl )
			*( dstStr++ ) = ccHexCharsArr[ *p1 & 15 ];

		*dstStr = 0;
	}
	return dstStr;
}

char *wDblToStr( char *dstStr, double in_Value, int in_nAfter )
{
	if ( dstStr )
	{
		char tmpChar[ 100 ]; *tmpChar = 0;
		int nFr, sign;
		_fcvt_s( tmpChar, 98, in_Value, in_nAfter, &nFr, &sign );
		tmpChar[ 99 ] = 0;
		if ( *tmpChar )
		{
			char *pC = tmpChar;
			if ( sign > 0 )  *( dstStr++ ) = '-';
			if ( nFr <= 0 )  *( dstStr++ ) = '0';
			while ( nFr > 0 ) { *( dstStr++ ) = *( pC++ ); nFr--; }
			if ( in_nAfter > 0 ) *( dstStr++ ) = '.';
			while (nFr < 0) { *(dstStr++) = '0'; nFr++; in_nAfter--; }
			while ( in_nAfter-- > 0 ) *( dstStr++ ) = *( pC++ );
		}
		else { *( dstStr++ ) = '0'; }
		*dstStr = 0;
	}
	return dstStr;
}

char *wStrToStr( char *dstStr, const char *srcStr )
{
	if (dstStr)
	{
		if (srcStr)
		{
			while ((*(dstStr++) = *(srcStr++)));
			dstStr--;
		}
		*dstStr = 0;
	}

	return dstStr;
}

char *wStrToStrEx(char *dstStr, const char *srcStr)
{
	if ( dstStr )
	{
		if ( srcStr )
		{
			while ((*dstStr = *(srcStr++)) && *dstStr != '\r' && *dstStr != '\n' && *dstStr != '\t' && *dstStr != ' ')
				dstStr++;
		}
		*dstStr = 0;
	}

	return dstStr;
}


wchar_t *wStrToStrEx(wchar_t *dstStr, const char *srcStr)
{
	if ( dstStr )
	{
		if ( srcStr )
		{
			while ((*dstStr = *(srcStr++)) && *dstStr != '\r' && *dstStr != '\n' && *dstStr != '\t' && *dstStr != ' ')
				dstStr++;
		}
		*dstStr = 0;
	}

	return dstStr;
}

char *wStrToStr( char *dstStr, const char *srcStr, int in_Len )
{
	if ( dstStr )
	{
		if ( srcStr && in_Len > 0 )
		{
			while ( ( *( dstStr++ ) = *( srcStr++ ) ) && in_Len-- );

			dstStr--;
		}

		*dstStr = 0;
	}

	return dstStr;
}

int wStrCompare( const wchar_t *srcStr1, const wchar_t *srcStr2 )
{
	int ret = -12345678;
	if ( srcStr1 && srcStr2 )
	{
		while ( *srcStr1 && *srcStr1 == *srcStr2 ) { srcStr1++; srcStr2++; }

		ret = *srcStr1 - *srcStr2;
	}
	return ret;
}

int wStrCompare( const char *srcStr1, const wchar_t *srcStr2 )
{
	int ret = -12345678;
	if ( srcStr1 && srcStr2 )
	{
		while ( *srcStr1 && *srcStr1 == *srcStr2 ) { srcStr1++; srcStr2++; }

		ret = *srcStr1 - *srcStr2;
	}
	return ret;
}

int wStrCompare( const wchar_t *srcStr1, const char *srcStr2 )
{
	int ret = -12345678;
	if ( srcStr1 && srcStr2 )
	{
		while ( *srcStr1 && *srcStr1 == *srcStr2 ) { srcStr1++; srcStr2++; }

		ret = *srcStr1 - *srcStr2;
	}
	return ret;
}


int wStrCompare( const char *srcStr1, const char *srcStr2 )
{
	int ret = -12345678;
	if ( srcStr1 && srcStr2 )
	{
		while ( *srcStr1 && *srcStr1 == *srcStr2 ) { srcStr1++; srcStr2++; }

		ret = *srcStr1 - *srcStr2;
	}
	return ret;
}


void DrawArrow(HDC hDC, int posSX, int posSY, int posEX, int posEY, int in_ArrLen)
{
	if (hDC)
	{
		if (posEX == posSX && posEY == posSY)
		{
			MoveToEx(hDC, posSX, posSY, nullptr);
			LineTo(hDC, posEX + 1, posEY);
			LineTo(hDC, posEX + 1, posEY + 1);
			LineTo(hDC, posEX, posEY + 1);
			LineTo(hDC, posEX, posEY);
		}
		else
		{
			MoveToEx(hDC, posSX, posSY, nullptr);
			LineTo(hDC, posEX, posEY);

			// Вычисляем направление стрелки
			int dx = posEX - posSX;
			int dy = posEY - posSY;
			double length = (int)sqrt(dx * dx + dy * dy);
			double arrowSize = in_ArrLen / length;

			// Нормализуем вектор
			dx = (int)((double)dx * arrowSize);
			dy = (int)((double)dy * arrowSize);

			// Вычисляем точки для стрелки
			MoveToEx(hDC, posEX, posEY, nullptr);
			LineTo(hDC, posEX - (dx - dy), posEY - (dy + dx));
			MoveToEx(hDC, posEX, posEY, nullptr);
			LineTo(hDC, posEX - (dx + dy), posEY - (dy - dx));
		}
	}
}


//A*X*X + B*X + C = 0
TwoDoubleRoots QuadraticEequationSolution(double A, double B, double C)
{
	TwoDoubleRoots ret;
	A += A;
	// Вычисляем дискриминант
	double tmp = A * C;
	double discriminant = B * B - tmp - tmp;

	if (A == 0.0)
	{
		ret.root2.Bits = 0x7FF8000000000000ULL;
		if (B == 0.0) ret.root1.Bits = 0x7FF8000000000000ULL;
		else          ret.root1.Dbl = -C / B;
	}
	else
		if (discriminant > 0.0)
		{ // Два различных действительных корня
			double sqrtDiscr = sqrt(discriminant);

			ret.root1.Dbl = (-B + sqrtDiscr) / A;
			ret.root2.Dbl = (-B - sqrtDiscr) / A;
		}
		else
			if (discriminant == 0)
			{ // Уравнение имеет один действительный корень
				ret.root1.Dbl = -B / A;
				ret.root2.Bits = 0x7FF8000000000000ULL;
			}
			else // if (discriminant < 0.0) 
			{ // Комплексные корни
				ret.root1.Bits = 0x7FF8000000000000ULL;
				ret.root2.Bits = 0x7FF8000000000000ULL;
				//realPart = -b / A;
				//imaginaryPart = sqrt(-discriminant) / A;
			}

	return ret;
}

//A*X*X + B*X + C = 0
double QuadraticEequationSolutionNOpt(double A, double B, double C)
{
	DoubleBitsUnion ret;

	// Вычисляем дискриминант
	double discriminant = B * B - 4.0 * A * C;

	if (A == 0.0)
	{
		if (B == 0.0)
			ret.Bits = 0x7FF8000000000000ULL;
		else
			ret.Dbl = -C / B;
	}
	else
		if (discriminant > 0.0)
		{ // Два различных действительных корня
			ret.Dbl = (-B + sqrt(discriminant)) / (2 * A);
			if (ret.Dbl < 0)
				ret.Dbl = (-B - sqrt(discriminant)) / (2 * A);
		}
		else
			if (discriminant == 0)
			{ // Уравнение имеет один действительный корень
				ret.Dbl = -B / (2 * A);
			}
			else // if (discriminant < 0.0) 
			{ // Комплексные корни
				ret.Bits = 0x7FF8000000000000ULL;
				//realPart = -b / (2 * A);
				//imaginaryPart = sqrt(-discriminant) / (2 * A);
			}

	return ret.Dbl;
}


TwoDoubleRoots GetLinearViewTime222(const SData& BaseParticle, const SData& ViewParticle)
{
	//=================================Eequation
	//(p1.x - (p2.x + p2.vx * t))^2 + (p1.y - (p2.y + p2.vy * t))^2 = (t - dt)^2 
	//(p1.x - (p2.x + p2.vx * t))^2 + (p1.y - (p2.y + p2.vy * t))^2 = t^2 - 2 * t * dt + dt^2 
	// p1.x^2 - 2 * p1.x*(p2.x + p2.vx * t) + (p2.x + p2.vx * t)^2 + p1.y^2 - 2*p1.y*(p2.y + p2.vy * t)+(p2.y + p2.vy * t)^2 -t^2 + 2 * t * dt - dt^2 
	//=================================
	//(p1.x  - p2.x) ^ 2 + (p1.y - p2.y) ^ 2 	- dt ^ 2
	//2*t*( p2.vx*(p2.x - p1.x) + p2.vy*(p2.y - p1.y) + dt)
	// t ^ 2 * (p2.vx ^ 2+ p2.vy ^ 2- 1)
	//=================================
	// A = (p2.vx ^ 2+ p2.vy ^ 2- 1)
	// B = 2*( p2.vx*(p2.x - p1.x) + p2.vy*(p2.y - p1.y) + dt)
	// C = (p1.x  - p2.x) ^ 2 + (p1.y - p2.y) ^ 2 	- dt ^ 2
	//=================================
	// A = p2v^2- 1
	// B = 2*( p2.vx*(p2.x - p1.x) + p2.vy*(p2.y - p1.y) + dt)
	// C = dist_p1p2 ^ 2 - dt ^ 2
	//=================================
	// A = ViewParticle.Velocity.LENPWR2()- 1.0
	// B = 2 * ( p2.vx*(p2.x - p1.x) + p2.vy*(p2.y - p1.y) + dt)
	// C = dist_p1p2 ^ 2 - dt ^ 2
	//=================================


	double dt = BaseParticle.TimePoint - ViewParticle.TimePoint;
	
	//double A = ViewParticle.Velocity.LENPWR2() - 1.0;
	
	SPos dist = ViewParticle.Position;
	dist.MINUS(BaseParticle.Position);
	double C = dist.LENPWR2() - dt * dt;

	double B = dist.DOT(ViewParticle.Velocity) + dt;
	B += B;
	

	TwoDoubleRoots ret = QuadraticEequationSolution(ViewParticle.Velocity.LENPWR2() - 1.0, B, C);

	dist= ViewParticle.Velocity;
	dist.MULT(ret.root1.Dbl);
	dist.ADD(ViewParticle.Position);

	return QuadraticEequationSolution(ViewParticle.Velocity.LENPWR2() - 1.0, B, C);
}

//Функция вычисления точки на прямой как проекции на прямую произвольеной точки PointToProjection. Возвращает время если LineDirection это скорость
//PointOnLine - точка на прямой. 
//LineDirection - вектор направления прямой из (0.0.0)
//PointToProjection 
double GetProjectionPointOnLine(SPos &retPoint, const SPos &PointOnLine, const SPos LineDirection, const SPos &PointToProjection)
{
	double DenominatorAndTime = LineDirection.DOT_ME(); //LENPWR2

	if (DenominatorAndTime != 0.0)
	{
		SPos VPB = PointToProjection;
		VPB -= PointOnLine;
		DenominatorAndTime = VPB.DOT(LineDirection) / DenominatorAndTime;

		// Координаты основания перпендикуляра
		retPoint = LineDirection;
		retPoint *= DenominatorAndTime;
		retPoint += PointOnLine;
	}
	else retPoint = PointOnLine;
	//<0 == сближение
	return -DenominatorAndTime;
}

double GetLinearViewTime(const SData& BaseParticle, const SData& ViewParticle)
{
	//lim(inf) =  (sqrt(D_pwr2 + H_pwr2 * (1.0 - V_pwr2)) - D * V) / (1.0 - V_pwr2);
	SPos VP_AtTime = ViewParticle.Velocity;
	VP_AtTime *= BaseParticle.TimePoint - ViewParticle.TimePoint;
	VP_AtTime += ViewParticle.Position;

	SPos ProjPoint;
	double TimeToNorm = GetProjectionPointOnLine(ProjPoint, VP_AtTime, ViewParticle.Velocity, BaseParticle.Position);
	double V_pwr2 = ViewParticle.Velocity.LENPWR2();
	double V = sqrt(V_pwr2);
	if (TimeToNorm < 0) V = -V;
	double D_pwr2 = ProjPoint.GetDistancePwr2(VP_AtTime);
	double D = sqrt(D_pwr2);
	double H_pwr2 = ProjPoint.GetDistancePwr2(BaseParticle.Position);
	//double H = sqrt(H_pwr2);

	if (fabs(TimeToNorm) < 1.0e-15) TimeToNorm = 0.0;
	if (H_pwr2 < 1.0e-14) H_pwr2 = 0.0;
	if (D_pwr2 < 1.0e-14) D_pwr2 = D = 0.0;
	if (V_pwr2 < 1.0e-14) V_pwr2 = V = 0.0;

	double SS = 0.0;
	if (V_pwr2 >= 1.0)
	{
		if (H_pwr2 == 0.0)
		{
			if (D_pwr2 == 0.0) SS = 0.0;
			else if (V > 0.0)		SS = SP_MAX_TIME_DISTANCE;
			else					SS = -SP_MAX_TIME_DISTANCE;
		}
		else
		{
			if (TimeToNorm <= 0.0)	SS = -SP_MAX_TIME_DISTANCE;
			else if (D_pwr2 >= H_pwr2)		SS = -(D_pwr2 - H_pwr2) * 0.5 / D - D;
			else					SS = -(D_pwr2 + H_pwr2) * 0.5 / D;
		}
	}
	else
		SS = -(sqrt(D_pwr2 + H_pwr2 * (1.0 - V_pwr2)) - D * V) / (1.0 - V_pwr2);

	return SS;
}



double CalcTheta(SPos& k1, SPos& k2, SPos& k3)
{
	SPos tmp1 = k2; tmp1 -= k3;
	SPos tmp2 = k1; tmp1 -= k2;
	double minTh = 0.1;
	double maxTh = 0.01;
	double tmpVal = 0.15;

	if (tmp2.X != 0.0)
	{
		tmpVal = fabs(tmp1.X / tmp2.X);
		if (tmpVal < minTh) minTh = tmpVal;
		if (tmpVal > maxTh) maxTh = tmpVal;
	}
	if (tmp2.Y != 0.0)
	{
		tmpVal = fabs(tmp1.Y / tmp2.Y);
		if (tmpVal < minTh) minTh = tmpVal;
		if (tmpVal > maxTh) maxTh = tmpVal;
	}
	if (tmp2.Z != 0.0)
	{
		tmpVal = fabs(tmp1.Z / tmp2.Z);
		if (tmpVal < minTh) minTh = tmpVal;
		if (tmpVal > maxTh) maxTh = tmpVal;
	}

	return maxTh > 0.1 ? maxTh : minTh;
}

void LimitedItgrl_rk411(SPos &result, SPos& resultOfFirst, SPos& FirstDerivative, SPos& SecondDerivative, double LimitValue, double dt, double *Theta)
{
	SPos k1_v, k1_r, k2_v, k2_r, k3_v, k3_r, k4_v, k4_r;

	k1_v = SecondDerivative;  k1_v *= dt;
	k1_r = FirstDerivative; k1_r *= dt;

	k2_v = SecondDerivative; k2_v *= dt;
	k2_r = k1_v; k2_r *= 0.5; k2_r += FirstDerivative;	k2_r.LIMIT_BY(LimitValue);	k2_r *= dt;

	k3_v = SecondDerivative; k3_v *= dt;
	k3_r = k2_v; k3_r *= 0.5; k3_r += FirstDerivative;	k3_r.LIMIT_BY(LimitValue);	k3_r *= dt;

	k4_v = SecondDerivative;  k4_v *= dt;
	k4_r = FirstDerivative; k4_r += k3_v;	k4_r.LIMIT_BY(LimitValue);	k4_r *= dt;

	// Финальное обновление
	resultOfFirst = k1_v; resultOfFirst += k2_v; resultOfFirst += k2_v; resultOfFirst += k3_v; resultOfFirst += k3_v; resultOfFirst += k4_v; resultOfFirst *= (1.0 / 6.0);
	resultOfFirst += FirstDerivative;
	resultOfFirst.LIMIT_BY(LimitValue);

	result = k1_r; result += k2_r; result += k2_r; result += k3_r; result += k3_r; result += k4_r; result *= (1.0 / 6.0);

	if (Theta)
	{
		*Theta = CalcTheta(k1_v, k2_v, k3_v);
		if (*Theta < 0.1)
			*Theta = CalcTheta(k1_r, k2_r, k3_r);
	}
}



void LimitedItgrl_rk4(SPos &result, SPos& resultOfFirst, SPos& FirstDerivative, SPos& SecondDerivative, double LimitValue, double dt, double *Theta)
{

	SPos vel0 = FirstDerivative;

	// Шаг 1: k1
	SPos k1_pos = vel0;                   
	SPos k1_vel = SecondDerivative;         

	// Шаг 2: k2
	SPos pos_k2 = k1_pos; pos_k2 *= (dt / 2.0);
	SPos vel_k2 = k1_vel;  vel_k2 *= (dt / 2.0); vel_k2 += vel0; vel_k2.LIMIT_BY(LimitValue);
	SPos k2_pos = vel_k2;
	SPos k2_vel = SecondDerivative;

	// Шаг 3: k3
	SPos pos_k3 = k2_pos; pos_k3 *= (dt / 2.0);
	SPos vel_k3 = k2_vel; vel_k3 *= (dt / 2.0); vel_k3 += vel0; vel_k3.LIMIT_BY(LimitValue);
	SPos k3_pos = vel_k3;
	SPos k3_vel = SecondDerivative;

	// Шаг 4: k4
	SPos pos_k4 = k3_pos;  pos_k4 *= dt;
	SPos vel_k4 = k3_vel;  vel_k4 *= dt; vel_k4 += vel0;
	SPos k4_pos = vel_k4;
	SPos k4_vel = SecondDerivative;

	// Расчёт итогового перемещения (интегрирование скорости)
	result = k1_pos; result += k2_pos; result += k2_pos; result += k3_pos; result += k3_pos; result += k4_pos; result *= dt * (1.0 / 6.0);

	// Расчёт итоговой скорости (интегрирование ускорения)
	resultOfFirst = k1_vel; resultOfFirst += k2_vel; resultOfFirst += k2_vel; resultOfFirst += k3_vel; resultOfFirst += k3_vel; resultOfFirst += k4_vel; resultOfFirst *= dt * (1.0 / 6.0);
	resultOfFirst += FirstDerivative;
	resultOfFirst.LIMIT_BY(LimitValue);

	if (Theta)
	{
		*Theta = CalcTheta(k1_vel, k2_vel, k3_vel);
		if (*Theta < 0.1)
			*Theta = CalcTheta(k1_pos, k2_pos, k3_pos);
	}


	/*

	// Шаг 1: k1 — производные в начальной точке
	Vector3D k1_pos = vel0;                            // d(pos)/dt = velocity
	Vector3D k1_vel = calculateAcceleration(pos0);   // d(vel)/dt = acceleration(pos)

	// Шаг 2: k2 — прогноз на середину шага (t + dt/2)
	Vector3D pos_k2 = pos0 + k1_pos * (dt / 2.0);  // позиция через dt/2
	Vector3D vel_k2 = vel0 + k1_vel * (dt / 2.0);  // скорость через dt/2

	// Вычисляем k2 на основе прогноза pos_k2 и vel_k2
	Vector3D k2_pos = vel_k2;                        // скорость в точке pos_k2
	Vector3D k2_vel = calculateAcceleration(pos_k2); // ускорение в точке pos_k2

	// Шаг 3: k3 — уточнённый прогноз на середину шага (t + dt/2)
	Vector3D pos_k3 = pos0 + k2_pos * (dt / 2.0);  // уточнённая позиция через dt/2
	Vector3D vel_k3 = vel0 + k2_vel * (dt / 2.0);  // уточнённая скорость через dt/2

	// Вычисляем k3 на основе уточнённого прогноза pos_k3 и vel_k3
	Vector3D k3_pos = vel_k3;                        // скорость в точке pos_k3
	Vector3D k3_vel = calculateAcceleration(pos_k3); // ускорение в точке pos_k3

	// Шаг 4: k4 — прогноз на конец шага (t + dt)
	Vector3D pos_k4 = pos0 + k3_pos * dt;          // позиция через полный шаг dt
	Vector3D vel_k4 = vel0 + k3_vel * dt;         // скорость через полный шаг dt

	// Вычисляем k4 на основе прогноза pos_k4 и vel_k4
	Vector3D k4_pos = vel_k4;                        // скорость в точке pos_k4
	Vector3D k4_vel = calculateAcceleration(pos_k4); // ускорение в точке pos_k4

	// Итоговое обновление позиции и скорости
	finalPosition = pos0 + (k1_pos + 2.0 * k2_pos + 2.0 * k3_pos + k4_pos) * (dt / 6.0);
	finalVelocity = vel0 + (k1_vel + 2.0 * k2_vel + 2.0 * k3_vel + k4_vel) * (dt / 6.0);

	// Ограничение скорости в конце расчёта
	double finalMag = finalVelocity.magnitude();
	if (finalMag > Vmax && finalMag > 1e-10) {
		finalVelocity = finalVelocity.normalize() * Vmax;
	}

	*/

}




//
//void LimitedItgrl_rk4(SPos& result, SPos& resultOfFirst, SPos& FirstDerivative, SPos& SecondDerivative, double LimitValue, double dt, double* Theta)
//{
//	SPos k1_v, k1_r, k2_v, k2_r, k3_v, k3_r, k4_v, k4_r, v_temp, r_temp;
//
//	// Этап 1
//	k1_v = SecondDerivative;  k1_v *= dt;
//	k1_r = FirstDerivative; k1_r *= dt;
//
//	// Этап 2
//	v_temp = k1_v; v_temp *= 0.5; v_temp += FirstDerivative;
//	v_temp.LIMIT_BY(LimitValue);
//
//	/////r_temp = k1_r; r_temp*=0.5;
//	k2_v = SecondDerivative; k2_v *= dt;
//	k2_r = v_temp; k2_r *= dt;
//
//	// Этап 3
//	v_temp = k2_v; v_temp *= 0.5; v_temp += FirstDerivative;
//	v_temp.LIMIT_BY(LimitValue);
//	/////r_temp = k2_r; r_temp *= 0.5;
//
//	k3_v = SecondDerivative; k3_v *= dt;
//	k3_r = v_temp; k3_r *= dt;
//
//	// Этап 4
//	v_temp = FirstDerivative; v_temp += k3_v;
//
//	v_temp.LIMIT_BY(LimitValue);
//	////r_temp = k3_r;
//	k4_v = SecondDerivative;  k4_v *= dt;
//	k4_r = v_temp; k4_r *= dt;
//
//	// Финальное обновление
//
//	resultOfFirst = k1_v; resultOfFirst += k2_v; resultOfFirst += k2_v; resultOfFirst += k3_v; resultOfFirst += k3_v; resultOfFirst += k4_v; resultOfFirst *= (1.0 / 6.0);
//	resultOfFirst += FirstDerivative;
//	//	velocity = FirstDerivative + (k1_v + k2_v + k3_v + k4_v) / 6.0;
//	//	velocity.LIMIT_BY(LimitValue);// финальное ограничение
//	resultOfFirst.LIMIT_BY(LimitValue);
//	result = k1_r; result += k2_r; result += k2_r; result += k3_r; result += k3_r; result += k4_r; result *= (1.0 / 6.0);
//
//	if (Theta)
//	{
//		SPos tmp1 = k2_v; tmp1 -= k3_v;
//		SPos tmp2 = k1_v; tmp1 -= k2_v;
//		double minTh = 0.1;
//		double maxTh = 0.01;
//		double tmpVal = 0.15;
//
//		if (tmp2.X != 0.0)
//		{
//			tmpVal = fabs(tmp1.X / tmp2.X);
//			if (tmpVal < minTh) minTh = tmpVal;
//			if (tmpVal > maxTh) maxTh = tmpVal;
//		}
//		if (tmp2.Y != 0.0)
//		{
//			tmpVal = fabs(tmp1.Y / tmp2.Y);
//			if (tmpVal < minTh) minTh = tmpVal;
//			if (tmpVal > maxTh) maxTh = tmpVal;
//		}
//		if (tmp2.Z != 0.0)
//		{
//			tmpVal = fabs(tmp1.Z / tmp2.Z);
//			if (tmpVal < minTh) minTh = tmpVal;
//			if (tmpVal > maxTh) maxTh = tmpVal;
//		}
//
//		if (maxTh < 0.1)
//		{
//			tmp1 = k2_r; tmp1 -= k3_r;
//			tmp2 = k1_r; tmp1 -= k2_r;
//
//			if (tmp2.X != 0.0)
//			{
//				tmpVal = fabs(tmp1.X / tmp2.X);
//				if (tmpVal < minTh) minTh = tmpVal;
//				if (tmpVal > maxTh) maxTh = tmpVal;
//			}
//			if (tmp2.Y != 0.0)
//			{
//				tmpVal = fabs(tmp1.Y / tmp2.Y);
//				if (tmpVal < minTh) minTh = tmpVal;
//				if (tmpVal > maxTh) maxTh = tmpVal;
//			}
//			if (tmp2.Z != 0.0)
//			{
//				tmpVal = fabs(tmp1.Z / tmp2.Z);
//				if (tmpVal < minTh) minTh = tmpVal;
//				if (tmpVal > maxTh) maxTh = tmpVal;
//			}
//		}
//
//		if (maxTh > 0.1)
//			*Theta = maxTh;
//		else
//			*Theta = minTh;
//	}
//}

void test_LinearVP()
{
	SData BaseParticle = { 0 }, ViewParticle = { 0 };

	double VPxArr[11] = { 0.0, -0.1, -0.2, -0.5, -0.99999, -1.0, -1.5, -2.0, -10.0 , -100.0 };
	double VPyArr[11] = { 0.0, 0.1, 0.2, 0.5, 0.75, 0.99999, 1.0, 1.5, 2.0, 10.0, -100.0 };
	double VVelArr[11] = { 1.0,-1.0,0.0, 0.1,-0.1, 0.5,-0.5, 0.75,-0.75, 0.99999,-0.99999, };


	BaseParticle.Position.X = 2;
	BaseParticle.Position.Y = 0;
	BaseParticle.Position.Z = 0;

	BaseParticle.TimePoint = 0.0;

	FILE* tmp;
	fopen_s(&tmp, "test_LinearVP.txt", "wt");
	fprintf(tmp, "VParticle.Pos.X\tVParticle.Pos.Y\tVParticle.Vel.X\tVP_AtTime.X\tD\tH\tS\tV\tTimeToNorm\tSS\tSS_M2\tSS-SS_M2\n");


	for (int Px = 0; Px < 11; Px++)
		for (int Py = 0; Py < 11; Py++)
			for (int Vx = 0; Vx < 11; Vx++)
			{
				ViewParticle.Position.X = VPxArr[Px];
				ViewParticle.Position.Y = VPyArr[Py];
				ViewParticle.Velocity.X = VVelArr[Vx];

				//аналитический метод
				////////////////////////////////////////////////////////////////////////
				SPos VP_AtTime = ViewParticle.Velocity;
				VP_AtTime *= BaseParticle.TimePoint - ViewParticle.TimePoint;
				VP_AtTime += ViewParticle.Position;

				SPos ProjPoint;
				double TimeToNorm = GetProjectionPointOnLine(ProjPoint, VP_AtTime, ViewParticle.Velocity, BaseParticle.Position);
				double V_pwr2 = ViewParticle.Velocity.LENPWR2();
				double V = sqrt(V_pwr2);
				if (TimeToNorm < 0) V = -V;
				double D_pwr2 = ProjPoint.GetDistancePwr2(VP_AtTime);
				double D = sqrt(D_pwr2);
				double H_pwr2 = ProjPoint.GetDistancePwr2(BaseParticle.Position);
				//double H = sqrt(H_pwr2);

				if (fabs(TimeToNorm) < 1.0e-15) TimeToNorm = 0.0;
				if (H_pwr2 < 1.0e-14) H_pwr2 = 0.0;
				if (D_pwr2 < 1.0e-14) D_pwr2 = D = 0.0;
				if (V_pwr2 < 1.0e-14) V_pwr2 = V = 0.0;

				double SS = 0.0;
				if (V_pwr2 >= 1.0)
				{
					if (H_pwr2 == 0.0)
					{
						if (D_pwr2 == 0.0) SS = 0.0;
						else if (V > 0.0)		SS = SP_MAX_TIME_DISTANCE;
						else					SS = -SP_MAX_TIME_DISTANCE;
					}
					else
					{
						if (TimeToNorm <= 0.0)	SS = -SP_MAX_TIME_DISTANCE;
						else if (D_pwr2 >= H_pwr2)		SS = -(D_pwr2 - H_pwr2) * 0.5 / D - D;
						else					SS = -(D_pwr2 + H_pwr2) * 0.5 / D;
					}
				}
				else
					SS = - (sqrt(D_pwr2 + H_pwr2 * (1.0 - V_pwr2)) - D * V) / (1.0 - V_pwr2);
				
				//////////////////////////////////////////////////////////////////////////////////

				double SS_M2;
				double S = VP_AtTime.GetDistance(BaseParticle.Position);

				{//счетный метод /2
					SData UpBrd = ViewParticle;
					UpBrd.Position = VP_AtTime;
					UpBrd.TimePoint = BaseParticle.TimePoint;
					SData DnBrd;
					DnBrd = UpBrd;

					double DnMove = S * (1.0 - V);
					UpBrd.LinearMove(-DnMove);
					if (V > 0.0)
					{
						DnMove = S / (1 + V) + fabs(V);
					}
					else if (V < 0.0)
					{
						DnMove = S / (1 + V) + fabs(V);
					}
					else
						DnMove = S;


					if (DnMove > 1.0e10) DnMove = 1.0e10;

					DnBrd.LinearMove(-DnMove);
				
					SData MidBrd;
					double dist;

					do
					{
						MidBrd = DnBrd;
						MidBrd.LinearMove((UpBrd.TimePoint - DnBrd.TimePoint) * 0.5);
						dist = BaseParticle.Position.GetDistance(MidBrd.Position);
						if (dist > BaseParticle.TimePoint - MidBrd.TimePoint)
							UpBrd = MidBrd;
						else
							if (dist < BaseParticle.TimePoint - MidBrd.TimePoint)
								DnBrd = MidBrd;
							else
							{
								UpBrd = DnBrd = MidBrd;
							}
					} while (UpBrd.TimePoint - DnBrd.TimePoint > 1.0e-14 * (fabs(UpBrd.TimePoint)));

					SS_M2 = MidBrd.TimePoint;
					if (dist > BaseParticle.TimePoint - MidBrd.TimePoint + 1.0e-12)
						SS_M2 = -SP_MAX_TIME_DISTANCE;
					else
						if (dist < BaseParticle.TimePoint - MidBrd.TimePoint - 1.0e-12)
							SS_M2 = SP_MAX_TIME_DISTANCE;
						else
							if (fabs(V) == 1.0 && (dist > S + 1.0e-12 || dist < S - 1.0e-12))
							{
								SS_M2 = SP_MAX_TIME_DISTANCE;
								if (V > 0)
									SS_M2 = SP_MAX_TIME_DISTANCE;
								else
									SS_M2 = -SP_MAX_TIME_DISTANCE;
							}
							else if (fabs(V) < 1.0e-12)
								SS_M2 = -S;
				}

				double dlt = SS_M2 - SS;
				if (fabs(dlt) < 1.0e-11) dlt = 0.0;

				fprintf(tmp, "%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\n",

					ViewParticle.Position.X,
					ViewParticle.Position.Y,
					ViewParticle.Velocity.X,
					VP_AtTime.X,
					D,
					sqrt(H_pwr2),
					S,
					V,
					TimeToNorm,
					SS,
					SS_M2,
					dlt
				);
			}
	fclose(tmp);

	exit(0);
}



void my_Test()
{
	test_LinearVP();
}

