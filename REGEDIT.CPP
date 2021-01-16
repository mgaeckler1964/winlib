/*
		Project:		Windows Class Library
		Module:			REGEDIT.CPP
		Description:	A StringEditor used to edit registry values
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

#include <winlib\regedit.h>

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



void RegistryEditor::create( BasicWindow *parent, const char *keyValue, bool userReg )
{
	STRING			value;
	char 			buffer[1024];
	unsigned long	buff_size = sizeof( buffer );
	unsigned long	dummy;
	HKEY			theKey;

	char *keyName;
	const char *valueName;
	char *tmpKey = strdup( keyValue );
	char *slashPos = strrchr( tmpKey, '\\' );

	if( !slashPos )
	{
		keyName = "";
		valueName = keyValue;
	}
	else
	{
		keyName = tmpKey;
		*slashPos = 0;
		valueName = slashPos+1;
	}

	if( RegCreateKey(userReg ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE,
		keyName,
		&theKey ) == ERROR_SUCCESS
	)
		if(
			RegQueryValueEx(
				theKey,
				valueName,
				NULL,
				&dummy,
				(LPBYTE)buffer, &buff_size
			) != ERROR_SUCCESS
		)
		{
			*buffer = 0;
		}
		else
		{
			buffer[buff_size] = 0;
		}

	{
		value = StringEditor::create( parent, valueName, buffer );
		if( value != buffer )
		{
			RegSetValueEx(
				theKey,
				valueName,
				0,
				REG_SZ,
				(LPBYTE)((const char*)value), DWORD(value.strlen())
			);
		}
		RegCloseKey( theKey );
	}

	free( tmpKey );
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif



