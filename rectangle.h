/*
		Project:		Windows Class Library
		Module:			rectangle.h
		Description:	Defines dimensions for windows and drawing functions
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

#ifndef WINLIB_RECTANGLE_H
#define WINLIB_RECTANGLE_H

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

struct Size
{
	int width, height;

	Size( int newWidth, int newHeight ) : width( newWidth ), height( newHeight )
	{
	}
	Size()
	{
	}

	bool operator == ( const Size &other ) const
	{
		return width == other.width && height == other.height;
	}
	bool operator != ( const Size &other ) const
	{
		return !(*this == other);
	}
};

struct Point : public POINT
{

	Point() {};
	Point( long newX, long newY )
	{
		x = newX;
		y = newY;
	} 
	Point( const Size &size )
	{
		x = size.width;
		y = size.height;
	}
	const Point & operator = ( const Size &size )
	{
		x = size.width;
		y = size.height;
		return *this;
	}

	bool operator == ( const Point &other ) const
	{
		return x == other.x && x == other.x;
	}
	bool operator != ( const Point &other ) const
	{
		return !(*this == other);
	}
};

struct RectSize;
struct RectBorder : public RECT
{
	void substractBorder( const RectBorder &border )
	{
		left	+= border.left;
		top		+= border.top;
		right	-= border.right;
		bottom	-= border.bottom;
	}

	RectBorder getWithoutBorder( const RectBorder &border ) const
	{
		RectBorder result;

		result.left		= left		+ border.left;
		result.top		= top		+ border.top;
		result.right	= right		- border.right;
		result.bottom	= bottom	- border.bottom;

		return result;
	}

	int getWidth() const
	{
		return right - left;
	}
	int getHeight() const
	{
		return bottom - top;
	}
	operator RectSize () const;

	Point getTopLeft( void ) const
	{
		return Point( left, top );
	}
	Point getTopRight( void ) const
	{
		return Point( right, top );
	}
	Point getBottomLeft( void ) const
	{
		return Point( left, bottom );
	}
	Point getBottomRight( void ) const
	{
		return Point( right, bottom );
	}

	void setTopLeft( const Point &corner )
	{
		top = corner.y;
		left = corner.x;
	}
	void setTopRight( const Point &corner )
	{
		top = corner.y;
		right = corner.x;
	}
	void setBottomLeft( const Point &corner )
	{
		bottom = corner.y;
		left = corner.x;
	}
	void setBottomRight( const Point &corner )
	{
		bottom = corner.y;
		right = corner.x;
	}
};

struct RectSize : public Point, public Size
{
	RectSize()
	{}

	RectSize( const Point &dest ) : Point( dest ) 
	{
		width = height = 0;
	}

	RectSize( const Size &size ) : Size( size ) 
	{
		x = y = 0;
	}

	void substractBorder( const RectBorder &border )
	{
		x		+= border.left;
		y		+= border.top;
		width	-= border.left + border.right;
		height	-= border.top + border.bottom;
	}

	RectSize getWithoutBorder( const RectBorder &border ) const
	{
		RectSize result;

		result.x		= x			+ border.left;
		result.y		= y			+ border.top;
		result.width	= width		- border.left - border.right;
		result.height	= height	- border.top - border.bottom;

		return result;
	}

	operator RectBorder () const
	{
		RectBorder result;

		result.left = x;
		result.top = y;
		result.right = x + width;
		result.bottom = x + height;

		return result;
	}

	Point getTopLeft( void ) const
	{
		return Point( x, y );
	}
	Point getTopRight( void ) const
	{
		return Point( x+width, y );
	}
	Point getBottomLeft( void ) const
	{
		return Point( x, y+height );
	}
	Point getBottomRight( void ) const
	{
		return Point( x+width, y+height );
	}

	void setTopLeft( const Point &corner )
	{
		x = corner.x;
		y = corner.y;
	}
	void setTopRight( const Point &corner )
	{
		width = corner.x - x;
		y = corner.y;
	}
	void setBottomLeft( const Point &corner )
	{
		x = corner.x;
		height = corner.y - y;
	}
	void setBottomRight( const Point &corner )
	{
		width = corner.x - x;
		height = corner.y - y;
	}
};

inline RectBorder::operator RectSize () const
{
	RectSize	result;

	result.x = left;
	result.y = top;
	result.width = getWidth();
	result.height = getHeight();

	return result;
}

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

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif
