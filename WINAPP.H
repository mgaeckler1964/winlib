/*
		Project:		Windows Class Library
		Module:			WINAPP.H
		Description:	Application Controller
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

#ifndef WIN_APP_H
#define WIN_APP_H

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
#	pragma warning( disable: 4668 4820 )
#endif

#include <windows.h>

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#include <gak/gaklib.h>
#include <gak/string.h>
#include <gak/xml.h>
#include <gak/directory.h>

#include <WINLIB/WINLIB.H>
#include <WINLIB/bitmap.h>
#include <WINLIB/resourceTags.h>
#include <WINLIB/menu.h>

// --------------------------------------------------------------------- //
// ----- module switches ----------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT-
#	pragma option -b
#	pragma option -a4
#	pragma option -pc
#endif

#ifdef _MSC_VER
#	pragma warning( disable: 4355 4365 4512 4625 4626 )
#	pragma warning( 1: 4061 4063 4263 )
#	pragma comment( lib, "winlib.lib" )
#endif

namespace winlib
{

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class CallbackWindow;
class BasicWindow;

#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 4265 )
#endif

class Application
{
	static
	HINSTANCE			m_appInstance;
	static
	HINSTANCE			m_resInstance;

	STRING				m_company;
	STRING				m_country;

	STRING				m_applicationName;
	F_STRING			m_fileName;
	F_STRING			m_pathName;

	HICON				m_applicationIcon;
	const char			*m_iconName;
	CallbackWindow		*m_mainWindow;
	gak::xml::Document	*m_guiDoc;
	gak::xml::Element	*m_guiRoot;

	gak::IntStrMap		m_resourceStrings;

	private:
	STRING getApplicationFromFile();

	protected:
	Application( const char *iconName );

	virtual bool 	startApplication( HINSTANCE hInstance, const char *cmdLine );


	public:
	Application( int iconID=-1 );
	~Application();

	gak::xml::Document *getGuiDoc( const STRING &xmlSrc );
	virtual gak::xml::Document *getGuiDoc();
	F_STRING getGuiFile()
	{
		gak::xml::Document	*doc = getGuiDoc();

#ifdef __BORLANDC__
	#pragma warn -inl
#endif
		return doc ? doc->getFilename() : NULL_STRING;
#ifdef __BORLANDC__
	#pragma warn +inl
#endif
	}

	gak::xml::Element *getResource( const STRING &identifier, const STRING &type, const STRING &filterType );
	gak::xml::Element *getFormResource( const char *resourceName )
	{
		return getResource( resourceName, FORM_TAG, NAME_ATTR );
	}
	gak::xml::Element *getFrameResource( const char *resourceName )
	{
		return getResource( resourceName, FRAME_TAG, NAME_ATTR );
	}
	gak::xml::Element *getScrollerResource( const char *resourceName )
	{
		return getResource( resourceName, SCROLLER_TAG, NAME_ATTR );
	}
	gak::xml::Element *getMenuResource( const char *resourceName )
	{
		return getResource( resourceName, MENU_TAG, NAME_ATTR );
	}
	gak::xml::Element *getMenuResource( int id )
	{
		return getResource( gak::formatNumber( id ), MENU_TAG, ID_ATTR );
	}
	gak::xml::Element *getStringResource( const char *resourceName )
	{
		return getResource( resourceName, STRING_TAG, NAME_ATTR );
	}
	gak::xml::Element *getStringResource( int id )
	{
		return getResource( gak::formatNumber( id ), STRING_TAG, ID_ATTR );
	}

	virtual CallbackWindow  *createMainWindow( const char *cmdLine, int nCmdShow );
	virtual void deleteMainWindow( BasicWindow  *mainWindow );

	virtual int AppMain( HINSTANCE hInstance, const char *cmdLine, int nCmdShow );

	static void setInstance( HINSTANCE theInst )
	{
		m_appInstance  = theInst;
	}
	static void setResInstance( HINSTANCE hInstance )
	{
		m_resInstance = hInstance;
	}
	static HINSTANCE getAppInstance()
	{
		return m_appInstance;
	}
	static HINSTANCE getResInstance()
	{
		return m_resInstance ? m_resInstance : m_appInstance;
	}

	STRING loadString( int stringId );
	void setCountry( const STRING &country )
	{
		m_country = country;
	}
	STRING getCountry() const
	{
		STRING country = m_country;
		if( country.isEmpty() )
		{
			char countryBuff[128];

			GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, countryBuff, sizeof(countryBuff) );

			country = countryBuff;
		}
#ifdef __BORLANDC__
	#pragma warn -inl
#endif
		return country;
#ifdef __BORLANDC__
	#pragma warn +inl
#endif
	}
	void setComapny( const char *name )
	{
		m_company = name;
	}
	void setApplication( const STRING &name )
	{
		m_applicationName = name;
	}
	const STRING &getApplication();
	const F_STRING &getFileName();
	const F_STRING &getPathName();
	const F_STRING getPathName( const STRING &fName )
	{
		return gak::makeFullPath( getFileName(), fName );
	}

	STRING	GetProfile( const char *section, const char *variable, const STRING &defaultVal, bool stupidMode=false );
	long	GetProfile( const char *section, const char *variable, long defaultVal, bool stupidMode=false );
	long	WriteProfile( bool publicVal, const char *section, const char *variable, const char *value, bool stupidMode=false );
	long	WriteProfile( bool publicVal, const char *section, const char *variable, long value, bool stupidMode=false );
	long	DeleteProfile( bool publicVal, const char *section="", const char *variable="" );
	long	DeleteCompanyProfile( bool publicVal );

	template <class RESULT_MENU, class MENU_T>
	RESULT_MENU *loadMenuNew( MENU_T menuName )
	{
		RESULT_MENU *theMenu = new RESULT_MENU();
		if( theMenu->load( getResInstance(), menuName ) )
		{
			if( theMenu->load( getAppInstance(), menuName ) )
			{
				if( theMenu->load( getMenuResource( menuName ) ) )
				{
					delete theMenu;
					theMenu = NULL;
				}
			}
		}

		return theMenu;
	}

	Menu *loadMenu( const char *menuName )
	{
		return loadMenuNew<Menu>(menuName);
	}
	Menu *loadMenu( int menuId )
	{
		return loadMenuNew<Menu>(menuId);
	}

	static HACCEL loadAccelerators( const char *acceleratorsName )
	{
		HACCEL theAccelerators = LoadAccelerators( getResInstance(), acceleratorsName );
		if( !theAccelerators )
			theAccelerators = LoadAccelerators( getAppInstance(), acceleratorsName );

		return theAccelerators;
	}
	static HACCEL loadAccelerators( int acceleratorsId )
	{
		return loadAccelerators( MAKEINTRESOURCE( acceleratorsId ) );
	}
	static Bitmap loadBitmap( const char *bitmapName )
	{
		Bitmap theBitmap = LoadBitmap( getResInstance(), bitmapName );
		if( !theBitmap )
		{
			theBitmap = LoadBitmap( getAppInstance(), bitmapName );
		}
		if( !theBitmap && !IS_INTRESOURCE(bitmapName) )
		{
			theBitmap = HBITMAP(LoadImage( NULL, bitmapName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ));
		}
#ifdef __BORLANDC__
#	pragma warn -inl
#endif
		return theBitmap;
#ifdef __BORLANDC__
#	pragma warn +inl
#endif
	}
	static Bitmap loadBitmap( int bitmapId )
	{
		return loadBitmap( MAKEINTRESOURCE( bitmapId ) );
	}
	static HICON loadIcon( const char *iconName )
	{
		HICON theIcon = LoadIcon( getResInstance(), iconName );
		if( !theIcon )
			theIcon = LoadIcon( getAppInstance(), iconName );

		return theIcon;
	}
	static HICON loadIcon( int iconId )
	{
		return loadIcon( MAKEINTRESOURCE( iconId ) );
	}
	HICON getIcon()
	{
		if( !m_applicationIcon && m_iconName )
			m_applicationIcon = loadIcon( m_iconName );
		if( !m_applicationIcon )
			m_applicationIcon = LoadIcon( NULL, IDI_APPLICATION );
		return m_applicationIcon;
	}
	CallbackWindow *getMainWindow() const
	{
		return m_mainWindow;
	}

#if	0 // ndef __BORLANDC__
	/// TODO: analyse whether this skeleton can be used
	bool isNative()
	{
#ifdef __WIN64__
		return true;
#else
		bool result;
		IsWow64Process( GetCurrentProcess(), &result );
		return !result;
#endif
	}
#endif
};

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

// --------------------------------------------------------------------- //
// ----- exported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

extern Application	*appObject;

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif

