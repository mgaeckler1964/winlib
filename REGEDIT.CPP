/*
		Project:		Windows Class Library
		Module:			REGEDIT.CPP
		Description:	A StringEditor used to edit registry values
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

#include <winlib/regedit.h>

#include <gak/stdlib.h>
#include <winlib/registry.h>

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
	STRING			oldValue, newValue;

	Registry		theKey;

	char *keyName;
	const char *valueName;
	Buffer<char> tmpKey(strdup( keyValue ));
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

	if( theKey.createKey(!userReg, keyName ) == ERROR_SUCCESS )
	{
		if( theKey.readValue( valueName, &oldValue ) != rsOK )
		{
			oldValue.release();
		}

		newValue = StringEditor::create( parent, valueName, oldValue );
		if( newValue != oldValue )
		{
			theKey.writeValue( valueName, newValue );
		}
	}
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif



