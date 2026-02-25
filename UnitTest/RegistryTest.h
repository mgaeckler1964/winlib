/*
		Project:		WINLIB
		Module:			RegistryTest.h
		Description:	Registry keys
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

#include <iostream>
#include <gak/unitTest.h>

#include <winlib/registry.h>

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

namespace gak
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

class RegistryTest : public UnitTest
{
	virtual const char *GetClassName() const
	{
		return "RegistryTest";
	}
	virtual void PerformTest()
	{
		doEnterFunctionEx(gakLogging::llInfo, "RegistryTest::PerformTest");
		TestScope scope( "PerformTest" );

		const char * const GakWindowsTester = "GakWindowsTester";
		const char * const valueName = "ValueName";
		const char * const expValName = "expValName";

		Registry	software;
		software.openPrivate( "SOFTWARE" );
		UT_ASSERT_TRUE( software );
		char *unnamedValue = "dummy";
		long result = software.setValue(GakWindowsTester, rtSTRING, unnamedValue, strlen(unnamedValue)+1 );
		UT_ASSERT_EQUAL( result, ERROR_SUCCESS );

		{
			Registry	testerKey;
			result = testerKey.openSubkey( software, GakWindowsTester, KEY_ALL_ACCESS );
			UT_ASSERT_EQUAL( result, ERROR_SUCCESS );

			STRING		namedValue = "namedValue";
			result = testerKey.writeValue( valueName, namedValue );  
			UT_ASSERT_EQUAL( result, ERROR_SUCCESS );

			STRING		origValue = "Test %TMP% Test";
			STRING		expectedValue = STRING("Test ") + getenv("TMP") + " Test";
			result = testerKey.setValueEx( expValName, rtENV, origValue, origValue.size()+1 );  
			UT_ASSERT_EQUAL( result, ERROR_SUCCESS );

			STRING	readValue;
			bool success = testerKey.readValue( &readValue );
			UT_ASSERT_TRUE( success );
			UT_ASSERT_EQUAL( readValue, unnamedValue );

			success = testerKey.readValue( valueName, &readValue );
			UT_ASSERT_TRUE( success );
			UT_ASSERT_EQUAL( readValue, namedValue );

			readValue = "not found";
			success = testerKey.readValue( expValName, &readValue );
			UT_ASSERT_TRUE( success );
			UT_ASSERT_EQUAL( readValue, expectedValue );

			{
				ArrayOfStrings myValues;
				testerKey.getValueNames( &myValues );
				UT_ASSERT_EQUAL( myValues.size(), 3U );
				UT_ASSERT_NOT_EQUAL( myValues.findElement( valueName ), ArrayOfStrings::no_index );
				UT_ASSERT_NOT_EQUAL( myValues.findElement( expValName ), ArrayOfStrings::no_index );
				UT_ASSERT_NOT_EQUAL( myValues.findElement( "" ), ArrayOfStrings::no_index );
			}

			{
				testerKey.setValue( "key1", "value1" );
				testerKey.setValue( "key2", "value2" );

				ArrayOfStrings myKeys;
				testerKey.getKeyNames( &myKeys );
				UT_ASSERT_EQUAL( myKeys.size(), 2U );
				UT_ASSERT_NOT_EQUAL( myKeys.findElement( "key1" ), ArrayOfStrings::no_index );
				UT_ASSERT_NOT_EQUAL( myKeys.findElement( "key2" ), ArrayOfStrings::no_index );
			}

			testerKey.deleteSubkey("key1");
			testerKey.deleteSubkey("key2");
		}
		result = software.deleteSubkey(GakWindowsTester);
		UT_ASSERT_EQUAL( result, ERROR_SUCCESS );
	}
};

// --------------------------------------------------------------------- //
// ----- exported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module static data -------------------------------------------- //
// --------------------------------------------------------------------- //

static RegistryTest myRegistryTest;

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

}	// namespace gak

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif
