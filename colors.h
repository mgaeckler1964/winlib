/*
		Project:		WINLIB
		Module:			colors.h
		Description:	color definitions and macros
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

#ifndef WL_COLORS_H
#define WL_COLORS_H

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifndef STRICT
#define STRICT 1
#endif

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 4986 4820 4668 )
#endif

#include <windows.h>

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

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

namespace colors
{
	// color codes copied from here: https://www.rapidtables.com/web/color/RGB_Color.html

	static const COLORREF	BLACK=RGB(0,0,0);
	static const COLORREF	WHITE=RGB(255,255,255);
	static const COLORREF	RED=RGB(255,0,0);
	static const COLORREF	LIME=RGB(0,255,0);
	static const COLORREF	BLUE=RGB(0,0,255);
	static const COLORREF	YELLOW=RGB( 255, 255, 0 );
	static const COLORREF	MAGENTA=RGB( 255, 0, 255 );
	static const COLORREF	CYAN=RGB( 0, 255, 255 );
	static const COLORREF	MAROON=RGB(128,0,0);
	static const COLORREF	GREEN=RGB(0,128,0);
	static const COLORREF	NAVY=RGB(0,0,128);
	static const COLORREF	OLIVE=RGB(128,128,0);
	static const COLORREF	PURPE=RGB(128,0,128);
	static const COLORREF	TEAL=RGB(0,128,128);
	static const COLORREF	GRAY=RGB(128,128,128);
	static const COLORREF	SILVER=RGB(192,192,192);

	static const COLORREF	DARK_RED=RGB(139,0,0);
	static const COLORREF	BROWN=RGB(165,42,42);
	static const COLORREF	FIREBRICK=RGB(178,34,34);
	static const COLORREF	CRIMSON=RGB(220,20,60);
	static const COLORREF	TOMATO=RGB(255,99,71);
	static const COLORREF	CORAL=RGB(255,127,80);
	static const COLORREF	INDIAN_RED=RGB(205,92,92);
	static const COLORREF	SALMON=RGB(240,128,128);
	static const COLORREF	LIGHT_SALMON=RGB(255,160,122);
	static const COLORREF	ORANGE=RGB(255,165,0);
	static const COLORREF	GOLD=RGB(255,215,0);
	static const COLORREF	GOLD_ROD=RGB(218,165,32);
	static const COLORREF	KHAKI=RGB(240,230,140);
	static const COLORREF	YELLOW_GREEN=RGB(154,205,50);
	static const COLORREF	OLIVE_DRAB=RGB(107,142,35);
	static const COLORREF	LAWN_GREEN=RGB(124,252,0);
	static const COLORREF	GREEN_YELLOW=RGB(173,255,47);
	static const COLORREF	DARK_GREEN=RGB(0,100,0);
	static const COLORREF	FORREST_GREEN=RGB(34,139,34);
	static const COLORREF	LIME_GREEN=RGB(50,205,50);
	static const COLORREF	PALE_GREEN=RGB(152,251,152);
	static const COLORREF	SPRING_GREEN=RGB(0,255,127);
	static const COLORREF	SEA_GREEN=RGB(46,139,87);
	static const COLORREF	TURQUOISE=RGB(64,224,208);
	static const COLORREF	AQUA_MARINE=RGB(127,255,212);

	static const COLORREF	POWDER_BLUE=RGB(176,224,230);
	static const COLORREF	CADET_BLUE=RGB(95,158,160);
	static const COLORREF	STEEL_BLUE=RGB(70,130,180);
	static const COLORREF	CORN_FLOWER_BLUE=RGB(100,149,237);
	static const COLORREF	DODGER_BLUE=RGB(30,144,255);
	static const COLORREF	SKY_BLUE=RGB(135,206,235);
	static const COLORREF	MIDNIGHT_BLUE=RGB(25,25,112);
	static const COLORREF	DARK_BLUE=RGB(0,0,139);
	static const COLORREF	ROYAL_BLUE=RGB(65,105,225);
	static const COLORREF	BLUE_VIOLET=RGB(138,43,226);
	static const COLORREF	INDIGO=RGB(75,0,130);
	static const COLORREF	SLATE_BLUE=RGB(106,90,205);
	static const COLORREF	THISTLE=RGB(216,191,216);
	static const COLORREF	PLUM=RGB(221,160,221);
	static const COLORREF	VIOLET=RGB(238,130,238);
	static const COLORREF	ORCHID=RGB(218,112,214);
	static const COLORREF	PINK=RGB(255,192,203);
	static const COLORREF	ANTIQUE_WHITE=RGB(250,235,215);
	static const COLORREF	BEIGE=RGB(245,245,220);
	static const COLORREF	BISQUE=RGB(255,228,196);
	static const COLORREF	BLANCHED_ALMOND=RGB(255,235,205);
	static const COLORREF	WHEAT=RGB(245,222,179);
	static const COLORREF	CORN_SILK=RGB(255,248,220);
	static const COLORREF	LEMON_CHIFFON=RGB(255,250,205);
	static const COLORREF	SADDLE_BROWN=RGB(139,69,19);
	static const COLORREF	SIENNA=RGB(160,82,45);
	static const COLORREF	CHOCOLATE=RGB(210,105,30);
	static const COLORREF	PERU=RGB(205,133,63);
	static const COLORREF	SANDY_BROWN=RGB(244,164,96);
	static const COLORREF	BURLY_WOOD=RGB(222,184,135);
	static const COLORREF	TAN=RGB(210,180,140);
	static const COLORREF	ROSY_BROWN=RGB(188,143,143);
	static const COLORREF	MOCCASIN=RGB(255,228,181);
	static const COLORREF	NAVAJO_WHITE=RGB(255,222,173);
	static const COLORREF	REACH_PUFF=RGB(255,218,185);
	static const COLORREF	MISTY_ROSE=RGB(255,228,225);
	static const COLORREF	LAVENDER_BLUSH=RGB(255,240,245);
	static const COLORREF	LINEN=RGB(250,240,230);
	static const COLORREF	OLD_LACE=RGB(253,245,230);
	static const COLORREF	PAPAYA_WHIP=RGB(255,239,213);
	static const COLORREF	SEA_SHELL=RGB(255,245,238);
	static const COLORREF	MINT_GREEN=RGB(245,255,250);
	static const COLORREF	LAVENDER=RGB(230,230,250);
	static const COLORREF	FLORAL_WHITE=RGB(255,250,240);
	static const COLORREF	ALICE_BLUE=RGB(240,248,255);
	static const COLORREF	GHOST_WHITE=RGB(248,248,255);
	static const COLORREF	HONEY_DEW=RGB(240,255,240);
	static const COLORREF	IVORY=RGB(255,255,240);
	static const COLORREF	AURE=RGB(240,255,255);
	static const COLORREF	SNOW=RGB(255,250,250);
}

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

inline unsigned getGrayValue( unsigned red, unsigned green, unsigned blue )
{
	return (red + green + blue)/3;
}

inline unsigned getGrayValue( COLORREF color )
{
	return getGrayValue( GetRValue(color), GetGValue(color), GetBValue(color) );
}

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

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif	// WL_COLORS_H