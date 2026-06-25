/*
		Project:		WINLIB
		Module:			version.cpp
		Description:	program version information
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

#define _CRT_SECURE_NO_WARNINGS 1

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <winlib/version.h>

#include <gak/fmtNumber.h>
#include <gak/logfile.h>

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

ProgramVersionInfo::ProgramVersionInfo( const gak::STRING &appFileName )
{
	doEnterFunction("ProgramVersionInfo::ProgramVersionInfo");
	DWORD	dummy1, versionInfoSize;
	UINT	dummy2;

	versionInfoSize = GetFileVersionInfoSize(
		appFileName, &dummy1
	);
	doLogValue(appFileName);
	if( versionInfoSize )
	{
		gak::Buffer<char> data( versionInfoSize );
		if( data )
		{
			VS_FIXEDFILEINFO	*fixedFileInfo;

			GetFileVersionInfo( appFileName, 0L, versionInfoSize, data );
			if( VerQueryValue( data, "\\", (void **)&fixedFileInfo, &dummy2 ) )
			{
				m_major = (unsigned short)(fixedFileInfo->dwFileVersionMS >> 16);
				m_minor = (unsigned short)(fixedFileInfo->dwFileVersionMS & 0xFFFF);
				m_patch = (unsigned short)(fixedFileInfo->dwFileVersionLS >> 16);
				m_build = (unsigned short)(fixedFileInfo->dwFileVersionLS & 0xFFFF);
				m_versionString =
					gak::formatNumber( m_major ) + '.' +
					gak::formatNumber( m_minor ) + '.' +
					gak::formatNumber( m_patch ) + '.' +
					gak::formatNumber( m_build )
				;
			}
			doLogValue(m_versionString);

			struct LANGANDCODEPAGE
			{
				WORD wLanguage;
				WORD wCodePage;
			} *lpTranslate;

			const char		*cpData;
			gak::STRING		valueName;

			if( VerQueryValue(
				data,
				"\\VarFileInfo\\Translation", (void**)&lpTranslate,
				&dummy2
			) )
			{
				gak::STRING		valueNameStart =  gak::STRING("\\StringFileInfo\\") 
						.add(gak::formatBinary( lpTranslate->wLanguage, 16, 4, '0' ))
						.add(gak::formatBinary( lpTranslate->wCodePage, 16, 4, '0' ));

				valueName = valueNameStart + "\\LegalCopyright";
				if( VerQueryValue(
					data,
					valueName, (void**)&cpData,
					&dummy2
				) )
				{
					m_legalCopyRight = cpData;
					doLogValue(m_legalCopyRight);
				}

				valueName = valueNameStart + "\\FileDescription";
				if( VerQueryValue(
					data,
					valueName, (void**)&cpData,
					&dummy2
				) )
				{
					m_fileDescription = cpData;
					doLogValue(m_fileDescription);
				}
				valueName = valueNameStart + "\\CompanyName";
				if( VerQueryValue(
					data,
					valueName, (void**)&cpData,
					&dummy2
				) )
				{
					m_companyName = cpData;
					doLogValue(m_companyName);
				}
				valueName = valueNameStart + "\\ProductName";
				if( VerQueryValue(
					data,
					valueName, (void**)&cpData,
					&dummy2
				) )
				{
					m_productName = cpData;
					doLogValue(m_productName);
				}
			}
		}
	}
}

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

} // namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

