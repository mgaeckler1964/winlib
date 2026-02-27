/*
		Project:		WINLIB
		Module:			registry.cpp
		Description:	Registry keys (reserved code space)
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

#include <winlib/registry.h>

// --------------------------------------------------------------------- //
// ----- imported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module switches ----------------------------------------------- //
// --------------------------------------------------------------------- //

using namespace gak;

#ifdef __BORLANDC__
#	pragma option -RT-
#	pragma option -b
#	pragma option -a4
#	pragma option -pc
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

RegistryType Registry::queryValue( HKEY key, const char *var, void *buffer, size_t *io_size )
{
	DWORD		typeVar;
	DWORD		size = DWORD(*io_size);

	long openResult = RegQueryValueEx(
		key, var, nullptr, &typeVar, LPBYTE(buffer), &size 
	);
	*io_size = size;
	if( openResult == ERROR_SUCCESS )
		return RegistryType(typeVar);
	else if( openResult == ERROR_FILE_NOT_FOUND )
		return rtMISSING;

	return  rtERROR;
}

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

long Registry::createKey2( HKEY parent, const char *name, unsigned long perm )
{
	DWORD	dummy;
	long	openResult = RegCreateKeyEx(
		parent, name, 0, "",
		REG_OPTION_NON_VOLATILE,
		perm, nullptr,
		&m_key,
		&dummy 
	);
	if( openResult != ERROR_SUCCESS )
	{
		m_key = 0;
	}
	else
	{
		m_perm = perm;
	}
	return openResult;
}
long Registry::openKey2( HKEY parent, const char *name, unsigned long perm )
{
	long openResult = RegOpenKeyEx(	
		parent, name, 
		0,
		perm, &m_key 
	);
	if( openResult != ERROR_SUCCESS )
	{
		m_key = 0;
	}
	else
	{
		m_perm = perm;
	}
	return openResult;
}

// --------------------------------------------------------------------- //
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //
   
// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

void Registry::getKeyNames( ArrayOfStrings *keyNames )
{
	DWORD	numValues, maxTitleSize;
	STRING	keyName;

	long openResult = RegQueryInfoKey(
		m_key, 
		nullptr, nullptr, nullptr, 
		&numValues, &maxTitleSize, 
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
	);
	if( openResult == ERROR_SUCCESS )
	{
		++maxTitleSize;
		for( DWORD i=0; i<numValues; ++i )
		{
			DWORD titleSize = maxTitleSize;
			keyName.setMinSize( titleSize+1 );
			openResult = RegEnumKeyEx( m_key, i, LPSTR(keyName.c_str()), &titleSize, nullptr, nullptr, nullptr, nullptr );
			if( openResult == ERROR_SUCCESS )
			{
				keyName.setActSize(titleSize);
				keyNames->addElement( keyName );
			}
		}
	}
}

void Registry::getValueNames( ArrayOfStrings *valueNames )
{
	DWORD	numValues, maxTitleSize;
	STRING	valueName;

	long openResult = RegQueryInfoKey(
		m_key, 
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		&numValues, &maxTitleSize, 
		nullptr, nullptr, nullptr
	);
	if( openResult == ERROR_SUCCESS )
	{
		++maxTitleSize;
		for( DWORD i=0; i<numValues; ++i )
		{
			DWORD titleSize = maxTitleSize;
			valueName.setMinSize( titleSize+1 );
			openResult = RegEnumValue( m_key, i, LPSTR(valueName.c_str()), &titleSize, nullptr, nullptr, nullptr, nullptr );
			if( openResult == ERROR_SUCCESS )
			{
				valueName.setActSize(titleSize);
				valueNames->addElement( valueName );
			}
		}
	}
}

void Registry::getValuePairs( RegValuePairs *valuePairs )
{
	DWORD	numValues, maxTitleSize;

	long openResult = RegQueryInfoKey(
		m_key, 
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		&numValues, &maxTitleSize, 
		nullptr, nullptr, nullptr
	);

	if( openResult == ERROR_SUCCESS )
	{
		STRING	valueName;

		valuePairs->setChunkSize( numValues );
		++maxTitleSize;
		for( DWORD i=0; i<numValues; ++i )
		{
			DWORD		type, valueSizeDw;
			DWORD		titleSize = maxTitleSize;
			RegValuePair &newValue = valuePairs->createElement();

			newValue.valueName.setMinSize( titleSize+1 );
			openResult = RegEnumValue(
				m_key, i, 
				LPSTR(newValue.valueName.c_str()), &titleSize, 
				nullptr, 
				&type, nullptr, &valueSizeDw 
			);
			if( openResult == ERROR_SUCCESS )
			{
				size_t valueSize = valueSizeDw;

				newValue.valueName.setActSize(titleSize);
				newValue.valueBuffer.setMinSize( valueSize+1 );
				newValue.type = queryValue( 
					newValue.valueName, 
					(void *)newValue.valueBuffer.c_str(), 
					&valueSize );
				newValue.valueBuffer.setActSize( valueSize );
			}
		}
	}
}

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

} // namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif
