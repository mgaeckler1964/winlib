/*
		Project:		Windows Class Library
		Module:			Device.CPP
		Description:	Graphic devices
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

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifndef STRICT
#define STRICT 1
#endif

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak/string.h>

#include <winlib/device.h>

// --------------------------------------------------------------------- //
// ----- module switches ----------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT-
#	pragma option -b
#	pragma option -a4
#	pragma option -pc
#endif

namespace winlib
{

using namespace gak;

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

int Device::getLineHeight( void ) const
{
	TEXTMETRIC	tm;

	GetTextMetrics( device, &tm );

	return tm.tmHeight;
}

int Device::getCharWidth( void ) const
{
	TEXTMETRIC	tm;
	int			pixels;

	pixels = GetDeviceCaps( device, HORZRES	);
	GetTextMetrics( device, &tm );

	return pixels / tm.tmAveCharWidth;
}

int Device::getNumLines( void ) const
{
	TEXTMETRIC	tm;
	int			pixels;

	pixels = GetDeviceCaps( device, VERTRES	);
	GetTextMetrics( device, &tm );

	return pixels / tm.tmHeight;
}

void Device::printText( const char *text ) const
{
	TextOut( device, 0, 0, text, int(strlen( text )) );
}

void Device::getTextExtentWithBreak( const char *text, Size *size )
{
	const char *cp1, *cp2;
	SIZE		lineSize;

	cp1 = text;
	size->width = 0;
	size->height = 0;
	while( 1 )
	{
		for( cp2 = cp1; *cp2 && *cp2 != '\n'; cp2++ )
			;

		GetTextExtentPoint32( device, cp1, int(cp2-cp1), &lineSize );
		size->height += lineSize.cy;

		size->width   = math::max( size->width, int(lineSize.cx) );
		if( !*cp2 )
/*v*/		break;

		cp1 = cp2+1;
	}
}

void Device::getTextExtent( const char *text, size_t len, Size *size )
{
	SIZE	xSize;
	GetTextExtentPoint32( device, text, int(len), &xSize );
	size->width = xSize.cx;
	size->height = xSize.cy;
}

void PrintDevice::printText( const char *document, const char *text ) const
{
	char			c;
	const char		*cp;
	STRING			junk;
	int				lines;
	int				numLines;
	int				lineHeight;
	int				x ; // = getxOffset();
	int				y = getyOffset();
	int				tabWidth;

	numLines = getNumLines();
	lineHeight = getLineHeight();
	tabWidth = getTabWidth();

	startDocument( document );

	cp = text;
	while( *cp )
	{
		junk = "";
		lines = 0;
		startPage();

		moveTo( 0, y );

		while( lines <= numLines && *cp )
		{
			c = *cp++;
			switch( c )
			{
				case '\t':
					if( junk[0U] )
					{
						Device::printText( junk );
						junk = "";
					}
					getPosition( &x, &y );
					x += tabWidth - (x % tabWidth);
					moveTo( x, y );
					break;
				case '\r':
					if( junk[0U] )
					{
						Device::printText( junk );
						junk = "";
					}
					getPosition( NULL, &y );
					moveTo( 0, y );
					break;

				case '\n':
					if( junk[0U] )
					{
						Device::printText( junk );
						junk = "";
					}
					getPosition( &x, &y );
					y += lineHeight;
					moveTo( x, y );
					lines ++;
					break;

				case '\f':
					if( junk[0U] )
					{
						Device::printText( junk );
						junk = "";
					}
					lines = numLines+1;
					break;
				default:
					junk += c;
					break;
			}
		}
		if( junk[0U] )
		{
			Device::printText( junk );
			junk = "";
		}
		endPage();
	}
	endDocument();
}

int	GetListOfPrinters( PRINTER_ARRAY &list )
{
	unsigned long	requiredSize, numPrinters, i;
	PRINTER_INFO_2	*buffer = NULL;
	PRINTER			thePrinter;

	EnumPrinters(
		PRINTER_ENUM_LOCAL, "", 2, (LPBYTE)buffer, 0, &requiredSize, &numPrinters
	);

	buffer = (PRINTER_INFO_2 *)malloc( requiredSize );

	EnumPrinters(
		PRINTER_ENUM_LOCAL, "", 2, (LPBYTE)buffer,
		requiredSize, &requiredSize, &numPrinters
	);

	for( i=0; i<numPrinters; i++ )
	{
		thePrinter.printerName = buffer[i].pPrinterName;
		thePrinter.portName = buffer[i].pPortName;

		list[i] = thePrinter;
	}

	return (int)numPrinters;
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif


