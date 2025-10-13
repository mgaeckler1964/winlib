/*
		Project:		Windows Class Library
		Module:			xmlEditorChild.cpp
		Description:	A control used to edit XML data
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

#ifndef STRICT
#define STRICT 1
#endif

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak/xml.h>

#include <winlib/xmlEditorChild.h>
#include <winlib/device.h>
#include <WINLIB/messages.h>

// --------------------------------------------------------------------- //
// ----- module switches ----------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT-
#	pragma option -b
#	pragma option -a4
#	pragma option -pc
 
#	pragma option -x
#	pragma option -xd
#	pragma option -xf
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

const char XMLeditorChild::className[] = "XMLeditorChild";

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

void XMLeditorChild::registerClass()
{
	static bool registered = false;

	if( !registered )
	{
		WNDCLASS	wc;

		fillDefaultClass( &wc );
		wc.style			|= CS_DBLCLKS;
		wc.lpszClassName	 = className;

		registered = ChildWindow::registerClass( &wc );
	}
}

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

STRING XMLeditorChild::getWindowClassName() const
{
	return className;
}

ProcessStatus XMLeditorChild::handleVertScroll( VertScrollCode scrollCode, int nPos, HWND )
{
	switch( scrollCode )
	{
		case vscTOP:
			nPos = 0;
			break;
		case vscLINE_UP:
			nPos = m_vertOffset - 20;
			break;
		case vscPAGE_UP:
			nPos = m_vertOffset - m_size.height;
			break;
		case vscLINE_DOWN:
			nPos = m_vertOffset + 20;
			break;
		case vscPAGE_DOWN:
			nPos = m_vertOffset + m_size.height;
			break;
		case vscBOTTOM:
			nPos = m_boxSize.bottom - m_size.height;
			break;

		case vscTHUMB_POSITION:
		case vscTHUMB_TRACK:
			break;

		default:
/***/		return psDO_DEFAULT;
	}

	if( nPos < 0 )
		nPos = 0;
	else if( nPos > m_boxSize.bottom - m_size.height )
		nPos = m_boxSize.bottom - m_size.height;

	setVertScrollPos( nPos );
	m_vertOffset = nPos;

	invalidateWindow();

	return psPROCESSED;
}

ProcessStatus XMLeditorChild::handleHorizScroll( HorizScrollCode scrollCode, int nPos, HWND )
{
	switch( scrollCode )
	{
		case hscLEFT:
			nPos = 0;
			break;
		case hscLINE_LEFT:
			nPos = m_horizOffset - 20;
			break;
		case hscPAGE_LEFT:
			nPos = m_horizOffset - m_size.width;
			break;
		case hscLINE_RIGHT:
			nPos = m_horizOffset + 20;
			break;
		case hscPAGE_RIGHT:
			nPos = m_horizOffset + m_size.width;
			break;
		case hscRIGHT:
			nPos = m_boxSize.right - m_size.width;
			break;

		case hscTHUMB_POSITION:
		case hscTHUMB_TRACK:
			break;

		default:
/***/		return psDO_DEFAULT;
	}

	if( nPos < 0 )
		nPos = 0;
	else if( nPos > m_boxSize.right - m_size.width )
		nPos = m_boxSize.right - m_size.width;

	setHorizScrollPos( nPos );
	m_horizOffset = nPos;

	invalidateWindow();

	return psPROCESSED;
}

void XMLeditorChild::drawCursor( Device &context )
{
	if( m_cursorPos.getViewerBox() && m_cursorPos.getLine() != -1 && m_cursorPos.getChunk() != -1 )
	{
		m_cursorVisible = !m_cursorVisible;

		const internal::XML_LINE		&theLine = m_cursorPos.getViewerBox()->getLine(m_cursorPos.getLine());
		const internal::XML_LINE_CHUNK	&theChunk = theLine.m_theLine[m_cursorPos.getChunk()];

		context.getPen().selectPen( Pen::spBlack );
		int old = context.setROP2( R2_NOT );

		context.verticalLine(
			theChunk.getPosition().x-m_horizOffset+m_cursorPos.getHorizOffset(),
			theChunk.getPosition().y-m_vertOffset,
			theChunk.getPosition().y+theLine.m_lineHeight-m_vertOffset
		);
		context.setROP2( old );
	}
}

void XMLeditorChild::handleTimer()
{
	DrawDevice	context( this );

	drawCursor( context );
}

ProcessStatus XMLeditorChild::handleRepaint( Device &hDC )
{
	doEnterFunctionEx(gakLogging::llDetail, "XMLeditorChild::handleRepaint");

	if( m_theViewerBox )
	{
		m_cursorVisible = false;

		m_theViewerBox->draw( hDC, this );

		drawCursor( hDC );
	}
	return psPROCESSED;
}

ProcessStatus XMLeditorChild::handleResize( const Size &size )
{
	doEnterFunctionEx(gakLogging::llDetail, "XMLeditorChild::handleResize");

	m_size = size;
	if( m_theViewerBox )
	{
		DrawDevice	context( this );

		hideCursor( context );
		/*
			don't clear cursor -> otherwise handleCharacterInput may crash
		*/
//		cursorPos.clear();

		int maxWidth = size.width;
		while( true )
		{
			try
			{
				m_theViewerBox->calcSize(
					context, 0, 0, maxWidth, maxWidth, size.height, &m_boxSize
				);
				break;
			}
			catch( internal::ResizeException &e )
			{
				maxWidth += e.getIncWidth();
			}
			catch( ... )
			{
/*@*/			throw;
			}
		}

		if( m_boxSize.bottom > size.height )
		{
			showVertScrollBar( 0, m_boxSize.bottom - size.height );
		}
		else
		{
			m_vertOffset = 0;
			hideVertScrollBar();
		}

		if( m_boxSize.right > size.width )
		{
			showHorizScrollBar( 0, m_boxSize.right - size.width );
		}
		else
		{
			m_horizOffset = 0;
			hideHorizScrollBar();
		}

		invalidateWindow();
#ifdef _DEBUG
		dump();
#endif
	}
	return psDO_DEFAULT;
}

void XMLeditorChild::handleFocus()
{
	if( !m_cursorPos.getViewerBox() )
	{
		m_theViewerBox->locateFirstElement( &m_cursorPos );
	}

	enableCursor();
}

void XMLeditorChild::handleKillFocus()
{
	DrawDevice	context( this );

	disableCursor( context );
}

ProcessStatus XMLeditorChild::handleLeftButton(
	LeftButton leftButton, WPARAM modifier, const Point &position
)
{
	doEnterFunctionEx(gakLogging::llDetail, "XMLeditorChild::handleLeftButton");

	if( leftButton == lbUP && !modifier && bool(m_theViewerBox) )
	{
		DrawDevice	context( this );

		hideCursor( context );

		gak::xml::Element *theElement = m_theViewerBox->findElementOnScreen(
			context,
			position.x, position.y, m_horizOffset, m_vertOffset,
			&m_cursorPos
		);
		if( theElement )
		{
			getParent()->postMessage( WM_XML_ITEM_CLICK, 0, (LPARAM)theElement );
			enableCursor();
		}
		else
		{
			disableCursor( context );
		}
	}
	return psPROCESSED;
}

ProcessStatus XMLeditorChild::handleKeyDown( int key )
{
	ProcessStatus	handled = psDO_DEFAULT;

	if( m_cursorPos.getViewerBox() )
	{
		DrawDevice	context( this );
		gak::xml::Element *oldElement = m_cursorPos.getElement();

		handled = psPROCESSED;
		switch( key )
		{
			case VK_DOWN:
				hideCursor( context );
				m_cursorPos.getViewerBox()->moveCursorDown( context, &m_cursorPos );
				break;
			case VK_UP:
				hideCursor( context );
				m_cursorPos.getViewerBox()->moveCursorUp( context, &m_cursorPos );
				break;

			case VK_RIGHT:
				hideCursor( context );
				if( !isControlKey() )
					m_cursorPos.getViewerBox()->moveCursorRight( context, &m_cursorPos );
				else
					m_cursorPos.getViewerBox()->moveCursorRightWord( context, &m_cursorPos );
				break;

			case VK_LEFT:
				hideCursor( context );
				if( !isControlKey() )
					m_cursorPos.getViewerBox()->moveCursorLeft( context, &m_cursorPos );
				else
					m_cursorPos.getViewerBox()->moveCursorLeftWord( context, &m_cursorPos );
				break;

			case VK_HOME:
				hideCursor( context );
				if( !isControlKey() )
					m_cursorPos.getViewerBox()->moveCursorStart( &m_cursorPos );
				else
					m_theViewerBox->locateFirstElement( &m_cursorPos );
				break;

			case VK_END:
				hideCursor( context );
				if( !isControlKey() )
					m_cursorPos.getViewerBox()->moveCursorEnd( &m_cursorPos );
				else
					m_theViewerBox->locateLastElement( &m_cursorPos );
				break;


			case VK_DELETE:
			{
				bool	doWrap;

				hideCursor( context );
				gak::xml::XmlText *xmlText = m_cursorPos.getViewerBox()->deleteCharacter(
					context, this, &m_cursorPos, &doWrap
				);
				if( xmlText )
				{
					if( doWrap )
					{
						handleResize( m_size );
						m_cursorPos.getViewerBox()->findLineNChunk(
							context, xmlText, m_cursorPos.getInsertPos(),
							&m_cursorPos
						);
					}
					getParent()->postMessage( WM_XML_ITEM_CHANGED, 0, (LPARAM)xmlText );
				}
				break;
			}

			case VK_BACK:
			{
				bool	doWrap;

				hideCursor( context );
				if( m_cursorPos.getViewerBox()->moveCursorLeft( context, &m_cursorPos ) )
				{
					gak::xml::XmlText *xmlText = m_cursorPos.getViewerBox()->deleteCharacter(
						context, this, &m_cursorPos, &doWrap
					);
					if( xmlText )
					{
						if( doWrap )
						{
							handleResize( m_size );
							m_cursorPos.getViewerBox()->findLineNChunk(
								context, xmlText, m_cursorPos.getInsertPos(),
								&m_cursorPos
							);
						}
						getParent()->postMessage( WM_XML_ITEM_CHANGED, 0, (LPARAM)xmlText );
					}
				}
				break;
			}

			default:
				handled = psDO_DEFAULT;
		}

		if( m_cursorPos.getViewerBox() )
		{
			const internal::XML_LINE		&theLine = m_cursorPos.getViewerBox()->getLine(m_cursorPos.getLine());
			const internal::XML_LINE_CHUNK	&theChunk = theLine.m_theLine[m_cursorPos.getChunk()];

			int x = theChunk.getPosition().x-m_horizOffset+m_cursorPos.getHorizOffset();
			int yTop = theChunk.getPosition().y-m_vertOffset;
			int yBot = theChunk.getPosition().y+theLine.m_lineHeight-m_vertOffset;

			if( yTop<0 )
				scrollVertical(  m_vertOffset+yTop );
			else if( yBot > m_size.height )
				scrollVertical(  m_vertOffset+(yBot-m_size.height) );
			if( x<0 )
				scrollHorizontal( m_horizOffset+x );
			else if( x>m_size.width )
				scrollHorizontal( m_horizOffset+(x-m_size.width) );
		}

		gak::xml::Element *newElement = m_cursorPos.getElement();
		if( newElement != oldElement )
			getParent()->postMessage( WM_XML_ITEM_CLICK, 0, (LPARAM)newElement );

	}
	return handled;
}

