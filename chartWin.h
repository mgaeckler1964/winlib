/*
		Project:		WINLIB
		Module:			chartWin.h
		Description:	A child displaying charts
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

#ifndef WINLIB_CHAR_WIN_H
#define WINLIB_CHAR_WIN_H

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifndef STRICT
#define STRICT 1
#endif

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak/ensemble.h>
#include <gak/math.h>

#include <winlib/childWin.h>


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

typedef gak::Duo<double,double>		Chart2dPoint;
typedef gak::Array<Chart2dPoint>	Chart2dData;

struct CommonChart
{
	int				lineWidth;
	COLORREF		color;
	CommonChart( int width=1, COLORREF color=RGB(0,0,0) ) : lineWidth(width), color(color) {}
};

struct	Chart2D : public CommonChart
{
	Chart2dData		data;

	Chart2D( int width=1, COLORREF color=RGB(0,0,0), const Chart2dData &data = Chart2dData() ) : CommonChart(width,color), data(data) {}
	Chart2D( int width, COLORREF color, Chart2dData *data ) : CommonChart(width,color)
	{
		this->data.moveFrom( *data );
	}

	Chart2D &moveFrom( Chart2D &source )
	{
		lineWidth = source.lineWidth;
		color = source.color;
		data.moveFrom( source.data );

		return *this;
	}
};
typedef gak::Array<Chart2D>		AllCharts2D;

struct	Chart1D : public CommonChart
{
	double		value;

	Chart1D( COLORREF color=0, double value=0 ) : CommonChart(0,color), value(value) {}
};
typedef gak::Array<Chart1D>		AllCharts1D;

enum Chart1dTypes
{
	BarChart, PieChart
};

enum Chart2dTypes
{
	LineChart, StapleChart
};

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class ChartChild : public ChildWindow
{
	static void registerClass();
	virtual STRING getWindowClassName() const;
	public:
	static const char className[];

	bool						m_useDemoData;
	// the line charts
	AllCharts2D					m_2dData;
	gak::math::MinMax<double>	m_xBounds, m_yBounds;
	Chart2dTypes				m_2dType;

	// the bar or pie chart
	AllCharts1D					m_1dData;
	gak::math::MinMax<double>	m_1dBounds;
	Chart1dTypes				m_1dType;

	void add2dChart2( Chart2D *data );

	Point value2Pixel(const Chart2dPoint &value, const Size &size );
	void drawBarCharts(Device &hDC, const Size &size);
	void drawPieCharts(Device &hDC, const Size &size);
	void paintLine(Device &hDC, const Chart2D &lineData, const Size &size);
	virtual ProcessStatus handleRepaint( Device &hDC );

public:
	ChartChild( BasicWindow *owner ) 
		: ChildWindow( owner ), m_useDemoData(true), m_1dType(BarChart)
	{
		registerClass();
	}

	void add2dChart( Chart2D *data );
	void add2dChart( const Chart2D &data );

	void add1dChart( const Chart1D &value );
	void set1Dtype(Chart1dTypes new1dType)
	{
		m_1dType = new1dType;
	}
	void clearData();
};

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

#endif // WINLIB_CHAR_WIN_H
