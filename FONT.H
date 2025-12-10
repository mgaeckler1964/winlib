/*
		Project:		Windows Class Library
		Module: 		FONTS.H
		Description:	Handling fonts
		Author:			Martin Gäckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2025 Martin Gäckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin Gäckler, Linz, Austria ``AS IS''
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
#include <WINLIB/WINLIB.H>

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

	LOGFONT 		m_fontData;
	int				m_pointSize;

	BasicWindow		*m_window;

	void send2Window();

	friend class BasicWindow;
	friend class ControlWindow;
	friend class CallbackWindow;
	operator HFONT () const
	{
		return getHandle();
	}


	friend class Device;
	Font( HDC device ) : GdiObject<HFONT>( device )
	{
		m_window = NULL;
		m_pointSize = 0;
		memset( &m_fontData, 0, sizeof( m_fontData ) );
	}
	public:
	Font( BasicWindow *window=NULL )
	{
		m_window = window;
		m_pointSize = 0;
		if( readFont() )
			memset( &m_fontData, 0, sizeof( m_fontData ) );
	}
	Font( const Font &source ) : GdiObject<HFONT>( source )
	{
		m_window = source.m_window;
		m_pointSize = source.m_pointSize;
		m_fontData = source.m_fontData;
	}
	const Font &operator = ( const Font &source )
	{
		GdiObject<HFONT>::operator= ( source );
		if( !m_window )
			m_window = source.m_window;
		m_pointSize = source.m_pointSize;
		m_fontData = source.m_fontData;

		return *this;
	}
	bool isAssigned() const
	{
		return getHandle() ? true : false;
	}

	void deleteFont();

	Font &setAnsiFont()
	{
		m_fontData.lfCharSet = ANSI_CHARSET;
		if( isAssigned() )
			create();

		return *this;
	}
	Font &setOemFont()
	{
		m_fontData.lfCharSet = OEM_CHARSET;
		if( isAssigned() )
			create();

		return *this;
	}
	Font &setSymbolFont()
	{
		m_fontData.lfCharSet = SYMBOL_CHARSET;
		if( isAssigned() )
			create();

		return *this;
	}
	Font &setVariableFont()
	{
		m_fontData.lfPitchAndFamily &= 0xFC;
		m_fontData.lfPitchAndFamily |= VARIABLE_PITCH;
		if( isAssigned() )
			create();

		return *this;
	}
	Font &setFixedFont()
	{
		m_fontData.lfPitchAndFamily &= 0xFC;
		m_fontData.lfPitchAndFamily |= FIXED_PITCH;
		if( isAssigned() )
			create();

		return *this;
	}
	Font &setFontName( const char *name )
	{
		strncpy( m_fontData.lfFaceName, name, LF_FACESIZE -1 );
		if( isAssigned() )
			create();

		return *this;
	}
	const char *getFontName() const
	{
		return m_fontData.lfFaceName;
	}
	Font &setFontSize( int newSize )
	{
		m_pointSize = newSize;
		m_fontData.lfHeight = 0;
		if( isAssigned() )
			create();

		return *this;
	}
	Font &setFontSizePixel( int newSize )
	{
		m_pointSize = 0;
		m_fontData.lfHeight = -newSize;
		if( isAssigned() )
			create();

		return *this;
	}
	int getFontSize() const;
	int getFontSizePixel() const;

	Font &setBold()
	{
		setWeight( FW_BOLD );
		if( isAssigned() )
			create();

		return *this;
	}
	Font &setNormal()
	{
		setWeight( FW_NORMAL );
		if( isAssigned() )
			create();

		return *this;
	}
	Font &setWeight( int weight )
	{
		m_fontData.lfWeight = weight;
		if( isAssigned() )
			create();

		return *this;
	}

	bool isItalic() const
	{
		return m_fontData.lfItalic;
	}
	Font &setItalic( bool style )
	{
		m_fontData.lfItalic = BYTE(style);
		if( isAssigned() )
			create();

		return *this;
	}
	bool isUnderline() const
	{
		return m_fontData.lfUnderline;
	}
	Font &setUnderline( bool style )
	{
		m_fontData.lfUnderline = BYTE(style);
		if( isAssigned() )
			create();

		return *this;
	}
	bool isStrikeOut() const
	{
		return m_fontData.lfStrikeOut;
	}
	Font &setStrikeOut( bool style )
	{
		m_fontData.lfStrikeOut = BYTE(style);
		if( isAssigned() )
			create();

		return *this;
	}
	unsigned long getStyle() const
	{
		unsigned long style = m_fontData.lfWeight & 0xFFFFUL;
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
		m_fontData.lfWeight = style & 0xFFFFUL;
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

	long getCharWidth() const
	{
		return m_fontData.lfWidth;
	}

	bool selectFont(
		const BasicWindow	*parent,
		bool				allwaysCreate,
		unsigned long		Flags = CF_BOTH|CF_INITTOLOGFONTSTRUCT|CF_FORCEFONTEXIST
	);

	bool create();
	enum SystemFont
	{
		sfOemFixed = OEM_FIXED_FONT,
		sfAnsiFixed = ANSI_FIXED_FONT,
		sfAnsiVar =  ANSI_VAR_FONT,
		sfSytemVar = SYSTEM_FONT,
		sfSystemFixed = SYSTEM_FIXED_FONT
	};

	void selectFont( SystemFont font );
	bool readFont();

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

