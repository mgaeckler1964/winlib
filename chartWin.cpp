/*
		Project:		WINLIB
		Module:			chartWin.cpp
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

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#ifndef STRICT
#define STRICT 1
#endif

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <WINLIB/chartWin.h>

#include <WINLIB/DEVICE.H>

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

const char ChartChild::className[] = "ChartChild";

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

void ChartChild::registerClass()
{
	static bool registered = false;

	if( !registered )
	{
		WNDCLASS	wc;

		fillDefaultClass( &wc );
		wc.style			|= CS_DBLCLKS;
		wc.lpszClassName	 = className;

		registered = ChildWindow::registerClass( &wc );
	}
}

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

Point ChartChild::value2Pixel(const ChartLinePoint &value, const Size &size )
{
	const double valWidth = m_xBounds.getMax() - m_xBounds.getMin();
	const double valWidthPerPixel = valWidth / size.width;
	const int pixelX = int((value.val1 - m_xBounds.getMin()) / valWidthPerPixel + 0.5);

	const double valHeight = m_yBounds.getMax() - m_yBounds.getMin();
	const double valHeightPerPixel = valHeight / size.height;
	const int pixelY = int(size.height - (value.val2 - m_yBounds.getMin()) / valHeightPerPixel + 0.5);

	return Point(pixelX, pixelY);
}

void ChartChild::paintLine(Device &hDC, const LineChart &lineData, const Size &size)
{
	gak::Array<Point>	myPolyline;

	hDC.getPen().create(Pen::psSolid, lineData.lineWidth, lineData.color);
	myPolyline.setMinSize(lineData.data.size());
	std::back_insert_iterator< gak::Array<Point> > inserter = std::back_inserter(myPolyline);
	for(
		Chart2dData::const_iterator it = lineData.data.cbegin(), endIT = lineData.data.cend();
		it != endIT;
		++it
	)
	{
		*inserter = value2Pixel( *it, size );
	}

	hDC.polyline(myPolyline);
}


void ChartChild::addChartLine2( LineChart *data )
{
	for(
		Chart2dData::const_iterator it = data->data.cbegin(), endIT = data->data.cend();
		it != endIT;
		++it
	)
	{
		m_xBounds.test( it->val1 );
		m_yBounds.test( it->val2 );
	}
	m_chartData.createElement().moveFrom( *data );
}

// --------------------------------------------------------------------- //
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

STRING ChartChild::getWindowClassName() const
{
	return className;
}

ProcessStatus ChartChild::handleRepaint( Device &hDC )
{
	if( !m_chartData.size() && !m_barCharts.size() )
	{
		BarChart	bData( RGB(255,0,0), 50);
		addBarChart( bData );
		BarChart	bData2( RGB(0,255,0), -20);
		addBarChart( bData2 );
		BarChart	bData3( RGB(0,0,255), 40);
		addBarChart( bData3 );

		LineChart	cData(3,RGB(255,0,0));
		cData.data.addElement(ChartLinePoint(0,0));
		cData.data.addElement(ChartLinePoint(1,25));
		cData.data.addElement(ChartLinePoint(2,20));
		cData.data.addElement(ChartLinePoint(3,40));
		cData.data.addElement(ChartLinePoint(4,-10));
		cData.data.addElement(ChartLinePoint(5,0));
		addChartLine2( &cData );

		LineChart	cData2(2,RGB(0,0,255));
		cData2.data.addElement(ChartLinePoint(0,100));
		cData2.data.addElement(ChartLinePoint(1,55));
		cData2.data.addElement(ChartLinePoint(2,-80));
		cData2.data.addElement(ChartLinePoint(3,40));
		cData2.data.addElement(ChartLinePoint(4,0));
		cData2.data.addElement(ChartLinePoint(5,30));
		addChartLine2( &cData2 );

		LineChart	cData3(2,RGB(0,255,0));

		cData3.data.addElement(ChartLinePoint(1,80));
		cData3.data.addElement(ChartLinePoint(2,-80));
		cData3.data.addElement(ChartLinePoint(3,0));
		cData3.data.addElement(ChartLinePoint(4,33));
		cData3.data.addElement(ChartLinePoint(5,44));
		cData3.data.addElement(ChartLinePoint(6,0));
		addChartLine2( &cData3 );
	}

	Size size = getSize();

	if( m_barCharts.size() )
	{
		const int bottom = m_barBounds.getMin() < 0 
			? size.height + m_barBounds.getMin() * size.height / m_barBounds.getRange()
			: size.height;
		const int width = size.width / m_barCharts.size();
		int left = 0;
		int right = left + width;
		for(
			AllBarCharts::const_iterator it = m_barCharts.cbegin(), endIT = m_barCharts.cend();
			it != endIT;
			++it
		)
		{
			const int top = bottom - (it->value * bottom) / m_barBounds.getMax();

			hDC.getBrush().create(it->color);
			hDC.rectangle(left,top,right,bottom);

			left=right;
			right = left+width;
		}
	}

	for(
		AllLineCharts::const_iterator it = m_chartData.cbegin(), endIT = m_chartData.cend();
		it != endIT;
		++it
	)
	{
		paintLine( hDC, *it, size );
	}

	return psPROCESSED;
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

void ChartChild::clearData()
{
	m_xBounds = gak::math::MinMax<double>();
	m_yBounds = gak::math::MinMax<double>();
	m_barBounds = gak::math::MinMax<double>();
	m_chartData.clear();
	m_barCharts.clear();
	m_useDemoData = false;
}

void ChartChild::addChartLine( LineChart *data )
{
	if(m_useDemoData)
	{
		clearData();
	}
	addChartLine2(data);
}

void ChartChild::addChartLine( const LineChart &data )
{
	LineChart copy( data );
	addChartLine(&copy);
}

void ChartChild::addBarChart( const BarChart &value )
{
	if(m_useDemoData)
	{
		clearData();
	}
	m_barCharts.addElement( value );
	m_barBounds.test( value.value );
}

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
