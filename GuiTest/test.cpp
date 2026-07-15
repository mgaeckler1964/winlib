/*
		Project:		Windows Class Library
		Module:			test.CPP
		Description:	some tests
		Author:			Martin Gäckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2026 Martin Gäckler

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

#include <gak/gaklib.h>
#include <gak/fmtNumber.h>
#include <gak/logfile.h>

#include <WINLIB/WINAPP.H>
#include <WINLIB/popup.h>
#include <WINLIB/CONTROLW.H>
#include <WINLIB/xmlEditorChild.h>
#include <WINLIB/chartWin.h>
#include <WINLIB/OLESRVR.H>

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

using namespace winlib;

// --------------------------------------------------------------------- //
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

static const int WIDTH = 4000;		// for the meta file
static const int HEIGHT = 1000;

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class TestWindow : public PopupWindow
{
	PushButton		myButton;
	EditControl		myResult;
	XMLeditorChild	myXmlChild;
	ChartChild		myChartChild;
	ChartChild		myPieChild;
	ComboBox		myCombo;

	void doRepaint()
	{
		DrawDevice	hDC( this );
		Size		size = getClientSize();

		unsigned char red = (unsigned char)randomNumber( 255 );
		unsigned char green = (unsigned char)randomNumber( 255 );
		unsigned char blue = (unsigned char)randomNumber( 255 );

		hDC.getBrush().create( red, green, blue );
		hDC.rectangle( 0, 0, size.width, size.height );
		//hDC.pie(size.width/2, size.height/2, size.height/2, -1, 0 );

	}
	virtual ProcessStatus handleOk()
	{
		OverlappedWindow	*popup = new OverlappedWindow( nullptr );
		Label *label = new Label( popup );
		label->setText( "POPUP with WS_POPUP && CAPTION" );
		popup->setText( "Overlapped" );
		popup->removeStyle( WS_OVERLAPPEDWINDOW );
		popup->addStyle( WS_POPUP|WS_CAPTION );
		popup->move( 0, 0 );
		popup->create( nullptr, 300, 100 );
		label->create( popup, 0, 0, 0, 0 );

		popup = new OverlappedWindow( nullptr );
		label = new Label( popup );
		label->setText( "POPUP with WS_POPUP && Parent && DLG" );
		popup->setText( "Overlapped" );
		popup->removeStyle( WS_OVERLAPPEDWINDOW );
		popup->addStyle( WS_POPUP|WS_DLGFRAME );
		popup->move( 0, 100 );
		popup->create( this, 300, 100 );
		label->create( popup, 0, 0, 0, 0 );

		popup = new OverlappedWindow( nullptr );
		label = new Label( popup );
		label->setText( "POPUP" );
		popup->setText( "Overlapped" );
		popup->removeStyle( WS_OVERLAPPEDWINDOW );
		popup->move( 0, 200 );
		popup->create( nullptr, 300, 100 );
		label->create( popup, 0, 0, 0, 0 );

		popup = new OverlappedWindow( nullptr );
		label = new Label( popup );
		label->setText( "POPUP with parent" );
		popup->setText( "Overlapped" );
		popup->removeStyle( WS_OVERLAPPEDWINDOW );
		popup->move( 0, 300 );
		popup->create( this, 300, 100 );
		label->create( popup, 0, 0, 0, 0 );

		popup = new OverlappedWindow( nullptr );
		label = new Label( popup );
		label->setText( "POPUP with parent thick" );
		popup->setText( "Overlapped" );
		popup->removeStyle( WS_OVERLAPPEDWINDOW );
		popup->addStyle( WS_THICKFRAME );
		popup->move( 0, 300 );
		popup->create( this, 300, 100 );
		label->create( popup, 0, 0, 0, 0 );

		popup = new OverlappedWindow( nullptr );
		label = new Label( popup );
		label->setText( "POPUP with parent dlg" );
		popup->setText( "Overlapped" );
		popup->removeStyle( WS_OVERLAPPEDWINDOW );
		popup->addStyle( WS_DLGFRAME );
		popup->move( 0, 300 );
		popup->create( this, 300, 100 );
		label->create( popup, 0, 0, 0, 0 );


		popup = new OverlappedWindow( nullptr );
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
	TestWindow() : PopupWindow(nullptr), myXmlChild(nullptr), myChartChild(nullptr), myPieChild(nullptr) {}
	void create()
	{
		addStyle( WS_CLIPCHILDREN );

		setText( "Test" );
		SuccessCode error = PopupWindow::create( nullptr, 300, 200 );
		if( error == scSUCCESS)
		{
			myButton.setId( IDOK );
			myButton.setText( "Start" );
			myButton.removeStyle( WS_CHILD );
			myButton.create( this, 10, 10, 30, 25 );
			myResult.create( this, 50, 10, 60, 25 );

			myXmlChild.sizeNmove(10, 50, 50, 50);
			myXmlChild.create( this );

			myChartChild.sizeNmove( 70, 50, 50, 50 );
			myChartChild.create( this );

			myPieChild.sizeNmove(125, 50, 50, 50);
			myPieChild.create( this );
			myPieChild.add1dChart(Chart1D(RGB(255,0,0),20));
			myPieChild.add1dChart(Chart1D(RGB(0,255,0),29));
			myPieChild.add1dChart(Chart1D(RGB(0,0,255),10));
			myPieChild.set1Dtype( PieChart );

			myCombo.sizeNmove(10, 110, 100, 100 );
			myCombo.create( this );
			myCombo.addEntry( "hello" );
			myCombo.addEntry( "world" );
			myCombo.addEntry( "the" );
			myCombo.addEntry( "quick" );
			myCombo.addEntry( "brown" );
			myCombo.addEntry( "fox" );
			myCombo.addEntry( "jumps" );
			myCombo.addEntry( "over" );
			myCombo.addEntry( "the" );
			myCombo.addEntry( "lazy" );
			myCombo.addEntry( "dog" );
		}
	}
};

class TestApp : Application
{
	virtual bool startApplication( HINSTANCE hInstance, const char *cmdLine );

	virtual CallbackWindow  *createMainWindow( const char * /* cmdLine */, int /* nCmdShow */ )
	{
		TestWindow	*newWindow = new TestWindow;
		newWindow->create();

		return newWindow;
	}
	virtual void deleteMainWindow( BasicWindow  *mainWindow );
};

