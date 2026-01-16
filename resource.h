/*
		Project:		Windows Class Library
		Module:			ressource.h
		Description:	Create windows from a ressource file
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
#ifndef WINLIB_RESOURCE_H
#define WINLIB_RESOURCE_H

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak/numericString.h>
#include <gak/xmlParser.h>

#include <WINLIB/BASICWIN.H>

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

class FrameChild;
class CallbackWindow;
struct LayoutManager;

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

void createChildWindows( 
	const F_STRING &resourceFileName, 
	gak::xml::Element *resource, 
	BasicWindow *parent, 
	bool designerMode 
);

LayoutManager *createLayoutManager( const STRING &type, bool designerMode );

STRING getColorName( size_t index );
BasicWindow::BackgroundColor getColorValue( const STRING &name );

SuccessCode createFrame( 
	const F_STRING &resourceFileName, gak::xml::Element *root, 
	const char *frameName, FrameChild *frame, CallbackWindow *parent, 
	bool designerMode 
);

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

template <class WindowClass> 
SuccessCode createForm( 
	const F_STRING &resourceFileName, gak::xml::Element *resource, 
	WindowClass *form, BasicWindow *parent, bool designerMode 
)
{
	STRING		caption = resource->getAttribute( CAPTION_ATTR );

	unsigned long style = resource->getAttribute( STYLE_ATTR ).getValueN<unsigned long>();
	if( designerMode )
	{
		style |= WS_VISIBLE|WS_OVERLAPPEDWINDOW|WS_CAPTION;
		style &= ~(WS_POPUP|WS_MINIMIZE|WS_MAXIMIZE|WS_DLGFRAME|WS_DISABLED);

		if( caption.isEmpty() )
			caption = resource->getAttribute( NAME_ATTR );
	}

	form->setResource( resource );
	form->setText( caption );

	form->setStyle( style );
	
	if( form->create( parent ) == scSUCCESS )
	{
		form->setLoading();
		form->disallowNotifications();

		createChildWindows( resourceFileName, resource, form, designerMode );

		form->doLayout();

		form->getControls();
		form->allowDialogProcessing();
		form->allowNotifications();

		form->handleCreate();

		form->setReady();
		return scSUCCESS;
	}

	return scERROR;
}

inline SuccessCode createFrame( 
	gak::xml::Document *resourceDoc, const char *frameName, 
	FrameChild *frame, CallbackWindow *parent, 
	bool designerMode 
)
{
	return createFrame( 
		resourceDoc->getFilename(), resourceDoc->getRoot(), 
		frameName, frame, parent, designerMode 
	);
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif
