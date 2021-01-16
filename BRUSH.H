/*
		Project:		Windows Class Library
		Module:			BRUSH.H
		Description:	Windows Brushes
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

#ifndef WIN_BRUSH_H
#define WIN_BRUSH_H

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

#include <WINLIB/gdi.h>

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

class Brush : public GdiObject<HBRUSH>
{
	LOGBRUSH	logBrush;

	void setBrush( HBRUSH newBrush )
	{
		setHandle( newBrush );
	}

	friend class CallbackWindow;
	friend class ToolTipWindow;
	operator HBRUSH ( void ) const
	{
		return getHandle();
	}

	friend class Device;
	Brush( HDC device ) : GdiObject<HBRUSH>( device )
	{
		memset( &logBrush, 0, sizeof( logBrush ) );
	}

	public:
	Brush()
	{
		memset( &logBrush, 0, sizeof( logBrush ) );
	}

	Brush( const Brush &src ) : GdiObject<HBRUSH>( src )
	{
		logBrush = src.logBrush;
	}
	const Brush & operator = ( const Brush &src )
	{
		GdiObject<HBRUSH>::operator= ( src );
		logBrush = src.logBrush;

		return *this;
	}
	void clearBrush( void )
	{
		clear();
	}

	public:
	enum Style
	{
		bsSolid = BS_SOLID,
		bsNull = BS_NULL,
		bsHatched = BS_HATCHED
	};
	enum FillStyle
	{
		fsHorizontal = HS_HORIZONTAL,
		fsVertical = HS_VERTICAL,
		fsFDiagonal = HS_FDIAGONAL,
		fsBDiagonal = HS_BDIAGONAL,
		fsCross = HS_CROSS,
		fsDiagCross = HS_DIAGCROSS
	};
	/*
		Solid Brush
	*/
	void create( COLORREF color )
	{
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = color;
		setBrush( 
			CreateSolidBrush( logBrush.lbColor = color ) 
		);
	}
	void create( unsigned char red, unsigned char green, unsigned char blue )
	{
		create( RGB( red, green, blue ) );
	}
	void createSyscolor( SystemColor color )
	{
		HBRUSH	handle = HBRUSH( GetSysColorBrush( color ) );
		setBrush( handle );
		GetObject( handle, sizeof(logBrush), &logBrush );
	}
	COLORREF getBackgroundColor() const
	{
		return logBrush.lbColor;
	}
	void create( FillStyle fillStyle, COLORREF fillColor )
	{
		logBrush.lbStyle = BS_HATCHED;
		logBrush.lbHatch = fillStyle;
		logBrush.lbColor = fillColor;
		setBrush( CreateHatchBrush( fillStyle, fillColor ) );
	}
	void create( FillStyle fillStyle, unsigned char red, unsigned char green, unsigned char blue )
	{
		create( fillStyle, RGB( red, green, blue ) );
	}

	/*
		standard brush
	*/
	enum SystemBrush
	{
		sbNull = NULL_BRUSH,
		sbWhite = WHITE_BRUSH,
		sbLightGray = LTGRAY_BRUSH,
		sbGray = GRAY_BRUSH,
		sbDarkGray = DKGRAY_BRUSH,
		sbBlack = BLACK_BRUSH
	};

	void selectBrush( SystemBrush brush )
	{
		HBRUSH	handle = HBRUSH( GetStockObject(brush) );
		setBrush( handle );
		GetObject( handle, sizeof(logBrush), &logBrush );
	}
};

}	// namespace winlib


namespace gak
{
	template <>
	inline void toBinaryStream ( std::ostream &stream, const winlib::Brush::FillStyle &value )
	{
		binaryToBinaryStream( stream, value );
	}
	template <>
	inline void fromBinaryStream( std::istream &stream, winlib::Brush::FillStyle *value )
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

