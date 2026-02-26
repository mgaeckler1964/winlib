/*
		Project:		Windows Class Library
		Module:			WINAPP.CPP
		Description:	Application Controller
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

#ifndef STRICT
#define STRICT 1
#endif

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak/xmlParser.h>
#include <gak/directory.h>
#include <gak/stringStream.h>
#include <winlib/callwin.h>
#include <winlib/winapp.h>
#include <winlib/resource.h>
#include <winlib/menu.h>
#include <WINLIB/registry.h>

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

using namespace gak;

// --------------------------------------------------------------------- //
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

static const char SOFTWARE[] = "SOFTWARE";

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

enum Wow6432Mode_t
{
	regTry1, regTry2, useStupidPath
};

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- exported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

Application	*appObject = nullptr;

// --------------------------------------------------------------------- //
// ----- class statics ------------------------------------------------- //
// --------------------------------------------------------------------- //

HINSTANCE Application::m_appInstance = 0;
HINSTANCE Application::m_resInstance = 0;

// --------------------------------------------------------------------- //
// ----- prototypes ---------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module functions ---------------------------------------------- //
// --------------------------------------------------------------------- //

template <class INPUTTYPE>
static long writeRegistration( 
	bool publicVal, const char *company, const char *app, const char *section, 
	const char *var, const INPUTTYPE &i_value, 
	bool stupidMode 
)
{
	long	openResult;
	REGSAM	perm;

	if( publicVal )
	{
		perm = stupidMode ? KEY_ALL_ACCESS|KEY_WOW64_32KEY : KEY_ALL_ACCESS|KEY_WOW64_64KEY;
	}
	else
	{
		perm = KEY_ALL_ACCESS;
	}

	Registry	softKey;

	if( (openResult = softKey.openKey( publicVal, SOFTWARE, perm )) == ERROR_SUCCESS )
	{
		Registry companyKey;

		if( (openResult = companyKey.createKey( softKey, company, perm )) == ERROR_SUCCESS )
		{
			Registry	appKey;

			if( (openResult = appKey.createKey( companyKey, app, perm )) == ERROR_SUCCESS )
			{

				if( section && *section )
				{
					Registry	sectionKey;

					if( (openResult = sectionKey.createKey( appKey, section, perm )) == ERROR_SUCCESS )
					{
						openResult = sectionKey.writeValue( var, i_value );
					}
				}
				else
				{
					openResult = appKey.writeValue( var, i_value );
				}
			}
		}
	}

	return openResult;
}

static long deleteRegistration( bool publicVal, const char *company, const char *app="", const char *section="", const char *var="" )
{
	long	openResult;
	REGSAM	perm[2];
	size_t	permCount;
	bool	deleted = false;

	if( publicVal )
	{
		perm[0] = KEY_ALL_ACCESS;
#ifdef __WIN32__
		perm[1] = KEY_ALL_ACCESS|KEY_WOW64_64KEY;
#else
		perm[1] = KEY_ALL_ACCESS|KEY_WOW64_32KEY;
#endif
		permCount = 2;
	}
	else
	{
		perm[0] = KEY_ALL_ACCESS;
		permCount = 1;
	}

	for( size_t i=0; i<permCount; ++ i )
	{
		Registry	softKey;

		if( (openResult = softKey.openKey( publicVal, SOFTWARE, perm[i] )) == ERROR_SUCCESS )
		{
			if( isEmpty(app) )
			{
				openResult = softKey.deleteSubkey(company);
				deleted = deleted || (openResult == ERROR_SUCCESS);
			}
			else
			{
				Registry	companyKey;

				if( (openResult = companyKey.openSubkey( softKey, company, perm[i] )) == ERROR_SUCCESS )
				{
					if( isEmpty(section) && isEmpty( var ) )
					{
						openResult = companyKey.deleteSubkey(app);
						deleted = deleted || (openResult == ERROR_SUCCESS);
					}
					else
					{
						Registry appKey;
						if( (openResult = appKey.openSubkey( companyKey, app, perm[i] ))  == ERROR_SUCCESS )
						{
							if( !isEmpty(section) )
							{
								Registry sectionKey;
								if( (openResult = sectionKey.openSubkey( appKey, section, perm[i] )) == ERROR_SUCCESS )
								{
									openResult = sectionKey.deleteValue( var );
									deleted = deleted || (openResult == ERROR_SUCCESS);
								}
							}
							else
							{
								openResult = appKey.deleteValue(var);
								deleted = deleted || (openResult == ERROR_SUCCESS);
							}
						}
					}
				}
			}
		}
	}

	return deleted ? ERROR_SUCCESS : openResult;
}

template <class RESULTTYPE>
static RESULTTYPE readRegistration( 
	const char *company, const char *app, const char *section, const char *var, const RESULTTYPE &def, 
	bool publicVal = false,
	Wow6432Mode_t *wow6432mode = nullptr		// stupid microsoft store registries from 32 bit programs in a 64 bit windows in another path
)
{
	doEnterFunctionEx( gakLogging::llInfo, "readRegistration( const char *company, const char *app, const char *section, const char *var, const RESULTTYPE &def, bool publicVal = false, Wow6432Mode_t *wow6432mode = nullptr )" );
	doLogValueEx( gakLogging::llInfo, company ? company : "<NULL>" );
	doLogValueEx( gakLogging::llInfo, app ? app : "<NULL>" );
	doLogValueEx( gakLogging::llInfo, section ? section : "<NULL>" );
	doLogValueEx( gakLogging::llInfo, var ? var : "<NULL>" );
	doLogValueEx( gakLogging::llInfo, publicVal );
	typedef ResultTraits<RESULTTYPE>	tResultTraits;

	Wow6432Mode_t	defaultRegMode = regTry1;

	REGSAM		perm;
	Registry	software;
	bool		valueFound = false;
	RESULTTYPE	result;
	STRING		valueBuff;

	if( !publicVal )
	{
		perm = KEY_READ;
	}
	else
	{
		if( !wow6432mode )
		{
			wow6432mode = &defaultRegMode;
		}
		if( *wow6432mode == regTry2 )
		{
			perm = KEY_READ|KEY_WOW64_32KEY;
		}
		else
		{
			perm = KEY_READ|KEY_WOW64_64KEY;
		}
	}
	if( software.openKey( publicVal, SOFTWARE, perm ) == ERROR_SUCCESS)
	{
		Registry	companyKey;

		if( companyKey.openSubkey( software, company, perm ) == ERROR_SUCCESS )
		{
			Registry	appKey;

			if( appKey.openSubkey( companyKey, app, perm ) == ERROR_SUCCESS )
			{
				if( section && *section )
				{
					Registry	sectionKey;

					if( sectionKey.openSubkey( appKey, section, perm ) == ERROR_SUCCESS )
					{
						valueFound = sectionKey.readValue( var, &result ) == rsOK;
					}
				}
				else
				{
					valueFound = appKey.readValue( var, &result ) == rsOK;
				}
			}
		}
	}

	if( valueFound )
	{
		if( publicVal && *wow6432mode == regTry2 )
		{
			deleteRegistration( true, company, app, section, var );
			writeRegistration( true, company, app, section, var, result, false );
		}
	}
	else if( !publicVal )
	{
		result = readRegistration<RESULTTYPE>( company, app, section, var, def, true, wow6432mode );
	}
	else if( *wow6432mode == regTry1 )
	{
		*wow6432mode = regTry2;

		result = readRegistration<RESULTTYPE>( company, app, section, var, def, true, wow6432mode );
	}
	else
	{
		result = def;
	}

	return result;
}

// --------------------------------------------------------------------- //
// ----- class inlines ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

Application::Application( const char *newIconName )
{
	m_guiRoot = nullptr;
	m_guiDoc = nullptr;
	m_company = "gak";
	appObject = this;
	m_iconName = newIconName;
	m_applicationIcon = nullptr;
}

Application::Application( int newIconID )
{
	m_guiRoot = nullptr;
	m_guiDoc = nullptr;
	m_company = "gak";
	appObject = this;
	if( newIconID > 0 )
		m_iconName = MAKEINTRESOURCE( newIconID );
	else
		m_iconName = nullptr;
	m_applicationIcon = nullptr;
}

Application::~Application()
{
	if( m_guiDoc )
		delete m_guiDoc;
}

// --------------------------------------------------------------------- //
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

STRING Application::getApplicationFromFile()
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
	return nullptr;
}

void Application::deleteMainWindow( BasicWindow * )
{
}

int Application::AppMain( HINSTANCE hInstance, const char * cmdLine, int nCmdShow )
{
	doEnterFunctionEx(gakLogging::llInfo, "Application::AppMain");

	setInstance( hInstance );

	if( startApplication( hInstance, cmdLine ) )
		return 1;

	m_mainWindow = createMainWindow( cmdLine, nCmdShow );
	if( !m_mainWindow )
		return 1;

	m_mainWindow->mainLoop();

	deleteMainWindow( m_mainWindow );

	return 0;
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

xml::Document *Application::getGuiDoc()
{
	if( !m_guiDoc )
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
			m_guiDoc = theParser.readFile( false );
			m_guiRoot = m_guiDoc->getRoot();
		}
	}

	return m_guiDoc;
}

xml::Document *Application::getGuiDoc( const STRING &xmlSrc )
{
	if( !m_guiDoc )
	{
		STRING		appName = getApplication();
		STRING		country = getCountry();
		F_STRING	fileName = appName + '.' + country + ".gui";

		gak::iSTRINGstream	theStream(xmlSrc);

		xml::Parser		theParser( &theStream, fileName );
		m_guiDoc = theParser.readFile( false );
		m_guiRoot = m_guiDoc->getRoot();
	}

	return m_guiDoc;
}

xml::Element	*Application::getResource( const STRING &identifier, const STRING &type, const STRING &filterType )
{
	xml::Element	*resource = nullptr;

	if( !m_guiDoc )
		getGuiDoc();

	if( m_guiRoot )
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

		xml::Element	*container = m_guiRoot->getElement( containerType );
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
	if( !m_resourceStrings.hasElement( textId ) )
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

		m_resourceStrings[textId] = newString;
	}

	return m_resourceStrings[textId];
}

const STRING &Application::getApplication()
{
	if( m_applicationName.isEmpty() )
	{
		m_applicationName = getApplicationFromFile();
	}

	return m_applicationName;
}

const F_STRING &Application::getFileName()
{
	if( m_fileName.isEmpty() )
	{
		DWORD	bufferSize = 1024;
		size_t	curSize;

		while( 1 )
		{
			m_fileName.setMinSize( bufferSize-- );

			curSize = GetModuleFileName( m_appInstance, (LPSTR)m_fileName.c_str(), bufferSize );
			if( curSize < bufferSize )
			{
				m_fileName.setActSize( curSize );
				break;
			}
			bufferSize *= 2;
		}
	}

	return m_fileName;
}

const F_STRING &Application::getPathName()
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

STRING Application::GetProfile( const char *section, const char *variable, const STRING &defaultVal, bool stupidMode )
{
	Wow6432Mode_t	regMode = regTry1;
	if( stupidMode )
		regMode = useStupidPath;
	return readRegistration<STRING>( m_company, getApplication(), section, variable, defaultVal, false, &regMode );
}

long Application::GetProfile( const char *section, const char *variable, long defaultVal, bool stupidMode )
{
	Wow6432Mode_t	regMode = regTry1;
	if( stupidMode )
		regMode = useStupidPath;
	return readRegistration<long>( m_company, getApplication(), section, variable, defaultVal, false, &regMode );
}

#ifdef __BORLANDC__
#pragma argsused
#endif

long Application::WriteProfile( bool publicVal, const char *section, const char *variable, const char *value, bool stupidMode )
{
	return writeRegistration( publicVal, m_company, getApplication(), section, variable, value, stupidMode );
}

long Application::WriteProfile( bool publicVal, const char *section, const char *variable, long value, bool stupidMode )
{
	return writeRegistration( publicVal, m_company, getApplication(), section, variable, value, stupidMode );
}

long Application::DeleteProfile( bool publicVal, const char *section, const char *variable )
{
	return deleteRegistration( publicVal, m_company, getApplication(), section, variable );
}

long Application::DeleteCompanyProfile( bool publicVal )
{
	return deleteRegistration( publicVal, m_company );
}

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

}	// namespace winlib

using namespace winlib;

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE /* hPrevInstance */,
							LPSTR cmdLine, int nCmdShow)
{
	/// TODO: Logfile crashes in Borland C++ Builder -> analyse and fix
	doEnableLogEx( gakLogging::llInfo );
	doDisableLog();
	doEnterFunctionEx(gakLogging::llInfo, "WinMain");
	int	result = 0;

	try
	{
		if( appObject )
		{
			Application::setInstance( hInstance );
			result = appObject->AppMain( hInstance, cmdLine, nCmdShow );

		}
	}
	catch( std::exception &e )
	{
		MessageBox( nullptr, LPCSTR(e.what()), LPCSTR("Exception"), MB_ICONERROR );
		result = -1;
	}
	catch( ... )
	{
		MessageBox( nullptr, LPCSTR("Unknown error 8-("), LPCSTR("Exception"), MB_ICONERROR );
		result = -2;
	}
	return result;
}

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

