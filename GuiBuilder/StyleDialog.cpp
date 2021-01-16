/*
		Project:		GUI Builder
		Module:			StyleDialog.cpp
		Description:	The windows style editor
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


// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include "StyleDialog.h"

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

struct ConstantValues
{
	char			name[32];
	unsigned long	value;
	unsigned long	mask;
};

// --------------------------------------------------------------------- //
// ----- exported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module static data -------------------------------------------- //
// --------------------------------------------------------------------- //

static ConstantValues windowStyles[] =
{
	{ "WS_VISIBLE", WS_VISIBLE, WS_VISIBLE },
	{ "WS_BORDER", WS_BORDER, WS_BORDER },
	{ "WS_VSCROLL", WS_VSCROLL, WS_VSCROLL },
	{ "WS_HSCROLL", WS_HSCROLL, WS_HSCROLL },
	{ "WS_CLIPSIBLINGS", WS_CLIPSIBLINGS, WS_CLIPSIBLINGS },
	{ "WS_CLIPCHILDREN", WS_CLIPCHILDREN, WS_CLIPCHILDREN },
};

static ConstantValues formStyles[] =
{
	{ "WS_DLGFRAME", WS_DLGFRAME, WS_THICKFRAME|WS_DLGFRAME },
	{ "WS_THICKFRAME", WS_THICKFRAME, WS_THICKFRAME|WS_DLGFRAME },

	{ "WS_SYSMENU", WS_SYSMENU, WS_SYSMENU },
	{ "WS_MAXIMIZEBOX", WS_MAXIMIZEBOX, WS_MAXIMIZEBOX },
	{ "WS_MINIMIZEBOX", WS_MINIMIZEBOX, WS_MINIMIZEBOX },
};

static const unsigned long labelMask = SS_LEFT|SS_CENTER|SS_RIGHT|SS_BLACKRECT|SS_GRAYRECT|SS_WHITERECT|SS_BLACKFRAME|SS_GRAYFRAME|SS_WHITEFRAME|SS_ETCHEDFRAME|SS_ETCHEDHORZ|SS_SIMPLE|SS_ETCHEDVERT;
static ConstantValues labelStyles[] =
{
	{ "WS_GROUP", WS_GROUP, WS_GROUP },

	{ "SS_LEFT", SS_LEFT, labelMask },
	{ "SS_CENTER", SS_CENTER, labelMask },
	{ "SS_RIGHT", SS_RIGHT, labelMask },
	{ "SS_BLACKRECT", SS_BLACKRECT, labelMask },
	{ "SS_GRAYRECT", SS_GRAYRECT, labelMask },
	{ "SS_WHITERECT", SS_WHITERECT, labelMask },
	{ "SS_BLACKFRAME", SS_BLACKFRAME, labelMask },
	{ "SS_GRAYFRAME", SS_GRAYFRAME, labelMask },
	{ "SS_WHITEFRAME", SS_WHITEFRAME, labelMask },
	{ "SS_ETCHEDFRAME", SS_ETCHEDFRAME, labelMask },
	{ "SS_ETCHEDHORZ", SS_ETCHEDHORZ, labelMask },
	{ "SS_ETCHEDVERT", SS_ETCHEDVERT, labelMask },
 	{ "SS_SIMPLE", SS_SIMPLE, labelMask },
 	{ "SS_SUNKEN", SS_SUNKEN, SS_SUNKEN },
 	{ "SS_NOTIFY", SS_NOTIFY, SS_NOTIFY },
 	{ "SS_NOPREFIX", SS_NOPREFIX, SS_NOPREFIX },
};

static ConstantValues pushButtonStyles[] =
{
	{ "WS_GROUP", WS_GROUP, WS_GROUP },
	{ "WS_TABSTOP", WS_TABSTOP, WS_TABSTOP },

	{ "BS_LEFT", BS_LEFT, BS_LEFT|BS_CENTER|BS_RIGHT },
	{ "BS_CENTER", BS_CENTER, BS_LEFT|BS_CENTER|BS_RIGHT },
	{ "BS_RIGHT", BS_RIGHT, BS_LEFT|BS_CENTER|BS_RIGHT },

	{ "BS_TOP", BS_TOP, BS_TOP|BS_VCENTER|BS_BOTTOM },
	{ "BS_VCENTER", BS_VCENTER, BS_TOP|BS_VCENTER|BS_BOTTOM },
	{ "BS_BOTTOM", BS_BOTTOM, BS_TOP|BS_VCENTER|BS_BOTTOM },

	{ "BS_ICON", BS_ICON, BS_ICON|BS_BITMAP },
	{ "BS_BITMAP", BS_BITMAP, BS_ICON|BS_BITMAP },

	{ "BS_DEFPUSHBUTTON", BS_DEFPUSHBUTTON, BS_DEFPUSHBUTTON },
	{ "BS_FLAT", BS_FLAT, BS_FLAT },
};

static ConstantValues checkBoxStyles[] =
{
	{ "WS_GROUP", WS_GROUP, WS_GROUP },
	{ "WS_TABSTOP", WS_TABSTOP, WS_TABSTOP },

	{ "BS_LEFT", BS_LEFT, BS_LEFT|BS_CENTER|BS_RIGHT },
	{ "BS_CENTER", BS_CENTER, BS_LEFT|BS_CENTER|BS_RIGHT },
	{ "BS_RIGHT", BS_RIGHT, BS_LEFT|BS_CENTER|BS_RIGHT },

	{ "BS_TOP", BS_TOP, BS_TOP|BS_VCENTER|BS_BOTTOM },
	{ "BS_VCENTER", BS_VCENTER, BS_TOP|BS_VCENTER|BS_BOTTOM },
	{ "BS_BOTTOM", BS_BOTTOM, BS_TOP|BS_VCENTER|BS_BOTTOM },

	{ "BS_ICON", BS_ICON, BS_ICON|BS_BITMAP },
	{ "BS_BITMAP", BS_BITMAP, BS_ICON|BS_BITMAP },

	{ "BS_RIGHTBUTTON", BS_RIGHTBUTTON, BS_RIGHTBUTTON },
	{ "BS_PUSHLIKE", BS_PUSHLIKE, BS_PUSHLIKE },
	{ "BS_FLAT", BS_FLAT, BS_FLAT },
};

#define radioButtonStyles	checkBoxStyles
#define groupBoxStyles		checkBoxStyles

static ConstantValues listBoxStyles[] =
{
	{ "WS_GROUP", WS_GROUP, WS_GROUP },
	{ "WS_TABSTOP", WS_TABSTOP, WS_TABSTOP },

	{ "LBS_DISABLENOSCROLL", LBS_DISABLENOSCROLL, LBS_DISABLENOSCROLL },
	{ "LBS_EXTENDEDSEL", LBS_EXTENDEDSEL, LBS_EXTENDEDSEL },
	{ "LBS_MULTICOLUMN", LBS_MULTICOLUMN, LBS_MULTICOLUMN },
	{ "LBS_MULTIPLESEL", LBS_MULTIPLESEL, LBS_MULTIPLESEL },
	{ "LBS_NOINTEGRALHEIGHT", LBS_NOINTEGRALHEIGHT, LBS_NOINTEGRALHEIGHT },
	{ "LBS_NOSEL", LBS_NOSEL, LBS_NOSEL },
	{ "LBS_NOTIFY", LBS_NOTIFY, LBS_NOTIFY },
	{ "LBS_SORT", LBS_SORT, LBS_SORT },
	{ "LBS_USETABSTOPS", LBS_USETABSTOPS, LBS_USETABSTOPS },
	{ "LBS_WANTKEYBOARDINPUT", LBS_WANTKEYBOARDINPUT, LBS_WANTKEYBOARDINPUT }
};

static ConstantValues comboBoxStyles[] =
{
	{ "WS_GROUP", WS_GROUP, WS_GROUP },
	{ "WS_TABSTOP", WS_TABSTOP, WS_TABSTOP },

	{ "CBS_SIMPLE", CBS_SIMPLE, CBS_SIMPLE|CBS_DROPDOWN|CBS_DROPDOWNLIST },
	{ "CBS_DROPDOWN", CBS_DROPDOWN, CBS_SIMPLE|CBS_DROPDOWN|CBS_DROPDOWNLIST },
	{ "CBS_DROPDOWNLIST", CBS_DROPDOWNLIST, CBS_SIMPLE|CBS_DROPDOWN|CBS_DROPDOWNLIST },

	{ "CBS_LOWERCASE", CBS_LOWERCASE, CBS_LOWERCASE|CBS_UPPERCASE },
	{ "CBS_UPPERCASE", CBS_UPPERCASE, CBS_LOWERCASE|CBS_UPPERCASE },

	{ "CBS_AUTOHSCROLL", CBS_AUTOHSCROLL, CBS_AUTOHSCROLL },
	{ "CBS_DISABLENOSCROLL", CBS_DISABLENOSCROLL, CBS_DISABLENOSCROLL },
	{ "CBS_SORT", CBS_SORT, CBS_SORT },
};

static ConstantValues editControlStyles[] =
{
	{ "WS_GROUP", WS_GROUP, WS_GROUP },
	{ "WS_TABSTOP", WS_TABSTOP, WS_TABSTOP },

	{ "ES_LEFT", ES_LEFT, ES_LEFT|ES_CENTER|ES_RIGHT },
	{ "ES_CENTER", ES_CENTER, ES_LEFT|ES_CENTER|ES_RIGHT },
	{ "ES_RIGHT", ES_RIGHT, ES_LEFT|ES_CENTER|ES_RIGHT },

	{ "ES_LOWERCASE", ES_LOWERCASE, ES_LOWERCASE|ES_UPPERCASE },
	{ "ES_UPPERCASE", ES_UPPERCASE, ES_LOWERCASE|ES_UPPERCASE },

	{ "ES_AUTOHSCROLL", ES_AUTOHSCROLL, ES_AUTOHSCROLL },
	{ "ES_NOHIDESEL", ES_NOHIDESEL, ES_NOHIDESEL },
	{ "ES_NUMBER", ES_NUMBER, ES_NUMBER },
	{ "ES_PASSWORD", ES_PASSWORD, ES_PASSWORD },
	{ "ES_READONLY", ES_READONLY, ES_READONLY },
};

static ConstantValues memoControlStyles[] =
{
	{ "WS_GROUP", WS_GROUP, WS_GROUP },
	{ "WS_TABSTOP", WS_TABSTOP, WS_TABSTOP },

	{ "ES_LEFT", ES_LEFT, ES_LEFT|ES_CENTER|ES_RIGHT },
	{ "ES_CENTER", ES_CENTER, ES_LEFT|ES_CENTER|ES_RIGHT },
	{ "ES_RIGHT", ES_RIGHT, ES_LEFT|ES_CENTER|ES_RIGHT },

	{ "ES_LOWERCASE", ES_LOWERCASE, ES_LOWERCASE|ES_UPPERCASE },
	{ "ES_UPPERCASE", ES_UPPERCASE, ES_LOWERCASE|ES_UPPERCASE },

	{ "ES_AUTOHSCROLL", ES_AUTOHSCROLL, ES_AUTOHSCROLL },
	{ "ES_AUTOVSCROLL", ES_AUTOVSCROLL, ES_AUTOVSCROLL },
	{ "ES_NOHIDESEL", ES_NOHIDESEL, ES_NOHIDESEL },
	{ "ES_READONLY", ES_READONLY, ES_READONLY },
};

static ConstantValues trackBarStyles[] =
{
	{ "WS_GROUP", WS_GROUP, WS_GROUP },
	{ "WS_TABSTOP", WS_TABSTOP, WS_TABSTOP },

	{ "TBS_HORZ", TBS_HORZ, TBS_HORZ|TBS_VERT },
	{ "TBS_VERT", TBS_VERT, TBS_HORZ|TBS_VERT },

	{ "TBS_LEFT/TOP", TBS_LEFT, TBS_LEFT|TBS_RIGHT|TBS_BOTH|TBS_NOTICKS },
	{ "TBS_RIGHT/BOTTOM", TBS_RIGHT, TBS_LEFT|TBS_RIGHT|TBS_BOTH|TBS_NOTICKS },
	{ "TBS_BOTH", TBS_BOTH, TBS_LEFT|TBS_RIGHT|TBS_BOTH|TBS_NOTICKS },
	{ "TBS_NOTICKS", TBS_NOTICKS, TBS_LEFT|TBS_RIGHT|TBS_BOTH|TBS_NOTICKS },


	{ "TBS_AUTOTICKS", TBS_AUTOTICKS, TBS_AUTOTICKS },
	{ "TBS_ENABLESELRANGE", TBS_ENABLESELRANGE, TBS_ENABLESELRANGE },
	{ "TBS_FIXEDLENGTH", TBS_FIXEDLENGTH, TBS_FIXEDLENGTH },
	{ "TBS_NOTHUMB", TBS_NOTHUMB, TBS_NOTHUMB },
	{ "TBS_TOOLTIPS", TBS_TOOLTIPS, TBS_TOOLTIPS },
	{ "TBS_REVERSED", TBS_REVERSED, TBS_REVERSED },
	{ "TBS_DOWNISLEFT", TBS_DOWNISLEFT, TBS_ENABLESELRANGE },
	{ "TBS_NOTIFYBEFOREMOVE", TBS_NOTIFYBEFOREMOVE, TBS_NOTIFYBEFOREMOVE },
	{ "TBS_TRANSPARENTBKGND", TBS_TRANSPARENTBKGND, TBS_TRANSPARENTBKGND },
};

static ConstantValues scrollBarStyles[] =
{
	{ "WS_GROUP", WS_GROUP, WS_GROUP },
	{ "WS_TABSTOP", WS_TABSTOP, WS_TABSTOP },

	{ "SBS_HORZ", SBS_HORZ, SBS_HORZ|SBS_VERT },
	{ "SBS_VERT", SBS_VERT, SBS_HORZ|SBS_VERT },

	{ "SBS_SIZEBOX", SBS_SIZEBOX, SBS_SIZEBOX|SBS_SIZEGRIP },
	{ "SBS_SIZEGRIP", SBS_SIZEGRIP, SBS_SIZEBOX|SBS_SIZEGRIP },

	{ "SBS_LEFT/TOP_ALIGN", SBS_LEFTALIGN, SBS_LEFTALIGN },
	{ "SBS_RIGHT/BOTTOM_ALIGN", SBS_RIGHTALIGN, SBS_RIGHTALIGN },
};

static ConstantValues upDownButtonStyles[] =
{
	{ "WS_GROUP", WS_GROUP, WS_GROUP },
	{ "WS_TABSTOP", WS_TABSTOP, WS_TABSTOP },

	{ "UDS_ALIGNLEFT", UDS_ALIGNLEFT, UDS_ALIGNLEFT },
	{ "UDS_ALIGNRIGHT", UDS_ALIGNRIGHT, UDS_ALIGNRIGHT },
	{ "UDS_ARROWKEYS", UDS_ARROWKEYS, UDS_ARROWKEYS },
	{ "UDS_AUTOBUDDY", UDS_AUTOBUDDY, UDS_AUTOBUDDY },
	{ "UDS_HORZ", UDS_HORZ, UDS_HORZ },
	{ "UDS_HOTTRACK", UDS_HOTTRACK, UDS_HOTTRACK },
	{ "UDS_NOTHOUSANDS", UDS_NOTHOUSANDS, UDS_NOTHOUSANDS },
	{ "UDS_SETBUDDYINT", UDS_SETBUDDYINT, UDS_SETBUDDYINT },
	{ "UDS_WRAP", UDS_WRAP, UDS_WRAP },
};

static ConstantValues treeViewStyles[] =
{
	{ "WS_GROUP", WS_GROUP, WS_GROUP },
	{ "WS_TABSTOP", WS_TABSTOP, WS_TABSTOP },

	{ "TVS_CHECKBOXES", TVS_CHECKBOXES, TVS_CHECKBOXES },
	{ "TVS_DISABLEDRAGDROP", TVS_DISABLEDRAGDROP, TVS_DISABLEDRAGDROP },
	{ "TVS_EDITLABELS", TVS_EDITLABELS, TVS_EDITLABELS },
	{ "TVS_FULLROWSELECT", TVS_FULLROWSELECT, TVS_FULLROWSELECT },
	{ "TVS_HASBUTTONS", TVS_HASBUTTONS, TVS_HASBUTTONS },
	{ "TVS_HASLINES", TVS_HASLINES, TVS_HASLINES },
	{ "TVS_INFOTIP", TVS_INFOTIP, TVS_INFOTIP },
	{ "TVS_LINESATROOT", TVS_LINESATROOT, TVS_LINESATROOT },
	{ "TVS_NOHSCROLL", TVS_NOHSCROLL, TVS_NOHSCROLL },
	{ "TVS_NONEVENHEIGHT", TVS_NONEVENHEIGHT, TVS_NONEVENHEIGHT },
	{ "TVS_NOSCROLL", TVS_NOSCROLL, TVS_NOSCROLL },
	{ "TVS_NOTOOLTIPS", TVS_NOTOOLTIPS, TVS_NOTOOLTIPS },

	{ "TVS_SHOWSELALWAYS", TVS_SHOWSELALWAYS, TVS_SHOWSELALWAYS },
	{ "TVS_SINGLEEXPAND", TVS_SINGLEEXPAND, TVS_SINGLEEXPAND },
	{ "TVS_TRACKSELECT", TVS_TRACKSELECT, TVS_TRACKSELECT }
};

static ConstantValues gridViewStyles[] =
{
	{ "WS_GROUP", WS_GROUP, WS_GROUP },
	{ "WS_TABSTOP", WS_TABSTOP, WS_TABSTOP },

	{ "gvGRID", gvGRID, gvGRID },
	{ "gvEDITABLE", gvEDITABLE, gvEDITABLE },
	{ "gvCAPTION_EDITABLE", gvCAPTION_EDITABLE, gvCAPTION_EDITABLE },
	{ "gvCOL_SIZABLE", gvCOL_SIZABLE, gvCOL_SIZABLE },
	{ "gvCOL_CREATE", gvCOL_CREATE, gvCOL_CREATE },
	{ "gvROW_CREATE", gvROW_CREATE, gvROW_CREATE },
};

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

StyleDialog::StyleDialog( unsigned  long style, const STRING &curentType ) 
: style(style), StyleDialog_form( NULL ) 
{
	if( curentType == ListBox::className )
	{
		additional = listBoxStyles;
		numAdditional = arraySize( listBoxStyles );
	}
	else if( curentType == PushButton::className )
	{
		additional = pushButtonStyles;
		numAdditional = arraySize( pushButtonStyles );
	}
	else if( curentType == CheckBox::className )
	{
		additional = checkBoxStyles;
		numAdditional = arraySize( checkBoxStyles );
	}
	else if( curentType == GroupBox::className )
	{
		additional = groupBoxStyles;
		numAdditional = arraySize( groupBoxStyles );
	}
	else if( curentType == RadioButton::className )
	{
		additional = radioButtonStyles;
		numAdditional = arraySize( radioButtonStyles );
	}
	else if( curentType == Label::className )
	{
		additional = labelStyles;
		numAdditional = arraySize( labelStyles );
	}
	else if( curentType == EditControl::className )
	{
		additional = editControlStyles;
		numAdditional = arraySize( editControlStyles );
	}
	else if( curentType == MemoControl::className )
	{
		additional = memoControlStyles;
		numAdditional = arraySize( memoControlStyles );
	}
	else if( curentType == ComboBox::className )
	{
		additional = comboBoxStyles;
		numAdditional = arraySize( comboBoxStyles );
	}
	else if( curentType == TrackBar::className )
	{
		additional = trackBarStyles;
		numAdditional = arraySize( trackBarStyles );
	}
	else if( curentType == ScrollBar::className )
	{
		additional = scrollBarStyles;
		numAdditional = arraySize( scrollBarStyles );
	}
	else if( curentType == UpDownButton::className )
	{
		additional = upDownButtonStyles;
		numAdditional = arraySize( upDownButtonStyles );
	}
	else if( curentType == FORM_TAG )
	{
		additional = formStyles;
		numAdditional = arraySize( formStyles );
	}
	else if( curentType == TreeView::className )
	{
		additional = treeViewStyles;
		numAdditional = arraySize( treeViewStyles );
	}
	else if( curentType == GridViewer::className )
	{
		additional = gridViewStyles;
		numAdditional = arraySize( gridViewStyles );
	}
	else
	{
		additional = NULL;
		numAdditional = 0;
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
   
ProcessStatus StyleDialog::handleCreate( void )
{
	styleMask = 0;
	for( size_t i=0; i<arraySize( windowStyles ); i++ )
	{
		styleMask |= windowStyles[i].mask;
		StyleListBox->addEntry( windowStyles[i].name );
	}
	for( size_t i=0; i<numAdditional; i++ )
	{
		styleMask |= additional[i].mask;
		StyleListBox->addEntry( additional[i].name );
	}

	for( size_t i=0; i<arraySize( windowStyles ); i++ )
	{
		if( (style & windowStyles[i].mask) == windowStyles[i].value )
			StyleListBox->selectEntry( int(i) );
	}
	for( size_t i=0; i<numAdditional; i++ )
	{
		if( (style & additional[i].mask) == additional[i].value )
			StyleListBox->selectEntry( int(arraySize( windowStyles ) + i) );
	}

	return StyleDialog_form::handleCreate();
}

ProcessStatus StyleDialog::handleOk( void )
{
	calcStyle();

	return StyleDialog_form::handleOk();
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

void StyleDialog::calcStyle()
{
	unsigned  long	newStyle = 0;
	ArrayOfInts		selItems;

	size_t numItems = StyleListBox->getSelectedItems( &selItems );
	if( numItems )
	{
		for( 
			ArrayOfInts::const_iterator it = selItems.cbegin(), endIT = selItems.cend();
			it != endIT;
			++it 
		)
		{
			size_t	idx = size_t(*it);
			if( idx < arraySize( windowStyles ) )
			{
				newStyle &= ~windowStyles[idx].mask;
				newStyle |= windowStyles[idx].value;
			}
			else if( (idx -= arraySize( windowStyles )) < numAdditional )
			{
				newStyle &= ~additional[idx].mask;
				newStyle |= additional[idx].value;
			}
		}
	}

	style = newStyle;
}

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

