/*
		Project:		GAKLIB
		Module:			WinAppTest.h
		Description:	Unit test for winlib::Application
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

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <iostream>
#include <gak/unitTest.h>

#include <WINLIB/WINAPP.H>

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

class WinApp : public winlib::Application
{
};

static char theKeyName[] = "theKeyName";

class WinAppTest : public UnitTest
{
	WinApp	m_appObject;

	virtual const char *GetClassName() const
	{
		return "WinAppTest";
	}
	virtual void PerformTest()
	{
		doEnterFunctionEx(gakLogging::llInfo, "WinAppTest::PerformTest");
		TestScope scope( "PerformTest" );
		UT_ASSERT_EQUAL( winlib::appObject, &m_appObject );

		m_appObject.setComapny("GakWinlibUnitTester");
		m_appObject.setApplication("UnitTestApp");

		long tester = m_appObject.GetProfile("", theKeyName, 666);
		UT_ASSERT_EQUAL( tester, 666 );
		long result = m_appObject.WriteProfile(false, "", theKeyName, 333);
		UT_ASSERT_EQUAL( result, ERROR_SUCCESS );
		tester = m_appObject.GetProfile("", theKeyName, 666);
		UT_ASSERT_EQUAL( tester, 333 );

		result = m_appObject.DeleteProfile(false);
		UT_ASSERT_EQUAL( result, ERROR_SUCCESS );
		result = m_appObject.DeleteProfile(false);
		UT_ASSERT_EQUAL( result, ERROR_FILE_NOT_FOUND );
		tester = m_appObject.GetProfile("", theKeyName, 666);
		UT_ASSERT_EQUAL( tester, 666 );

		result = m_appObject.WriteProfile(true, "", theKeyName, 999);
		if( result == ERROR_SUCCESS )
		{
			std::cout << "Admintest" << std::endl;

			tester = m_appObject.GetProfile("", theKeyName, 666);
			UT_ASSERT_EQUAL( tester, 999 );
			result = m_appObject.WriteProfile(false, "", theKeyName, 333);
			UT_ASSERT_EQUAL( result, ERROR_SUCCESS );
			tester = m_appObject.GetProfile("", theKeyName, 666);
			UT_ASSERT_EQUAL( tester, 333 );

			/// TODO: check for WinXP. (newer versions will delete this automatically)
			result = m_appObject.DeleteProfile(true);
			UT_ASSERT_EQUAL( result, ERROR_SUCCESS );
			result = m_appObject.DeleteProfile(false);
			UT_ASSERT_EQUAL( result, ERROR_SUCCESS );

			result = m_appObject.DeleteCompanyProfile( true );
			UT_ASSERT_EQUAL( result, ERROR_SUCCESS );
			result = m_appObject.DeleteCompanyProfile( true );
			UT_ASSERT_EQUAL( result, ERROR_FILE_NOT_FOUND );
		}
		else
		{
			std::cout << "No Admintest" << std::endl;
		}

		result = m_appObject.DeleteCompanyProfile( false );
		UT_ASSERT_EQUAL( result, ERROR_SUCCESS );
		result = m_appObject.DeleteCompanyProfile( false );
		UT_ASSERT_EQUAL( result, ERROR_FILE_NOT_FOUND );
	}
};

// --------------------------------------------------------------------- //
// ----- exported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module static data -------------------------------------------- //
// --------------------------------------------------------------------- //

static WinAppTest myWinAppTest;

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
