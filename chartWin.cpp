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

Point ChartChild::value2Pixel(const Chart2dPoint &value, const Size &size )
{
	const double valWidth = m_xBounds.getMax() - m_xBounds.getMin();
	const double valWidthPerPixel = valWidth / size.width;
	const int pixelX = int((value.val1 - m_xBounds.getMin()) / valWidthPerPixel + 0.5);

	const double valHeight = m_yBounds.getMax() - m_yBounds.getMin();
	const double valHeightPerPixel = valHeight / size.height;
	const int pixelY = int(size.height - (value.val2 - m_yBounds.getMin()) / valHeightPerPixel + 0.5);

	return Point(pixelX, pixelY);
}

void ChartChild::drawBarCharts(Device &hDC, const Size &size)
{
	const int bottom = m_1dBounds.getMin() < 0 
		? size.height + int(m_1dBounds.getMin() * size.height / m_1dBounds.getRange() +0.5)
		: size.height;
	const int width = size.width / int(m_1dData.size());
	int left = 0;
	int right = left + width;
	for(
		AllCharts1D::const_iterator it = m_1dData.cbegin(), endIT = m_1dData.cend();
		it != endIT;
		++it
	)
	{
		bool textInBar = false;
		const int top = bottom - int((it->value * bottom) / m_1dBounds.getMax() +0.5);

		hDC.getPen().create(Pen::psNull, 0, 0);
		hDC.getBrush().create(it->color);
		hDC.rectangle(left,top,right,bottom);

		const STRING value = gak::formatNumber(it->value);
		Size textSize;
		hDC.getTextExtent( value, &textSize );

		RectBorder	textRect(left, top, right, bottom);
		if( textRect.top > textRect.bottom )
		{
			swap( &textRect.top, &textRect.bottom );
		}
		if( textRect.top - textSize.height >= 0 )
		{
			textRect.bottom = textRect.top;
			textRect.top -= textSize.height;
		}
		else if(textRect.bottom + textSize.height <= size.height)
		{
			textRect.top = textRect.bottom;
			textRect.bottom += textSize.height;
		}
		else
		{
			textInBar = true;
			textRect.top++;
			textRect.bottom--;
		}

		if( textInBar )
		{
			hDC.setTextColor( getGrayValue(it->color) < 128 ? colors::WHITE : colors::BLACK );
			hDC.setBackgroundColor( it->color );
		}
		else
		{
			hDC.setTextColor( colors::BLACK );
			hDC.setBackgroundColor( colors::WHITE );
		}
		hDC.drawText( value, textRect, DT_BOTTOM );

		left=right;
		right = left+width;
	}
}

void ChartChild::drawPieCharts(Device &hDC, const Size &size)
{
	hDC.getPen().create(Pen::psNull, 0, 0);

	Point center( size.width/2, size.height/2);
	int radius = gak::math::min(size.width, size.height) /2;
	double sum = 0;

	for(
		AllCharts1D::const_iterator it = m_1dData.cbegin(), endIT = m_1dData.cend();
		it != endIT;
		++it
	)
	{
		sum += it->value;
	}
	const double	pi2 = M_PI*2;
	double			startAngle = 0;
	for(
		AllCharts1D::const_iterator it = m_1dData.cbegin(), endIT = m_1dData.cend();
		it != endIT;
		++it
	)
	{
		hDC.getBrush().create(it->color);
		const double pieAngle = it->value * pi2 / sum;
		const double nextAngle = startAngle+pieAngle;
		hDC.pie(center,radius,nextAngle, startAngle );
		startAngle = nextAngle;
	}
}



void ChartChild::paintLine(Device &hDC, const Chart2D &lineData, const Size &size)
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


void ChartChild::add2dChart2( Chart2D *data )
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
	m_2dData.createElement().moveFrom( *data );
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
	if( m_useDemoData && !m_2dData.size() && !m_1dData.size() )
	{
		Chart1D	bData( colors::RED, 50);
		add1dChart( bData );
		Chart1D	bData2( colors::LIME, -20);
		add1dChart( bData2 );
		Chart1D	bData3( colors::BLUE, 40);
		add1dChart( bData3 );

		Chart2D	cData(3,colors::RED);
		cData.data.addElement(Chart2dPoint(0,0));
		cData.data.addElement(Chart2dPoint(1,25));
		cData.data.addElement(Chart2dPoint(2,20));
		cData.data.addElement(Chart2dPoint(3,40));
		cData.data.addElement(Chart2dPoint(4,-10));
		cData.data.addElement(Chart2dPoint(5,0));
		add2dChart2( &cData );

		Chart2D	cData2(2,colors::BLUE);
		cData2.data.addElement(Chart2dPoint(0,100));
		cData2.data.addElement(Chart2dPoint(1,55));
		cData2.data.addElement(Chart2dPoint(2,-80));
		cData2.data.addElement(Chart2dPoint(3,40));
		cData2.data.addElement(Chart2dPoint(4,0));
		cData2.data.addElement(Chart2dPoint(5,30));
		add2dChart2( &cData2 );

		Chart2D	cData3(2,colors::LIME);

		cData3.data.addElement(Chart2dPoint(1,80));
		cData3.data.addElement(Chart2dPoint(2,-80));
		cData3.data.addElement(Chart2dPoint(3,0));
		cData3.data.addElement(Chart2dPoint(4,33));
		cData3.data.addElement(Chart2dPoint(5,44));
		cData3.data.addElement(Chart2dPoint(6,0));
		add2dChart2( &cData3 );
	}

	Size size = getSize();

	if( m_1dData.size() )
	{
		if( m_1dType == BarChart )
			drawBarCharts(hDC, size);
		else if( m_1dType == PieChart )
			drawPieCharts(hDC, size);
	}

	for(
		AllCharts2D::const_iterator it = m_2dData.cbegin(), endIT = m_2dData.cend();
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
	m_1dBounds = gak::math::MinMax<double>();
	m_2dData.clear();
	m_1dData.clear();
	m_useDemoData = false;
}

void ChartChild::add2dChart( Chart2D *data )
{
	if(m_useDemoData)
	{
		clearData();
	}
	add2dChart2(data);
}

void ChartChild::add2dChart( const Chart2D &data )
{
	Chart2D copy( data );
	add2dChart(&copy);
}

void ChartChild::add1dChart( const Chart1D &value )
{
	if(m_useDemoData)
	{
		clearData();
	}
	m_1dData.addElement( value );
	m_1dBounds.test( value.value );
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
