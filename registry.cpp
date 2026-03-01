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

Registry::key_iterator::key_iterator( const Registry &reg ) : m_reg( reg ), m_index(0), m_loaded(false)
{
	long openResult = RegQueryInfoKey(
		m_reg.m_key,
		nullptr, nullptr, nullptr,
		&m_numKeys, &m_maxTitleSize,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
	);
	if( openResult == ERROR_SUCCESS )
	{
		++m_maxTitleSize;
	}
	else
	{
		m_numKeys = 0;
		m_maxTitleSize = 0;
	}
}

Registry::value_iterator::value_iterator( const Registry &reg ) : m_reg( reg ), m_index(0), m_loaded(false)
{
	long openResult = RegQueryInfoKey(
		m_reg.m_key,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		&m_numValues, &m_maxTitleSize, &m_maxValueSize,
		nullptr, nullptr
	);
	if( openResult == ERROR_SUCCESS )
	{
		++m_maxTitleSize;
		++m_maxValueSize;
	}
	else
	{
		m_numValues = 0;
		m_maxTitleSize = 0;
		m_maxValueSize = 0;
	}
}

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

const STRING &Registry::key_iterator::operator * ()
{
	if( !m_loaded )
	{
		DWORD titleSize = m_maxTitleSize;
		m_name.setMinSize( titleSize+1 );
		long openResult = RegEnumKeyEx( m_reg.m_key, m_index, LPSTR(m_name.c_str()), &titleSize, nullptr, nullptr, nullptr, nullptr );
		if( openResult == ERROR_SUCCESS )
		{
			m_name.setActSize(titleSize);
		}
		m_loaded = true;
	}
	return m_name;
}

const RegValuePair &Registry::value_iterator::operator * ()
{
	if( !m_loaded )
	{
		DWORD	type;
		DWORD	titleSize = m_maxTitleSize;
		DWORD	valueSize = m_maxValueSize;

		m_value.valueName.release();
		m_value.valueName.setMinSize( titleSize+1 );
		m_value.valueBuffer.release();
		m_value.valueBuffer.setMinSize( valueSize+1);

		long openResult = RegEnumValue(
			m_reg.m_key, m_index, 
			LPSTR(m_value.valueName.c_str()), &titleSize, 
			nullptr, 
			&type, LPBYTE(m_value.valueBuffer.c_str()), &valueSize 
		);
		if( openResult == ERROR_SUCCESS )
		{
			m_value.type = RegistryType(type);
			m_value.valueBuffer.setActSize( valueSize );
			m_value.valueName.setActSize( titleSize );
			m_loaded = true;
		}
		else
			m_value.type = rtERROR;
	}
	return m_value;
}

void Registry::_getKeyNames( ArrayOfStrings *keyNames )
{
	for(
		key_iterator it = kbegin(), endIT = kend();
		it != endIT;
		++it
	)
	{
		keyNames->addElement( *it );
	}
}

void Registry::_getValueNames( ArrayOfStrings *valueNames )
{
	for(
		value_iterator it = vbegin(), endIT = vend();
		it != endIT;
		++it
	)
	{
		valueNames->addElement( it->valueName );
	}
}

void Registry::_getValuePairs( RegValuePairs *valuePairs )
{
	for(
		value_iterator it = vbegin(), endIT = vend();
		it != endIT;
		++it
	)
	{
		valuePairs->addElement( *it );
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
