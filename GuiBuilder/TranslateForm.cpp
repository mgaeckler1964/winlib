/*
		Project:		GUI Builder
		Module:			TranslateForm.cpp
		Description:	The translator
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

#include "TranslateForm.h"

#include <WINLIB/messages.h>

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

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

void TranslateForm::saveDict()
{
	size_t max = m_captions.size();

	if( m_currentDict )
	{
		for( size_t i=0; i<max; ++i )
		{
			STRING	original = translationsGrid->getCell( 0, i ),
					translation = translationsGrid->getCell( 1, i );
			if( translation.isEmpty() || translation == original )
			{
				m_currentDict->removeElementByKey( original );
			}
			else
			{
				(*m_currentDict)[original] = translation;
			}
		}
	}
}

void TranslateForm::loadNewDict( const STRING &newLanguage )
{
	size_t max = m_captions.size();

	saveDict();

	if( !newLanguage.isEmpty() )
	{
		m_currentDict = &m_dictionarys[newLanguage];
		for( size_t i=0; i<max; ++i )
		{
			STRING	original = translationsGrid->getCell( 0, i );
			STRING	translation = (*m_currentDict)[original];
			if( translation.isEmpty() )
			{
				translation = original;
				translationsGrid->setCellColor( 1, i, 0xAAAAFF );
			}
			else
			{
				translationsGrid->setCellColor( 1, i, 0xAAFFAA );
			}
			translationsGrid->setCell( 1, i, translation );
		}
		translationsGrid->invalidateWindow();
	}
}

// --------------------------------------------------------------------- //
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

ProcessStatus TranslateForm::handleCreate()
{
	size_t	row = 0;
	translationsGrid->createData( 2, m_captions.size() );
	translationsGrid->setFixedCols( 1 );
	for(
		Captions::const_iterator	it = m_captions.cbegin(), endIT = m_captions.cend();
		it != endIT;
		++it
	)
	{
		translationsGrid->setCell( 0, row++, *it );
	}

	for(
		Dictionarys::const_iterator it = m_dictionarys.cbegin(), endIT = m_dictionarys.cend();
		it != endIT;
		++it
	)
	{
		targetLanguageCombo->addEntry( it->getKey() );
	}
	targetLanguageCombo->selectEntry( 0 );
	handleSelectionChange( targetLanguageCombo_id );
	return TranslationForm_form::handleCreate();
}
   
ProcessStatus TranslateForm::handleEditChange( int control )
{
	if( control == targetLanguageCombo_id )
	{
		STRING	newLanguage = targetLanguageCombo->getText();
		loadNewDict( newLanguage );
		targetLanguageCombo->addEntry( targetLanguageCombo->getText() );
		return psPROCESSED;
	}
	else
	{
		return TranslationForm_form::handleEditChange( control );
	}
}


ProcessStatus TranslateForm::handleOk()
{
	saveDict();

	return TranslationForm_form::handleOk();
}

ProcessStatus TranslateForm::handleSelectionChange( int control )
{
	if( control == targetLanguageCombo_id )
	{
		STRING	newLanguage = targetLanguageCombo->getSelectedText();
		loadNewDict( newLanguage );
		return psPROCESSED;
	}
	else
	{
		return TranslationForm_form::handleSelectionChange( control );
	}
}

ProcessStatus TranslateForm::handleMessage( UINT msg, WPARAM control, LPARAM lParam )
{
	if( control == translationsGrid_id && msg == WM_GRID_ITEM_EXIT )
	{
		int col = LOWORD(lParam);
		int row = HIWORD(lParam);

		STRING	original = translationsGrid->getCell( 0, row ),
				translation = translationsGrid->getCell( 1, row );
		if( translation.isEmpty() || translation == original )
		{
			translationsGrid->setCellColor( 1, row, 0xAAAAFF );
		}
		else
		{
			translationsGrid->setCellColor( 1, row, 0xAAFFAA );
		}
		return psPROCESSED;
	}
	return TranslationForm_form::handleMessage( msg, control, lParam );
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

