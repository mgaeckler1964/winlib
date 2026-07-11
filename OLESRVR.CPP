/*
		Project:		Windows Class Library
		Module:			olesvr.cpp
		Description:	Defines an OLE server (not complete)
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

#include <gak/wideString.h>
#include <gak/string.h>
#include <winlib/olesrvr.h>
#include <winlib/winapp.h>
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

static int s_registeredFactoriesCount = 0;

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
   
// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

DWORD initOleServer(STRING *guidStr, GUID *guid, IClassFactory *factory)
{
	DWORD	registerID = 0;
	bool	ok = false;

	if( !s_registeredFactoriesCount && !SUCCEEDED(OleInitialize(nullptr) ) )
	{
		return 0;
	}

	if( !guidStr->isEmpty() )
	{
		uSTRING	guidWstring;
		guidWstring.decodeUTF8(*guidStr);
		if( SUCCEEDED(CLSIDFromString(guidWstring, guid ) ) )
		{
			ok = true;
		}
	}
	else
	{
		if (SUCCEEDED(CoCreateGuid(guid))) 
		{
			LPOLESTR wstr;
			if (SUCCEEDED(StringFromCLSID(*guid, &wstr))) 
			{
				uSTRING	guidWstring(wstr);
				*guidStr = guidWstring.toString();
				CoTaskMemFree(wstr);
				ok = true;
			}
		}
	}

	if( ok )
	{
		if( !SUCCEEDED(CoRegisterClassObject(*guid, factory, CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &registerID) ) )
		{
			registerID=0;
		}
		else
		{
			++s_registeredFactoriesCount;
		}

	}

	return registerID;
}

void registerOleServer(const gak::STRING &guidStr, const gak::STRING &descr, const char *document )
{
	STRING appDoc = STRING(appObject->getApplication()).add(document);
	STRING appDocVersion = STRING(appObject->getApplication()).add(document).add(".1");

	if( RegistryClassesRoot().setKeyValue( appDoc, descr ) == ERROR_SUCCESS )
	{
		Registry reg;
		reg.openSubkey( RegistryClassesRoot(), appDoc );
		reg.setKeyValue("Insertable", "");
		reg.setKeyValue("CLSID", guidStr);
	}

	Registry reg;
	if( reg.openSubkey( RegistryClassesRoot(), "CLSID", KEY_ALL_ACCESS|KEY_WOW64_64KEY ) == ERROR_SUCCESS )
	{
		if( reg.setKeyValue( guidStr, descr ) == ERROR_SUCCESS )
		{
			Registry guidKey;
			if( guidKey.openSubkey(reg, guidStr) == ERROR_SUCCESS )
			{
				STRING exeName = STRING('"') + appObject->getFileName() + STRING('"');
				guidKey.setKeyValue("Insertable", "");
				guidKey.setKeyValue("InprocHandler32", "ole32.dll");
				guidKey.setKeyValue("LocalServer32", exeName);
				guidKey.setKeyValue("ProgID", appDoc);
			}
		}
	}
}

void exitOleServer( DWORD registerID )
{
	CoRevokeClassObject(registerID);
	if( !--s_registeredFactoriesCount )
		OleUninitialize();
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

