/*
		Project:		Windows Class Library
		Module:			ToolTip.CPP
		Description:	ToolTip Windows
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
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <winlib\ToolTip.h>
#include <winlib\device.h>

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
// ----- class static data --------------------------------------------- //
// --------------------------------------------------------------------- //

STRING	ToolTipWindow::className;
Brush	ToolTipWindow::s_brush;

// --------------------------------------------------------------------- //
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

void ToolTipWindow::registerClass()
{
	static bool s_registered = false;

	if( !s_registered )
	{
		WNDCLASS wc;

		className = appObject->getApplication();
		className += "_TOOL_TIP_WINDOW";

		fillDefaultClass( &wc );
		s_brush.create( 255, 255, 128 );
		wc.hbrBackground	= s_brush;
		wc.lpszClassName = className;
		s_registered = CallbackWindow::registerClass( &wc );
	}
}

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

STRING ToolTipWindow::getWindowClassName() const
{
	return className;
}

ProcessStatus ToolTipWindow::handleDestroy()
{
	return psDO_DEFAULT;
}

void ToolTipWindow::changeText( const char *text )
{
	Size		size;
	DrawDevice	device( this );

	device.getTextExtentWithBreak( text, &size );
	resize( size.width+1, size.height );
	m_textWindow.resize( size );
	m_textWindow.setText( text );
}

SuccessCode ToolTipWindow::create( BasicWindow *parent, const char *text )
{
	DrawDevice	device( parent );
	Size		size;
	Point		cursor;

	device.getTextExtentWithBreak( text, &size );
	GetCursorPos( &cursor );

	cursor.x -= size.width/2;
	cursor.y -= size.height;

	if( cursor.x < 0 )
		cursor.x = 0;
	if( cursor.y < 0 )
		cursor.y = 0;

	sizeNmove( cursor.x, cursor.y, size.width+1, size.height+1 );
	SuccessCode	error = CallbackWindow::create( parent );
	if( error == scSUCCESS )
	{
		m_textWindow.setBackgroundColor( 255, 255, 128 );
		m_textWindow.create( this, 0, 0, size.width, size.height );
		m_textWindow.setText( text );
	}
	return error;
}

SuccessCode ToolTipWindow::create( BasicWindow *parent, int textId )
{
	STRING	text = appObject->loadString( textId );

	if( !text.isNullPtr() )
		return create( parent, text );
	else
		return scERROR;
}

SuccessCode ToolTipWindow::close()
{
	m_textWindow.close();
	return CallbackWindow::close();
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

