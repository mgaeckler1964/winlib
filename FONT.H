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

#ifndef WIN_FONTS_H
#define WIN_FONTS_H

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifndef STRICT
#define STRICT 1
#endif

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 4986 4820 4668 )
#endif

#include <string.h>

#include <windows.h>

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#include <gak/gaklib.h>

#include <WINLIB/gdi.h>
#include <WINLIB\WINLIB.H>

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

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class BasicWindow;

class Font : public GdiObject<HFONT>
{
	friend class FontDialog;
	friend std::ostream &operator << ( std::ostream &out, const Font &font );
	friend std::istream &operator >> ( std::istream &in, Font &font );

	LOGFONT 		fontData;
	int				pointSize;

	BasicWindow		*window;

	void send2Window();

	friend class BasicWindow;
	friend class ControlWindow;
	friend class CallbackWindow;
	operator HFONT ( void ) const
	{
		return getHandle();
	}


	friend class Device;
	Font( HDC device ) : GdiObject<HFONT>( device )
	{
		this->window = NULL;
		pointSize = 0;
		memset( &fontData, 0, sizeof( fontData ) );
	}
	public:
	Font( BasicWindow *window=NULL )
	{
		this->window = window;
		pointSize = 0;
		memset( &fontData, 0, sizeof( fontData ) );
	}
	Font( const Font &source ) : GdiObject<HFONT>( source )
	{
		window = source.window;
		pointSize = source.pointSize;
		fontData = source.fontData;
	}
	const Font &operator = ( const Font &source )
	{
		GdiObject<HFONT>::operator= ( source );
		if( !window )
			window = source.window;
		pointSize = source.pointSize;
		fontData = source.fontData;

		return *this;
	}
	bool isAssigned( void ) const
	{
		return getHandle() ? true : false;
	}

	void deleteFont( void );

	Font &setAnsiFont( void )
	{
		fontData.lfCharSet = ANSI_CHARSET;
		if( isAssigned() )
			create();

		return *this;
	}
	Font &setOemFont( void )
	{
		fontData.lfCharSet = OEM_CHARSET;
		if( isAssigned() )
			create();

		return *this;
	}
	Font &setSymbolFont( void )
	{
		fontData.lfCharSet = SYMBOL_CHARSET;
		if( isAssigned() )
			create();

		return *this;
	}
	Font &setVariableFont( void )
	{
		fontData.lfPitchAndFamily &= 0xFC;
		fontData.lfPitchAndFamily |= VARIABLE_PITCH;
		if( isAssigned() )
			create();

		return *this;
	}
	Font &setFixedFont( void )
	{
		fontData.lfPitchAndFamily &= 0xFC;
		fontData.lfPitchAndFamily |= FIXED_PITCH;
		if( isAssigned() )
			create();

		return *this;
	}
	Font &setFontName( const char *name )
	{
		strncpy( fontData.lfFaceName, name, LF_FACESIZE -1 );
		if( isAssigned() )
			create();

		return *this;
	}
	const char *getFontName( void ) const
	{
		return fontData.lfFaceName;
	}
	Font &setFontSize( int newSize )
	{
		pointSize = newSize;
		fontData.lfHeight = 0;
		if( isAssigned() )
			create();

		return *this;
	}
	Font &setFontSizePixel( int newSize )
	{
		pointSize = 0;
		fontData.lfHeight = -newSize;
		if( isAssigned() )
			create();

		return *this;
	}
	int getFontSize( void ) const;
	int getFontSizePixel( void ) const;

	Font &setBold( void )
	{
		setWeight( FW_BOLD );
		if( isAssigned() )
			create();

		return *this;
	}
	Font &setNormal( void )
	{
		setWeight( FW_NORMAL );
		if( isAssigned() )
			create();

		return *this;
	}
	Font &setWeight( int weight )
	{
		fontData.lfWeight = weight;
		if( isAssigned() )
			create();

		return *this;
	}

	bool isItalic( void ) const
	{
		return fontData.lfItalic;
	}
	Font &setItalic( bool style )
	{
		fontData.lfItalic = BYTE(style);
		if( isAssigned() )
			create();

		return *this;
	}
	bool isUnderline( void ) const
	{
		return fontData.lfUnderline;
	}
	Font &setUnderline( bool style )
	{
		fontData.lfUnderline = BYTE(style);
		if( isAssigned() )
			create();

		return *this;
	}
	bool isStrikeOut( void ) const
	{
		return fontData.lfStrikeOut;
	}
	Font &setStrikeOut( bool style )
	{
		fontData.lfStrikeOut = BYTE(style);
		if( isAssigned() )
			create();

		return *this;
	}
	unsigned long getStyle( void ) const
	{
		unsigned long style = fontData.lfWeight & 0xFFFFUL;
		if( isItalic() )
			style |= 0x10000;
		if( isUnderline() )
			style |= 0x20000;
		if( isStrikeOut() )
			style |= 0x40000;

		return style;
	}
	Font &setStyle( unsigned long style )
	{
		fontData.lfWeight = style & 0xFFFFUL;
		if( style & 0x10000 )
			setItalic(true);
		else
			setItalic(false);
		if( style & 0x20000 )
			setUnderline(true);
		else
			setUnderline(false);
		if( style & 0x40000 )
			setStrikeOut(true);
		else
			setStrikeOut(false);

		if( isAssigned() )
			create();

		return *this;
	}

	long getCharWidth( void ) const
	{
		return fontData.lfWidth;
	}

	bool selectFont(
		const BasicWindow	*parent,
		bool				allwaysCreate,
		unsigned long		Flags = CF_BOTH|CF_INITTOLOGFONTSTRUCT|CF_FORCEFONTEXIST
	);

	bool create( void );
	enum SystemFont
	{
		sfOemFixed = OEM_FIXED_FONT,
		sfAnsiFixed = ANSI_FIXED_FONT,
		sfAnsiVar =  ANSI_VAR_FONT,
		sfSytemVar = SYSTEM_FONT,
		sfSystemFixed = SYSTEM_FIXED_FONT
	};

	void selectFont( SystemFont font );

	STRING toString() const;
	void fromString( const STRING &source, bool alwaysCreate );
};

std::ostream &operator >> ( std::ostream &out, const Font &font );
std::istream &operator << ( std::istream &in, Font &font );

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif

