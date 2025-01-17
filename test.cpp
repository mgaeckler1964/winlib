/*
		Project:		Windows Class Library
		Module:			test.CPP
		Description:	some tests
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

#include <gak/gaklib.h>
#include <gak/fmtNumber.h>

#include <WINLIB/WINAPP.H>
#include <WINLIB/popup.h>
#include <WINLIB/CONTROLW.H>
#include <WINLIB/xmlEditorChild.h>

using namespace winlib;

class TestWindow : public PopupWindow
{
	PushButton		myButton;
	EditControl		myResult;
	XMLeditorChild	myXmlChild;

	void doRepaint()
	{
		DrawDevice	hDC( this );
		Size		size = getClientSize();

		unsigned char red = randomNumber( 255 );
		unsigned char green = randomNumber( 255 );
		unsigned char blue = randomNumber( 255 );

		hDC.getBrush().create( red, green, blue );
		hDC.rectangle( 0, 0, size.width, size.height );

	}
	virtual ProcessStatus handleOk()
	{
		OverlappedWindow	*popup = new OverlappedWindow( NULL );
		Label *label = new Label( popup );
		label->setText( "POPUP with WS_POPUP && CAPTION" );
		popup->setText( "Overlapped" );
		popup->removeStyle( WS_OVERLAPPEDWINDOW );
		popup->addStyle( WS_POPUP|WS_CAPTION );
		popup->move( 0, 0 );
		popup->create( NULL, 300, 100 );
		label->create( popup, 0, 0, 0, 0 );

		popup = new OverlappedWindow( NULL );
		label = new Label( popup );
		label->setText( "POPUP with WS_POPUP && Parent && DLG" );
		popup->setText( "Overlapped" );
		popup->removeStyle( WS_OVERLAPPEDWINDOW );
		popup->addStyle( WS_POPUP|WS_DLGFRAME );
		popup->move( 0, 100 );
		popup->create( this, 300, 100 );
		label->create( popup, 0, 0, 0, 0 );

		popup = new OverlappedWindow( NULL );
		label = new Label( popup );
		label->setText( "POPUP" );
		popup->setText( "Overlapped" );
		popup->removeStyle( WS_OVERLAPPEDWINDOW );
		popup->move( 0, 200 );
		popup->create( NULL, 300, 100 );
		label->create( popup, 0, 0, 0, 0 );

		popup = new OverlappedWindow( NULL );
		label = new Label( popup );
		label->setText( "POPUP with parent" );
		popup->setText( "Overlapped" );
		popup->removeStyle( WS_OVERLAPPEDWINDOW );
		popup->move( 0, 300 );
		popup->create( this, 300, 100 );
		label->create( popup, 0, 0, 0, 0 );

		popup = new OverlappedWindow( NULL );
		label = new Label( popup );
		label->setText( "POPUP with parent thick" );
		popup->setText( "Overlapped" );
		popup->removeStyle( WS_OVERLAPPEDWINDOW );
		popup->addStyle( WS_THICKFRAME );
		popup->move( 0, 300 );
		popup->create( this, 300, 100 );
		label->create( popup, 0, 0, 0, 0 );

		popup = new OverlappedWindow( NULL );
		label = new Label( popup );
		label->setText( "POPUP with parent dlg" );
		popup->setText( "Overlapped" );
		popup->removeStyle( WS_OVERLAPPEDWINDOW );
		popup->addStyle( WS_DLGFRAME );
		popup->move( 0, 300 );
		popup->create( this, 300, 100 );
		label->create( popup, 0, 0, 0, 0 );


		popup = new OverlappedWindow( NULL );
		label = new Label( popup );
		label->setText( "POPUP with parent dlg&thick" );
		popup->setText( "Overlapped" );
		popup->removeStyle( WS_OVERLAPPEDWINDOW );
		popup->addStyle( WS_DLGFRAME|WS_THICKFRAME );
		popup->move( 0, 300 );
		popup->create( this, 300, 100 );
		label->create( popup, 0, 0, 0, 0 );


		clock_t startTime = clock();
		for( size_t i=0; i<100000; i++ )
		{
			doRepaint();
		}
		myResult.setText( gak::formatFloat( double(clock() - startTime) / CLOCKS_PER_SEC ) );
		return psPROCESSED;
	}
	virtual ProcessStatus handleCancel()
	{
		return psDO_DEFAULT;
	}
	public:
	TestWindow() : PopupWindow( NULL ), myXmlChild(NULL) {}
	void create( void )
	{
		addStyle( WS_CLIPCHILDREN );

		setText( "Test" );
		SuccessCode error = PopupWindow::create( NULL, 300, 200 );
		if( error == scSUCCESS)
		{
			myButton.setId( IDOK );
			myButton.setText( "Start" );
			myButton.removeStyle( WS_CHILD );
			myButton.create( this, 10, 10, 30, 25 );
			myResult.create( this, 50, 10, 60, 25 );

			myXmlChild.sizeNmove(10, 50, 50, 50);
			myXmlChild.create( this );
		}
	}
};

class TestApp : Application
{
	virtual CallbackWindow  *createMainWindow( const char *cmdLine, int nCmdShow )
	{
		TestWindow	*newWindow = new TestWindow;
		newWindow->create();

		return newWindow;
	}
};





static TestApp app;
















#if 0
#include <winlib/device.h>

int main( void )
{
	PRINTER_ARRAY	buffer;
	long			*resolutions;
	long			numRes;

	int	numPrinters = GetListOfPrinters( buffer );

	int	i,j;

	for( i=0; i<numPrinters; i++ )
	{
		unsigned long	paper = DeviceCapabilities(
			buffer[i].printerName,
			buffer[i].portName,
			DC_MAXEXTENT,
			NULL,
			NULL
		);

		printf(
			"%s on %s\nPaper Size: %d*%d\n",
			(const char *)buffer[i].printerName,
			(const char *)buffer[i].portName,
			LOWORD(paper),
			HIWORD(paper)
		);
		numRes = DeviceCapabilities(
			buffer[i].printerName,
			buffer[i].portName,
			DC_ENUMRESOLUTIONS,
			NULL,
			NULL
		);
		if( numRes > 0 )
		{
			resolutions = (long*)malloc( numRes * sizeof( long ) * 2 );
			DeviceCapabilities(
				buffer[i].printerName,
				buffer[i].portName,
				DC_ENUMRESOLUTIONS,
				(LPTSTR)resolutions,
				NULL
			);
			for( j=0;j<numRes; j++ )
			{
				printf( "Resolution: %d * %d\n", (int)resolutions[j*2], (int)resolutions[j*2+1] );
			}
			free( resolutions );
		}

	}
}
#endif

#if 0
#include <stdio.h>
#include <windows.h>

int main( void )
{
	int				result;
	unsigned char	ansi[2];
	wchar_t			unicode[10];

	ansi[1] = 0;
	for( int i=128; i<256; i++ )
	{
		ansi[0] = i;

		result = MultiByteToWideChar( CP_ACP, MB_USEGLYPHCHARS, ansi, 1, unicode, 9 );
		printf( "{ 0x%04X, 0x%02X }, // '%c' ", unicode[0], i, i );
		if( unicode[0] == ansi[0] )
			printf( "= " );
		else if( unicode[0] > 255 )
			printf( "> " );
		else
			printf( "< " );

		printf( "%d\n", result );
	}

	return 0;
}
#endif
