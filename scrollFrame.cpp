/*
		Project:		Windows Class Library
		Module:			scrollFrame.cpp
		Description:	A FrameChild with a scroll bar
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


// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <winlib/scrollFrame.h>

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

// --------------------------------------------------------------------- //
// ----- exported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module static data -------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class static data --------------------------------------------- //
// --------------------------------------------------------------------- //

const char ScrollFrame::className[] = "ScrollFrame";

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

void ScrollFrame::registerClass()
{
	static bool registered = false;

	if( !registered )
	{
		WNDCLASS	wc;

		fillDefaultClass( &wc );
		wc.style			|= CS_DBLCLKS;
		wc.lpszClassName	 = className;
		wc.hbrBackground	 = NULL;

		registered = CallbackWindow::registerClass( &wc );
	}
}

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

CallbackWindow	*ScrollFrame::getChild()
{
	const ChildWindows &children = getChildren();
	if( children.size() >= 1 )
	{
		if( m_child != children[0] )
		{
			m_child = dynamic_cast<CallbackWindow*>(children[0]);
		}
	}
	else
	{
		m_child = nullptr;
	}

	return m_child;
}

// --------------------------------------------------------------------- //
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //
   
STRING ScrollFrame::getWindowClassName() const
{
	return className;
}

ProcessStatus ScrollFrame::handleResize( const Size &newSize )
{
	CallbackWindow	*child = getChild();
	if( child )
	{
		Size	calcedSize = child->calcSize( newSize );

		child->sizeNmove( 0, 0, calcedSize.width, calcedSize.height );
		if( calcedSize.width <= newSize.width )
		{
			hideHorizScrollBar();
		}
		else
		{
			setHorizScrollPos( 0 );
			showHorizScrollBar(0, calcedSize.width - newSize.width);
		}
		if( calcedSize.height <= newSize.height )
		{
			hideVertScrollBar();
		}
		else
		{
			setVertScrollPos( 0 );
			showVertScrollBar(0, calcedSize.height - newSize.height);
		}

		return psPROCESSED; 
	}

	return psDO_DEFAULT; 
}

ProcessStatus ScrollFrame::handleVertScroll( VertScrollCode scrollCode, int nPos, HWND  )
{
	CallbackWindow	*child = getChild();
	if( child )
	{
		int		rowHeight = 20;
		int		vertOffset;
		int		horizOffset;
		child->getRelativePosition(&horizOffset, &vertOffset);
		vertOffset = -vertOffset;
		horizOffset = -horizOffset;

		Size	size = getClientSize();
		Size	childSize = child->getSize();
		int		totalHeight = childSize.height;

		switch( scrollCode )
		{
			case vscTOP:
				nPos = 0;
				break;
			case vscLINE_UP:
				nPos = vertOffset - rowHeight;
				break;
			case vscPAGE_UP:
				nPos = vertOffset - size.height;
				break;
			case vscLINE_DOWN:
				nPos = vertOffset + rowHeight;
				break;
			case vscPAGE_DOWN:
				nPos = vertOffset + size.height;
				break;
			case vscBOTTOM:
				nPos = totalHeight - size.height;
				break;

			case vscTHUMB_POSITION:
			case vscTHUMB_TRACK:
				break;

			default:
	/***/		return psDO_DEFAULT;
		}

		int maxNpos;
		if( nPos < 0 )
			nPos = 0;
		else if( nPos > (maxNpos = totalHeight - size.height) )
			nPos = maxNpos;

		setVertScrollPos( nPos );
		vertOffset = nPos;
		child->move( -horizOffset, -vertOffset );

		getTopWindow()->invalidateWindow();

		return psPROCESSED;
	}
	return psDO_DEFAULT; 
}

ProcessStatus ScrollFrame::handleHorizScroll( HorizScrollCode scrollCode, int nPos, HWND )
{
	CallbackWindow	*child = getChild();
	if( child )
	{
		int		rowHeight = 20;
		int		vertOffset;
		int		horizOffset;
		child->getRelativePosition(&horizOffset, &vertOffset);
		vertOffset = -vertOffset;
		horizOffset = -horizOffset;

		Size	size = getClientSize();
		Size	childSize = child->getSize();
		int		totalWidth = childSize.width;

		switch( scrollCode )
		{
			case hscLEFT:
				nPos = 0;
				break;
			case hscLINE_LEFT:
				nPos = horizOffset - rowHeight;
				break;
			case hscPAGE_LEFT:
				nPos = horizOffset - size.width;
				break;
			case hscLINE_RIGHT:
				nPos = horizOffset + rowHeight;
				break;
			case hscPAGE_RIGHT:
				nPos = horizOffset + size.width;
				break;
			case hscRIGHT:
				nPos = totalWidth - size.width;
				break;

			case hscTHUMB_POSITION:
			case hscTHUMB_TRACK:
				break;

			default:
/***/			return psDO_DEFAULT;
		}

		int maxNpos;
		if( nPos < 0 )
			nPos = 0;
		else if( nPos > (maxNpos = totalWidth - size.width) )
			nPos = maxNpos;

		setHorizScrollPos( nPos );
		horizOffset = nPos;
		child->move( -horizOffset, -vertOffset );

		getTopWindow()->invalidateWindow();

		return psPROCESSED;
	}
	return psPROCESSED;
}

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