class MyOleDocument : public OleBaseDocument
{
	STRING				m_currentText;

	public:
	MyOleDocument( GUID guid ) : OleBaseDocument(guid), m_currentText( gak::DateTime().getOriginalTime() )
	{
	}

	// ==========================================================
	// IPersistStorage
	// ==========================================================
	virtual HRESULT STDMETHODCALLTYPE Load( 
	/* [unique][in] */ __RPC__in_opt IStorage *pStgLoad)
	{
		doEnterFunctionEx(gakLogging::llInfo, "MyOleDocument::Load");

		if (!pStgLoad) 
			return E_POINTER;

		IStream* pStream = nullptr;
		// Den bestehenden Stream "Contents" zum Lesen öffnen
		HRESULT hr = pStgLoad->OpenStream(
			L"Contents", 
			nullptr, 
			STGM_READ | STGM_SHARE_EXCLUSIVE, 
			0, &pStream
		);

		if (SUCCEEDED(hr))
		{
			ULONG bytesRead = 0;
			ULONG length = 0;

			// 1. Länge einlesen
			hr = pStream->Read(&length, sizeof(length), &bytesRead);
        
			if (SUCCEEDED(hr) && length > 0)
			{
				m_currentText.setMinSize(length+1);
				hr = pStream->Read((void *)m_currentText.c_str(), length, &bytesRead);
				m_currentText.setActSize(length);
				if (!SUCCEEDED(hr))
				{
					m_currentText.release();
				}
				if(getDataAdviseHolder())
				{
					doLogPositionEx(gakLogging::llInfo);
					getDataAdviseHolder()->SendOnDataChange(this, 0, 0);
				}
				doLogValueEx(gakLogging::llInfo, m_currentText);
			}

			pStream->Release();
		}


		return hr;
	}

