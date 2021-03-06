/*
		Project:		Windows Class Library
		Module:			LISTCONT.CPP
		Description:	Implementation of ListboxContainer
		Author:			Martin G�ckler
		Address:		Hopfengasse 15, A-4020 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1992-2021 Martin G�ckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin G�ckler, Germany, Munich ``AS IS''
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
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <winlib/listcont.h>

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
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

ListboxContainer::~ListboxContainer()
{
	size_t			i = data.size();
	ListObject		*element;
	while( i>0 )
	{
		i--;
		element = data[i];

		delete element;
	}
}


ListboxContainer::ListObjectPtr ListboxContainer::getSelectedEntry( void )
{
	ListObjectPtr		entry = NULL;

	if( control )
	{
		T_STRING	selection = control->getSelectedItems();
		if( !selection.isEmpty() )
		{
			STRING	line = selection.getFirstToken( "\n" );
			if( !line.isEmpty() )
			{
				size_t	i, numElems;

				numElems = data.size();
				i = 0;
				while( i<numElems )
				{
					entry = data[i];
					if( !entry || (STRING)*entry == line )
						break;
					i++;
				}
			}
		}
	}
	return entry;
}

void ListboxContainer::removeEntry( ListObjectPtr object )
{
	size_t	i, numElems;

	numElems = data.size();
	for( i=0; i<numElems; i++ )
	{
		if( data[i] == object )
			break;
	}

	if( i<numElems )
	{
		data.removeElementAt( i );
		if( control )
			control->deleteEntry( int(i) );
	}
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

