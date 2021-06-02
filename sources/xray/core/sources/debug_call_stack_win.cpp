////////////////////////////////////////////////////////////////////////////
//	Created 	: 15.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/os_include.h>
#include <dbghelp.h>
#include "debug_call_stack.h"

#define	_T( a )						a
#define	_tcscpy_s					xray::strings::copy
#define	_tcscmp						strcmp
#define	_tcschr						strchr
#define	_tcsstr						strstr
#define	_tcscat_s					strcat_s
#define	_stprintf_s					sprintf_s
#define	_tcslen						strlen

static u32 const s_max_stack		= 512;
static u32 const s_buffer_size		= 8192;

static bool		s_initialized		= false;
static bool		s_use_dbghelp		= false;
static HMODULE	s_dbghelp_handle	= 0;
static pcstr	s_dbghelp_id		= "dbghelp.dll";
static HMODULE	s_psapi_handle		= 0;
static pcstr	s_psapi_id			= "psapi.dll";

typedef USHORT (WINAPI *pfnCaptureStackBackTrace)(ULONG, ULONG, PVOID*, PULONG);
static pfnCaptureStackBackTrace	s_pfnCaptureStackBackTrace = 0;

typedef BOOL IMAGEAPI
	StackWalk64_type (
		DWORD                             MachineType,
		HANDLE                            hProcess,
		HANDLE                            hThread,
		LPSTACKFRAME64                    StackFrame,
		PVOID                             ContextRecord,
		PREAD_PROCESS_MEMORY_ROUTINE64    ReadMemoryRoutine,
		PFUNCTION_TABLE_ACCESS_ROUTINE64  FunctionTableAccessRoutine,
		PGET_MODULE_BASE_ROUTINE64        GetModuleBaseRoutine,
		PTRANSLATE_ADDRESS_ROUTINE64      TranslateAddress
	);
static StackWalk64_type*				s_StackWalk64 = 0;

typedef PVOID IMAGEAPI 
	SymFunctionTableAccess64_type(
		HANDLE  hProcess,
		DWORD64 AddrBase
    );
static SymFunctionTableAccess64_type*	s_SymFunctionTableAccess64	= 0;

typedef DWORD64 IMAGEAPI 
	SymGetModuleBase64_type(
		HANDLE  hProcess,
		DWORD64 AddrBase
    );
static SymGetModuleBase64_type*			s_SymGetModuleBase64 = 0;

typedef DWORD IMAGEAPI 
	UnDecorateSymbolName_type(
		PCTSTR,
		PCTSTR,
		DWORD,
		DWORD
    );
static UnDecorateSymbolName_type*		s_UnDecorateSymbolName = 0;

typedef BOOL IMAGEAPI
	SymGetSymFromAddr64_type(
		HANDLE              hProcess,
		DWORD64             qwAddr,
		PDWORD64            pdwDisplacement,
		PIMAGEHLP_SYMBOL64  Symbol
    );
static SymGetSymFromAddr64_type*		s_SymGetSymFromAddr64 = 0;

typedef BOOL IMAGEAPI
	SymGetLineFromAddr64_type(
		HANDLE              hProcess,
		DWORD64             qwAddr,
		PDWORD              pdwDisplacement,
		PIMAGEHLP_LINE64    Line64
    );
static SymGetLineFromAddr64_type*		s_SymGetLineFromAddr64 = 0;

typedef BOOL IMAGEAPI
	SymGetModuleInfo64_type(
		HANDLE,
		DWORD64,
		PIMAGEHLP_MODULE64
    );
static SymGetModuleInfo64_type*			s_SymGetModuleInfo64 = 0;

typedef BOOL IMAGEAPI
	SymInitialize_type(
		HANDLE				hProcess,
		PSTR				UserSearchPath,
		BOOL				fInvadeProcess
    );
static SymInitialize_type*				s_SymInitialize = 0;

typedef BOOL IMAGEAPI
	SymCleanup_type(
		HANDLE				hProcess
    );
static SymCleanup_type*					s_SymCleanup = 0;

typedef DWORD IMAGEAPI
	SymSetOptions_type(
		DWORD				SymOptions
    );
static SymSetOptions_type*				s_SymSetOptions = 0;

typedef DWORD IMAGEAPI	SymGetOptions_type( );
static SymGetOptions_type*				s_SymGetOptions = 0;

