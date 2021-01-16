/*
		Project:		Windows Class Library
		Module:			F_TYPE.CPP
		Description:	Registers file extensions
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

#include <windows.h>
#include <shellapi.h>

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#include <winlib/f_type.h>

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

using namespace gak;

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

bool getFileType( const char *extension, const char *cmd, FileTypeRegistry *type )
{
	long	openResult;
	char	valueBuff[512];
	long	valueSize;
	HKEY	typeKey, ddeKey;
	STRING	tmp;
	bool	error = true;

	type->extension = ".";
	type->extension += extension;
	type->cmd = cmd;

	valueSize = sizeof( valueBuff ) -1;
	openResult = RegQueryValue( HKEY_CLASSES_ROOT, type->extension, valueBuff, &valueSize );
	if( openResult == ERROR_SUCCESS && *valueBuff )
	{
		type->type = valueBuff;
		valueSize = sizeof( valueBuff ) -1;
		openResult = RegQueryValue( HKEY_CLASSES_ROOT, type->type, valueBuff, &valueSize );
		if( openResult == ERROR_SUCCESS )
			type->type_description = valueBuff;
		else
			type->type_description = type->type;

		tmp = type->type + "\\DefaultIcon";
		valueSize = sizeof( valueBuff ) -1;
		openResult = RegQueryValue( HKEY_CLASSES_ROOT, tmp, valueBuff, &valueSize );
		if( openResult == ERROR_SUCCESS )
			type->icon = valueBuff;
		else
			type->icon = "";

		tmp = type->type + "\\shell\\" + cmd;

		openResult = RegOpenKey( HKEY_CLASSES_ROOT, tmp, &typeKey );
		if( openResult == ERROR_SUCCESS  )
		{
			valueSize = sizeof( valueBuff ) -1;
			openResult = RegQueryValue( typeKey, NULL, valueBuff, &valueSize );
			if( openResult == ERROR_SUCCESS  )
				type->cmd_description = valueBuff;
			else
				type->cmd_description = type->cmd;

			valueSize = sizeof( valueBuff ) -1;
			openResult = RegQueryValue( typeKey, "command", valueBuff, &valueSize );
			if( openResult == ERROR_SUCCESS )
			{
				type->commandLine = valueBuff;
				error = false;

				openResult = RegOpenKey( typeKey, "ddeexec", &ddeKey );
				if( openResult == ERROR_SUCCESS  )
				{
					valueSize = sizeof( valueBuff ) -1;
					openResult = RegQueryValue( ddeKey, NULL, valueBuff, &valueSize );
					if( openResult == ERROR_SUCCESS )
						type->ddeCommand = valueBuff;
					else
						type->ddeCommand = "";

					valueSize = sizeof( valueBuff ) -1;
					openResult = RegQueryValue( ddeKey, "ifexec", valueBuff, &valueSize );
					if( openResult == ERROR_SUCCESS )
						type->ddeIfExec = valueBuff;
					else
						type->ddeIfExec = "";

					valueSize = sizeof( valueBuff ) -1;
					openResult = RegQueryValue( ddeKey, "Application", valueBuff, &valueSize );
					if( openResult == ERROR_SUCCESS )
						type->application = valueBuff;
					else
						type->application = "";

					valueSize = sizeof( valueBuff ) -1;
					openResult = RegQueryValue( ddeKey, "topic", valueBuff, &valueSize );
					if( openResult == ERROR_SUCCESS )
						type->topic = valueBuff;
					else
						type->topic = "";
				}
				else
				{
					type->ddeCommand = "";
					type->ddeIfExec = "";
					type->application = "";
					type->topic = "";
				}

			}

			RegCloseKey( typeKey );
		}
	}

	return error;
}

bool addFileType( FileTypeRegistry *type )
{
	long	openResult;
	HKEY	typeKey, ddeKey;
	STRING	tmp;
	bool	error;

	error = (bool)RegSetValue( HKEY_CLASSES_ROOT, type->extension, REG_SZ, type->type, DWORD(type->type.strlen()) );
	if( !error )
		error = RegSetValue( HKEY_CLASSES_ROOT, type->type, REG_SZ, type->type_description, DWORD(type->type_description.strlen()) );

	if( !error )
	{
		tmp = type->type + "\\DefaultIcon";
		if( type->icon[0U] )
			error = RegSetValue( HKEY_CLASSES_ROOT, tmp, REG_SZ, type->icon, DWORD(type->icon.strlen()) );
		else
			RegDeleteKey( HKEY_CLASSES_ROOT, tmp );
	}

	if( !error )
	{
		tmp = type->type + "\\shell\\" + type->cmd;
		openResult = RegCreateKey( HKEY_CLASSES_ROOT, tmp, &typeKey );
	}
	if( !error && openResult == ERROR_SUCCESS )
	{
		RegSetValue( typeKey, NULL, REG_SZ, type->cmd_description, DWORD(type->cmd_description.strlen()) );

		RegSetValue( typeKey, "command", REG_SZ, type->commandLine, DWORD(type->commandLine.strlen()) );

		if( type->ddeCommand[0U] && type->application[0U] )
		{
			openResult = RegCreateKey( typeKey, "ddeexec", &ddeKey );
			if( openResult == ERROR_SUCCESS )
			{
				RegSetValue( ddeKey, NULL, REG_SZ, type->ddeCommand, DWORD(type->ddeCommand.strlen()) );
				if( type->ddeIfExec[0U] )
					RegSetValue( ddeKey, "ifexec", REG_SZ, type->ddeIfExec, DWORD(type->ddeIfExec.strlen()) );
				else
					RegDeleteKey( ddeKey, "ifexec" );

				RegSetValue( ddeKey, "Application", REG_SZ, type->application, DWORD(type->application.strlen()) );
				if( type->topic[0U] )
					RegSetValue( ddeKey, "Topic", REG_SZ, type->topic, DWORD(type->topic.strlen()) );
				else
					RegDeleteKey( ddeKey, "Topic" );

				RegCloseKey( ddeKey );
				error = false;
			}
		}
		else
		{
			RegDeleteKey(typeKey, "ddeexec\\Application" );
			RegDeleteKey(typeKey, "ddeexec" );
			error = false;
		}

		RegCloseKey( typeKey );
	}

	return error;
}

bool removeFileType( FileTypeRegistry *type )
{
	long	deleteResult;
	STRING	tmp;
	bool	error = true;

	tmp = type->type + "\\shell\\" + type->cmd;
	deleteResult = RegDeleteKey( HKEY_CLASSES_ROOT, tmp );
	if( deleteResult == ERROR_SUCCESS )
	{
		error = false;
	}

	return error;
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

