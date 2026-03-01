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

using namespace gak;

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

		// the key
		const char * const	GakWindowsTester = "GakWindowsTester";
		STRING				unnamedValue = "dummy";

		// first value
		const char * const	valueName = "ValueName";
		STRING				namedValue = "namedValue";

		// second value
		const char * const	expValName = "expValName";
		STRING				origValue = "Test %TMP% Test";
		STRING				expectedValue = STRING("Test ") + getenv("TMP") + " Test";

		// third value
		const char * const	longName = "longName";
		long				longValue = 666;

		// forth value
		const char * const	long64Name = "long64Name";
		gak::int64			long64Value = std::numeric_limits<gak::int64>::min();

		// fifth value
		const char * const	badStrName = "badName";
		const char * const	badValue = "66645";

		// sixth value
		const char * const	nullName = "nullName";
		STRING				nullValue;

		// seventh value
		const char * const	emptyName = "emptyName";
		STRING				emptyValue = "";

		// eight value for error checking
		const char * const	errorName = "wrongValues";

		Registry	software;
		software.openPrivate( "SOFTWARE" );
		UT_ASSERT_TRUE( software );
		long result = software.setKeyValue(GakWindowsTester, unnamedValue );
		UT_ASSERT_EQUAL( result, ERROR_SUCCESS );

		{
			/*
				Writing
			*/
			Registry	testerKey;
			result = testerKey.openSubkey( software, GakWindowsTester, KEY_ALL_ACCESS );
			UT_ASSERT_EQUAL( result, ERROR_SUCCESS );

			// ---------------------- 1 --------------------------------
			result = testerKey.writeValue( valueName, namedValue );  
			UT_ASSERT_EQUAL( result, ERROR_SUCCESS );
			size_t size = 0;
			UT_ASSERT_EQUAL( testerKey.getValueSize(valueName, &size), ERROR_SUCCESS );
			UT_ASSERT_EQUAL( size, namedValue.strlen()+1 );

			// ---------------------- 2 --------------------------------
			result = testerKey.setValueEx( expValName, rtENV, origValue, origValue.size()+1 );  
			UT_ASSERT_EQUAL( result, ERROR_SUCCESS );

			EnvSTRING origEnv = origValue;
			result = testerKey.writeValue( expValName, origEnv );  
			UT_ASSERT_EQUAL( result, ERROR_SUCCESS );

			// ---------------------- 3 --------------------------------
			result = testerKey.writeValue( longName, longValue );  
			UT_ASSERT_EQUAL( result, ERROR_SUCCESS );

			// ---------------------- 4 --------------------------------
			result = testerKey.writeValue( long64Name, long64Value );  
			UT_ASSERT_EQUAL( result, ERROR_SUCCESS );

			// ---------------------- 5 --------------------------------
			// writing a string that it not 0-terminated
			result = testerKey.setValueEx( badStrName, rtSTRING, badValue, 3 );  // do not write the trailing 0
			UT_ASSERT_EQUAL( result, ERROR_SUCCESS );

			UT_ASSERT_EQUAL( testerKey.getValueSize(badStrName, &size), ERROR_SUCCESS );
			UT_ASSERT_EQUAL( size, 3 );

			// ---------------------- 6 --------------------------------
			// writing a null string
			result = testerKey.writeValue( nullName, nullValue );  
			UT_ASSERT_EQUAL( result, ERROR_SUCCESS );
			UT_ASSERT_EQUAL( testerKey.getValueSize(nullName, &size), ERROR_SUCCESS );
			UT_ASSERT_EQUAL( size, 0 );

			// ---------------------- 7 --------------------------------
			// writing an empty string
			result = testerKey.writeValue( emptyName, emptyValue );  
			UT_ASSERT_EQUAL( result, ERROR_SUCCESS );
			UT_ASSERT_EQUAL( testerKey.getValueSize(emptyName, &size), ERROR_SUCCESS );
			UT_ASSERT_EQUAL( size, 1 );

			/*
				Reading
			*/

			STRING		readValue;
			ReadSuccess	success = testerKey.readValue( &readValue );
			UT_ASSERT_EQUAL( success, rsOK );
			UT_ASSERT_EQUAL( readValue, unnamedValue );
			UT_ASSERT_EQUAL( readValue.strlen(), strlen(readValue.c_str()) );

			success = testerKey.readValue( valueName, &readValue );
			UT_ASSERT_EQUAL( success, rsOK );
			UT_ASSERT_EQUAL( readValue, namedValue );
			UT_ASSERT_EQUAL( readValue.strlen(), strlen(readValue.c_str()) );

			readValue = "not found";
			success = testerKey.readValue( expValName, &readValue );
			UT_ASSERT_EQUAL( success, rsOK );
			UT_ASSERT_EQUAL( readValue, expectedValue );
			UT_ASSERT_EQUAL( readValue.strlen(), strlen(readValue.c_str()) );

			EnvSTRING myEnvValue;
			success = testerKey.readValue( expValName, &myEnvValue );
			UT_ASSERT_EQUAL( success, rsOK );
			UT_ASSERT_EQUAL( myEnvValue, origValue );
			UT_ASSERT_EQUAL( myEnvValue.expand(), expectedValue );

			long readLong;
			success = testerKey.readValue( longName, &readLong );
			UT_ASSERT_EQUAL( success, rsOK );
			UT_ASSERT_EQUAL( readLong, longValue );

			gak::int64 readLong64;
			success = testerKey.readValue( long64Name, &readLong64 );
			UT_ASSERT_EQUAL( success, rsOK );
			UT_ASSERT_EQUAL( readLong64, long64Value );


			// reading a string that was not 0-terminated
			STRING read666;
			success = testerKey.readValue( badStrName, &read666 );
			UT_ASSERT_EQUAL( success, rsOK );
			UT_ASSERT_EQUAL( read666.strlen(), 3 );
			UT_ASSERT_EQUAL( read666.strlen(), strlen(read666.c_str()) );

			/// reading a NULL strings
			nullValue = "Dummy";	// ensure readValue will change the string
			success = testerKey.readValue( nullName, &nullValue );
			UT_ASSERT_EQUAL( success, rsOK );
			UT_ASSERT_TRUE(nullValue.isNullPtr());

			/// reading an and empty strings
			emptyValue = "Dummy";	// ensure readValue will change the string
			success = testerKey.readValue( emptyName, &emptyValue );
			UT_ASSERT_EQUAL( success, rsOK );
			UT_ASSERT_FALSE(emptyValue.isNullPtr());
			UT_ASSERT_EQUAL( emptyValue.size(), 0 );

			/// check errors
			long testValue=666;
			result = testerKey.setValueEx( errorName, rtINTEGER, &testValue, 1 );  
			UT_ASSERT_EQUAL( result, ERROR_SUCCESS );
			UT_ASSERT_EQUAL( testerKey.getValueSize(errorName, &size), ERROR_SUCCESS );
			UT_ASSERT_EQUAL( size, 1 );
			success = testerKey.readValue( errorName, &testValue );
			UT_ASSERT_EQUAL( success, rsBadSize );

			success = testerKey.readValue( "dummyNotExisting", &readLong64 );
			UT_ASSERT_EQUAL( success, rsNotFound );

			success = testerKey.readValue( longName, &readLong64 );
			UT_ASSERT_EQUAL( success, rsBadType );

			{
				ArrayOfStrings myValuesNames;
				testerKey._getValueNames( &myValuesNames );
				UT_ASSERT_EQUAL( myValuesNames.size(), 9U );
				UT_ASSERT_NOT_EQUAL( myValuesNames.findElement( valueName ), ArrayOfStrings::no_index );
				UT_ASSERT_NOT_EQUAL( myValuesNames.findElement( longName ), ArrayOfStrings::no_index );
				UT_ASSERT_NOT_EQUAL( myValuesNames.findElement( long64Name ), ArrayOfStrings::no_index );
				UT_ASSERT_NOT_EQUAL( myValuesNames.findElement( expValName ), ArrayOfStrings::no_index );
				UT_ASSERT_NOT_EQUAL( myValuesNames.findElement( badStrName ), ArrayOfStrings::no_index );
				UT_ASSERT_NOT_EQUAL( myValuesNames.findElement( nullName ), ArrayOfStrings::no_index );
				UT_ASSERT_NOT_EQUAL( myValuesNames.findElement( emptyName ), ArrayOfStrings::no_index );
				UT_ASSERT_NOT_EQUAL( myValuesNames.findElement( errorName ), ArrayOfStrings::no_index );
				UT_ASSERT_NOT_EQUAL( myValuesNames.findElement( "" ), ArrayOfStrings::no_index );

				RegValuePairs	myPairs;
				testerKey._getValuePairs( &myPairs );
				UT_ASSERT_EQUAL( myValuesNames.size(), myPairs.size() );
			}

			{
				testerKey.setKeyValue( "key1", "value1" );
				testerKey.setKeyValue( "key2", "value2" );

				ArrayOfStrings myKeys;
				testerKey._getKeyNames( &myKeys );
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
