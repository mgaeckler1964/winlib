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
	const int pixelX = (value.val1 - m_xBounds.getMin()) / valWidthPerPixel;

	const double valHeight = m_yBounds.getMax() - m_yBounds.getMin();
	const double valHeightPerPixel = valHeight / size.height;
	const int pixelY = size.height - (value.val2 - m_yBounds.getMin()) / valHeightPerPixel;

	return Point(pixelX, pixelY);
}

void ChartChild::paintLine(Device &hDC, const LineChart &lineData, const Size &size)
{

	gak::Array<Point>	myPolygon;

	Pen	linePen;
	linePen.create(Pen::psSolid, lineData.lineWidth, lineData.color);
	hDC.selectPen( linePen );
	myPolygon.setMinSize(lineData.data.size());
	std::back_insert_iterator< gak::Array<Point> > inserter = std::back_inserter(myPolygon);
	for(
		Chart2dData::const_iterator it = lineData.data.cbegin(), endIT = lineData.data.cend();
		it != endIT;
		++it
	)
	{
		*inserter = value2Pixel( *it, size );
	}

	hDC.polyline(myPolygon);

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
	if( m_useDemoData && !m_chartData.size() )
	{
		LineChart	cData(3,RGB(255,0,0));
		cData.data.addElement(ChartLinePoint(0,0));
		cData.data.addElement(ChartLinePoint(1,25));
		cData.data.addElement(ChartLinePoint(2,20));
		cData.data.addElement(ChartLinePoint(3,40));
		cData.data.addElement(ChartLinePoint(4,-10));
		cData.data.addElement(ChartLinePoint(5,0));
		addChartLine( &cData );

		LineChart	cData2(2,RGB(0,0,255));
		cData2.data.addElement(ChartLinePoint(0,100));
		cData2.data.addElement(ChartLinePoint(1,55));
		cData2.data.addElement(ChartLinePoint(2,-80));
		cData2.data.addElement(ChartLinePoint(3,40));
		cData2.data.addElement(ChartLinePoint(4,0));
		cData2.data.addElement(ChartLinePoint(5,30));
		addChartLine( &cData2 );

		LineChart	cData3(2,RGB(0,255,0));

		cData3.data.addElement(ChartLinePoint(1,80));
		cData3.data.addElement(ChartLinePoint(2,-80));
		cData3.data.addElement(ChartLinePoint(3,0));
		cData3.data.addElement(ChartLinePoint(4,33));
		cData3.data.addElement(ChartLinePoint(5,44));
		cData3.data.addElement(ChartLinePoint(6,0));
		addChartLine( &cData3 );

		m_useDemoData = true;
	}

	Size size = getSize();
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

void ChartChild::addChartLine( LineChart *data )
{
	if(m_useDemoData)
	{
		m_xBounds = gak::math::MinMax<double>();
		m_yBounds = gak::math::MinMax<double>();
		m_useDemoData = false;
	}
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
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif
