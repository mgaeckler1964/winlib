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

	typedef gak::Duo<double,double>		ChartLinePoint;
	typedef gak::Array<ChartLinePoint>	Chart2dData;

	struct CommonChart
	{
		int				lineWidth;
		COLORREF		color;
		CommonChart( int width=1, COLORREF color=RGB(0,0,0) ) : lineWidth(width), color(color) {}
	};

	struct	LineChart : public CommonChart
	{
		Chart2dData		data;

		LineChart( int width=1, COLORREF color=RGB(0,0,0), const Chart2dData &data = Chart2dData() ) : CommonChart(width,color), data(data) {}
		LineChart( int width, COLORREF color, Chart2dData *data ) : CommonChart(width,color)
		{
			this->data.moveFrom( *data );
		}

		LineChart &moveFrom( LineChart &source )
		{
			lineWidth = source.lineWidth;
			color = source.color;
			data.moveFrom( source.data );

			return *this;
		}
	};
	typedef gak::Array<LineChart>		AllLineCharts;

	struct	BarChart : public CommonChart
	{
		double		value;

		BarChart( COLORREF color=0, double value=0 ) : CommonChart(0,color), value(value) {}
	};
	typedef gak::Array<BarChart>		AllBarCharts;


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
	AllLineCharts				m_chartData;
	gak::math::MinMax<double>	m_xBounds, m_yBounds;

	// the bar chart
	AllBarCharts				m_barCharts;
	gak::math::MinMax<double>	m_barBounds;

	void addChartLine2( LineChart *data );

	Point value2Pixel(const ChartLinePoint &value, const Size &size );
	void paintLine(Device &hDC, const LineChart &lineData, const Size &size);
	virtual ProcessStatus handleRepaint( Device &hDC );

public:
	ChartChild( BasicWindow *owner ) 
		: ChildWindow( owner ), m_useDemoData(true)
	{
		registerClass();
	}

	void addChartLine( LineChart *data );
	void addChartLine( const LineChart &data );
	void addBarChart( const BarChart &value );
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
