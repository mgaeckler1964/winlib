/*
		Project:		Windows Class Library
		Module:			pen.h
		Description:	Windows pens
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
#ifndef WIN_PEN_H
#define WIN_PEN_H


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

#include <windows.h>

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#include <gak/css.h>

#include <WINLIB/gdi.h>

// --------------------------------------------------------------------- //
// ----- imported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

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

class Pen : public GdiObject<HPEN>
{
	LOGPEN	logPen;

	void setPen( HPEN newBrush )
	{
		setHandle( newBrush );
	}
	operator HPEN ( void ) const
	{
		return getHandle();
	}


	friend class Device;
	Pen( HDC device ) : GdiObject<HPEN>( device )
	{
		memset( &logPen, 0, sizeof( logPen ) );
	}
	public:
	Pen()
	{
		memset( &logPen, 0, sizeof( logPen ) );
	}

	void clearPen( void )
	{
		clear();
	}



	/*
		user defined pen
	*/
	enum Style
	{
		psSolid = PS_SOLID,
		psDash = PS_DASH,
		psDot = PS_DOT,
		psDashDot = PS_DASHDOT,
		psDashDotDot = PS_DASHDOTDOT,
		psNull =  PS_NULL
	};

	void create( void )
	{
		LOGBRUSH		logBrush;

		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = logPen.lopnColor;

		setPen(
//			CreatePenIndirect( &logPen )
//			CreatePen( logPen.lopnStyle, logPen.lopnWidth.x, logPen.lopnColor )
			ExtCreatePen( PS_ENDCAP_FLAT|PS_GEOMETRIC|logPen.lopnStyle,logPen.lopnWidth.x,&logBrush,0, NULL )
		);
	}

	Pen &setColor( unsigned char red, unsigned char green, unsigned char blue, bool doCreate=true )
	{
		logPen.lopnColor = RGB( red, green, blue );
		if( doCreate )
			create();

		return *this;
	}
	Pen &setWidth( int width, bool doCreate=true )
	{
		logPen.lopnWidth.x = width;
		if( doCreate )
			create();

		return *this;
	}
	Pen &setStyle( Style style, bool doCreate=true )
	{
		logPen.lopnStyle = style;
		if( doCreate )
			create();

		return *this;
	}
	void setStyle( gak::css::Border borderStyle, bool doCreate=true )
	{
		Style penStyle;

		if( borderStyle == gak::css::BORD_HIDDEN )
			penStyle = psNull;
		else if( borderStyle == gak::css::BORD_DOTTED )
			penStyle = psDot;
		else if( borderStyle == gak::css::BORD_DASHED )
			penStyle = psDash;
		else if( borderStyle == gak::css::BORD_GROOVE )
			penStyle = psDashDot;
		else if( borderStyle == gak::css::BORD_RIDGE )
			penStyle = psDashDotDot;
		else
			penStyle = psSolid;

		setStyle( penStyle, doCreate );
	}

	/*
		simple Pen
	*/
	void create( Style style, int width, COLORREF color )
	{
		logPen.lopnStyle = style;
		logPen.lopnWidth.x = width;
		logPen.lopnColor = color;

		setPen( CreatePen( style, width, color ) );
	}
	void create( Style style, int width, unsigned char red, unsigned char green, unsigned char blue )
	{
		create( style, width, RGB( red, green, blue ) );
	}
	/*
		standard pen
	*/
	enum SystemPen
	{
		spNull = NULL_PEN,
		spWhite = WHITE_PEN,
		spBlack = BLACK_PEN
	};
	void selectPen( SystemPen pen )
	{
		HPEN	handle = HPEN( GetStockObject(pen) );
		setPen( handle );
		GetObject( handle, sizeof(logPen), &logPen );
	}
};

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

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

}	// namespace winlib

namespace gak
{
	template <>
	inline void toBinaryStream ( std::ostream &stream, const winlib::Pen::Style &value )
	{
		binaryToBinaryStream( stream, value );
	}
	template <>
	inline void fromBinaryStream( std::istream &stream, winlib::Pen::Style *value )
	{
		binaryFromBinaryStream( stream, value );
	}
}

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif

