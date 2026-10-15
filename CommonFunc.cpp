// GP_term.cpp : Defines the class behaviors for the application.
//


#include "pch.h"
#include "CommonFunc.h"


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
			//else
			//	if (*inout_str >= 'а' && *inout_str <= 'я')
			//		*inout_str -= 'а' - 'А';

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
			//else
			//	if (*inout_str >= 'А' && *inout_str <= 'Я')
			//		*inout_str += 'а' - 'А';

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
		VariantCopy( &x, Item + ( left + right ) / 2 );
		do
		{
			while ( wcscmp( Item[ i ].bstrVal, x.bstrVal ) < 0 && i < right )
				i++;
			while ( wcscmp( x.bstrVal, Item[ j ].bstrVal ) < 0 && j > left )
				j--;

			if ( i <= j )
			{
				VariantCopy( &y, Item + i );
				VariantCopy( Item + i, Item + j );
				VariantCopy( Item + j, &y );
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
	if (destStr && delim && *delim)
	{
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
	if (dstStr)
	{
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
	}

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
	//(p1.x - (p2.x + p2.vx * t))^2 + (p1.y - (p2.y + p2.vy * t))^2 = (t - deltaTime)^2 
	//(p1.x - (p2.x + p2.vx * t))^2 + (p1.y - (p2.y + p2.vy * t))^2 = t^2 - 2 * t * deltaTime + deltaTime^2 
	// p1.x^2 - 2 * p1.x*(p2.x + p2.vx * t) + (p2.x + p2.vx * t)^2 + p1.y^2 - 2*p1.y*(p2.y + p2.vy * t)+(p2.y + p2.vy * t)^2 -t^2 + 2 * t * deltaTime - deltaTime^2 
	//=================================
	//(p1.x  - p2.x) ^ 2 + (p1.y - p2.y) ^ 2 	- deltaTime ^ 2
	//2*t*( p2.vx*(p2.x - p1.x) + p2.vy*(p2.y - p1.y) + deltaTime)
	// t ^ 2 * (p2.vx ^ 2+ p2.vy ^ 2- 1)
	//=================================
	// A = (p2.vx ^ 2+ p2.vy ^ 2- 1)
	// B = 2*( p2.vx*(p2.x - p1.x) + p2.vy*(p2.y - p1.y) + deltaTime)
	// C = (p1.x  - p2.x) ^ 2 + (p1.y - p2.y) ^ 2 	- deltaTime ^ 2
	//=================================
	// A = p2v^2- 1
	// B = 2*( p2.vx*(p2.x - p1.x) + p2.vy*(p2.y - p1.y) + deltaTime)
	// C = dist_p1p2 ^ 2 - deltaTime ^ 2
	//=================================
	// A = ViewParticle.Velocity_Linear.LENPWR2()- 1.0
	// B = 2 * ( p2.vx*(p2.x - p1.x) + p2.vy*(p2.y - p1.y) + deltaTime)
	// C = dist_p1p2 ^ 2 - deltaTime ^ 2
	//=================================


	double deltaTime = BaseParticle.TimePoint - ViewParticle.TimePoint;
	
	//double A = ViewParticle.Velocity_Linear.LENPWR2() - 1.0;
	
	SPos dist = ViewParticle.Position;
	dist.MINUS(BaseParticle.Position);
	double C = dist.LENPWR2() - deltaTime * deltaTime;

	double B = dist.DOT(ViewParticle.Velocity_Linear) + deltaTime;
	B += B;
	

	TwoDoubleRoots ret = QuadraticEequationSolution(ViewParticle.Velocity_Linear.LENPWR2() - 1.0, B, C);

	dist= ViewParticle.Velocity_Linear;
	dist.MULT(ret.root1.Dbl);
	dist.ADD(ViewParticle.Position);

	return QuadraticEequationSolution(ViewParticle.Velocity_Linear.LENPWR2() - 1.0, B, C);
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
	const SPos *VP_Velocity;

	VP_Velocity = &ViewParticle.Velocity_Linear;

	SPos VP_AtTime = *VP_Velocity;
	VP_AtTime *= BaseParticle.TimePoint - ViewParticle.TimePoint;
	VP_AtTime += ViewParticle.Position;

	SPos ProjPoint;
	double TimeToNorm = GetProjectionPointOnLine(ProjPoint, VP_AtTime, *VP_Velocity, BaseParticle.Position);
	double V_pwr2 = VP_Velocity->LENPWR2();
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

double GetIntersectionRadiusOfTwoSpheresPwr2(const SPos & Sph1_Center, double Sph1_Radius, const SPos& Sph2_Center, double Sph2_Radius)
{
	double RadiusPwr2;
	double distancePwr2 = Sph1_Center.GetDistancePwr2(Sph2_Center);
	double distance = sqrt(distancePwr2);
	double Sph1_RadiusPwr2 = Sph1_Radius * Sph1_Radius;

	// Проверяем условия существования пересечения
	if (distance <= Sph1_Radius + Sph2_Radius && distance > fabs(Sph1_Radius - Sph2_Radius))
	{
		// Вычисляем d1 — расстояние от Sph1_Center до плоскости пересечения
		double d1 = (Sph1_RadiusPwr2 - Sph2_Radius * Sph2_Radius + distancePwr2) / (distance + distance);

		// Радиус окружности пересечения
		RadiusPwr2 = Sph1_RadiusPwr2 - d1 * d1;
	}
	else RadiusPwr2 = -1.0;

	return RadiusPwr2;
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

void LimitedItgrl_rk411(SPos &result, SPos& resultOfFirst, SPos& FirstDerivative, SPos& SecondDerivative, double LimitValue, double deltaTime, double *Theta)
{
	SPos k1_v, k1_r, k2_v, k2_r, k3_v, k3_r, k4_v, k4_r;

	k1_v = SecondDerivative;  k1_v *= deltaTime;
	k1_r = FirstDerivative; k1_r *= deltaTime;

	k2_v = SecondDerivative; k2_v *= deltaTime;
	k2_r = k1_v; k2_r *= 0.5; k2_r += FirstDerivative;	k2_r.LIMIT_BY(LimitValue);	k2_r *= deltaTime;

	k3_v = SecondDerivative; k3_v *= deltaTime;
	k3_r = k2_v; k3_r *= 0.5; k3_r += FirstDerivative;	k3_r.LIMIT_BY(LimitValue);	k3_r *= deltaTime;

	k4_v = SecondDerivative;  k4_v *= deltaTime;
	k4_r = FirstDerivative; k4_r += k3_v;	k4_r.LIMIT_BY(LimitValue);	k4_r *= deltaTime;

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



void LimitedItgrl_rk4(SPos &result, SPos& resultOfFirst, SPos& FirstDerivative, SPos& SecondDerivative, double LimitValue, double deltaTime, double *Theta)
{

	SPos vel0 = FirstDerivative;

	// Шаг 1: k1
	SPos k1_pos = vel0;                   
	SPos k1_vel = SecondDerivative;         

	// Шаг 2: k2
	SPos pos_k2 = k1_pos; pos_k2 *= (deltaTime / 2.0);
	SPos vel_k2 = k1_vel;  vel_k2 *= (deltaTime / 2.0); vel_k2 += vel0; vel_k2.LIMIT_BY(LimitValue);
	SPos k2_pos = vel_k2;
	SPos k2_vel = SecondDerivative;

	// Шаг 3: k3
	SPos pos_k3 = k2_pos; pos_k3 *= (deltaTime / 2.0);
	SPos vel_k3 = k2_vel; vel_k3 *= (deltaTime / 2.0); vel_k3 += vel0; vel_k3.LIMIT_BY(LimitValue);
	SPos k3_pos = vel_k3;
	SPos k3_vel = SecondDerivative;

	// Шаг 4: k4
	SPos pos_k4 = k3_pos;  pos_k4 *= deltaTime;
	SPos vel_k4 = k3_vel;  vel_k4 *= deltaTime; vel_k4 += vel0;
	SPos k4_pos = vel_k4;
	SPos k4_vel = SecondDerivative;

	// Расчёт итогового перемещения (интегрирование скорости)
	result = k1_pos; result += k2_pos; result += k2_pos; result += k3_pos; result += k3_pos; result += k4_pos; result *= deltaTime * (1.0 / 6.0);

	// Расчёт итоговой скорости (интегрирование ускорения)
	resultOfFirst = k1_vel; resultOfFirst += k2_vel; resultOfFirst += k2_vel; resultOfFirst += k3_vel; resultOfFirst += k3_vel; resultOfFirst += k4_vel; resultOfFirst *= deltaTime * (1.0 / 6.0);
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
	Vector3D k1_pos = vel0;                            // d(pos)/deltaTime = velocity
	Vector3D k1_vel = calculateAcceleration(pos0);   // d(vel)/deltaTime = acceleration(pos)

	// Шаг 2: k2 — прогноз на середину шага (t + deltaTime/2)
	Vector3D pos_k2 = pos0 + k1_pos * (deltaTime / 2.0);  // позиция через deltaTime/2
	Vector3D vel_k2 = vel0 + k1_vel * (deltaTime / 2.0);  // скорость через deltaTime/2

	// Вычисляем k2 на основе прогноза pos_k2 и vel_k2
	Vector3D k2_pos = vel_k2;                        // скорость в точке pos_k2
	Vector3D k2_vel = calculateAcceleration(pos_k2); // ускорение в точке pos_k2

	// Шаг 3: k3 — уточнённый прогноз на середину шага (t + deltaTime/2)
	Vector3D pos_k3 = pos0 + k2_pos * (deltaTime / 2.0);  // уточнённая позиция через deltaTime/2
	Vector3D vel_k3 = vel0 + k2_vel * (deltaTime / 2.0);  // уточнённая скорость через deltaTime/2

	// Вычисляем k3 на основе уточнённого прогноза pos_k3 и vel_k3
	Vector3D k3_pos = vel_k3;                        // скорость в точке pos_k3
	Vector3D k3_vel = calculateAcceleration(pos_k3); // ускорение в точке pos_k3

	// Шаг 4: k4 — прогноз на конец шага (t + deltaTime)
	Vector3D pos_k4 = pos0 + k3_pos * deltaTime;          // позиция через полный шаг deltaTime
	Vector3D vel_k4 = vel0 + k3_vel * deltaTime;         // скорость через полный шаг deltaTime

	// Вычисляем k4 на основе прогноза pos_k4 и vel_k4
	Vector3D k4_pos = vel_k4;                        // скорость в точке pos_k4
	Vector3D k4_vel = calculateAcceleration(pos_k4); // ускорение в точке pos_k4

	// Итоговое обновление позиции и скорости
	finalPosition = pos0 + (k1_pos + 2.0 * k2_pos + 2.0 * k3_pos + k4_pos) * (deltaTime / 6.0);
	finalVelocity = vel0 + (k1_vel + 2.0 * k2_vel + 2.0 * k3_vel + k4_vel) * (deltaTime / 6.0);

	// Ограничение скорости в конце расчёта
	double finalMag = finalVelocity.magnitude();
	if (finalMag > Vmax && finalMag > 1e-10) {
		finalVelocity = finalVelocity.normalize() * Vmax;
	}

	*/

}




//
//void LimitedItgrl_rk4(SPos& result, SPos& resultOfFirst, SPos& FirstDerivative, SPos& SecondDerivative, double LimitValue, double deltaTime, double* Theta)
//{
//	SPos k1_v, k1_r, k2_v, k2_r, k3_v, k3_r, k4_v, k4_r, v_temp, r_temp;
//
//	// Этап 1
//	k1_v = SecondDerivative;  k1_v *= deltaTime;
//	k1_r = FirstDerivative; k1_r *= deltaTime;
//
//	// Этап 2
//	v_temp = k1_v; v_temp *= 0.5; v_temp += FirstDerivative;
//	v_temp.LIMIT_BY(LimitValue);
//
//	/////r_temp = k1_r; r_temp*=0.5;
//	k2_v = SecondDerivative; k2_v *= deltaTime;
//	k2_r = v_temp; k2_r *= deltaTime;
//
//	// Этап 3
//	v_temp = k2_v; v_temp *= 0.5; v_temp += FirstDerivative;
//	v_temp.LIMIT_BY(LimitValue);
//	/////r_temp = k2_r; r_temp *= 0.5;
//
//	k3_v = SecondDerivative; k3_v *= deltaTime;
//	k3_r = v_temp; k3_r *= deltaTime;
//
//	// Этап 4
//	v_temp = FirstDerivative; v_temp += k3_v;
//
//	v_temp.LIMIT_BY(LimitValue);
//	////r_temp = k3_r;
//	k4_v = SecondDerivative;  k4_v *= deltaTime;
//	k4_r = v_temp; k4_r *= deltaTime;
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
	SData BaseParticle, ViewParticle;
	BaseParticle.CLEAR();
	ViewParticle.CLEAR();

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




double SpeedByPhantom(const SData& Phantom, const SPos& Particle, const SPos& ParticleDirection, SPos *ResVelocity) 
{
	SPos r0; r0.SET_AS_DELTA(Particle, Phantom.Position);

	SPos DirectionNorm = ParticleDirection; // единичный вектор направления
	DirectionNorm.NORMALIZE();

	double a = r0.DOT(Phantom.Velocity_Linear);       // числитель
	double b = r0.DOT(DirectionNorm);       // знаменатель
	double ret = a / b;

	if (ResVelocity)
	{
		DirectionNorm *= ret;
		*ResVelocity = DirectionNorm;
	}

	return ret;
}

//две точки A и B произвольно и равномерно движутся в пространстве.В некоторый момент T0 точка C находится на одинаковом расстоянии R==1 от A и B.Рассчитать положение и скорость C в момент T1, если расстояние от C до A и B не изменилось, а Т1 - Т0 мало и перемещение C минимизировано
int compute_2NP_master_LinearMove_R1(const SData& Phantom_Acur, const SData& Phantom_Bcur, const SData& Phantom_Anext, const SData& Phantom_Bnext, const SData& DataCur, SData& data_Out, double deltaTime)
{
	int ret;
	//know how
	SPos movement;
	GetMoveToClosestEequiDistantPoint_R1_FAST(Phantom_Anext.Position, Phantom_Bnext.Position, DataCur.Position, movement);

	double vMoveLEN = movement.LEN();
	double tmpSpeed = vMoveLEN / deltaTime;
	data_Out.Velocity = movement;

	if (tmpSpeed > 1.0)
	{
		data_Out.Velocity.SET_LENGHT_TO_1();
		data_Out.AbsSpeed = 1.0;
		movement.SET_AS_MULT(data_Out.Velocity, deltaTime);
		ret = 0x11;
	}
	else
	{
		data_Out.Velocity.SET_LENGHT(tmpSpeed);
		data_Out.AbsSpeed = tmpSpeed;
		ret = 0x00;
	}

	data_Out.Position.SET_AS_SUMM(DataCur.Position, movement);
	data_Out.Velocity_Linear = data_Out.Velocity;

	return ret;
}

//две точки A и B произвольно и равномерно движутся в пространстве.В некоторый момент T0 точка C находится на одинаковом расстоянии R от A и B.A и B влияют только на C.Рассчитать положение и скорость C в момент T1, если A и B вырабатывают значительное ускорение модулем  W, которое стремится сместить C с расстояния R от точки B.Перемещение C минимизировано.Скорость не может по модулю превышать 1. W стремится к бесконечности
int compute_2AP_master_Motion(const SData& Phantom_A, const SData& Phantom_B, const SData& DataCur, SData& data_Out, double deltaTime)
{
	int ret = 0x11;
	SPos distanceTo_A; distanceTo_A.SET_AS_DELTA(DataCur.Position, Phantom_A.Position);
	SPos distanceTo_B; distanceTo_B.SET_AS_DELTA(DataCur.Position, Phantom_B.Position);

	if (distanceTo_A.DOT(Phantom_A.Velocity_Linear) < 0) distanceTo_A.INVERT();
	if (distanceTo_B.DOT(Phantom_B.Velocity_Linear) < 0) distanceTo_B.INVERT();

	data_Out.Accel_2_3D = { 0 };
	data_Out.Jerk_3_3D = { 0 };

	data_Out.Velocity.SET_AS_SUMM_EQ1(distanceTo_A, distanceTo_B);

	if (isnan(data_Out.Velocity.X))
	{
		data_Out.Velocity = { 0 };
		data_Out.Velocity_Linear = { 0 };
		data_Out.AbsSpeed = 0.0;
	}
	else
	{
		data_Out.AbsSpeed = 1.0;
		data_Out.Velocity_Linear = data_Out.Velocity;
		data_Out.Position.SET_AS_IntegralOf(DataCur.Position, data_Out.Velocity_Linear, deltaTime);
	}

	return ret;
}

//две точки A и B произвольно и с произвольным постоянным ускорением движутся в пространстве.В некоторый момент T0 точка C находится на одинаковом расстоянии от A и B.Рассчитать положение и скорость C в момент T1, если расстояние R от C до A и B не изменилось, а Т1 - Т0 мало и перемещение C минимизировано
int compute_2NP_master_AccMove(const SData& Phantom_A, const SData& Phantom_B, SData& data_Out, double deltaTime)
{
	int ret;
	SPos distanceTo_A = data_Out.Position; distanceTo_A -= Phantom_A.Position;
	SPos distanceTo_B = data_Out.Position; distanceTo_B -= Phantom_B.Position;

	double D2 = distanceTo_A.LENPWR2();
	double ProjectionAB = distanceTo_A.DOT(distanceTo_B);          // ProjectionAB
	double D4 = D2 * D2;
	double D = D4 - ProjectionAB * ProjectionAB;            // определитель
	SPos tmp;

	// Вырожденный случай: почти на прямой AB
	if (fabs(D) < 1e-12 * D4)
	{
		data_Out.Velocity.SET_AS_MIDDLE_POINT(Phantom_A.Velocity_Linear, Phantom_B.Velocity_Linear);
		data_Out.Accel_2_3D.SET_AS_MIDDLE_POINT(Phantom_A.Accel_2_3D, Phantom_B.Accel_2_3D);

		tmp = data_Out.Velocity; tmp *= deltaTime;
		data_Out.Position += tmp;
		tmp = data_Out.Accel_2_3D;
		tmp *= deltaTime * deltaTime * 0.5;
		data_Out.Position += tmp;
		ret = 0x11;
		data_Out.Velocity_Linear = data_Out.Velocity;
	}
	else
	{
		double pA = distanceTo_A.DOT(Phantom_A.Velocity_Linear);
		double pB = distanceTo_B.DOT(Phantom_B.Velocity_Linear);
		D = 1.0 / D;
		double lambda = (pA * D2 - pB * ProjectionAB) * D;
		double mu = (pB * D2 - pA * ProjectionAB) * D;


		data_Out.Velocity = distanceTo_A;
		data_Out.Velocity *= lambda;
		tmp = distanceTo_B; tmp *= mu;
		data_Out.Velocity += tmp;

		if (data_Out.Velocity.LENPWR2() > 1.0)
		{
			data_Out.Velocity.SET_LENGHT_TO_1();
			ret = 0x11;
		}
		else
			ret = 0x00;

		// Квадраты модулей разностей скоростей
		SPos uA = data_Out.Velocity; distanceTo_A -= Phantom_A.Velocity_Linear;
		SPos uB = data_Out.Velocity; distanceTo_B -= Phantom_B.Velocity_Linear;

		double uA2 = uA.LENPWR2();
		double uB2 = uB.LENPWR2();

		double qA = distanceTo_A.DOT(Phantom_A.Accel_2_3D) - uA2;
		double qB = distanceTo_B.DOT(Phantom_B.Accel_2_3D) - uB2;

		double alpha = (qA * D2 - qB * ProjectionAB) * D;
		double beta = (qB * D2 - qA * ProjectionAB) * D;

		data_Out.Accel_2_3D = distanceTo_A; data_Out.Accel_2_3D *= alpha;
		distanceTo_B *= beta;
		data_Out.Accel_2_3D += distanceTo_B;

		tmp = data_Out.Velocity; tmp *= deltaTime;
		data_Out.Position += tmp;
		tmp = data_Out.Accel_2_3D;
		tmp *= deltaTime * deltaTime * 0.5;
		data_Out.Position += tmp;
		data_Out.Velocity_Linear = data_Out.Velocity;
	}
	data_Out.AbsSpeed = data_Out.Velocity_Linear.LEN();

	return ret;
}

//две точки A и B произвольно и с произвольным постоянным ускорением движутся в пространстве.В некоторый момент T0 точка C находится на одинаковом расстоянии R==1 от A и B.Рассчитать положение и скорость C в момент T1, если расстояние R==1 от C до A и B не изменилось, а Т1 - Т0 мало и перемещение C минимизировано
int compute_2NP_master_AccMove_R1(const SData& Phantom_A, const SData& Phantom_B, SData& data_Out, double deltaTime)
{
	int ret;
	SPos distanceTo_A = data_Out.Position; distanceTo_A -= Phantom_A.Position;
	SPos distanceTo_B = data_Out.Position; distanceTo_B -= Phantom_B.Position;

	double ProjectionAB = distanceTo_A.DOT(distanceTo_B);          // ProjectionAB
	double D = 1.0 - ProjectionAB * ProjectionAB;            // определитель
	SPos tmp;

	// Вырожденный случай: почти на прямой AB
	if (fabs(D) < 1e-12)
	{
		data_Out.Velocity.SET_AS_MIDDLE_POINT(Phantom_A.Velocity_Linear, Phantom_B.Velocity_Linear);
		data_Out.Accel_2_3D.SET_AS_MIDDLE_POINT(Phantom_A.Accel_2_3D, Phantom_B.Accel_2_3D);

		tmp = data_Out.Velocity; tmp *= deltaTime;
		data_Out.Position += tmp;
		tmp = data_Out.Accel_2_3D;
		tmp *= deltaTime * deltaTime * 0.5;
		data_Out.Position += tmp;
		ret = 0x11;
		data_Out.Velocity_Linear = data_Out.Velocity;
	}
	else
	{
		double pA = distanceTo_A.DOT(Phantom_A.Velocity_Linear);
		double pB = distanceTo_B.DOT(Phantom_B.Velocity_Linear);
		D = 1.0 / D;
		double lambda = (pA - pB * ProjectionAB) * D;
		double mu = (pB - pA * ProjectionAB) * D;


		data_Out.Velocity = distanceTo_A;
		data_Out.Velocity *= lambda;
		tmp = distanceTo_B; tmp *= mu;
		data_Out.Velocity += tmp;

		if (data_Out.Velocity.LENPWR2() > 1.0)
		{
			data_Out.Velocity.SET_LENGHT_TO_1();
			ret = 0x11;
		}
		else
			ret = 0x00;

		// Квадраты модулей разностей скоростей
		SPos uA = data_Out.Velocity; distanceTo_A -= Phantom_A.Velocity_Linear;
		SPos uB = data_Out.Velocity; distanceTo_B -= Phantom_B.Velocity_Linear;

		double uA2 = uA.LENPWR2();
		double uB2 = uB.LENPWR2();

		double qA = distanceTo_A.DOT(Phantom_A.Accel_2_3D) - uA2;
		double qB = distanceTo_B.DOT(Phantom_B.Accel_2_3D) - uB2;

		double alpha = (qA - qB * ProjectionAB) * D;
		double beta = (qB - qA * ProjectionAB) * D;

		data_Out.Accel_2_3D = distanceTo_A; data_Out.Accel_2_3D *= alpha;
		distanceTo_B *= beta;
		data_Out.Accel_2_3D += distanceTo_B;

		tmp = data_Out.Velocity; tmp *= deltaTime;
		data_Out.Position += tmp;
		tmp = data_Out.Accel_2_3D;
		tmp *= deltaTime * deltaTime * 0.5;
		data_Out.Position += tmp;
		data_Out.Velocity_Linear = data_Out.Velocity;
	}
	data_Out.AbsSpeed = data_Out.Velocity_Linear.LEN();
	return ret;
}

//две точки A и B произвольно и равномерно движутся в пространстве.В некоторый момент T0 точка C находится на одинаковом расстоянии R от A и B.A и B влияют только на C.Рассчитать положение и скорость C в момент T1, если A вырабатывает значительное ускорение модулем  W, которое стремится вернуть C на расстояние R от точки A, а B вырабатывает ускорение модулем W, которое стремится сместить C с расстояния R от точки B.Перемещение C минимизировано.Скорость не может по модулю превышать 1. W стремится к бесконечности
int compute_NPAP_master_LinearMove(const SData& Phantom_NP, const SData& Phantom_NP_next, const SData& Phantom_AP, const SData& data_Cur, SData& data_Out, double deltaTime)
{
	//выравнивание точки на критическое расстояние не здесь
	//предельный случай идеального расположения на одной прямой не рассматриваем
	//внешнее ускорение не учитывается, так как есть частица AP, которая гасит внешнее влияние своим бесконечным ускорением
	//на выходе всегда предельная скорость
	//модули скорости от хозяев одинаковы
	SData Res_AP;
	compute_1_Master_LinearMovement(Phantom_NP, data_Cur, data_Out.Position, data_Out.Velocity, deltaTime);		//перемещение 

	//для AP только вектор скорости
	compute_1_Master_LinearMovement(Phantom_AP, data_Cur, Res_AP.Position, Res_AP.Velocity, deltaTime);		//перемещение 


	//скорость от AP == 1 по сути - ускорение равно бесконечности.Следовательно в нормальной плоскости максимально возможное с учетом скорости от NP
	Res_AP.Velocity.SET_LENGHT_TO_1();
	//AP всегда на барьере усиливает движение
	//if (data_Out.Velocity.DOT(Res_AP.Velocity) < 0.0)
		Res_AP.Velocity.INVERT();
	
	//приблизительно. считаем что шаг мал и AP в плоскости нормали
	Res_AP.Velocity.SET_AS_PROJECTION_ONTO_PLANE(data_Out.Velocity);
	Res_AP.Velocity.SET_LENGHT(sqrt(1.0- data_Out.Velocity.LENPWR2()));

	data_Out.Velocity += Res_AP.Velocity;
	data_Out.Velocity.SET_LENGHT_TO_1();

	data_Out.AbsSpeed = 1.0;
	data_Out.Velocity_Linear = data_Out.Velocity;

	data_Out.Position.SET_AS_IntegralOf(data_Cur.Position, data_Out.Velocity, deltaTime);

	//приведение на барьер NP
	data_Out.Position -= Phantom_NP_next.Position;
	data_Out.Position.SET_LENGHT_TO_1();
	data_Out.Position += Phantom_NP_next.Position;

	int ret;
	if (fabs(data_Out.Position.GetDistance(data_Cur.Position) - 1.0) < 1.0e-12)
		ret = 0x00;
	else
		ret = 0x10;



	return ret;
}

int compute_NPAP_master_LinearMove_AI(const SData& Phantom_NP, const SData& Phantom_AP, SData& data_Out, double deltaTime) 
{
	int ret;
	SPos distanceTo_A = data_Out.Position; distanceTo_A -= Phantom_NP.Position;
	SPos distanceTo_B = data_Out.Position; distanceTo_B -= Phantom_AP.Position;

	double criticalDistancePwr2 = distanceTo_A.LENPWR2();
	double ProjectionAB = distanceTo_A.DOT(distanceTo_B);          // ProjectionAB

	double D = (criticalDistancePwr2 - ProjectionAB) * (criticalDistancePwr2 + ProjectionAB);

	if (D < 1e-14) {
		// Вырожденный случай: distanceTo_A и distanceTo_B почти параллельны
		data_Out.Velocity = Phantom_NP.Velocity_Linear;
		SPos tmp = data_Out.Velocity; tmp *= deltaTime;
		data_Out.Position += tmp;
		ret = 0x10;
	}
	else
	{
		double pNP = distanceTo_A.DOT(Phantom_NP.Velocity_Linear);
		double pAP = distanceTo_B.DOT(Phantom_AP.Velocity_Linear);
		D = 1.0 / D;
		double lambda = (pNP * criticalDistancePwr2 - pAP * ProjectionAB) * D;
		double mu = (pAP * criticalDistancePwr2 - pNP * ProjectionAB) * D;

		distanceTo_A *= lambda;
		distanceTo_B *= mu;
		data_Out.Velocity = distanceTo_A;
		data_Out.Velocity += distanceTo_B;

		// Ограничение на модуль скорости: 
		if (data_Out.Velocity.LENPWR2() > 1.0)
		{
			data_Out.Velocity.SET_LENGHT_TO_1();
			ret = 0x11;
		}
		else ret = 0x10;

		SPos tmp = data_Out.Velocity; tmp *= deltaTime;
		data_Out.Position += tmp;
	}

	data_Out.AbsSpeed = data_Out.Velocity.LEN();
	data_Out.Velocity_Linear = data_Out.Velocity;

	return ret;
}

int compute_NPAP_master_LinearMove_R1_AI(const SData& Phantom_NP, const SData& Phantom_AP, SData& data_Out, double deltaTime)
{
	int ret;
	SPos distanceTo_A = data_Out.Position; distanceTo_A -= Phantom_NP.Position;
	SPos distanceTo_B = data_Out.Position; distanceTo_B -= Phantom_AP.Position;

	double ProjectionAB = distanceTo_A.DOT(distanceTo_B);          // ProjectionAB

	double D = (1.0 - ProjectionAB) * (1.0 + ProjectionAB);

	if (D < 1e-14) {
		// Вырожденный случай: distanceTo_A и distanceTo_B почти параллельны
		data_Out.Velocity = Phantom_NP.Velocity_Linear;
		SPos tmp = data_Out.Velocity; tmp *= deltaTime;
		data_Out.Position += tmp;
		ret = 0x10;
	}
	else
	{
		double pNP = distanceTo_A.DOT(Phantom_NP.Velocity_Linear);
		double pAP = distanceTo_B.DOT(Phantom_AP.Velocity_Linear);
		D = 1.0 / D;
		double lambda = (pNP  - pAP * ProjectionAB) * D;
		double mu = (pAP - pNP * ProjectionAB) * D;

		distanceTo_A *= lambda;
		distanceTo_B *= mu;
		data_Out.Velocity = distanceTo_A;
		data_Out.Velocity += distanceTo_B;

		// Ограничение на модуль скорости: 
		if (data_Out.Velocity.LENPWR2() > 1.0)
		{
			data_Out.Velocity.SET_LENGHT_TO_1();
			ret = 0x11;
		}
		else ret = 0x10;

		SPos tmp = data_Out.Velocity; tmp *= deltaTime;
		data_Out.Position += tmp;
	}

	data_Out.AbsSpeed = data_Out.Velocity.LEN();
	data_Out.Velocity_Linear = data_Out.Velocity;

	return ret;
}

//точка A произвольно и равноускоренно движется в пространстве.В некоторый момент T0 точка C находится на расстоянии R от A.Рассчитать положение, ускорение и скорость C в момент T1, если расстояние от C до A не изменилось, а Т1 - Т0 мало и перемещение C минимизировано
void compute_1_master_AccMove(const SData& Phantom_A, SData& data_Out, double deltaTime)
{
	SPos deltaPosition = Phantom_A.Position; deltaPosition -= data_Out.Position;
	double DistancePwr2 = deltaPosition.LENPWR2();

	if (DistancePwr2 < 1e-14) //Вырожденный случай
	{
		SPos tmp = Phantom_A.Velocity_Linear; tmp *= deltaTime;
		data_Out.Position += tmp;
		tmp = Phantom_A.Accel_2_3D;
		tmp *= deltaTime * deltaTime * 0.5;
		data_Out.Position += tmp;
	}
	else
	{
		double p = deltaPosition.DOT(Phantom_A.Velocity_Linear);
		data_Out.Velocity = deltaPosition;
		data_Out.Velocity *= p / DistancePwr2;

		SPos DeltaVelocity = Phantom_A.Velocity_Linear;  DeltaVelocity -= data_Out.Velocity;            // разность скоростей
		double DeltaVelocityPwr2 = DeltaVelocity.LENPWR2();
		double q = deltaPosition.DOT(Phantom_A.Accel_2_3D) + DeltaVelocityPwr2;
		data_Out.Accel_2_3D = deltaPosition;
		data_Out.Accel_2_3D *= (q / DistancePwr2);

		SPos tmp = data_Out.Velocity; tmp *= deltaTime;
		data_Out.Position += tmp;
		tmp = data_Out.Accel_2_3D;
		tmp *= deltaTime * deltaTime * 0.5;
		data_Out.Position += tmp;
	}
	data_Out.AbsSpeed = data_Out.Velocity.LEN();
	data_Out.Velocity_Linear = data_Out.Velocity;
}

//точка A произвольно и равноускоренно движется в пространстве.В некоторый момент T0 точка C находится на расстоянии 1 от A.Рассчитать положение, ускорение и скорость C в момент T1, если расстояние от C до A не изменилось, а Т1 - Т0 мало и перемещение C минимизировано
void compute_1_master_AccMoveR1(const SData& Phantom_A, SData& data_Out, double deltaTime)
{
	SPos deltaPosition = Phantom_A.Position; deltaPosition -= data_Out.Position;
	deltaPosition.SET_LENGHT_TO_1();
	//выравнивание точки на критическое расстояние
	data_Out.Position = Phantom_A.Position;
	data_Out.Position -= deltaPosition;

	//определение ыектора скорости
	data_Out.Velocity = deltaPosition;
	data_Out.Velocity *= deltaPosition.DOT(Phantom_A.Velocity_Linear);

	// разность скоростей
	SPos DeltaVelocity = Phantom_A.Velocity_Linear;  DeltaVelocity -= data_Out.Velocity;

	data_Out.Accel_2_3D = deltaPosition;
	data_Out.Accel_2_3D *= deltaPosition.DOT(Phantom_A.Accel_2_3D) + DeltaVelocity.LENPWR2();

	SPos tmp = data_Out.Velocity; tmp *= deltaTime;
	data_Out.Position += tmp;
	tmp = data_Out.Accel_2_3D;
	tmp *= deltaTime * deltaTime * 0.5;
	data_Out.Position += tmp;
	data_Out.AbsSpeed = data_Out.Velocity.LEN();
	data_Out.Velocity_Linear = data_Out.Velocity;
}

//точка A произвольно и равномерно движется в пространстве.В некоторый момент T0 точка C находится на расстоянии R от A.Рассчитать положение, ускорение и скорость C в момент T1, если расстояние от C до A не изменилось, а Т1 - Т0 мало и перемещение C минимизировано
void compute_1_Master_LinearMove(const SData& Phantom_A, SData& data_Out, double deltaTime) 
{
	SPos deltaPosition = Phantom_A.Position; deltaPosition -= data_Out.Position;
	double DistancePwr2Inv = 1.0 / deltaPosition.LENPWR2();

	if (DistancePwr2Inv > 1e14) //Вырожденный случай
	{
		SPos tmp = Phantom_A.Velocity_Linear; tmp *= deltaTime;
		data_Out.Position += tmp;
	}
	else
	{
		data_Out.Velocity = deltaPosition;
		data_Out.Velocity *= deltaPosition.DOT(Phantom_A.Velocity_Linear) * DistancePwr2Inv;

		SPos deltaVekociy = Phantom_A.Velocity_Linear;  deltaVekociy -= data_Out.Velocity_Linear;
		data_Out.Accel_2_3D = deltaPosition;
		data_Out.Accel_2_3D *=  deltaVekociy.LENPWR2() * DistancePwr2Inv;

		SPos tmp = data_Out.Velocity; tmp *= deltaTime;
		data_Out.Position += tmp;
		tmp = data_Out.Accel_2_3D;
		tmp *= deltaTime * deltaTime * 0.5;
		data_Out.Position += tmp;
	}
	data_Out.AbsSpeed = data_Out.Velocity.LEN();
	data_Out.Velocity_Linear = data_Out.Velocity;
}

//точка A произвольно и равномерно движется в пространстве.В некоторый момент T0 точка C находится на расстоянии 1 от A.Рассчитать положение, ускорение и скорость C в момент T1, если расстояние от C до A не изменилось, а Т1 - Т0 мало и перемещение C минимизировано
void compute_1_Master_LinearMoveR1_Fine(const SData& Phantom_A, SData& data_Out, double deltaTime)
{//!!!!!есть данные фантома в будущем. есть возможность посчитать более точно

	SPos deltaPosition; deltaPosition.SET_AS_DELTA(data_Out.Position, Phantom_A.Position);

	if (Phantom_A.AbsSpeed == 0.0)
	{
		double newTime = data_Out.TimePoint;
		data_Out = Phantom_A;
		data_Out.TimePoint = newTime;
	}
	else
	{
		SPos vHat = Phantom_A.Velocity_Linear;
		vHat *= 1.0 / Phantom_A.AbsSpeed; // единичный вектор скорости A

		// Разложение deltaPosition на компоненты вдоль и поперёк vHat
		double proj = deltaPosition.DOT(vHat); // проекция на vHat
		SPos d_par = vHat; d_par *= proj;      // параллельная составляющая
		SPos d_perp = deltaPosition;  d_perp -= d_par; // перпендикулярная составляющая

		double d_perp_len = d_perp.LEN();

		// Если d_perp нулевой (deltaPosition параллелен Phantom_A.Velocity_Linear), то C движется строго параллельно A
		if (d_perp_len == 0.0)
		{
			double newTime = data_Out.TimePoint;
			SPos NewPos = Phantom_A.Velocity_Linear;  NewPos *= deltaTime; NewPos += data_Out.Position;
			data_Out = Phantom_A;
			data_Out.Position = NewPos;
			data_Out.TimePoint = newTime;
	}
		else
		{
			// Единичный вектор в направлении d_perp
			SPos d_perp_hat = d_perp; d_perp_hat /= d_perp_len;

			// Нормаль к плоскости (vHat, d_perp) — ось вращения
			// n уже единичный, потому что vHat и d_perp_hat ортогональны и единичны
			SPos n; n.SET_AS_CROSS(vHat, d_perp_hat);

			// Угол поворота за deltaTime: theta = Phantom_A.AbsSpeed * deltaTime (линейная скорость Phantom_A.AbsSpeed, радиус 1)
			double theta = Phantom_A.AbsSpeed * deltaTime;

			// Поворот d_perp в плоскости на угол theta вокруг оси n
			// Формула Родрига: d_perp(t) = d_perp*cos(theta) + (n × d_perp)*sin(theta)
			SPos n_cross_dperp; n_cross_dperp.SET_AS_CROSS(n, d_perp_hat);   // это единичный вектор, перпендикулярный d_perp и n

			SPos tmp;
			tmp.SET_AS_MULT(n_cross_dperp, sin(theta));
			SPos d_perp_rot; d_perp_rot.SET_AS_MULT(d_perp_hat, cos(theta));
			d_perp_rot += tmp;
			// вернуть исходную длину
			d_perp_rot *= d_perp_len;

			// Итоговый вектор d(T1)
			SPos d1; d1.SET_AS_SUMM(d_par, d_perp_rot);

			// Позиция A в T1
			SPos Phantom_ANewPos = Phantom_A.Velocity_Linear;  Phantom_ANewPos *= deltaTime; Phantom_ANewPos += Phantom_A.Position;

			// Позиция C в T1
			data_Out.Position.SET_AS_SUMM(Phantom_ANewPos, d1);

			// Скорость data_Out.Velocity = Phantom_A.Velocity_Linear + ddot
			// ddot — производная d(t): поворот вектора d_perp с угловой скоростью Phantom_A.AbsSpeed вокруг n
			// ddot = Phantom_A.AbsSpeed * (n × d(t))
			SPos n_cross_d1; n_cross_d1.SET_AS_CROSS(n, d1);
			SPos ddot; ddot.SET_AS_MULT(n_cross_d1, Phantom_A.AbsSpeed);
			data_Out.Velocity.SET_AS_SUMM(Phantom_A.Velocity_Linear, ddot);
			data_Out.Velocity_Linear = data_Out.Velocity;
			data_Out.AbsSpeed = data_Out.Velocity.LEN();

			// Ускорение C: aC = ddotdot = -Phantom_A.AbsSpeed^2 * d_perp_rot (центростремительное в плоскости)
			// Либо через вторую производную: aC = Phantom_A.AbsSpeed * (n × ddot) = Phantom_A.AbsSpeed^2 * (n × (n × d1)) = -Phantom_A.AbsSpeed^2 * (d1 - d_par)
			// Но проще: aC = -Phantom_A.AbsSpeed*Phantom_A.AbsSpeed * d_perp_rot
			data_Out.Accel_2_3D.SET_AS_MULT(d_perp_rot, -Phantom_A.AbsSpeed * Phantom_A.AbsSpeed);
			data_Out.Velocity_Linear = data_Out.Velocity;
		}
	}
}


void compute_1_Master_LinearMoveR1_Fast____(const SData& Phantom_A1, const SData& Phantom_A2, const SData& data_C1, SData& data_C2_Out, double deltaTime)
{
	// 1. Скорость движения точки А (постоянная)
	SPos v_A; v_A.SET_AS_DELTA(Phantom_A2.Position, Phantom_A1.Position);
	v_A /= deltaTime;

	// 2. Начальное направление связи от А1 к С1
	SPos u1;
	u1.SET_AS_DELTA(data_C1.Position, Phantom_A1.Position);
	u1.NORMALIZE();

	// 3. Расчет начального ускорения C1_Position с учетом связи
	SPos relative_v1; relative_v1.SET_AS_DELTA(data_C1.Velocity, v_A);
	double lambda1 = -relative_v1.LENPWR2() - u1.DOT(data_C1.Accel_2_3D);
	SPos a_C1 = u1;
	a_C1 *= lambda1; 
	a_C1+= data_C1.Accel_2_3D;

	// 4. Интегрирование Тейлора для нахождения нового положения и скорости в момент deltaTime
	SPos tmp;
	tmp = a_C1; a_C1 *= deltaTime;
	data_C2_Out.Velocity.SET_AS_SUMM(data_C1.Velocity, tmp);

	data_C2_Out.Position = data_C1.Velocity;
	data_C2_Out.Position *= deltaTime;
	tmp *= (0.5 * deltaTime);
	data_C2_Out.Position += tmp;
	data_C2_Out.Position += data_C1.Position;

	// 5. Коррекция направления связи в конечной точке и расчет конечного ускорения
	SPos u2;
	u2.SET_AS_DELTA(data_C2_Out.Position, Phantom_A2.Position);
	u2.NORMALIZE();
	SPos relative_v2; relative_v2.SET_AS_DELTA(data_C2_Out.Velocity, v_A);
	double lambda2 = -relative_v2.LENPWR2() - u2.DOT(data_C1.Accel_2_3D);
	data_C2_Out.Accel_2_3D = u2;
	data_C2_Out.Accel_2_3D *= lambda2;
	data_C2_Out.Accel_2_3D += data_C1.Accel_2_3D;
}

void compute_1_Master_LinearMovement(const SData& Phantom_A, const SData& data_Src, SPos& Out_Position, SPos& Out_Velocity, double deltaTime)
{//!!!!!есть данные фантома в будущем. есть возможность посчитать более точно
	SPos deltaPosition;
	deltaPosition.SET_AS_DELTA(Phantom_A.Position, data_Src.Position);

	Out_Velocity = deltaPosition;
	Out_Velocity *= deltaPosition.DOT(Phantom_A.Velocity_Linear);

	Out_Position = Out_Velocity; 
	Out_Position *= deltaTime;
}


//переместить заданную точку в пространстве на ближайшую точку, которая равноудалена от точек Point_A и Point_B
void MoveToClosestEequiDistantPoint(const SPos& Point_A, const SPos& Point_B, SPos& Point_To_Move)
{
	SPos Delta_BA; Delta_BA.SET_AS_DELTA(Point_B, Point_A);
	double L2 = Delta_BA.LENPWR2();

	if (L2 > 1e-14)
	{
		SPos Point_Middle_AB;  Point_Middle_AB.SET_AS_MIDDLE_POINT(Point_A, Point_B);
		Point_Middle_AB -= Point_To_Move;
		Delta_BA *= Point_Middle_AB.DOT(Delta_BA) / L2;
		Point_To_Move += Delta_BA;
	}
}

//переместить заданную точку в пространстве на ближайшую точку, которая равноудалена от точек Point_A и Point_B на расстояние EequiDistant
void MoveToClosestEequiDistantPoint(const SPos& Point_A, const SPos& Point_B, SPos& Point_To_Move, double EequiDistant)
{
	SPos Point_Middle_AB;  
	Point_Middle_AB.SET_AS_MIDDLE_POINT(Point_A, Point_B);

	SPos Delta_BA; Delta_BA.SET_AS_DELTA(Point_B, Point_A);
	double L2 = Delta_BA.LENPWR2();

	if (L2 > 1e-14) 
	{
		SPos PTF_To_MAB = Point_Middle_AB; 
		PTF_To_MAB -= Point_To_Move;
		Delta_BA *= PTF_To_MAB.DOT(Delta_BA) / L2;
		Point_To_Move += Delta_BA;
		Point_To_Move -= Point_Middle_AB;
		Point_To_Move.SET_LENGHT(sqrt(EequiDistant * EequiDistant - L2 * 0.25));
		Point_To_Move += Point_Middle_AB;
	}
	else
	{
		Point_To_Move -= Point_Middle_AB;
		Point_To_Move.SET_LENGHT(EequiDistant);
		Point_To_Move += Point_Middle_AB;
	}
}

//переместить заданную точку в пространстве на ближайшую точку, которая равноудалена от точек Point_A и Point_B на расстояние 1
void MoveToClosestEequiDistantPoint_R1(const SPos& Point_A, const SPos& Point_B, SPos& Point_To_Move)
{
	SPos Point_Middle_AB;
	Point_Middle_AB.SET_AS_MIDDLE_POINT(Point_A, Point_B);

	SPos Delta_BA; Delta_BA.SET_AS_DELTA(Point_B, Point_A);
	double L2 = Delta_BA.LENPWR2();

	if (L2 > 1e-14)
	{
		SPos PTF_To_MAB = Point_Middle_AB;
		PTF_To_MAB -= Point_To_Move;
		Delta_BA *= PTF_To_MAB.DOT(Delta_BA) / L2;
		Point_To_Move += Delta_BA;
		Point_To_Move -= Point_Middle_AB;
		Point_To_Move.SET_LENGHT(sqrt(1.0 - L2 * 0.25));
		Point_To_Move += Point_Middle_AB;
	}
	else
	{
		Point_To_Move -= Point_Middle_AB;
		Point_To_Move.SET_LENGHT_TO_1();
		Point_To_Move += Point_Middle_AB;
	}
}

void MoveToClosestEequiDistantPoint_R1_FAST(const SPos& Point_A, const SPos& Point_B, SPos& Point_To_Move)
{	//3 +*-+*- *+*- +* * + 1 // sqrt  

	SPos Point_Middle_AB; //центр координат 0 0 0
	Point_Middle_AB.SET_AS_MIDDLE_POINT(Point_A, Point_B);
	SPos PlaneNormal;
	PlaneNormal.SET_AS_DELTA(Point_A, Point_Middle_AB);
	double PlaneNormalLenPwr2 = PlaneNormal.LENPWR2();

	if (PlaneNormalLenPwr2 > 1e-14)
	{
		 SPos pojPoint;
		 pojPoint.SET_AS_DELTA(Point_To_Move, Point_Middle_AB);//приведение к координатам 0 0 0 == Point_Middle_AB
		 pojPoint.SET_AS_PROJECTION_ONTO_PLANE(PlaneNormal, PlaneNormalLenPwr2);
		 pojPoint.SET_LENGHT(sqrt(1.0 - PlaneNormalLenPwr2));
		 Point_To_Move.SET_AS_SUMM(pojPoint, Point_Middle_AB);
	}
	else
	{
		Point_To_Move -= Point_Middle_AB;
		Point_To_Move.SET_LENGHT_TO_1();
		Point_To_Move += Point_Middle_AB;
	}
}
//вычислить перемещение точки в пространстве на ближайшую точку, которая равноудалена от точек Point_A и Point_B на расстояние 1
void GetMoveToClosestEequiDistantPoint_R1_FAST(const SPos& Point_A, const SPos& Point_B, const SPos& Point_To_Move, SPos& Movement)
{	//3 +*-+*- *+*- +* * + 1 // sqrt  

	SPos Point_Middle_AB; //центр координат 0 0 0
	Point_Middle_AB.SET_AS_MIDDLE_POINT(Point_A, Point_B);
	SPos PlaneNormal;
	PlaneNormal.SET_AS_DELTA(Point_A, Point_Middle_AB);
	double PlaneNormalLenPwr2 = PlaneNormal.LENPWR2();

	if (PlaneNormalLenPwr2 > 1e-14)
	{
		SPos MovementStart00;
		MovementStart00.SET_AS_DELTA(Point_To_Move, Point_Middle_AB);//приведение к координатам 0 0 0 == Point_Middle_AB
		Movement = MovementStart00;
		Movement.SET_AS_PROJECTION_ONTO_PLANE(PlaneNormal, PlaneNormalLenPwr2);
		Movement.SET_LENGHT(sqrt(1.0 - PlaneNormalLenPwr2));
		Movement -= MovementStart00;
	}
	else
	{
		Movement.SET_AS_DELTA(Point_B, Point_A);
	}
}

//переместить заданную точку в пространстве на расстояние EequiDistant от точки Point_A
void MoveToDistantPoint(const SPos& Point_A, SPos& Point_To_Move, double EequiDistant)
{
	Point_To_Move -= Point_A;
	Point_To_Move.SET_LENGHT(EequiDistant);
	Point_To_Move += Point_A;
}

//переместить заданную точку в пространстве на расстояние 1 от точки Point_A
void MoveToDistantPoint_R1(const SPos& Point_A, SPos& Point_To_Move)
{
	Point_To_Move -= Point_A;
	Point_To_Move.SET_LENGHT_TO_1();
	Point_To_Move += Point_A;
}







/*
две точки A и B произвольно и равномерно движутся в пространстве. В некоторый момент T0 точка C находится на одинаковом расстоянии R от A и B. A и B влияют только на C. Рассчитать положение и скорость C в момент T1, если A вырабатывает значительное ускорение модулем  W, которое стремится вернуть C на расстояние R от точки A, а B вырабатывает ускорение модулем W, которое стремится сместить C с расстояния R от точки B. Перемещение C минимизировано. Скорость не может по модулю превышать 1. W стремится к бесконечности

ult compute_C_minimal_motion_with_cap(
	const SPos& rA, const SPos& rB, const SPos& rC,
	const SPos& vA, const SPos& vB,
	double R, double dt
) {
	const double eps = 1e-12;

	SPos distanceTo_A = rC - rA;
	SPos distanceTo_B = rC - rB;

	double R2 = R*R;
	double M = dot(distanceTo_A, distanceTo_B);
	double D = R2*R2 - M*M;

	if (D < eps) {
		// Вырожденный случай: distanceTo_A и distanceTo_B почти параллельны
		return {{0,0,0}, rC, false};
	}

	double pA = dot(distanceTo_A, vA);
	double pB = dot(distanceTo_B, vB);

	double lambda = (pA*R2 - pB*M) / D;
	double mu     = (pB*R2 - pA*M) / D;

	SPos vC = lambda * distanceTo_A + mu * distanceTo_B;

	// Ограничение на модуль скорости: |vC| <= 1
	double vLen = norm(vC);
	if (vLen > 1.0) {
		vC = (1.0 / vLen) * vC;  // нормируем
	}

	SPos rC1 = rC + dt * vC;

	return {vC, rC1, true};
}
*/

/*
две точки A и B произвольно и равномерно движутся в пространстве. В некоторый момент T0 точка C находится на одинаковом расстоянии R от A и B. A и B влияют только на C. Рассчитать положение и скорость C в момент T1, если A и B вырабатывает значительное ускорение модулем  W,  которое стремится сместить C с расстояния R от точки A и B соответственно. Перемещение C минимизировано. Скорость не может по модулю превышать 1. W стремится к бесконечности

#include <array>
#include <cmath>

using SPos = std::array<double,3>;

SPos operator+(const SPos& a, const SPos& b) {
	return {a[0]+b[0], a[1]+b[1], a[2]+b[2]};
}
SPos operator-(const SPos& a, const SPos& b) {
	return {a[0]-b[0], a[1]-b[1], a[2]-b[2]};
}
SPos operator*(double s, const SPos& v) {
	return {s*v[0], s*v[1], s*v[2]};
}
double dot(const SPos& a, const SPos& b) {
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}
double LENPWR2(const SPos& v) { return dot(v,v); }
double LEN(const SPos& v) { return std::sqrt(LENPWR2(v)); }

struct Result {
	SPos vC;
	SPos rC1;
	bool valid;
};

Result compute_C_motion_ideal_with_speed_cap(
	const SPos& rA, const SPos& rB, const SPos& rC,
	const SPos& vA, const SPos& vB,
	double R, double dt
) {
	const double eps = 1e-12;

	SPos distanceTo_A = rC - rA;
	SPos distanceTo_B = rC - rB;

	double R2 = R*R;
	double M = dot(distanceTo_A, distanceTo_B);
	double D = R2*R2 - M*M;

	if (D < eps) {
		// Вырожденный случай: distanceTo_A и distanceTo_B почти параллельны
		return {{0,0,0}, rC, false};
	}

	double pA = dot(distanceTo_A, vA);
	double pB = dot(distanceTo_B, vB);

	double lambda = (pA*R2 - pB*M) / D;
	double mu     = (pB*R2 - pA*M) / D;

	SPos vC = lambda * distanceTo_A + mu * distanceTo_B;

	// Ограничение на модуль скорости: |vC| <= 1
	double vLen = LEN(vC);
	if (vLen > 1.0) {
		vC = (1.0 / vLen) * vC;  // нормируем
	}

	SPos rC1 = rC + dt * vC;

	return {vC, rC1, true};
}


*/


/*
две точки A и B произвольно и равномерно движутся в пространстве. В некоторый момент T0 точка C находится на одинаковом расстоянии R от A и B. A и B влияют только на C. Рассчитать положение и скорость C в момент T1, если A и B вырабатывает значительное ускорение модулем  W,  которое стремится вернуть C на расстояние R от точки A и B соответственно. Перемещение C минимизировано. Скорость не может по модулю превышать 1. W стремится к бесконечности

#include <array>
#include <cmath>

using SPos = std::array<double,3>;

SPos operator+(const SPos& a, const SPos& b) {
	return {a[0]+b[0], a[1]+b[1], a[2]+b[2]};
}
SPos operator-(const SPos& a, const SPos& b) {
	return {a[0]-b[0], a[1]-b[1], a[2]-b[2]};
}
SPos operator*(double s, const SPos& v) {
	return {s*v[0], s*v[1], s*v[2]};
}
double dot(const SPos& a, const SPos& b) {
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}
double LENPWR2(const SPos& v) { return dot(v,v); }
double LEN(const SPos& v) { return std::sqrt(LENPWR2(v)); }

struct Result {
	SPos vC;
	SPos rC1;
	bool valid;
};

// Режим 1: минимизация скорости с жёстким ограничением |vC|<=1
Result compute_C_motion_speed_cap(
	const SPos& rA, const SPos& rB, const SPos& rC,
	const SPos& vA, const SPos& vB,
	double R, double dt
) {
	const double eps = 1e-12;

	SPos distanceTo_A = rC - rA;
	SPos distanceTo_B = rC - rB;

	double R2 = R*R;
	double M = dot(distanceTo_A, distanceTo_B);
	double D = R2*R2 - M*M;

	if (D < eps) {
		return {{0,0,0}, rC, false}; // вырожденный случай
	}

	double pA = dot(distanceTo_A, vA);
	double pB = dot(distanceTo_B, vB);

	double lambda = (pA*R2 - pB*M) / D;
	double mu     = (pB*R2 - pA*M) / D;

	SPos vC = lambda * distanceTo_A + mu * distanceTo_B;

	// Ограничение на модуль скорости: |vC| <= 1
	double vLen = LEN(vC);
	if (vLen > 1.0) {
		vC = (1.0 / vLen) * vC; // нормируем
	}

	SPos rC1 = rC + dt * vC;

	return {vC, rC1, true};
}

// Вспомогательная функция: проекция точки C на пересечение двух сфер |X-A|=R, |X-B|=R
// (возвращает ближайшую точку на окружности пересечения)
bool project_C_onto_spheres(
	SPos& rC,
	const SPos& rA, const SPos& rB,
	double R
) {
	const double eps = 1e-9;
	SPos AB = rB - rA;
	double AB2 = dot(AB, AB);
	if (AB2 < eps) return false; // A и B совпали — вырождение

	double d = std::sqrt(AB2);
	// Геометрия: пересечение двух сфер — окружность в плоскости, перпендикулярной AB
	// Середина отрезка AB:
	SPos mid = {0.5*(rA[0]+rB[0]), 0.5*(rA[1]+rB[1]), 0.5*(rA[2]+rB[2])};
	// Расстояние от середины до плоскости окружности:
	double h = (R*R - AB2/4.0);
	if (h < 0) return false; // сферы не пересекаются
	h = std::sqrt(h);

	// Вектор от середины к центру окружности (вдоль AB):
	SPos nAB = AB;
	nAB = (1.0/d) * nAB; // единичный

	// Проекция текущего C на плоскость серединного перпендикуляра:
	SPos C_to_mid = rC - mid;
	double proj_along_AB = dot(C_to_mid, nAB);
	SPos in_plane = C_to_mid - proj_along_AB * nAB;

	// Нормализуем в плоскости до радиуса h (если нужно)
	double r2 = LENPWR2(in_plane);
	if (r2 < eps) {
		// C почти на оси AB — выбираем произвольное направление в плоскости
		// например, поворот nAB вокруг оси X или Y
		SPos axis = {1,0,0};
		if (std::abs(dot(nAB, axis)) > 0.9) axis = {0,1,0};
		in_plane = nAB;
		in_plane[0] += axis[0]; in_plane[1] += axis[1]; in_plane[2] += axis[2];
		in_plane = in_plane * (h / LEN(in_plane));
	} else {
		in_plane = (h / std::sqrt(r2)) * in_plane;
	}

	rC = mid + in_plane;
	return true;
}

// Режим 2: приоритет расстояний — сначала без ограничения скорости, затем проекция
Result compute_C_motion_distance_priority(
	const SPos& rA, const SPos& rB, const SPos& rC_raw,
	const SPos& vA, const SPos& vB,
	double R, double dt
) {
	const double eps = 1e-12;

	// Сначала считаем идеальную минимальную скорость без ограничения |v|<=1
	SPos distanceTo_A = rC_raw - rA;
	SPos distanceTo_B = rC_raw - rB;

	double R2 = R*R;
	double M = dot(distanceTo_A, distanceTo_B);
	double D = R2*R2 - M*M;

	if (D < eps) {
		return {{0,0,0}, rC_raw, false};
	}

	double pA = dot(distanceTo_A, vA);
	double pB = dot(distanceTo_B, vB);

	double lambda = (pA*R2 - pB*M) / D;
	double mu     = (pB*R2 - pA*M) / D;

	SPos vC = lambda * distanceTo_A + mu * distanceTo_B;

	// Шаг по времени
	SPos rC1_raw = rC_raw + dt * vC;

	// Теперь проецируем rC1_raw обратно на пересечение сфер (окружность)
	SPos rC1 = rC1_raw;
	bool ok = project_C_onto_spheres(rC1, rA + dt*vA, rB + dt*vB, R);

	if (!ok) {
		// Если проекция не удалась (сферы не пересекаются), возвращаем шаг без проекции
		rC1 = rC1_raw;
	}

	// Скорость можно переопределить как (rC1 - rC_raw)/dt, если нужно
	return {vC, rC1, true};
}


*/






/*

SPos cross(const SPos& a, const SPos& b) {
	return { a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x };
}

double dot(const SPos& a, const SPos& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Перемещение при движении по дуге с |v|=1
SPos displacement_arc(const SPos& vi, const SPos& vf, double dt) {
	const double eps = 1e-12;
	double c = dot(vi, vf);
	// Ограничиваем cos(theta) в [-1,1] из-за ошибок округления
	if (c > 1.0) c = 1.0;
	if (c < -1.0) c = -1.0;

	double theta = std::acos(c);

	// Почти параллельные векторы: движение почти прямолинейное
	if (theta < eps) {
		return vi * dt;
	}

	SPos n = cross(vi, vf);
	double n_len = std::sqrt(dot(n, n));
	if (n_len < eps) {
		// Векторы коллинеарны и одинаково направлены (theta ~ 0 уже обработано)
		// или противоположно направлены: тогда theta = pi, но n=0.
		// Если theta ~ pi, то поворот на 180° вокруг любой оси в плоскости работает.
		// Для простоты в этом редком случае вернём линейную аппроксимацию.
		return (vi + vf) * (0.5 * dt);
	}
	n = n * (1.0 / n_len);

	// Формула перемещения по дуге
	double A = (dt / theta) * std::sin(theta);
	double B = (dt / theta) * (1.0 - c);

	return vi * A + cross(n, vi) * B;
}

SPos displacement_three_points_arc(const SPos& v0, const SPos& v1, const SPos& v2,
	double t0, double t1, double t2) {
	double dt1 = t1 - t0;
	double dt2 = t2 - t1;
	return displacement_arc(v0, v1, dt1) + displacement_arc(v1, v2, dt2);
}

*/





/*





#include <cmath>
#include <limits>

struct vec3 {
	double x, y, z;
	vec3 operator+(const vec3& b) const { return { x + b.x, y + b.y, z + b.z }; }
	vec3 operator-(const vec3& b) const { return { x - b.x, y - b.y, z - b.z }; }
	vec3 operator*(double s) const { return { x * s, y * s, z * s }; }
};

double dot(const vec3& a, const vec3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 cross(const vec3& a, const vec3& b) {
	return { a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x };
}

double norm_sq(const vec3& v) {
	return dot(v, v);
}

// Перемещение за время dt при движении по дуге с |v|=1, от vi к vf
vec3 displacement_arc_step(const vec3& vi, const vec3& vf, double dt) {
	const double eps = 1e-14;
	const double max_acos = 1.0 - std::numeric_limits<double>::epsilon();
	const double min_acos = -1.0 + std::numeric_limits<double>::epsilon();

	double c = dot(vi, vf);
	if (c > max_acos) c = max_acos;
	if (c < min_acos) c = min_acos;

	double theta = std::acos(c);

	// Почти параллельные векторы: движение почти прямолинейное
	if (theta < eps) {
		return vi * dt;
	}

	vec3 n = cross(vi, vf);
	double n_len_sq = norm_sq(n);
	if (n_len_sq < eps) {
		// Коллинеарные векторы: если угол ~0 — уже обработано, если ~pi —
		// поворот на 180° вокруг любой оси в плоскости даст тот же результат.
		// Для простоты вернём среднюю скорость.
		return (vi + vf) * (0.5 * dt);
	}
	double n_len = std::sqrt(n_len_sq);
	n = n * (1.0 / n_len);

	// Формула для перемещения по дуге
	double factor = dt / theta;
	double A = factor * std::sin(theta);
	double B = factor * (1.0 - c);

	return vi * A + cross(n, vi) * B;
}

// Полное перемещение от T0 до T2 при равных интервалах
vec3 displacement_three_points_equal_dt(const vec3& v0, const vec3& v1, const vec3& v2, double dt) {
	vec3 dr1 = displacement_arc_step(v0, v1, dt);
	vec3 dr2 = displacement_arc_step(v1, v2, dt);
	return dr1 + dr2;
}
*/


int PrepareParams___(const char* src_Params, char* dst_Params, int maxLen)
{
	const char* pCur = src_Params;
	char* pDst = dst_Params;
	int OutLen = 0;
	int nChanges = 0;

	while (*pCur)
	{
		while (*pCur && *pCur != 'P' && OutLen < maxLen)
		{
			OutLen++;
			*(pDst++) = *(pCur++);
		}

		if (*pCur == 'P' && strncmp(pCur, "PRESET_", 7) == 0)
		{
			const char* pCurTpm = pCur;
			int PresetNameLen = 0;
			while (*pCurTpm && *pCurTpm != ' ' && *pCurTpm != '=')
				{pCurTpm++; PresetNameLen++;}

			while (*pCurTpm == ' ') 
				pCurTpm++;

			if (*pCurTpm && *pCurTpm != '=')
			{
				//вставка предустановки
				char nameToFind[100];
				strncpy_s(nameToFind, PresetNameLen, pCur, PresetNameLen);
				nameToFind[PresetNameLen] = 0;

				const char* pCurFind = src_Params;

				while (pCurFind && (pCurFind = strstr(pCurFind, nameToFind)))
				{
					pCurFind += PresetNameLen;
					while (*pCurFind == ' ') pCurFind++;
					if (*pCurFind == '=')
					{
						pCurFind++;
						while (*pCurFind && *pCurFind != '\r' && *pCurFind != '\n' && OutLen < maxLen)
						{
							*(pDst++) = *(pCurFind++);
							OutLen++;
						}

						pCurFind = nullptr;
						pCur += (PresetNameLen - 1);
					}
				}

				nChanges++;
				pCur++;
			}
			else
			{
				*(pDst++) = *(pCur++);
				OutLen++;
			}
		}
		else
		{
			*(pDst++) = *(pCur++);
			OutLen++;
		}


	}
	*pDst = 0;

	if (nChanges > 0)
	{
		char* tmpSrc = new char[maxLen];
		strcpy_s(tmpSrc, maxLen, dst_Params);
		OutLen = PrepareParams___(tmpSrc, dst_Params, maxLen);
		delete[] tmpSrc;
	}

	return OutLen;
}

char *PrepareParamsRecursive(NamedStringArray& Presets, const char* src_Params, char* dst_Params, int maxLen)
{
	int OutLen = 0;
	char* pDst = dst_Params;

	if (dst_Params == nullptr)
		OutLen = -1;
	else if (src_Params == nullptr)
		*dst_Params = 0;
	else
	if (Presets.Count() < 1)
		OutLen = strcpy_s(dst_Params, maxLen, src_Params);
	else
	{
		const char* pCur = src_Params;

		while (*pCur)
		{
			while (*pCur && *pCur != 'P' && OutLen < maxLen)
			{
				OutLen++;
				*(pDst++) = *(pCur++);
			}

			if (*pCur == 'P' && strncmp(pCur, "PRESET_", 7) == 0)
			{
				const char* pCurTpm = pCur;
				int PresetNameLen = 0;
				while (*pCurTpm && *pCurTpm != ';' && *pCurTpm != '=' && (*pCurTpm > ' ' || *pCurTpm < 0))
				{
					pCurTpm++; PresetNameLen++;
				}

				while (*pCurTpm == ' ' || *pCurTpm == '\t')
					pCurTpm++;

				if (*pCurTpm == '=')
				{
					while (*pCur && *pCur != '\r' && *pCur != '\n')
						pCur++;

					while (*pCur == '\r' || *pCur == '\n')
						pCur++;
				}
				else
				{
					//вставка предустановки
					char nameToFind[100];
					strncpy_s(nameToFind, 100, pCur, PresetNameLen);
					nameToFind[PresetNameLen] = 0;

					const char* PresetValue = Presets.GetString(nameToFind);
					if (PresetValue == nullptr)
					{
						*(pDst++) = *(pCur++);
						OutLen++;
					}
					else
					{
						pDst = PrepareParamsRecursive(Presets, PresetValue, pDst, maxLen - OutLen);
						OutLen = (int)(pDst - dst_Params);
						pCur += PresetNameLen;
					}
				}
			}
			else if (*pCur)
			{
				*(pDst++) = *(pCur++);
				OutLen++;
			}


		}
		*pDst = 0;
	}
	return pDst;
}


int PrepareParams(const char* src_Params, char* dst_Params, int maxLen)
{
	char* dst_ParamsRes = dst_Params;
	if (src_Params && dst_Params)
	{
		NamedStringArray Presets(src_Params, "PRESET_", '=');
		dst_ParamsRes = PrepareParamsRecursive(Presets, src_Params, dst_Params, maxLen);
	}
	return (int)(dst_ParamsRes - dst_Params);
}