typedef DWORD WINAPI GetModuleBaseName_type ( HANDLE  hProcess   ,
                                            HMODULE hModule    ,
                                            LPTSTR  lpBaseName ,
                                            DWORD   nSize       ) ;
static GetModuleBaseName_type*			s_GetModuleBaseName = 0;

static void PCSTR2LPTSTR( PCSTR lpszIn, LPTSTR lpszOut, rsize_t outSize )
{
#if defined(UNICODE)||defined(_UNICODE)
   ULONG index = 0; 
   PCSTR lpAct = lpszIn;
   
	for( ; ; lpAct++ )
	{
		lpszOut[index++] = (TCHAR)(*lpAct);
		if ( *lpAct == 0 )
			break;
	} 
#else
   // This is trivial :)
	xray::strings::copy( lpszOut, (u32)outSize, lpszIn );
#endif
}

// for BugTrap compatibility
static inline HANDLE get_current_process	()
{
	return			GetCurrentProcess();
}

// Get the module name from a given address
static BOOL GetModuleNameFromAddress( DWORD64 address, LPTSTR lpszModule, rsize_t lpszModuleSize )
{
	BOOL              ret = FALSE;
	IMAGEHLP_MODULE64   moduleInfo;

	::ZeroMemory( &moduleInfo, sizeof(moduleInfo) );
	moduleInfo.SizeOfStruct = sizeof(moduleInfo);


	if ( s_SymGetModuleInfo64( GetCurrentProcess(), (DWORD)address, &moduleInfo ) )
	{
	   // Got it!
		PCSTR2LPTSTR( moduleInfo.ModuleName, lpszModule, lpszModuleSize );
		ret = TRUE;
	}
	else
	   // Not found :(
		_tcscpy_s( lpszModule, (u32)lpszModuleSize, _T("?") );
	
	return ret;
}

// Get function prototype and parameter info from ip address and stack address
static BOOL GetFunctionInfoFromAddresses( DWORD64 fnAddress, DWORD64 stackAddress, LPTSTR lpszSymbol, rsize_t lpszSymbolSize )
{
	XRAY_UNREFERENCED_PARAMETER	(stackAddress);
	BOOL              ret = FALSE;
	DWORD64             dwDisp = 0;
	DWORD             dwSymSize = 10000;
	TCHAR             lpszUnDSymbol[s_buffer_size]=_T("?");
	CHAR              lpszNonUnicodeUnDSymbol[s_buffer_size]="?";
//	LPTSTR            lpszParamSep = NULL;
	LPCTSTR           lpszParsed = lpszUnDSymbol;
	PIMAGEHLP_SYMBOL64  pSym = (PIMAGEHLP_SYMBOL64)GlobalAlloc( GMEM_FIXED, dwSymSize );

	::ZeroMemory( pSym, dwSymSize );
	pSym->SizeOfStruct = dwSymSize;
	pSym->MaxNameLength = dwSymSize - sizeof(IMAGEHLP_SYMBOL);

   // Set the default to unknown
	_tcscpy_s( lpszSymbol, (u32)lpszSymbolSize, _T("?") );

	// Get symbol info for IP
	if ( s_SymGetSymFromAddr64( get_current_process(), fnAddress, &dwDisp, pSym ) )
	{
	   // Make the symbol readable for humans
		s_UnDecorateSymbolName( pSym->Name, lpszNonUnicodeUnDSymbol, s_buffer_size, 
			UNDNAME_COMPLETE | 
			UNDNAME_NO_THISTYPE |
			UNDNAME_NO_SPECIAL_SYMS |
			UNDNAME_NO_MEMBER_TYPE |
			UNDNAME_NO_MS_KEYWORDS |
			UNDNAME_NO_ACCESS_SPECIFIERS
		);

      // Symbol information is ANSI string
		PCSTR2LPTSTR( lpszNonUnicodeUnDSymbol, lpszUnDSymbol, sizeof(lpszUnDSymbol) );

#if 0
      // I am just smarter than the symbol file :)
		if ( _tcscmp(lpszUnDSymbol, _T("_WinMain@16")) == 0 )
			_tcscpy_s(lpszUnDSymbol, sizeof(lpszUnDSymbol), _T("WinMain(HINSTANCE,HINSTANCE,LPCTSTR,int)"));
		else
		if ( _tcscmp(lpszUnDSymbol, _T("_main")) == 0 )
			_tcscpy_s(lpszUnDSymbol, sizeof(lpszUnDSymbol), _T("main(int,TCHAR * *)"));
		else
		if ( _tcscmp(lpszUnDSymbol, _T("_mainCRTStartup")) == 0 )
			_tcscpy_s(lpszUnDSymbol, sizeof(lpszUnDSymbol), _T("mainCRTStartup()"));
		else
		if ( _tcscmp(lpszUnDSymbol, _T("_wmain")) == 0 )
			_tcscpy_s(lpszUnDSymbol, sizeof(lpszUnDSymbol), _T("wmain(int,TCHAR * *,TCHAR * *)"));
		else
		if ( _tcscmp(lpszUnDSymbol, _T("_wmainCRTStartup")) == 0 )
			_tcscpy_s(lpszUnDSymbol, sizeof(lpszUnDSymbol), _T("wmainCRTStartup()"));

#endif // #if 0
		lpszSymbol[0] = _T('\0');

#if 0
    // Skip all templates
    if (_tcschr( lpszParsed, _T('<') ) == NULL) {
      // Let's go through the stack, and modify the function prototype, and insert the actual
      // parameter values from the stack
		  if ( _tcsstr( lpszUnDSymbol, _T("(void)") ) == NULL && _tcsstr( lpszUnDSymbol, _T("()") ) == NULL)
		  {
			  ULONG index = 0;
			  for( ; ; index++ )
			  {
				  lpszParamSep = const_cast<LPTSTR>(_tcschr( lpszParsed, _T(',') ));
				  if ( lpszParamSep == NULL )
					  break;

				  *lpszParamSep = _T('\0');

				  _tcscat_s( lpszSymbol, lpszSymbolSize, lpszParsed );
				  _stprintf_s( lpszSymbol + _tcslen(lpszSymbol), lpszSymbolSize-_tcslen(lpszSymbol), _T("=0x%08X,"), *((ULONG*)(stackAddress) + 2 + index) );

				  lpszParsed = lpszParamSep + 1;
			  }

			  lpszParamSep = const_cast<LPTSTR>(_tcschr( lpszParsed, _T(')') ));
			  if ( lpszParamSep != NULL )
			  {
				  *lpszParamSep = _T('\0');

				  _tcscat_s( lpszSymbol, lpszSymbolSize, lpszParsed );
				  _stprintf_s( lpszSymbol + _tcslen(lpszSymbol), lpszSymbolSize - _tcslen(lpszSymbol), _T("=0x%08X)"), *((ULONG*)(stackAddress) + 2 + index) );

				  lpszParsed = lpszParamSep + 1;
			  }
		  }
    }
#endif // #if 0

		_tcscat_s( lpszSymbol, lpszSymbolSize, lpszParsed );
   
		ret = TRUE;
	}

	GlobalFree( pSym );

	return ret;
}

// Get source file name and line number from IP address
// The output format is: "sourcefile(linenumber)" or
//                       "modulename!address" or
//                       "address"

