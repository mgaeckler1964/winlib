/*
		Project:		Windows Class Library
		Module:			BASICWIN.CPP
		Description:	The Basic Window Class
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

#undef STRICT
#define STRICT 1

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <string.h>
#include <stdlib.h>

#include <gak/numericString.h>

#include <WINLIB/frame.h>
#include <winlib/layout.h>
#include <winlib/basicwin.h>
#include <WINLIB/DEVICE.H>

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
// ----- class statics ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

ForeignWindow::~ForeignWindow()
{
}

BasicWindow::~BasicWindow()
{
	// for control windows this is not mandatory
	//assert( !isValid() );
	if( m_layoutData )
	{
		delete m_layoutData;
	}
}

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

void BasicWindow::checkSize( int width, int height )
{
	if( isValid() && (!width || !height) && !m_caption.isEmpty() )
	{
		Size		size;
		DrawDevice	hdc( this );

		if( m_font.isAssigned() )
		{
			hdc.selectFont( m_font );
		}

		hdc.getTextExtent( m_caption, m_caption.strlen(), &size );
		if( !width )
		{
			width = size.width + m_widthOffset;
			m_autoWidth = true;
		}

		if( !height )
		{
			height = size.height + m_heightOffset;
			m_autoHeight = true;
		}
	}

	m_width = width;
	m_height = height;
}

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

STRING ForeignWindow::getWindowClassName( void ) const
{
	STRING	myClassName;

	if( m_winHandle )
	{
		char buffer[10240];
		RealGetWindowClass( m_winHandle, buffer, sizeof( buffer ) );
		myClassName = buffer;
	}

	return myClassName;
}

bool BasicWindow::handleChildClose( BasicWindow *child, bool deleted )
{
	for(
		OwnedWindows::iterator it = m_ownedWindows.begin(), endIT = m_ownedWindows.end();
		it != endIT;
		++it
	)
	{
		if( child == *it )
		{
			if( !deleted )
			{
				delete child;
			}
			m_ownedWindows.erase( it );
			deleted = true;
/*v*/		break;
		}
	}

	m_childWindows.removeElement( child );

	return deleted;
}

bool BasicWindow::canClose( void )
{
	// check child windows! not owned windows
	for(
		ChildWindows::iterator it = m_childWindows.begin(), endIT = m_childWindows.end();
		it != endIT;
		++it
	)
	{
		if( !(*it)->canClose() )
		{
			return false;
		}
	}


	return true;
}

SuccessCode ForeignWindow::close( void )
{
	return DestroyWindow( handle() ) ? scSUCCESS : scERROR;
}

SuccessCode BasicWindow::close( void )
{
	// close owned windows that are not child windows (required for MIDI tools)
	for(  
		OwnedWindows::reverse_iterator it = m_ownedWindows.rbegin(), endIT = m_ownedWindows.rend();
		it != endIT;
		++it
	)
	{
		if( !m_childWindows.hasElement( *it ) )
		{
			if( (*it)->close() == scERROR)
			{
				return scERROR;
			}
		}
	}

	// close child windows! not owned windows
	for(  
		ChildWindows::reverse_iterator it = m_childWindows.rbegin(), endIT = m_childWindows.rend();
		it != endIT;
		++it
	)
	{
		if( (*it)->close() == scERROR)
		{
			return scERROR;
		}
	}

	return ForeignWindow::close();
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

void BasicWindow::saveWindowRect( const char *winName ) const
{
	STRING	title;

	if( !winName || !*winName )
	{
		title = getTitle(); 
		winName = title;
	}

	unsigned long	style = getStyle();
	if( style & WS_MINIMIZE )
	{
		appObject->WriteProfile( false, winName, "state", "minimize" );
	}
	else if( style & WS_MAXIMIZE )
	{
		appObject->WriteProfile( false, winName, "state", "maximize" );
	}
	else
	{
		appObject->WriteProfile( false, winName, "state", "normal" );

		RectBorder	rect = getWindowRectangle();

		appObject->WriteProfile( false, winName, "left", rect.left );
		appObject->WriteProfile( false, winName, "top", rect.top );
		appObject->WriteProfile( false, winName, "width", rect.right - rect.left );
		appObject->WriteProfile( false, winName, "height", rect.bottom - rect.top );
	}
}

void BasicWindow::restoreWindowRect(
	const char	*winName,
	int			nCmdShow 
)
{
	STRING	title;

	if( !winName || !*winName )
	{
		title = getTitle(); 
		winName = title;
	}

	STRING	winState = appObject->GetProfile( winName, "state", "" );
	if( winState.isEmpty() )
	{
		// window rect not saved
		show( nCmdShow );
	}
	else if( winState =="minimize" )
	{
		minimize();
	}
	else if( winState == "maximize" )
	{
		maximize();
	}
	else
	{
		int x = int(appObject->GetProfile( winName, "left", 0 ));
		int y = int(appObject->GetProfile( winName, "top", 0 ));
		int w = int(appObject->GetProfile( winName, "width", 0 ));
		int h = int(appObject->GetProfile( winName, "height", 0 ));

		if( h && w )
			sizeNmove( x, y, w, h );
		show( nCmdShow );
	}
}

void BasicWindow::restoreWindowPos(
	const char	*winName,
	int			nCmdShow 
)
{
	STRING	title;

	if( !winName || !*winName )
	{
		title = getTitle(); 
		winName = title;
	}

	STRING	winState = appObject->GetProfile( winName, "state", "" );
	if( !winState.isEmpty() )				// window rect not saved
	{
		int x = int(appObject->GetProfile( winName, "left", 0 ));
		int y = int(appObject->GetProfile( winName, "top", 0 ));
		move( x, y );
	}
	show( nCmdShow );
}

STRING ForeignWindow::getText( bool oemConvert ) const
{
	STRING	text;
	int		length = getTextLength();
	if( length )
	{
		char *textBuffer = text.setActSize( length );
		GetWindowText( m_winHandle, textBuffer, length+1 );

		if( oemConvert )
		{
			AnsiToOem( textBuffer, textBuffer );
		}
	}

	return text;
}

#ifndef __BORLANDC__
FrameChild *BasicWindow::findFrame( const Point &position ) const
{
	FrameChild	*foundChild = NULL;

	for( 
		ChildWindows::const_iterator it = m_childWindows.cbegin(), endIT = m_childWindows.cend();
		it != endIT;
		++it
	)
	{
		FrameChild	*child = dynamic_cast<FrameChild*>( *it );

		if( !child )
			continue;

		RectBorder	childRect = child->getRelativeRectangle();

		if( childRect.left > position.x )
			continue;

		if( childRect.right < position.x )
			continue;

		if( childRect.top > position.y )
			continue;

		if( childRect.bottom < position.y )
			continue;

		foundChild = child;
		break;
	}

	if( foundChild )
	{
		FrameChild	*child = foundChild->findFrame( position );
		if( child )
			foundChild = child;
	}

	return foundChild;
}
#endif

BasicWindow *BasicWindow::findChild( const Point &position ) const
{
	BasicWindow	*foundChild = NULL;
	long		smallest = std::numeric_limits<long>::max();

	for( 
		ChildWindows::const_iterator it = m_childWindows.cbegin(), endIT = m_childWindows.cend();
		it != endIT;
		++it
	)
	{
		BasicWindow	*child = *it;
		RectBorder	childRect = child->getRelativeRectangle();

		if( childRect.left > position.x )
			continue;

		if( childRect.right < position.x )
			continue;

		if( childRect.top > position.y )
			continue;

		if( childRect.bottom < position.y )
			continue;

		long size = (childRect.right-childRect.left) * (childRect.bottom-childRect.top);

		if( smallest > size )
		{
			foundChild = child;
			smallest = size;
		}
	}

	if( foundChild )
	{
		int		childX, childY;
		Point	newPosition;

		foundChild->getRelativePosition( &childX, &childY );
		newPosition.x = position.x - childX;
		newPosition.y = position.y - childY;
		BasicWindow	*child = foundChild->findChild( newPosition );
		if( child )
		{
			foundChild = child;
		}
	}

	return foundChild;
}

BasicWindow *BasicWindow::findChild( xml::Element *resource ) const
{
	if( resource == getResource() )
	{
		return const_cast<BasicWindow*>(this);
	}

	for( 
		ChildWindows::const_iterator it = m_childWindows.cbegin(), endIT = m_childWindows.cend();
		it != endIT;
		++it
	)
	{
		BasicWindow	*child = *it;
		if( resource == child->getResource() )
		{
			return child;
		}

		if( (child = child->findChild( resource )) != NULL )
		{
			return child;
		}
	}

	return NULL;
}

BasicWindow *BasicWindow::findChild( int id ) const
{
	if( id == m_controlId )
	{
		return const_cast<BasicWindow*>(this);
	}

	for( 
		ChildWindows::const_iterator it = m_childWindows.cbegin(), endIT = m_childWindows.cend();
		it != endIT;
		++it
	)
	{
		BasicWindow	*child = *it;
		if( id == child->m_controlId )
		{
			return child;
		}

		if( (child = child->findChild( id )) != NULL )
		{
			return child;
		}
	}

	return NULL;
}

 BasicWindow *BasicWindow::findChild( HWND iHandle ) const
{
	if( iHandle == handle() )
	{
		return const_cast<BasicWindow*>(this);
	}

	for( 
		ChildWindows::const_iterator it = m_childWindows.cbegin(), endIT = m_childWindows.cend();
		it != endIT;
		++it
	)
	{
		BasicWindow	*child = *it;
		if( iHandle == child->handle() )
		{
			return child;
		}

		if( (child = child->findChild( iHandle )) != NULL )
		{
			return child;
		}
	}

	return NULL;
}

const BasicWindow *BasicWindow::getTopWindow( void ) const
{
	const BasicWindow	*top = this;

	while( top->m_parent != NULL )
	{
		top = top->m_parent;
	}

	return top;
}

#if !defined( __BORLANDC__ ) || __BORLANDC__ > 0x0520
void BasicWindow::setResource( xml::Element *resource )
{
	m_resource = resource;

	BackgroundColor bgColor = getColorValue( resource->getAttribute( "bgColor" ) );
	unsigned char red = resource->getAttribute( "bg_red" ).getValueN<unsigned char>();
	unsigned char green = resource->getAttribute( "bg_green" ).getValueN<unsigned char>();
	unsigned char blue = resource->getAttribute( "bg_blue" ).getValueN<unsigned char>();
	setBackgroundColor( bgColor, red, green, blue );

	unsigned width = resource->getAttribute( LayoutData::widthAttr ).getValueN<unsigned>();
	unsigned height = resource->getAttribute( LayoutData::heightAttr ).getValueN<unsigned>();
	resize( width, height );

	STRING xml = resource->getAttribute( "font" );
	if( !xml.isEmpty() )
	{
		m_font.fromString( xml, true );
	}

	int id = resource->getAttribute( ID_ATTR ).getValueN<int>();
	setId( id );
}
#endif

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


