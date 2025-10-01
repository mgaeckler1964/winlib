/*
		Project:		Windows Class Library
		Module:			WINAPP.CPP
		Description:	Application Controller
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

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- exported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

Application	*appObject = NULL;

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

template <typename RESULTTYPE>
struct ResultTraits
{
	static const int	sRegistryType = REG_SZ;
	static RESULTTYPE	GetValue( char *valueBuff )
	{
		return valueBuff;
	}
	static size_t GetValueSize(const RESULTTYPE &value)
	{
		return strlen(value)+1;
	}
};

template <>
struct ResultTraits<long>
{
	static const int	sRegistryType = REG_DWORD;
	static long	GetValue(char *valueBuff)
	{
		return *((long*)valueBuff);
	}
	static size_t GetValueSize(long)
	{
		return sizeof(long);
	}
};

template <typename INPUTTYPE>
struct InputTraits : public ResultTraits<INPUTTYPE>
{
	const INPUTTYPE &m_value;
	
	InputTraits(const INPUTTYPE &value) : m_value(value) {}

	size_t GetValueSize()
	{
		return strlen(m_value)+1;
	}

	LPBYTE GetAdress() const
	{
		return (LPBYTE)static_cast<const char*>(m_value);
	}
};

template <>
struct InputTraits<long> : public ResultTraits<long>
{
	long m_value;
	
	InputTraits(long value) : m_value(value) {}

	static size_t GetValueSize()
	{
		return sizeof(long);
	}
	LPBYTE GetAdress() const
	{
		return (LPBYTE)(&m_value);
	}
};

template <class RESULTTYPE>
static RESULTTYPE readRegistration( const char *company, const char *app, const char *section, const char *var, const RESULTTYPE &def, bool publicVal = false )
{
	typedef ResultTraits<RESULTTYPE>	tResultTraits;

	long		openResult;
	HKEY		softKey;
	char		valueBuff[10240];
	DWORD		valueSize= sizeof( valueBuff ) -1;
	bool		valueFound = false;
	RESULTTYPE	result;

	openResult = RegOpenKeyEx(	publicVal	? HKEY_LOCAL_MACHINE
											: HKEY_CURRENT_USER,
								SOFTWARE, 0,
								KEY_READ, &softKey );
	if( openResult == ERROR_SUCCESS )
	{
		HKEY	companyKey;

		openResult = RegOpenKeyEx(	softKey,
									company, 0,
									KEY_READ,
									&companyKey );
		if( openResult == ERROR_SUCCESS )
		{
			HKEY	appKey;

			openResult = RegOpenKeyEx(	companyKey,
										app, 0,
										KEY_READ,
										&appKey );
			if( openResult == ERROR_SUCCESS )
			{
				DWORD	typeVar;

				if( section && *section )
				{
					HKEY	sectionKey;

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
						&&  typeVar == tResultTraits::sRegistryType )
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
					&&  typeVar == tResultTraits::sRegistryType )
						valueFound = true;
				}

				RegCloseKey( appKey );
			}

			RegCloseKey( companyKey );
		}
		RegCloseKey( softKey );
	}

	if( valueFound )
	{
		result = tResultTraits::GetValue(valueBuff);
	}
	else if( !publicVal )
	{
		result = readRegistration<RESULTTYPE>( company, app, section, var, def, true );
	}
	else
	{
		result = def;
	}

	return result;
}

template <class INPUTTYPE>
static void writeRegistration( bool publicVal, const char *company, const char *app, const char *section, const char *var, const INPUTTYPE &i_value )
{
	long	openResult;
	HKEY	softKey;

	openResult = RegOpenKeyEx(	publicVal	? HKEY_LOCAL_MACHINE
											: HKEY_CURRENT_USER,
								SOFTWARE, 0,
								KEY_CREATE_SUB_KEY, &softKey );
	if( openResult == ERROR_SUCCESS )
	{
		HKEY	companyKey;
		DWORD	dummy;

		openResult = RegCreateKeyEx(	softKey, company, 0, "",
										REG_OPTION_NON_VOLATILE,
										KEY_ALL_ACCESS,
										NULL,
										&companyKey,
										&dummy );
		if( openResult == ERROR_SUCCESS )
		{
			HKEY	appKey;

			openResult = RegCreateKeyEx(	companyKey, app, 0, "",
											REG_OPTION_NON_VOLATILE,
											KEY_ALL_ACCESS,
											NULL,
											&appKey,
											&dummy );
			if( openResult == ERROR_SUCCESS )
			{
				typedef InputTraits<INPUTTYPE>	tInputTraits;
				tInputTraits	value(i_value);
				DWORD			valueSize = DWORD(value.GetValueSize());

				if( section && *section )
				{
					HKEY	sectionKey;

					openResult = RegCreateKeyEx(	appKey, section, 0, "",
													REG_OPTION_NON_VOLATILE,
													KEY_ALL_ACCESS,
													NULL,
													&sectionKey,
													&dummy );
					if( openResult == ERROR_SUCCESS )
					{
						RegSetValueEx( sectionKey, (char *)var, 0,
							tInputTraits::sRegistryType, value.GetAdress(), valueSize );
						RegCloseKey( sectionKey );
					}
				}
				else
					RegSetValueEx( appKey, (char *)var, 0,
									tInputTraits::sRegistryType, value.GetAdress(), valueSize );

				RegCloseKey( appKey );
			}

			RegCloseKey( companyKey );
		}
		RegCloseKey( softKey );
	}
}

static inline bool isEmpty(const char *val)
{
	return !val || !*val;
}

static inline bool isOK(const char *val)
{
	return val && *val;
}

static void deleteRegistration( bool publicVal, const char *company, const char *app, const char *section, const char *var )
{
	long	openResult;
	HKEY	softKey;

	openResult = RegOpenKeyEx(	publicVal	? HKEY_LOCAL_MACHINE
											: HKEY_CURRENT_USER,
								SOFTWARE, 0,
								KEY_CREATE_SUB_KEY, &softKey );
	if( openResult == ERROR_SUCCESS )
	{
		if( isEmpty(app) )
		{
			RegDeleteKey(softKey, company);
		}
		else
		{
			HKEY	companyKey;

			openResult = RegOpenKeyEx( softKey, company, 0, KEY_ALL_ACCESS, &companyKey );
			if( openResult == ERROR_SUCCESS )
			{
				if( isEmpty(section) && isEmpty( var ) )
				{
					RegDeleteKey(companyKey, app);
				}
				else
				{
					HKEY appKey;
					openResult = RegOpenKeyEx( companyKey, app, 0, KEY_ALL_ACCESS, &appKey );
					if( openResult == ERROR_SUCCESS )
					{
						if( isOK(section) )
						{
							HKEY sectionKey;
							openResult = RegOpenKeyEx(	appKey, section, 0, KEY_ALL_ACCESS, &sectionKey );
							if( openResult == ERROR_SUCCESS )
							{
								RegDeleteValue( sectionKey, var );
								RegCloseKey( sectionKey );
							}
						}
						else
						{
							RegDeleteValue(appKey, var);
						}
						RegCloseKey( appKey );
					}
				}
				RegCloseKey( companyKey );
			}
		}
		RegCloseKey( softKey );
	}
}

// --------------------------------------------------------------------- //
// ----- class inlines ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

Application::Application( const char *newIconName )
{
	m_guiRoot = NULL;
	m_guiDoc = NULL;
	m_company = "gak";
	appObject = this;
	m_iconName = newIconName;
	m_applicationIcon = NULL;
}

Application::Application( int newIconID )
{
	m_guiRoot = NULL;
	m_guiDoc = NULL;
	m_company = "gak";
	appObject = this;
	if( newIconID > 0 )
		m_iconName = MAKEINTRESOURCE( newIconID );
	else
		m_iconName = NULL;
	m_applicationIcon = NULL;
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

xml::Document *Application::getGuiDoc( void )
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
	xml::Element	*resource = NULL;

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

		GetModuleFileName( m_appInstance, theFileName, sizeof( theFileName ) -1 );
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
	return readRegistration<STRING>( m_company, getApplication(), section, variable, defaultVal );;
}

long Application::GetProfile( const char *section, const char *variable, long defaultVal )
{
	return readRegistration<long>( m_company, getApplication(), section, variable, defaultVal );;
}

#ifdef __BORLANDC__
#pragma argsused
#endif

void Application::WriteProfile( bool publicVal, const char *section, const char *variable, const char *value )
{
	writeRegistration( publicVal, m_company, getApplication(), section, variable, value );
}

void Application::WriteProfile( bool publicVal, const char *section, const char *variable, long value )
{
	writeRegistration( publicVal, m_company, getApplication(), section, variable, value );
}

void Application::DeleteProfile( bool publicVal, const char *section, const char *variable )
{
	deleteRegistration( publicVal, m_company, getApplication(), section, variable );
}

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

}	// namespace winlib

using namespace winlib;

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE /* hPrevInstance */,
							LPSTR cmdLine, int nCmdShow)
{
	doEnterFunctionEx(gakLogging::llInfo, "WinMain");
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

