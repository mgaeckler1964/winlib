/*
		Project:		Windows Class Library
		Module: 		FONTS.H
		Description:	Handling fonts
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

#include <gak/stringStream.h>

#include <winlib/basicwin.h>
#include <winlib/stddlg.h>
#include <winlib/font.h>

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
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- exported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module static data -------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class static data --------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- prototypes ---------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module functions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class inlines ------------------------------------------------- //
// --------------------------------------------------------------------- //

inline void Font::send2Window()
{
	if( getHandle() && window )
	{
		window->setFont( *this );
	}
}

// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //
   
// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

bool Font::readFont()
{
	if( window && window->handle() )
	{
		HFONT fnt = HFONT(window->message( WM_GETFONT ));
		if( fnt )
		{
			GdiObject<HFONT>::setHandle( fnt );
			GetObject( fnt, sizeof(fontData), &fontData );
			return false;
		}
	}
	return true;
}

bool Font::selectFont(
	const BasicWindow *parent, bool allwaysCreate, unsigned long Flags
)
{
	FontDialog	fontDlg( &fontData, Flags );
	bool		ok =fontDlg.create( parent );

	if( ok )
	{
		setFontSize( fontDlg.fontDlg.iPointSize/10 );

		if( allwaysCreate && !isAssigned() )
			create();
	}

	return ok;
}

void Font::deleteFont( void )
{
	clear();
	if( window && window->isValid() )
		window->inheritFont();
}

int Font::getFontSize( void ) const
{
	if( pointSize )
		return pointSize;
	else
	{
		bool	release; 
		HDC		device = getConnection();
		if( !device )
		{
			device = GetDC( NULL );
			release = true;
		}
		else
			release = false;
		
		int pointSize = MulDiv(-fontData.lfHeight, 72, GetDeviceCaps(device, LOGPIXELSY));
		if( release )
			ReleaseDC( NULL, device );

		return pointSize;
	}
}

int Font::getFontSizePixel( void ) const
{
	if( pointSize )
	{
		bool	release; 
		HDC		device = getConnection();
		if( !device )
		{
			device = GetDC( NULL );
			release = true;
		}
		else
			release = false;
		
		int pixelSize = MulDiv(pointSize, GetDeviceCaps(device,LOGPIXELSY), 72);
		if( release )
			ReleaseDC( NULL, device );

		return pixelSize;
	}
	else 
		return abs(fontData.lfHeight);
}

STRING Font::toString() const
{
	STRING			result;
	oSTRINGstream	out( result );

	out << *this;
	out.flush();

	return result;
}

void Font::fromString( const STRING &source, bool alwaysCreate )
{
	if( !source.isEmpty() )
	{
		iSTRINGstream	in( source );
		in >> *this;

		if( alwaysCreate || isAssigned() )
			create();
	}
	else
		deleteFont();
}

bool Font::create( void )
{
	fontData.lfHeight = -getFontSizePixel();
	setHandle( CreateFontIndirect( &fontData ) );

	send2Window();

	return isAssigned();
}

void Font::selectFont( SystemFont font )
{
	HFONT	handle = HFONT( GetStockObject(font) );
	setHandle( handle );
	GetObject( handle, sizeof(fontData), &fontData );
	pointSize = 0;

	send2Window();
}

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

std::ostream &operator << ( std::ostream &out, const Font &font )
{
	int lfHeight = -font.getFontSizePixel();

	out	<< font.fontData.lfFaceName << ';' 
		<< lfHeight << ';' << font.fontData.lfWidth << ';' 
		<< font.fontData.lfEscapement << ';'  << font.fontData.lfOrientation << ';'  
		<< font.fontData.lfWeight << ';' 
		<< short(font.fontData.lfItalic) << ';'  << short(font.fontData.lfUnderline) << ';'  
		<< short(font.fontData.lfStrikeOut) << ';' << short(font.fontData.lfCharSet) << ';'  
		<< short(font.fontData.lfOutPrecision) << ';'  << short(font.fontData.lfClipPrecision) << ';' 
		<< short(font.fontData.lfQuality) << ';'  << short(font.fontData.lfPitchAndFamily)
	;

	return out;
}

std::istream &operator >> ( std::istream &in, Font &font )
{
	STRING	faceName;
	while( in.good() )
	{
		char c = char(in.get());
		if( c == ';' )
			break;
		faceName += c;
	}
	strncpy( font.fontData.lfFaceName, faceName, LF_FACESIZE );

	font.pointSize = 0;
	char semicolon;
	short lfItalic, lfUnderline, lfStrikeOut, lfCharSet, lfOutPrecision, lfClipPrecision, lfQuality, lfPitchAndFamily;

	in	>> font.fontData.lfHeight >> semicolon >> font.fontData.lfWidth >> semicolon 
		>> font.fontData.lfEscapement >> semicolon  >> font.fontData.lfOrientation >> semicolon 
		>> font.fontData.lfWeight >> semicolon 
		>> lfItalic >> semicolon  >> lfUnderline >> semicolon  
		>> lfStrikeOut >> semicolon >> lfCharSet >> semicolon  
		>> lfOutPrecision >> semicolon  >> lfClipPrecision >> semicolon 
		>> lfQuality >> semicolon  >> lfPitchAndFamily
	;
	font.fontData.lfItalic = BYTE(lfItalic);
	font.fontData.lfUnderline = BYTE(lfUnderline);
	font.fontData.lfStrikeOut = BYTE(lfStrikeOut);
	font.fontData.lfCharSet = BYTE(lfCharSet);
	font.fontData.lfOutPrecision = BYTE(lfOutPrecision);
	font.fontData.lfClipPrecision = BYTE(lfClipPrecision);
	font.fontData.lfQuality = BYTE(lfQuality);
	font.fontData.lfPitchAndFamily = BYTE(lfPitchAndFamily);

	return in;
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif


