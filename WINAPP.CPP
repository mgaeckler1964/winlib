/*
		Project:		Windows Class Library
		Module:			WINAPP.CPP
		Description:	Application Controller
		Author:			Martin G�ckler
		Address:		Hopfengasse 15, A-4020 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1992-2021 Martin G�ckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin G�ckler, Germany, Munich ``AS IS''
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

#include <gak/xmlParser.h>
#include <gak/directory.h>

#include <winlib/callwin.h>
#include <winlib/winapp.h>
#include <winlib/resource.h>
#include <winlib/menu.h>

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
// ----- exported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

Application	*appObject = NULL;

// --------------------------------------------------------------------- //
// ----- class statics ------------------------------------------------- //
// --------------------------------------------------------------------- //

HINSTANCE Application::appInstance = 0;
HINSTANCE Application::resInstance = 0;

// --------------------------------------------------------------------- //
// ----- module functions ---------------------------------------------- //
// --------------------------------------------------------------------- //

static STRING readRegistration( const char *company, const char *app, const char *section, const char *var, bool publicVal = false )
{
	long		openResult;
	HKEY		softKey;
	HKEY		companyKey;
	HKEY		appKey;
	HKEY		sectionKey;

	DWORD		typeVar;
	char		valueBuff[1024];
	DWORD		valueSize;
	bool		valueFound = false;
	STRING		result;

	valueSize = sizeof( valueBuff ) -1;

	openResult = RegOpenKeyEx(	publicVal	? HKEY_LOCAL_MACHINE
											: HKEY_CURRENT_USER,
								"SOFTWARE", 0,
								KEY_READ, &softKey );
	if( openResult == ERROR_SUCCESS )
	{
		openResult = RegOpenKeyEx(	softKey,
									company, 0,
									KEY_READ,
									&companyKey );
		if( openResult == ERROR_SUCCESS )
		{
			openResult = RegOpenKeyEx(	companyKey,
										app, 0,
										KEY_READ,
										&appKey );
			if( openResult == ERROR_SUCCESS )
			{
				if( section && *section )
				{
					openResult = RegOpenKeyEx(	appKey,
												section, 0,
												KEY_READ,
												&sectionKey );
					if( openResult == ERROR_SUCCESS )
					{
						openResult = RegQueryValueEx( sectionKey, (char *)var, NULL,
													&typeVar, (LPBYTE)valueBuff, &valueSize );
						if( openResult == ERROR_SUCCESS
						&&  valueSize > 0
						&&  typeVar == REG_SZ )
							valueFound = true;

						RegCloseKey( sectionKey );
					}
				}
				else
				{
					openResult = RegQueryValueEx( appKey, (char *)var, NULL,
												&typeVar, (LPBYTE)valueBuff, &valueSize );
					if( openResult == ERROR_SUCCESS
					&&  valueSize > 0
					&&  typeVar == REG_SZ )
						valueFound = true;
				}

				RegCloseKey( appKey );
			}

			RegCloseKey( companyKey );
		}
		RegCloseKey( softKey );
	}

	if( valueFound )
		result = valueBuff;
	else if( !publicVal )
		result = readRegistration( company, app, section, var, true );

	return result;
}

static void writeRegistration( bool publicVal, const char *company, const char *app, const char *section, const char *var, const char *value )
{
	DWORD			dummy;
	long			openResult;
	HKEY			softKey;
	HKEY			companyKey;
	HKEY			appKey;
	HKEY			sectionKey;

	DWORD			valueSize;

	valueSize = DWORD(strlen( value ) +1);

	openResult = RegOpenKeyEx(	publicVal	? HKEY_LOCAL_MACHINE
											: HKEY_CURRENT_USER,
								"SOFTWARE", 0,
								KEY_CREATE_SUB_KEY, &softKey );
	if( openResult == ERROR_SUCCESS )
	{
		openResult = RegCreateKeyEx(	softKey, company, 0, "",
										REG_OPTION_NON_VOLATILE,
										KEY_ALL_ACCESS,
										NULL,
										&companyKey,
										&dummy );
		if( openResult == ERROR_SUCCESS )
		{
			openResult = RegCreateKeyEx(	companyKey, app, 0, "",
											REG_OPTION_NON_VOLATILE,
											KEY_ALL_ACCESS,
											NULL,
											&appKey,
											&dummy );
			if( openResult == ERROR_SUCCESS )
			{
				if( section && *section )
				{
					openResult = RegCreateKeyEx(	appKey, section, 0, "",
													REG_OPTION_NON_VOLATILE,
													KEY_ALL_ACCESS,
													NULL,
													&sectionKey,
													&dummy );
					if( openResult == ERROR_SUCCESS )
					{
						RegSetValueEx( sectionKey, (char *)var, 0,
										REG_SZ, (LPBYTE)value, valueSize );
						RegCloseKey( sectionKey );
					}
				}
				else
					RegSetValueEx( appKey, (char *)var, 0,
									REG_SZ, (LPBYTE)value, valueSize );

				RegCloseKey( appKey );
			}

			RegCloseKey( companyKey );
		}
		RegCloseKey( softKey );
	}
}


// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

Application::Application( const char *newIconName )
{
	guiRoot = NULL;
	guiDoc = NULL;
	company = "CRESD";
	appObject = this;
	iconName = newIconName;
}

Application::Application( int newIconID )
{
	guiRoot = NULL;
	guiDoc = NULL;
	company = "CRESD";
	appObject = this;
	if( newIconID > 0 )
		iconName = MAKEINTRESOURCE( newIconID );
	else
		iconName = NULL;
}

Application::~Application()
{
	if( guiDoc )
		delete guiDoc;
}

// --------------------------------------------------------------------- //
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

STRING Application::getApplicationFromFile( void )
{
	STRING			applicationName;

	const F_STRING	&fileName = getFileName();
	size_t			backSlash = fileName.searchRChar( DIRECTORY_DELIMITER );
	size_t			dot = fileName.searchRChar( '.' );

	if( dot != fileName.no_index )
	{
		if( backSlash != fileName.no_index )
		{
			applicationName = fileName.subString( backSlash+1, dot-backSlash-1 );
		}
		else
		{
			applicationName =  fileName.leftString( dot );
		}

	}
	else if( backSlash != fileName.no_index )
	{
		applicationName = fileName.subString( backSlash+1 );
	}
	else
	{
		applicationName = fileName;
	}

	return applicationName;
}

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

bool Application::startApplication( HINSTANCE, const char * )
{
	return false;
}

CallbackWindow *Application::createMainWindow( const char *, int )
{
	return NULL;
}

void Application::deleteMainWindow( BasicWindow * )
{
}

int Application::AppMain( HINSTANCE hInstance, const char * cmdLine, int nCmdShow )
{
	doEnterFunction("Application::AppMain");

	setInstance( hInstance );

	if( startApplication( hInstance, cmdLine ) )
		return 1;

	mainWindow = createMainWindow( cmdLine, nCmdShow );
	if( !mainWindow )
		return 1;

	mainWindow->mainLoop();

	deleteMainWindow( mainWindow );

	return 0;
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

xml::Document *Application::getGuiDoc( void )
{
	if( !guiDoc )
	{
		STRING		appName = getApplication();
		STRING		country = getCountry();
		F_STRING	fileName = appName + '.' + country + ".gui";

		if( !exists( fileName ) )
		{
			fileName = getPathName( fileName );
		}
		if( !exists( fileName ) )
		{
			fileName = appName + ".gui";
		}
		if( !exists( fileName ) )
		{
			fileName = getPathName( fileName );
		}
		if( exists( fileName ) )
		{
			xml::Parser		theParser( fileName );
			guiDoc = theParser.readFile( false );
			guiRoot = guiDoc->getRoot();
		}
	}

	return guiDoc;
}

xml::Element	*Application::getResource( const STRING &identifier, const STRING &type, const STRING &filterType )
{
	xml::Element	*resource = NULL;

	if( !guiDoc )
		getGuiDoc();

	if( guiRoot )
	{
		STRING	containerType;
		if( type == FORM_TAG )
			containerType = FORMS_TAG;
		else if( type == FRAME_TAG || type == SCROLLER_TAG )
			containerType = FRAMES_TAG;
		else if( type == MENU_TAG )
			containerType = MENUS_TAG;
		else if( type == STRING_TAG )
			containerType = STRINGS_TAG;
		else
			throw LibraryException( "Don't know container type" );

		xml::Element	*container = guiRoot->getElement( containerType );
		if( container )
		{
			if( type == STRING_TAG )
			{
				xml::XmlArray	strings;
				container->getAllElements( &strings, type );
				for(
					xml::XmlArray::iterator it = strings.begin(), endIT = strings.end();
					it != endIT;
					++it
				)
				{
					if( (*it)->getAttribute( filterType ) == identifier )
					{
						resource = *it;
						break;
					}
				}

			}
			else
			{
				for( size_t i=0; i<container->getNumObjects(); i++ )
				{
					xml::Element	*child = container->getElement( i );
					if( child->getTag() == type && child->getAttribute( filterType ) == identifier )
					{
						resource = child;
						break;
					}
				}
			}
		}
	}

	return resource;
}

STRING Application::loadString( int textId )
{
	if( !resourceStrings.hasElement( textId ) )
	{
		char	text[10240];
		STRING	newString;

		if( LoadString( getResInstance(), textId, text, sizeof( text ) )
		||  LoadString( getAppInstance(), textId, text, sizeof( text ) ) )
			newString = text;
		else
		{
			xml::Element *resource = getStringResource( textId );
			if( resource )
				newString = resource->getAttribute( CAPTION_ATTR );
		}

		resourceStrings[textId] = newString;
	}

	return resourceStrings[textId];
}

Menu *Application::loadMenu( const char *menuName )
{
	Menu *theMenu = new Menu();
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

Menu *Application::loadMenu( int menuId )
{
	Menu *theMenu = new Menu();
	if( theMenu->load( getResInstance(), menuId ) )
	{
		if( theMenu->load( getAppInstance(), menuId ) )
		{
			if( theMenu->load( getMenuResource( menuId ) ) )
			{
				delete theMenu;
				theMenu = NULL;
			}
		}
	}

	return theMenu;
}

const STRING &Application::getApplication( void )
{
	if( m_applicationName.isEmpty() )
	{
		m_applicationName = getApplicationFromFile();
	}

	return m_applicationName;
}

const F_STRING &Application::getFileName( void )
{
	if( m_fileName.isEmpty() )
	{
		char	theFileName[10240];

		GetModuleFileName( appInstance, theFileName, sizeof( theFileName ) -1 );
		m_fileName = theFileName;
	}

	return m_fileName;
}

const F_STRING &Application::getPathName( void )
{
	if( m_pathName.isEmpty() )
	{
		const F_STRING	&fileName = getFileName();
		fsplit( fileName, &m_pathName );
		if( m_pathName.endsWith( DIRECTORY_DELIMITER ) )
		{
			m_pathName.cut( m_pathName.strlen() -1 );
		}
	}

	return m_pathName;
}

STRING Application::GetProfile( const char *section, const char *variable, const STRING &defaultVal )
{
	STRING	valueStr;

	valueStr = readRegistration( company, getApplication(), section, variable );
	if( !valueStr )
		valueStr = defaultVal;
	return valueStr;
}

long Application::GetProfile( const char *section, const char *variable, long defaultVal )
{
	STRING	value = GetProfile( section, variable, (const char *)0 );
	long	l;

	if( !value || !value[0U] )
		l = defaultVal;
	else
		l = atol( value );

	return l;
}

#ifdef __BORLANDC__
#pragma argsused
#endif

void Application::WriteProfile( bool publicVal, const char *section, const char *variable, const char *value )
{
	writeRegistration( publicVal, company, getApplication(), section, variable, value );
}

void Application::WriteProfile( bool publicVal, const char *section, const char *variable, long value )
{
	char valueCp[32];
	wsprintf( valueCp, "%ld", value );
	WriteProfile( publicVal, section, variable, valueCp );
}

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

}	// namespace winlib

using namespace winlib;

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE /* hPrevInstance */,
							LPSTR cmdLine, int nCmdShow)
{
	doEnterFunction("WinMain");
	int	result = 0;

	if( appObject )
	{
		Application::setInstance( hInstance );
		result = appObject->AppMain( hInstance, cmdLine, nCmdShow );

	}
	return result;
}

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

