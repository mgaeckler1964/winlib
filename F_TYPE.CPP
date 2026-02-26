/*
		Project:		Windows Class Library
		Module:			F_TYPE.CPP
		Description:	Registers file extensions
		Author:			Martin Gäckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2026 Martin Gäckler

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
#include <winlib/registry.h>

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
	doEnterFunctionEx( gakLogging::llInfo, "bool getFileType( const char *extension, const char *cmd, FileTypeRegistry *type )" );

	Registry	classesRoot;
	if( classesRoot.openPublic( "Software\\classes", KEY_READ|KEY_WOW64_64KEY ) != ERROR_SUCCESS )
/*@*/	return true;

	bool		error = true;
	STRING		valueBuff;

	type->extension = ".";
	type->extension += extension;
	type->cmd = cmd;

	Registry	extKey;		// for .../.<ext>
	Registry	typeKey;	// for .../<ext>file
	Registry	openKey;	// for .../<ext>file/shell/<cmd>	(open, print etc)
	Registry	ddeKey;		// for .../<ext>file/shell/<cmd>/ddeexec

	doLogValueEx( gakLogging::llInfo, type->extension );

	if( 
		extKey.openSubkey(classesRoot, type->extension, KEY_READ|KEY_WOW64_64KEY )==ERROR_SUCCESS && 
		extKey.readValue( &valueBuff )==rsOK && !valueBuff.isEmpty() 
	)
	{
		doLogValueEx(gakLogging::llInfo, valueBuff );
		type->type = valueBuff;

		if(
			typeKey.openSubkey( classesRoot, type->type, KEY_READ|KEY_WOW64_64KEY )==ERROR_SUCCESS && 
			typeKey.readValue( &valueBuff )==rsOK && !valueBuff.isEmpty() 
		)
			type->type_description = valueBuff;
		else
			type->type_description = type->type;
	}

	if( typeKey )
	{
		if( typeKey.readValue( "DefaultIcon", &valueBuff ) == rsOK )
			type->icon = valueBuff;
		else
			type->icon = type->type;


		STRING shellCmd = STRING("shell\\") + cmd;
		if(
			openKey.openSubkey( typeKey, shellCmd, KEY_READ|KEY_WOW64_64KEY )==ERROR_SUCCESS && 
			openKey.readValue( &valueBuff )==rsOK && !valueBuff.isEmpty() 
		)
			type->cmd_description = valueBuff;
		else
			type->cmd_description = type->cmd;
	}

	if( openKey && openKey.readValue( "command", &valueBuff)==rsOK && !valueBuff.isEmpty() )
	{
		type->commandLine = valueBuff;
		error = false;

		if( ddeKey.openSubkey( openKey, "ddeexec" ) == ERROR_SUCCESS  )
		{
			if( ddeKey.readValue( &valueBuff ) == rsOK )
				type->ddeCommand = valueBuff;
			else
				type->ddeCommand.release();

			if( ddeKey.readValue( "ifexec", &valueBuff ) == rsOK )
				type->ddeIfExec = valueBuff;
			else
				type->ddeIfExec.release();

			if( ddeKey.readValue( "Application", &valueBuff ) == rsOK )
				type->application = valueBuff;
			else
				type->application.release();

			if( ddeKey.readValue( "topic", &valueBuff ) == rsOK )
				type->topic = valueBuff;
			else
				type->topic.release();
		}
	}

	if( error )
	{
		type->cmd.release();
		type->cmd_description.release();
		type->commandLine.release();
		type->ddeCommand.release();
		type->ddeIfExec.release();
		type->application.release();
		type->topic.release();
	}
	doLogValueEx( gakLogging::llInfo, type->commandLine );
	doLogValueEx( gakLogging::llInfo, type->type_description );
	doLogValueEx( gakLogging::llInfo, error );
	return error;
}

bool addFileType( FileTypeRegistry *type )
{
	doEnterFunctionEx( gakLogging::llInfo, "bool addFileType( FileTypeRegistry *type )" );

	Registry	classesRoot;
	if( classesRoot.openPublic( "Software\\classes", KEY_READ|KEY_WOW64_64KEY ) != ERROR_SUCCESS )
/*@*/	return true;

	long		openResult;
	Registry	cmdTypeKey, // open edit etc.
				ddeKey;
	STRING		tmp;
	bool		error;

	doLogValueEx( gakLogging::llInfo, type->extension );
	doLogValueEx( gakLogging::llInfo, type->type );
	error = classesRoot.setValue( type->extension, rtSTRING, type->type.c_str(), type->type.strlen() ) != ERROR_SUCCESS;
	if( !error )
	{
		doLogValueEx( gakLogging::llInfo, type->type );
		doLogValueEx( gakLogging::llInfo, type->type_description );
		error = classesRoot.setValue( type->type, rtSTRING, type->type_description.c_str(), type->type_description.strlen() ) != ERROR_SUCCESS;
	}

	if( !error )
	{
		tmp = type->type + "\\DefaultIcon";
		if( type->icon[0U] )
			error = classesRoot.setValue( tmp, rtSTRING, type->icon.c_str(), type->icon.strlen() ) != ERROR_SUCCESS;
		else
			classesRoot.deleteSubkey( tmp );
	}

	if( !error )
	{
		tmp = type->type + "\\shell\\" + type->cmd;
		error = cmdTypeKey.createKey( classesRoot, tmp ) != ERROR_SUCCESS;
	}
	if( !error )
	{
		cmdTypeKey.setValue( rtSTRING, type->cmd_description.c_str(), type->cmd_description.strlen() );

		cmdTypeKey.setValue( "command", rtSTRING, type->commandLine.c_str(), type->commandLine.strlen() );

		if( type->ddeCommand[0U] && type->application[0U] )
		{
			openResult = ddeKey.createKey( cmdTypeKey, "ddeexec" );
			if( openResult == ERROR_SUCCESS )
			{
				ddeKey.setValue( rtSTRING, type->ddeCommand.c_str(), type->ddeCommand.strlen() );
				if( type->ddeIfExec[0U] )
					ddeKey.setValue( "ifexec", rtSTRING, type->ddeIfExec.c_str(), type->ddeIfExec.strlen() );
				else
					ddeKey.deleteSubkey( "ifexec" );

				ddeKey.setValue( "Application", rtSTRING, type->application.c_str(), type->application.strlen() );
				if( type->topic[0U] )
					ddeKey.setValue( "Topic", rtSTRING, type->topic.c_str(), type->topic.strlen() );
				else
					ddeKey.deleteSubkey( "Topic" );
				error = false;
			}
		}
		else
		{
			cmdTypeKey.deleteSubkey( "ddeexec\\Application" );
			cmdTypeKey.deleteSubkey( "ddeexec" );
			error = false;
		}
	}

	return error;
}

bool removeFileType( FileTypeRegistry *type )
{
	STRING	tmp = type->type + "\\shell\\" + type->cmd;
	bool	error = RegistryClassesRoot().deleteSubkey( tmp ) != ERROR_SUCCESS;

	return error;
}

}	// namespace winlib

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