static BOOL GetSourceInfoFromAddress( DWORD64 address, LPTSTR lpszModuleInfo, rsize_t lpszModuleInfoSize, LPTSTR lpszSourceInfo, rsize_t lpszSourceInfoSize, int& line, int& address_out )
{
	BOOL            ret = FALSE;
	IMAGEHLP_LINE64 lineInfo;
	DWORD           dwDisp;
//	TCHAR           lpszFileName[s_buffer_size] = _T("");
//	TCHAR           lpModuleInfo[s_buffer_size] = _T("");

	_tcscpy_s		( lpszSourceInfo, (u32)lpszSourceInfoSize, _T("?") );
	_tcscpy_s		( lpszModuleInfo, (u32)lpszModuleInfoSize, _T("?") );
	line			= -1;
	address_out		= 0x00000000;

	::ZeroMemory	( &lineInfo, sizeof( lineInfo ) );
	lineInfo.SizeOfStruct = sizeof( lineInfo );

	if ( s_SymGetLineFromAddr64( get_current_process(), address, &dwDisp, &lineInfo ) )
	{
	   // Got it. Let's use "sourcefile(linenumber)" format
		PCSTR2LPTSTR( lineInfo.FileName, lpszSourceInfo, lpszSourceInfoSize );
		line	= lineInfo.LineNumber;
		address_out	= (u32)(lineInfo.Address & u32(-1));

		if ( s_GetModuleBaseName ) {
			HMODULE module_handle =	(HMODULE)s_SymGetModuleBase64 ( get_current_process( ) , lineInfo.Address );
			s_GetModuleBaseName	(
				GetCurrentProcess( ),
				module_handle,
				lpszModuleInfo,
				(DWORD)lpszModuleInfoSize
			);
			if ( !*lpszModuleInfo )
		  		GetModuleNameFromAddress( address, lpszModuleInfo, lpszModuleInfoSize );
		}
		else
		  	GetModuleNameFromAddress( address, lpszModuleInfo, lpszModuleInfoSize );
		ret = TRUE;
	}
	else
	{
      // There is no source file information. :(
      // Let's use the "modulename!address" format
		if ( s_GetModuleBaseName ) {
			HMODULE module_handle =	(HMODULE)s_SymGetModuleBase64 ( get_current_process( ) , address );
			s_GetModuleBaseName	(
				GetCurrentProcess( ),
				module_handle,
				lpszModuleInfo,
				(DWORD)lpszModuleInfoSize
			);
			if ( !*lpszModuleInfo )
		  		GetModuleNameFromAddress( address, lpszModuleInfo, lpszModuleInfoSize );
		}
		else
		  	GetModuleNameFromAddress( address, lpszModuleInfo, lpszModuleInfoSize );

		address_out	= (u32)address;
		ret = FALSE;
	}
	
	return ret;
}

static void GetStackTraceFast( HANDLE hThread, DWORD64 ranOffsets[][2])
{
	XRAY_UNREFERENCED_PARAMETER	(hThread);
	if (!s_pfnCaptureStackBackTrace)
		return;

	void* stacktrace[s_max_stack + 1];
	int capcount = s_pfnCaptureStackBackTrace( 3, s_max_stack, stacktrace, NULL );
	int index;
	for (index = 0; index < capcount; index++) {
		ranOffsets[index][0] = (DWORD64) stacktrace[index];
		ranOffsets[index][1] = (DWORD64) stacktrace[index];
	}

	if (index <= s_max_stack) {
		ranOffsets[index][0] = 0;
		ranOffsets[index][1] = 0;
	}
}

static BOOL __stdcall CH_ReadProcessMemory ( HANDLE                      ,
                                      DWORD64 lpBaseAddress       ,
                                      LPVOID  lpBuffer            ,
                                      DWORD   nSize               ,
                                      LPDWORD lpNumberOfBytesRead  )
{
    return ( ReadProcessMemory ( GetCurrentProcess ( ) ,
                                 (LPCVOID)lpBaseAddress         ,
                                 lpBuffer              ,
                                 nSize                 ,
                                 (SIZE_T*)lpNumberOfBytesRead    ) ) ;
}

static void GetStackTrace(HANDLE hThread, /**DWORD64 ranOffsets[][2],/**/ _EXCEPTION_POINTERS* pointers, xray::core::debug::call_stack::Callback const& callback, bool invert_order )
{
	STACKFRAME64   callStack;
	BOOL           bResult;
	CONTEXT        context;
	HANDLE         hProcess = get_current_process();

	if ( !pointers ) {
		memset(&context, 0, sizeof(CONTEXT));
		context.ContextFlags = CONTEXT_FULL;
		if (hThread == GetCurrentThread())
		{
			RtlCaptureContext(&context);
		}
		else
		{
			if ( SuspendThread( hThread ) == -1 )
			{
				// whaaat ?!
//				OutputDebugStringFormat( _T("Call stack info(thread=0x%X) SuspendThread failed.\n") );
				return;
			}

		if (GetThreadContext(hThread, &context) == FALSE)
		{
//			OutputDebugStringFormat( _T("Call stack info(thread=0x%X) GetThreadContext failed.\n") );
			ResumeThread(hThread);
			return;
		}
		}
	}
	else
		xray::memory::copy	( &context, sizeof(context), pointers->ContextRecord, sizeof(CONTEXT) );
	
	::ZeroMemory( &callStack, sizeof(callStack) );

  DWORD imageType;
#ifdef _M_IX86
  // normally, call ImageNtHeader() and use machine info from PE header
  imageType = IMAGE_FILE_MACHINE_I386;
  callStack.AddrPC.Offset = context.Eip;
  callStack.AddrPC.Mode = AddrModeFlat;
  callStack.AddrFrame.Offset = context.Ebp;
  callStack.AddrFrame.Mode = AddrModeFlat;
  callStack.AddrStack.Offset = context.Esp;
  callStack.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
  imageType = IMAGE_FILE_MACHINE_AMD64;
  callStack.AddrPC.Offset = context.Rip;
  callStack.AddrPC.Mode = AddrModeFlat;
  callStack.AddrFrame.Offset = context.Rsp;
  callStack.AddrFrame.Mode = AddrModeFlat;
  callStack.AddrStack.Offset = context.Rsp;
  callStack.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
  imageType = IMAGE_FILE_MACHINE_IA64;
  callStack.AddrPC.Offset = context.StIIP;
  callStack.AddrPC.Mode = AddrModeFlat;
  callStack.AddrFrame.Offset = context.IntSp;
  callStack.AddrFrame.Mode = AddrModeFlat;
  callStack.AddrBStore.Offset = context.RsBSP;
  callStack.AddrBStore.Mode = AddrModeFlat;
  callStack.AddrStack.Offset = context.IntSp;
  callStack.AddrStack.Mode = AddrModeFlat;
#else
#error "Platform not supported!"
#endif

    int const num_levels_to_ignore = 0;
	TCHAR module_name[256] = _T("?");
	TCHAR file_name[2*260] = _T("?");
	TCHAR function[s_buffer_size] = _T("?");

	DWORD64 fnAddresses[256];
	DWORD64 stackAddresses[256];

	u32 call_stack_num_lines	=	0;

	for( ULONG index = 0; index<256; ++index ) 
	{
		bResult = s_StackWalk64(
			imageType,
			hProcess,
			hThread,
			&callStack,
			&context, 
			CH_ReadProcessMemory,
			s_SymFunctionTableAccess64,
			s_SymGetModuleBase64,
			NULL);

		// Ignore the first two levels (it's only TraceAlloc and operator new anyhow)
		if ( index < num_levels_to_ignore )
		    continue;

		// Break if we have fetched MAXSTACK levels
//		if ( index-num_levels_to_ignore == MAXSTACK)
//			break;

		// If we are at the top of the stackframe then break.
		if( !bResult || callStack.AddrFrame.Offset == 0) {

			fnAddresses[index]		=	0;
			stackAddresses[index]	=	0;
			call_stack_num_lines	=	index;
			break;
		}
    	

		fnAddresses[index]			=	callStack.AddrPC.Offset;
		stackAddresses[index]		=	callStack.AddrFrame.Offset;
	}

	int start_index					=	invert_order ? call_stack_num_lines-1 : 0;
	int step						=	invert_order ? -1 : +1;
	int end_index					=	invert_order ? -1 : call_stack_num_lines;
	
	for ( int index = start_index; index!=end_index; index += step)
	{
		int								line;
		int								address;
		GetSourceInfoFromAddress		( fnAddresses[index], module_name, sizeof(module_name), file_name, sizeof(file_name), line, address );
  		GetFunctionInfoFromAddresses	( fnAddresses[index], stackAddresses[index], function, sizeof(function) );
		callback						(
			index,
			call_stack_num_lines,
			module_name,
			file_name,
			line,
			function,
			address
		);
	}

	if ( hThread != GetCurrentThread() )
		ResumeThread( hThread );
}

static void WriteStackTrace ( DWORD64 ranOffsets[][2], u32 num_call_stack_lines, xray::core::debug::call_stack::Callback const& callback, bool invert_order )
{
	TCHAR module_name[256] = _T("?");
	TCHAR file_name[2*260] = _T("?");
	TCHAR function[s_buffer_size] = _T("?");

	u32 lines_count	=	0;
	while ( lines_count < s_max_stack && ranOffsets[lines_count][0] != 0 && ranOffsets[lines_count][1] != 0 )
		++lines_count;

	int start_index					=	invert_order ? lines_count-1 : 0;
	int step						=	invert_order ? -1 : +1;
	int end_index					=	invert_order ? -1 : lines_count;

	for (int index = start_index; index != end_index; index+=step) {
		int								line;
		int								address;
		GetSourceInfoFromAddress		( ranOffsets[index][0], module_name, sizeof(module_name), file_name, sizeof(file_name), line, address );
  		GetFunctionInfoFromAddresses	( ranOffsets[index][0], ranOffsets[index][1], function, sizeof(function) );
		if (
				!callback	(
					index,
					num_call_stack_lines,
					module_name,
					file_name,
					line,
					function,
					address
				)
			)
			break;
	}
}

// Let's figure out the path for the symbol files
// Search path= ".;%_NT_SYMBOL_PATH%;%_NT_ALTERNATE_SYMBOL_PATH%;%SYSTEMROOT%;%SYSTEMROOT%\System32;" + lpszIniPath
// Note: There is no size check for lpszSymbolPath!
static void InitSymbolPath( PSTR lpszSymbolPath, rsize_t symPathSize, PCSTR lpszIniPath )
{
	CHAR lpszPath[s_buffer_size];

   // Creating the default path
   // ".;%_NT_SYMBOL_PATH%;%_NT_ALTERNATE_SYMBOL_PATH%;%SYSTEMROOT%;%SYSTEMROOT%\System32;"
	xray::strings::copy( lpszSymbolPath, (u32)symPathSize, "." );

	// environment variable _NT_SYMBOL_PATH
	if ( GetEnvironmentVariableA( "_NT_SYMBOL_PATH", lpszPath, s_buffer_size ) )
	{
	  strcat_s( lpszSymbolPath, symPathSize, ";" );
		strcat_s( lpszSymbolPath, symPathSize, lpszPath );
	}

	// environment variable _NT_ALTERNATE_SYMBOL_PATH
	if ( GetEnvironmentVariableA( "_NT_ALTERNATE_SYMBOL_PATH", lpszPath, s_buffer_size ) )
	{
	  strcat_s( lpszSymbolPath, symPathSize, ";" );
		strcat_s( lpszSymbolPath, symPathSize, lpszPath );
	}

	// environment variable SYSTEMROOT
	if ( GetEnvironmentVariableA( "SYSTEMROOT", lpszPath, s_buffer_size ) )
	{
	  strcat_s( lpszSymbolPath, symPathSize, ";" );
		strcat_s( lpszSymbolPath, symPathSize, lpszPath );
		strcat_s( lpszSymbolPath, symPathSize, ";" );

		// SYSTEMROOT\System32
		strcat_s( lpszSymbolPath, symPathSize, lpszPath );
		strcat_s( lpszSymbolPath, symPathSize, "\\System32" );
	}

   // Add user defined path
	if ( lpszIniPath != NULL )
		if ( lpszIniPath[0] != '\0' )
		{
		  strcat_s( lpszSymbolPath, symPathSize, ";" );
			strcat_s( lpszSymbolPath, symPathSize, lpszIniPath );
		}
}

// Initializes the symbol files
static BOOL InitSymInfo( PCSTR lpszInitialSymbolPath )
{
	CHAR     lpszSymbolPath[s_buffer_size];
	DWORD    symOptions = s_SymGetOptions();

	symOptions |= SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS | SYMOPT_INCLUDE_32BIT_MODULES;
	symOptions &= ~SYMOPT_UNDNAME;
	s_SymSetOptions( symOptions );

   // Get the search path for the symbol files
	InitSymbolPath( lpszSymbolPath, sizeof(lpszSymbolPath), lpszInitialSymbolPath );
	return s_SymInitialize( get_current_process(), lpszSymbolPath, TRUE);
}

template < typename T >
static void load_function	( T*& result, HMODULE const module, pcstr const module_id, pcstr const function_id )
{
	if ( !s_use_dbghelp ) {
		result				= 0;
		return;
	}

	result					= ( T* ) GetProcAddress ( module, function_id );
	if ( !result ) {
		LOGI_WARNING		( "debug", "can't find function %s in %s", function_id, module_id );
		s_use_dbghelp		= false;
	}
}

