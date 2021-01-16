/*
		Project:		Windows Class Library
		Module:			WINLIB.CPP
		Description:	Some utility functions
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

#include <time.h>

#include <windows.h>

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#include <winlib\winlib.h>

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

void idleLoop( void )
{
	MSG		msg;

	while( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

void winSleep( unsigned miliSec )
{
	clock_t		endTime;

	endTime = clock() + (clock_t)((double)miliSec/1000.0 * (double)CLK_TCK);
	do
	{
		idleLoop();
	} while( clock() < endTime );
}

unsigned int SyncExec( const char *command, unsigned int )
{
	STARTUPINFO				startInfo;
	PROCESS_INFORMATION		procInfo;
	HANDLE					procHandle;

	memset( &startInfo, 0, sizeof( startInfo ) );
	startInfo.cb = sizeof( startInfo );
	
	if( CreateProcess(	NULL, (char *)command, NULL, NULL, FALSE, CREATE_SEPARATE_WOW_VDM,
						NULL, NULL, &startInfo, &procInfo ) )
	{
		procHandle = OpenProcess( SYNCHRONIZE, FALSE, procInfo.dwProcessId );
		while( WaitForSingleObject( procHandle, -1 ) == WAIT_TIMEOUT )
			;
	}
	return 0;
}

unsigned int SyncExec( const char *command, const char *arguments, unsigned int )
{
	STARTUPINFO				startInfo;
	PROCESS_INFORMATION		procInfo;
	HANDLE					procHandle;

	memset( &startInfo, 0, sizeof( startInfo ) );
	startInfo.cb = sizeof( startInfo );
	
	if( CreateProcess(	command, (char *)arguments, NULL, NULL, FALSE, CREATE_SEPARATE_WOW_VDM,
						NULL, NULL, &startInfo, &procInfo ) )
	{
		procHandle = OpenProcess( SYNCHRONIZE, FALSE, procInfo.dwProcessId );
		while( WaitForSingleObject( procHandle, -1 ) == WAIT_TIMEOUT )
			;
	}
	return 0;
}