ProcessStatus XMLeditorChild::handleCharacterInput( int c )
{
	if( m_cursorPos.getViewerBox() && c >= ' ' )
	{
		bool		doWrap;
		DrawDevice	context( this );

		hideCursor( context );

		gak::xml::XmlText *xmlText = m_cursorPos.getViewerBox()->insertCharacter(
			context, this, &m_cursorPos, c, &doWrap
		);
		if( xmlText )
		{
			if( doWrap )
			{
				handleResize( m_size );
				m_cursorPos.getViewerBox()->findLineNChunk(
					context, xmlText, m_cursorPos.getInsertPos(),
					&m_cursorPos
				);
			}
			getParent()->postMessage( WM_XML_ITEM_CHANGED, 0, (LPARAM)xmlText );
		}
	}

	return psPROCESSED;
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

/*
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
*/
void XMLeditorChild::setDocument( gak::xml::Document *newDocument )
{
	doEnterFunctionEx(gakLogging::llDetail, "XMLeditorChild::setDocument");

	if( m_theViewerBox )
	{
		m_theViewerBox = nullptr;
	}

	if( newDocument )
	{
		DrawDevice	context( this );

		m_theViewerBox = new internal::XML_VIEWER_BOX;
		m_theViewerBox->buildBoxTree( context, newDocument->getRoot(), nullptr );

		m_horizOffset = m_vertOffset = 0;

		if( m_size.width && m_size.height )
			handleResize( m_size );
#ifdef _DEBUG
		dump();
#endif
	}
}

void XMLeditorChild::refresh()
{
	doEnterFunctionEx(gakLogging::llDetail, "XMLeditorChild::refresh");

	if( m_theViewerBox )
	{
		gak::xml::Element	*theRoot = m_theViewerBox->getElement();

		if( theRoot )
		{
			DrawDevice	context( this );

			disableCursor( context );
			m_theViewerBox = new internal::XML_VIEWER_BOX;
			m_theViewerBox->buildBoxTree( context, theRoot, nullptr );

			handleResize( m_size );
		}
	}
}

void XMLeditorChild::showElement( gak::xml::Element *theElement )
{
	if( m_theViewerBox )
	{
		int			x, y;
		DrawDevice	context( this );

		hideCursor( context );
		bool found = m_theViewerBox->findElementScreenPosition(
			theElement, &x, &y, &m_cursorPos
		);
		if( found )
		{
			if( x<m_horizOffset || x>m_horizOffset+m_size.width )
			{
				m_horizOffset = x;
				setHorizScrollPos( x );
				invalidateWindow();
			}
			if( y<m_vertOffset || y>m_vertOffset+m_size.height )
			{
				m_vertOffset = y;
				setVertScrollPos( y );
				invalidateWindow();
			}
		}
	}
}

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