	virtual HRESULT STDMETHODCALLTYPE Save( 
	/* [unique][in] */ __RPC__in_opt IStorage *pStgSave,
	/* [in] */ BOOL /*fSameAsLoad*/)
	{
		doEnterFunctionEx(gakLogging::llInfo, "MyOleDocument::Save");

		if (!pStgSave) return E_POINTER;

		IStream* pStream = nullptr;
		// Einen Stream namens "Contents" im übergebenen Storage erstellen/öffnen
		HRESULT hr = pStgSave->CreateStream(
			L"Contents", 
			STGM_WRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE, 
			0, 0, &pStream
		);

		if (SUCCEEDED(hr))
		{
			ULONG bytesWritten = 0;
			ULONG length = static_cast<ULONG>(m_currentText.size());

			// 1. Zuerst die Länge des Strings schreiben
			pStream->Write(&length, sizeof(length), &bytesWritten);
        
			// 2. Dann den String-Inhalt selbst schreiben
			pStream->Write(m_currentText.c_str(), length, &bytesWritten);

			pStream->Release();
		}

		return S_OK;
	}

	// ==========================================================
	// IOleObject       
	// ==========================================================

	virtual HRESULT STDMETHODCALLTYPE DoVerb( 
	/* [in] */ LONG iVerb,
	/* [unique][in] */ __RPC__in_opt LPMSG /*lpmsg*/,
	/* [unique][in] */ __RPC__in_opt IOleClientSite *pActiveSite,
	/* [in] */ LONG /*lindex*/,
	/* [in] */ __RPC__in HWND hwndParent,
	/* [unique][in] */ __RPC__in_opt LPCRECT /*lprcPosRect*/)
	{
		doEnterFunctionEx(gakLogging::llInfo, "MyOleDocument::DoVerb");

		if (iVerb == OLEIVERB_PRIMARY || iVerb == OLEIVERB_SHOW || iVerb == OLEIVERB_OPEN)
		{
			SetClientSite(pActiveSite);
			std::unique_ptr<BasicWindow> wnd( new BasicWindow(hwndParent));
			StringEditor	editor;
			STRING newText = editor.create( wnd.get(), "Edit Text", m_currentText );
			if( m_currentText != newText )
			{
				m_currentText = newText;
				OnDataModified();
			}
		}
		return S_OK;
	}

	// ==========================================================
	// IDataObject
	// ==========================================================

	void RenderText(Device &hMetaDC, const RectBorder &rect )
	{
		doEnterFunctionEx(gakLogging::llInfo, "MyOleDocument::RenderText");
		Font	font;

		// Hintergrund weiß füllen (oder Rectangle zeichnen, um Grenzen zu definieren)
		Brush hBrush;
		hBrush.create(colors::WHITE);
		hMetaDC.rectangle(rect, hBrush);

		int fontSize = 30, fontWeight = FW_BOLD;
		if( m_currentText.size() > 25 )
		{
			fontWeight = FW_THIN;
			fontSize = 5;
		}
		else if( m_currentText.size() > 21 )
		{
			fontWeight = FW_NORMAL;
			fontSize = 8;
		}
		else if( m_currentText.size() > 7 )
		{
			fontWeight = FW_MEDIUM;
			fontSize = 12;
		}

		font.setFontSize(fontSize)
			.setWeight(fontWeight)
			.setNormal()
			.setDefaultFont()
			.setOutPrecision(OUT_TT_ONLY_PRECIS) // WICHTIG: Erzwingt TrueType für stufenlose Skalierung
			.setClipPrecision(CLIP_DEFAULT_PRECIS)
			.setQuality(ANTIALIASED_QUALITY)
//			.setPitchAndFamily(DEFAULT_PITCH | FF_DONTCARE)
			.setFontName("Arial");
		hMetaDC.selectFont( font );

		hMetaDC.clrBackgroundColor();
		hMetaDC.setTextColor(colors::BLACK);
		hMetaDC.drawText( m_currentText, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX );
	}
	
	virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetData( 
	/* [unique][in] */ FORMATETC *pformatetcIn,
	/* [out] */ STGMEDIUM *pmedium)
	{
		doEnterFunctionEx(gakLogging::llInfo, "MyOleDocument::GetData");

		RectBorder rect( 0, 0, WIDTH, HEIGHT );
		// OLE fragt nach der visuellen Repräsentation (Content als Metafile)
		if (pformatetcIn->cfFormat == CF_METAFILEPICT && (pformatetcIn->dwAspect & DVASPECT_CONTENT))
		{
			// Metafile Device Context erstellen
			MetaDevice hMetaDC;
			if (!hMetaDC) 
				return E_FAIL;

			{
				// all GDI tasks must be finnished before creating the meta file
				// => this is one BLOCK and all GDI Objects are destroyed
				// WICHTIG: Dem OLE-Container mitteilen, welchen Bereich das Metafile einnimmt
				hMetaDC.setMapMode(MM_ANISOTROPIC);
				hMetaDC.setWindowOrgEx( winlib::Point(0,0) );
				hMetaDC.setWindowExtEx( Size(WIDTH, HEIGHT));

				RenderText(hMetaDC, rect);
			}

			// In die OLE-Struktur METAFILEPICT verpacken
			LPMETAFILEPICT pMFP = (LPMETAFILEPICT)GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, sizeof(METAFILEPICT));
			if (!pMFP)
			{
				return E_OUTOFMEMORY;
			}

			// Metafile schließen und Handle holen
			HMETAFILE hMetafile = hMetaDC.releaseFile();
			if (!hMetafile) return E_FAIL;

			LPMETAFILEPICT pRes = (LPMETAFILEPICT)GlobalLock(pMFP);
			pRes->mm   = MM_ANISOTROPIC;
			pRes->xExt = WIDTH;		// Breite in OLE-Maßeinheiten (hi-metric)
			pRes->yExt = HEIGHT;	// Höhe in OLE-Maßeinheiten
			pRes->hMF  = hMetafile;
			GlobalUnlock(pMFP);

			// Speicher an das OLE-Medium übergeben
			pmedium->tymed          = TYMED_MFPICT;
			pmedium->hMetaFilePict  = pMFP;
			pmedium->pUnkForRelease = nullptr; // OLE kümmert sich um die Freigabe

			return S_OK;
		}
		else if (pformatetcIn->cfFormat == CF_ENHMETAFILE && (pformatetcIn->tymed & TYMED_ENHMF))
		{
			EnhMetaDevice hMetaDC( nullptr, "My DEMO OLE Server\0Text Objekt\0", &rect );
			if (!hMetaDC) 
				return E_FAIL;

			RenderText(hMetaDC, rect);

			// Das EMF-Handle schließen und generieren
			HENHMETAFILE hEMF = hMetaDC.releaseFile();
			if (!hEMF) return E_FAIL;

			// Die OLE-Struktur befüllen
			pmedium->tymed = TYMED_ENHMF;
			pmedium->hEnhMetaFile = hEMF;      // Das Handle direkt übergeben
			pmedium->pUnkForRelease = nullptr; // OLE übernimmt den Besitz und löscht es selbst via DeleteEnhMetaFile

			return S_OK;
		}

		return DV_E_FORMATETC;
	}
};

// --------------------------------------------------------------------- //
// ----- exported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module static data -------------------------------------------- //
// --------------------------------------------------------------------- //

static TestApp							app;
static OleClassFactory<MyOleDocument>	s_factory;

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
   
bool TestApp::startApplication( HINSTANCE , const char *cmdLine )
{
	doEnableLogEx(gakLogging::llInfo);
	STRING	guidStr = "{2AA3F3C8-0038-42F4-AF8A-F0114CE0DB5D}"; // GetProfile(nullptr, "guid", STRING() );
	GUID	guid;
	DWORD	registerID = initOleServer(&guidStr, &guid, &s_factory);
	s_factory.setRegister( registerID, guid );
	//WriteProfile( true, nullptr, "guid", guidStr ); 
	if( !guidStr.isEmpty() )
		registerOleServer(guidStr,"Mein Testobjekt");
	return 0;
}

void TestApp::deleteMainWindow( BasicWindow  *mainWindow )
{
	delete mainWindow;
	exitOleServer(s_factory.getRegisterID());
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

#if 0
#include <winlib/device.h>

int main()
{
	Printers	buffer;
	long		*resolutions;
	long		numRes;

	int	numPrinters = GetListOfPrinters( buffer );

	int	i,j;

	for( i=0; i<numPrinters; i++ )
	{
		unsigned long	paper = DeviceCapabilities(
			buffer[i].printerName,
			buffer[i].portName,
			DC_MAXEXTENT,
			nullptr,
			nullptr
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
			nullptr,
			nullptr
		);
		if( numRes > 0 )
		{
			resolutions = (long*)malloc( numRes * sizeof( long ) * 2 );
			DeviceCapabilities(
				buffer[i].printerName,
				buffer[i].portName,
				DC_ENUMRESOLUTIONS,
				(LPTSTR)resolutions,
				nullptr
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

int main()
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

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif
