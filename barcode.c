/*
		Project:		Windows Class Library
		Module:			barcode.c
		Description:	Draws and validates EAN8 and ENA13 bar codes
		Author:			Martin Gäckler
		Address:		Hopfengasse 15, A-4020 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1992-2021 Martin Gäckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin Gäckler, Germany, Munich ``AS IS''
		AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
		TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
		PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR
		CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
		SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
		LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
		USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
		ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
		OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
		OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
		SUCH DAMAGE.
*/

/* --------------------------------------------------------------------- */
/* ----- includes ------------------------------------------------------ */
/* --------------------------------------------------------------------- */

#include <winlib/barcode.h>

/* --------------------------------------------------------------------- */
/* ----- module switches ----------------------------------------------- */
/* --------------------------------------------------------------------- */

#ifdef __BORLANDC__
#	pragma option -RT-
#	pragma option -a4
#	pragma option -pc
#endif
#ifdef _MSC_VER
#	pragma warning( disable: 4996 )
#endif
/* --------------------------------------------------------------------- */
/* ----- constants ----------------------------------------------------- */
/* --------------------------------------------------------------------- */

#define EAN8_BAR_COUNT	(8*7 + 2*3 + 5)		// 8 digits + 2 borders + middle bars
#define EAN13_BAR_COUNT	(12*7 + 2*3 + 5)	// 12 digits + 2 borders + middle bars

/* --------------------------------------------------------------------- */
/* ----- type definitions ---------------------------------------------- */
/* --------------------------------------------------------------------- */

struct barCodes
{
	const char odd[8], even[8], right[8], digit13[8];
} barCodes[] =
{
	{ "0001101", "0100111", "1110010", "UUUUUU" },
	{ "0011001", "0110011", "1100110", "UUGUGG" },
	{ "0010011", "0011011", "1101100", "UUGGUG" },
	{ "0111101", "0100001", "1000010", "UUGGGU" },
	{ "0100011", "0011101", "1011100", "UGUUGG" },
	{ "0110001", "0111001", "1001110", "UGGUUG" },
	{ "0101111", "0000101", "1010000", "UGGGUU" },
	{ "0111011", "0010001", "1000100", "UGUGUG" },
	{ "0110111", "0001001", "1001000", "UGUGGU" },
	{ "0001011", "0010111", "1110100", "UGGUGU" },
};

/* --------------------------------------------------------------------- */
/* ----- macros -------------------------------------------------------- */
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* ----- prototypes ---------------------------------------------------- */
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* ----- imported datas ------------------------------------------------ */
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* ----- module statics ------------------------------------------------ */
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* ----- exported datas ------------------------------------------------ */
/* --------------------------------------------------------------------- */

/* --------------------------------------------------------------------- */
/* ----- module functions ---------------------------------------------- */
/* --------------------------------------------------------------------- */
static char getTestDigit( const char *barCode )
{
	int	digit, testDigit, sum=0, factor;

	if( strlen( barCode ) % 2 )
		factor = 3;
	else
		factor = 1;

	while( *barCode )
	{
		digit = *barCode++;
		digit -= '0';
		sum += digit * factor;
		if( factor == 1 )
			factor = 3;
		else
			factor = 1;
	}

	testDigit = sum%10;
	if( testDigit )
		testDigit = 10 - testDigit;

	return (char)(testDigit+'0');
}

/* --------------------------------------------------------------------- */
/* ----- entry points -------------------------------------------------- */
/* --------------------------------------------------------------------- */

void printEAN8Code( HDC hDC, const char *barCode, int x, int y, int width, int height )
{
	if( testEAN8Code( barCode ) )
	{
		int		barWidth = width / EAN8_BAR_COUNT;
		int		i, j, tempX = x;
		char	bars[EAN8_BAR_COUNT];
		SIZE	textSize;
		int		savedDC = SaveDC( hDC );

		j = 0;
		bars[j++] = '1';
		bars[j++] = '0';
		bars[j++] = '1';

		for( i=0; i<4; i++ )
		{
			strcpy( bars+j, barCodes[barCode[i]-'0'].odd );
			j += 7;
		}
		bars[j++] = '0';
		bars[j++] = '1';
		bars[j++] = '0';
		bars[j++] = '1';
		bars[j++] = '0';
		for( i=4; i<8; i++ )
		{
			strcpy( bars+j, barCodes[barCode[i]-'0'].right );
			j += 7;
		}
		bars[j++] = '1';
		bars[j++] = '0';
		bars[j] = '1';

		for( i=0; i<EAN8_BAR_COUNT; i++ )
		{
			if( bars[i] == '0' )
			{
				SelectObject( hDC, GetStockObject(WHITE_BRUSH) );
				SelectObject( hDC, GetStockObject(WHITE_PEN) );
			}
			else
			{
				SelectObject( hDC, GetStockObject(BLACK_BRUSH) );
				SelectObject( hDC, GetStockObject(BLACK_PEN) );
			}

			Rectangle( hDC, tempX, y, tempX+barWidth, y+height );
			tempX += barWidth;
		}

		GetTextExtentPoint( hDC, barCode, 8, &textSize );
		SelectObject( hDC, GetStockObject(WHITE_BRUSH) );
		SelectObject( hDC, GetStockObject(WHITE_PEN) );
		SetBkMode( hDC, TRANSPARENT );
		tempX = x + 3*barWidth;
		Rectangle( hDC, tempX, y+height-textSize.cy, tempX+4*7*barWidth, y+height );
		for( i=0; i<4; i++ )
		{
			TextOut( hDC, tempX, y+height-textSize.cy, barCode+i, 1 );
			tempX += 7*barWidth;
		}

		tempX += 5*barWidth;

		Rectangle( hDC, tempX, y+height-textSize.cy, tempX+4*7*barWidth, y+height );
		for( i=4; i<8; i++ )
		{
			TextOut( hDC, tempX, y+height-textSize.cy, barCode+i, 1 );
			tempX += 7*barWidth;
		}

		RestoreDC( hDC, savedDC );
	}
}

int testEAN8Code( const char *barCode )
{
	char	tmpBarCode[9], testDigit;

	if( strlen( barCode ) != 8 )
		return 0;

	strcpy( tmpBarCode, barCode );
	tmpBarCode[7] = 0;
	testDigit = getTestDigit( tmpBarCode );

	return testDigit == barCode[7];
}

void createEAN8Code( char *barCode )
{
	size_t len = strlen( barCode );
	while( len < 7 )
		barCode[len++] = '0';

	barCode[len]=0;
	barCode[len++] = getTestDigit( barCode );
	barCode[len]=0;
}

void printEAN13Code( HDC hDC, const char *barCode, int x, int y, int width, int height )
{
	if( testEAN13Code( barCode ) )
	{
		int			i, j, tempX, barWidth;
		int			firstDigit = *barCode - '0';
		const char	*firstCode = barCodes[firstDigit].digit13;
		char		bars[EAN13_BAR_COUNT];
		SIZE		textSize;
		int			savedDC = SaveDC( hDC );

		j = 0;
		bars[j++] = '1';
		bars[j++] = '0';
		bars[j++] = '1';

		for( i=1; i<7; i++ )
		{
			struct	barCodes	*barCodeStruct = barCodes + (barCode[i]-'0');
			const char			*code = *firstCode++ == 'U' ? barCodeStruct->odd : barCodeStruct->even;
			strcpy( bars+j, code );
			j += 7;
		}
		bars[j++] = '0';
		bars[j++] = '1';
		bars[j++] = '0';
		bars[j++] = '1';
		bars[j++] = '0';
		for( i=7; i<13; i++ )
		{
			strcpy( bars+j, barCodes[barCode[i]-'0'].right );
			j += 7;
		}
		bars[j++] = '1';
		bars[j++] = '0';
		bars[j] = '1';

		GetTextExtentPoint( hDC, "W", 1, &textSize );
		tempX = x+ textSize.cx;
		barWidth = (width-textSize.cx) / EAN13_BAR_COUNT;

		for( i=0; i<EAN13_BAR_COUNT; i++ )
		{
			if( bars[i] == '0' )
			{
				SelectObject( hDC, GetStockObject(WHITE_BRUSH) );
				SelectObject( hDC, GetStockObject(WHITE_PEN) );
			}
			else
			{
				SelectObject( hDC, GetStockObject(BLACK_BRUSH) );
				SelectObject( hDC, GetStockObject(BLACK_PEN) );
			}

			Rectangle( hDC, tempX, y, tempX+barWidth, y+height );
			tempX += barWidth;
		}

		SelectObject( hDC, GetStockObject(WHITE_BRUSH) );
		SelectObject( hDC, GetStockObject(WHITE_PEN) );
		SetBkMode( hDC, TRANSPARENT );
		Rectangle( hDC, x, y, x+textSize.cx, y+height );
		TextOut( hDC, x, y+height-textSize.cy, barCode, 1 );

		tempX = x + textSize.cx + 3*barWidth;
		Rectangle( hDC, tempX, y+height-textSize.cy, tempX+6*7*barWidth, y+height );
		for( i=1; i<7; i++ )
		{
			TextOut( hDC, tempX, y+height-textSize.cy, barCode+i, 1 );
			tempX += 7*barWidth;
		}

		tempX += 5*barWidth;

		Rectangle( hDC, tempX, y+height-textSize.cy, tempX+6*7*barWidth, y+height );
		for( i=7; i<13; i++ )
		{
			TextOut( hDC, tempX, y+height-textSize.cy, barCode+i, 1 );
			tempX += 7*barWidth;
		}

		RestoreDC( hDC, savedDC );
	}
}

int testEAN13Code( const char *barCode )
{
	char	tmpBarCode[14], testDigit;

	if( strlen( barCode ) != 13 )
		return 0;

	strcpy( tmpBarCode, barCode );
	tmpBarCode[12] = 0;
	testDigit = getTestDigit( tmpBarCode );

	return testDigit == barCode[12];
}

void createEAN13Code( char *barCode )
{
	size_t len = strlen( barCode );
	while( len < 12 )
		barCode[len++] = '0';

	barCode[len]=0;
	barCode[len++] = getTestDigit( barCode );
	barCode[len]=0;
}


#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -a.
#	pragma option -p.
#endif