static void load_library	( )
{
	R_ASSERT				( !s_dbghelp_handle, "you are trying to load bugtrap library twice");

	s_use_dbghelp			= true;

	s_dbghelp_handle		= LoadLibrary( s_dbghelp_id );
	if ( !s_dbghelp_handle ) {
		LOGI_ERROR			( "debug", "cannot load dbghelp library" );
		s_use_dbghelp		= false;
		return;
	}

	load_function			( s_StackWalk64					, s_dbghelp_handle, s_dbghelp_id, "StackWalk64");
	load_function			( s_SymFunctionTableAccess64	, s_dbghelp_handle, s_dbghelp_id, "SymFunctionTableAccess64");
	load_function			( s_SymGetModuleBase64			, s_dbghelp_handle, s_dbghelp_id, "SymGetModuleBase64");
	load_function			( s_UnDecorateSymbolName		, s_dbghelp_handle, s_dbghelp_id, "UnDecorateSymbolName");
	load_function			( s_SymGetSymFromAddr64			, s_dbghelp_handle, s_dbghelp_id, "SymGetSymFromAddr64");
	load_function			( s_SymGetLineFromAddr64		, s_dbghelp_handle, s_dbghelp_id, "SymGetLineFromAddr64");
	load_function			( s_SymGetModuleInfo64			, s_dbghelp_handle, s_dbghelp_id, "SymGetModuleInfo64");
	load_function			( s_SymInitialize				, s_dbghelp_handle, s_dbghelp_id, "SymInitialize");
	load_function			( s_SymCleanup					, s_dbghelp_handle, s_dbghelp_id, "SymCleanup");
	load_function			( s_SymSetOptions				, s_dbghelp_handle, s_dbghelp_id, "SymSetOptions");
	load_function			( s_SymGetOptions				, s_dbghelp_handle, s_dbghelp_id, "SymGetOptions");

	s_psapi_handle			= LoadLibrary( s_psapi_id );
	if ( !s_psapi_handle ) {
		LOGI_ERROR			( "debug", "cannot load psapi library" );
		return;
	}

	load_function			( s_GetModuleBaseName			, s_psapi_handle,	s_psapi_id,	  "GetModuleBaseNameA");
}

static void initialize		( )
{
	if ( s_initialized )
		return;

	s_initialized			= true;

	s_pfnCaptureStackBackTrace = ( pfnCaptureStackBackTrace )::GetProcAddress( GetModuleHandle( _T("ntdll.dll") ), "RtlCaptureStackBackTrace" );
	if ( !s_pfnCaptureStackBackTrace )
		LOGI_WARNING		( "debug", "can't find function RtlCaptureStackBackTrace in ntdll.dll");

	load_library			( );

	if ( s_use_dbghelp )
		InitSymInfo			( 0 );
}

void xray::core::debug::call_stack::iterate	( _EXCEPTION_POINTERS*	pointers,  // can be NULL
											  void*					callstack[], // can be NULL
											  Callback const&		callback,
											  bool					invert_order)
{
	::initialize			( );

	if ( !pointers && s_pfnCaptureStackBackTrace ) 
	{
		DWORD64		stack		[s_max_stack][2];
		u32			num_call_stack_lines	=	0;

		if ( callstack )
		{
			for ( u32 i=0; i<s_max_stack; ++i )
			{
				stack[i][0]	=	(DWORD64)callstack[i];
				stack[i][1]	=	(DWORD64)callstack[i];

				if ( !callstack[i] )
				{
					num_call_stack_lines	=	i;
					break;
				}
			}
		}
		else
		{
			GetStackTraceFast	(GetCurrentThread(), stack);
			num_call_stack_lines	=	0;
			for ( u32 i=0; i<s_max_stack && (stack[i][0] || stack[i][1]); ++i ) 
			{
				++num_call_stack_lines;
			}
		}

		WriteStackTrace			(stack, num_call_stack_lines, callback, invert_order);
	}
	else
	{
		GetStackTrace			(GetCurrentThread(), pointers, callback, invert_order);
	}
}

void xray::core::debug::call_stack::finalize_symbols( )
{
	if ( !s_SymCleanup )
		return;

	s_SymCleanup				( get_current_process() );
}

void xray::core::debug::call_stack::get_stack_trace (void* stacktrace[], u32 const stacktrace_max, u32 const num_to_capture, u32* out_hash)
{
	initialize							();
	XRAY_UNREFERENCED_PARAMETER			(stacktrace_max);
	ASSERT								(num_to_capture < stacktrace_max);
	if ( !s_pfnCaptureStackBackTrace )
	{
		stacktrace[0]				=	0;
		if ( out_hash )
		{
			*out_hash				=	0;
		}
		return;
	}

	ULONG hash_ulong;
	u32 const captured_once			=	s_pfnCaptureStackBackTrace(0, num_to_capture, stacktrace, &hash_ulong);
	stacktrace[captured_once]		=	0;
	if ( out_hash )
	{
		*out_hash					=	(u32)hash_ulong;
	}
}