/*****************************************************************************
**
**  Definition of DLFR DSystemTools system tools class
**
**  Creation date:  2019/08/16
**  Created by:     Oleksandr Ivanov
**
**
**  Copyright (c) 2019 Piql AS. All rights reserved.
**
**  This file is part of the DLFR common library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include    "dsystemtools.h"
#include    "dstringtools.h"
#include    "derror.h"

#if defined ( D_OS_UNIX )
//#include    <iconv.h>
#   if !defined ( D_OS_X )
#       include    <sys/sysinfo.h>             // sysinfo()
#   endif
#include    <sys/socket.h>
#include    <sys/statvfs.h>
#include    <sys/syscall.h>
#include    <netinet/in.h>
#include    <netdb.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <errno.h>
#include    <inttypes.h>
#include    <signal.h>
#include    <sys/wait.h>
#include    <sys/mman.h>
#elif defined ( D_OS_WIN32 )
#include    <process.h>
#include    <assert.h>
#endif // D_OS_UNIX
#if defined (D_OS_X)    
#include    <corefoundation/CFBundle.h>
#include    <corefoundation/CFString.h>
#include    <unistd.h>
#endif

//  NAMESPACE
//
D_NAMESPACE_USING(D_NAMESPACE)
D_NAMESPACE_USING(std)

/****************************************************************************/
/*! \class  DSystemTools  dsystemtools.h
 *  \ingroup basecommon
 *  \brief  This class provides basic system tools.
 *
 */


//============================================================================
//  P U B L I C   I N T E R F A C E
//============================================================================

//----------------------------------------------------------------------------
/*! \fn     int  DSystemTools::sleep( long sec, long usec, void* mask, int maxfd )
 *  \param  sec     Seconds value
 *  \param  usec    Microseconds value
 *  \param  mask    File descriptor set mask
 *  \param  maxfd   Maximum file descriptor value
 *  \return success status
 *
 *  Sleep for specified amount. Optionally wakeup on I/O activity.
 */

#if defined ( D_OS_WIN32 )

#   if !defined( NDEBUG )
 int  DSystemTools::sleep(long sec, long usec, void* mask, int maxfd)
#   else
 int  DSystemTools::sleep( long sec, long usec, void* /*mask*/, int /*maxfd*/ )
#   endif
{
    // Waiting on file descriptors not yet supported for win32
    assert( mask == NULL && maxfd == -1 );
    Sleep( sec * 1000 + usec /1000 );
    return 0;
}

#else

 int  DSystemTools::sleep( long sec, long usec, void* mask, int maxfd )
{
    struct timeval  timeout;
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;

    return  ::select( maxfd+1, (fd_set*) mask, (fd_set*) 0, (fd_set*) 0, &timeout );
}
#endif


//----------------------------------------------------------------------------
/*! \fn     int  DSystemTools::sleepMs( long ms, void* mask, int maxfd )
 *  \param  ms      Sleep time in milliseconds
 *  \param  mask    File descriptor set mask
 *  \param  maxfd   Maximum file descriptor value
 *  \return success status
 *
 *  Sleep for specified amount. Optionally wakeup on I/O activity.
 */

#if defined ( D_OS_WIN32 )
#   if !defined( NDEBUG )
int  DSystemTools::sleepMs(long ms, void* mask, int maxfd)
#   else
int  DSystemTools::sleepMs( long ms, void* /*mask*/, int /*maxfd*/ )
#   endif
{
    // Waiting on file descriptors not yet supported for win32
    assert( mask == NULL && maxfd == -1 );
    Sleep( ms );
    return 0;
}

#else

int  DSystemTools::sleepMs( long ms, void* mask, int maxfd )
{
    struct timeval  timeout;
    timeout.tv_sec = ms / 1000;
    timeout.tv_usec = (ms * 1000) % 1000000;

    return  ::select( maxfd+1, (fd_set*) mask, (fd_set*) 0, (fd_set*) 0, &timeout );
}
#endif


//----------------------------------------------------------------------------
/*! \fn     int  DSystemTools::doShellCommand( const std::string& cmd )
 *  \param  cmd     Command to be executed
 *  \return exit code from system() call
 *
 *  Execute shell command, return exit code.
 */

int  DSystemTools::doShellCommand( const std::string& cmd )
{
    ERROR_F( "DSystemTools::doShellCommand" );

    error << ERRdebug << "Command='" << cmd << "'" << endl;

#if defined ( D_OS_WIN32 )
    wstring wideString;
    if ( !DStringTools::toWstring( cmd, wideString ) )
    {
        error << ERRwarning << "String conversion failed for: '" << cmd << "'" << endl;
        return -1;
    }
    int ret = _wsystem( wideString.c_str() );
#else
    int ret = system( cmd.c_str() );
#endif

    error << ERRdebug << "Return value=" << ret << endl;

    return ret;
}

//----------------------------------------------------------------------------
/*! 
 *  \param  ret        Command output
 *  \param  cmd        Command to be executed
 *  \return success state
 *
 *  Execute shell command, return success state.
 */

static bool doShellCommandProgress( string& line, void* data )
{
    *(string*)data += line;
    return true;
}

static bool doShellCommandCancel( void* /*data*/ )
{
    return false;
}

bool DSystemTools::doShellCommand(std::string& ret, const std::string& cmd)
{
    ret = "";
    return doShellCommand( doShellCommandProgress, doShellCommandCancel, cmd, (void*)&ret );
}


//----------------------------------------------------------------------------
/*!
 *  \param  progressCB Called for each line read from cmd, if returns false command is stopped
 *  \param  cancelCB   Called for each line read from cmd, if returns true command is stopped
 *  \param  cmd        Command to be executed
 *  \param  data       User data passed to progressCB and cancelCB
 *  \return success state
 *
 *  Execute shell command, return success state.
 */

static bool killProcess( pid_t pid )
{
#if defined ( D_OS_UNIX )
    // Negate pid to kill process group
    kill( -pid, SIGINT );
    return true;
#else
    UINT uExitCode = 1;
    BOOL ok = TerminateProcess( (HANDLE)pid, uExitCode );
    return ok == TRUE;
#endif
}

bool DSystemTools::doShellCommand(bool(*progressCB)(std::string& progress, void* data), bool(*cancelCB)(void* data), const std::string& cmd, void* data)
{
    ERROR_F( "DSystemTools::doShellCommand" );

    error << ERRdebug << cmd << endl;
    
#if defined ( D_OS_UNIX )
    int filedes[6];
    if ( pipe(&filedes[0]) == -1 || pipe(&filedes[2]) == -1 || pipe(&filedes[4]) == -1 ) 
    {
        return false;
    }
    
    pid_t pid = fork();
    bool success = true;

    if ( pid < 0 )
    {
        return  false;
    }
    else if ( pid == 0 )
    {
        // This is the child
        close( filedes[1] );   // Close write end of stdin.
        close( filedes[2] );   // Close read end of stdout.
        close( filedes[4] );   // Close read end of stderr.

        dup2( filedes[0], STDIN_FILENO );  // Have stdin read from the first pipe.
        dup2( filedes[3], STDOUT_FILENO ); // Have stdout write to the second pipe.
        dup2( filedes[5], STDERR_FILENO ); // Have stderr write to the third pipe.
        
/*
        // Close all other open file descriptors
        int maxfd = fileDescriptorTableSize();
        for( int fd = 3; fd < maxfd; fd++ )
        {
            close( fd );
        }
*/

        execlp( "/bin/sh", "/bin/sh", "-c", cmd.c_str(), (char *) NULL );

        error << ERRerror << "execlp() failed" << endl;
        _exit( 1 );
    }
    else if ( pid != 0 )
    {
        close( filedes[0] );
        close( filedes[3] );
        close( filedes[5] );
        
        // This is the parent, read from child stdout
        fd_set set;
        struct timeval timeout;


        // Initialize the timeout data structure.
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        char buffer[4096];
        // select returns 0 if timeout, 1 if input available, -1 if error. 
        int res;
        string line;
        while ( success ) 
        {
            // Read all output
            ssize_t count = 1;
            while ( success && count > 0 )
            {
                // Initialize the file descriptor set.
                FD_ZERO( &set );
                FD_SET( filedes[2], &set );

                res = select( FD_SETSIZE, &set, NULL, NULL, &timeout );
                if ( res == 1 )
                {
                    count = read( filedes[2], buffer, sizeof(buffer) );
                    error << ERRdebug << "read: " << count << endl;
                    if ( count == -1 ) 
                    {
                        if ( errno == EINTR ) 
                        {
                            continue;
                        } 
                        else 
                        {
                            error << ERRerror << "Read error" << endl;
                            success = false;
                            break;
                        }
                    }
                    else 
                    {
                        // Scan for newline
                        for ( ssize_t i = 0; i < count; i++ )
                        {
                            if ( buffer[i] == '\n' )
                            {
                                error << ERRdebug << "line: " << line << endl;
                                if ( !progressCB( line, data ) )
                                {
                                    error << ERRinfo << "progress abort" << endl;
                                    // Kill child
                                    killProcess( pid );

                                    success = false;
                                    break;
                                }
                                line.clear();
                            }
                            else
                            {
                                line.push_back( buffer[i] );
                            }
                        }
                    }
                }
                else
                {
                    break;
                }
            }

            // Abort if read failed
            if ( !success )
            {
                break;
            }

            if ( cancelCB( data ) )
            {
                error << ERRinfo << "cancel abort" << endl;

                // Kill child
                killProcess( pid );

                success = false;
                break;
            }            

            int status;
            if ( waitpid( pid, &status, WNOHANG ) != 0 )
            {
                error << ERRinfo << "child has exited" << endl;
                success = status == 0 && success;
                break;
            }

            DSystemTools::sleepMs( 10 );
        }

        if ( line.length() )
        {
            //error << ERRdebug << "line: " << line << endl;
            progressCB( line, data );
        }

        close( filedes[1] );
        close( filedes[2] );
        close( filedes[4] );

        // Wait for child to complete
        ::wait( 0 );
    }

    return success;
#else
    #define BUFSIZE 4096 
 
    HANDLE g_hChildStd_OUT_Rd = NULL;
    HANDLE g_hChildStd_OUT_Wr = NULL;

    // Set the bInheritHandle flag so pipe handles are inherited. 
    SECURITY_ATTRIBUTES saAttr; 
    saAttr.nLength = sizeof( SECURITY_ATTRIBUTES ); 
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL; 

    // Create a pipe for the child process's STDOUT. 
    if ( !CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) 
    {
        error << ERRerror << "Error creating stdout pipe" << endl;
        return false; 
    }

    // Ensure the read handle to the pipe for STDOUT is not inherited.
    if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
    {
        error << ERRerror << "Error setting pipe flags" << endl;
        return false; 
    }

    // Create the child process. 
    PROCESS_INFORMATION pi;
    ZeroMemory( &pi, sizeof(pi) );

    STARTUPINFOA si;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof( si );
    si.hStdError = g_hChildStd_OUT_Wr;
    si.hStdOutput = g_hChildStd_OUT_Wr;
    si.dwFlags |= STARTF_USESTDHANDLES;

    // Win 32 does not inhert file descriptors
    // Start the child process. 
    if( !CreateProcessA( NULL,   // No module name (use command line)
        (LPSTR)cmd.c_str(),        // Command line
        NULL,             // Process handle not inheritable
        NULL,             // Thread handle not inheritable
        TRUE,             // Set handle inheritance to FALSE
        CREATE_NO_WINDOW, // No creation flags
        NULL,             // Use parent's environment block
        NULL,             // Use parent's starting directory 
        &si,              // Pointer to STARTUPINFO structure
        &pi ) )           // Pointer to PROCESS_INFORMATION structure
    {
        error << ERRerror << "Failed to start: " << cmd << endl;
        return false;
    }

    // Read output from the child process's pipe for STDOUT
    // Stop when there is no more data. 
    bool ok = true;
    bool aborted = false;
    string line;
    while ( ok && !aborted )
    { 
        DWORD bytesAvail = 0;
        if ( !PeekNamedPipe(g_hChildStd_OUT_Rd, NULL, 0, NULL, &bytesAvail, NULL) ) 
        {
            ok = false;
        }

        if ( bytesAvail ) 
        {
            CHAR chBuf[BUFSIZE];
            DWORD n;
            BOOL success = ReadFile( g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &n, NULL );
            if ( !success || n == 0 ) 
            {
                
            }

            // Scan for newline
            for ( size_t i = 0; i < n; i++ )
            {
                if ( chBuf[i] == '\n' )
                {
                    //error << ERRdebug << "line: " << line << endl;
                    if ( !progressCB( line, data ) )
                    {
                        error << ERRinfo << "progress abort" << endl;
              
                        // Kill child
                        killProcess( (pid_t)pi.hProcess );

                        aborted = true;
                        break;
                    }
                    line.clear();
                }
                else if ( chBuf[i] == '\r' )
                {
                    // Ignore
                }
                else
                {
                    line.push_back( chBuf[i] );
                }
            }

            if ( cancelCB( data ) )
            {
                error << ERRinfo << "cancel abort" << endl;

                // Kill child
                killProcess( (pid_t)pi.hProcess );

                aborted = true;
                break;
            }
        }

        DWORD exitCode;
        GetExitCodeProcess( pi.hProcess, &exitCode );
        if ( g_hChildStd_OUT_Wr && exitCode != STILL_ACTIVE ) 
        {
            CloseHandle( g_hChildStd_OUT_Wr );
            g_hChildStd_OUT_Wr = 0;
        }

        sleepMs( 10 );
    }

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

    return !aborted;
#endif
}


//----------------------------------------------------------------------------
/*! \fn     bool  DSystemTools::spawnNoFileDescriptorInheritance( const std::string& prog )
 *  \param  prog     Program to be spawned
 *  \param  blocking Wait for command execution to finish?
 *  \return success status
 *
 *  Spawn specified program but do not inherit any file descriptors.
 *  
 *  For more info see: 
 *  http://unix.derkeiler.com/Newsgroups/comp.unix.programmer/2009-02/msg00165.html
 */

// \todo Remove use of global variable and use WIFEXITED and WEXITSTATUS to determine success state.
static bool* globalSpawnSuccess;
bool  DSystemTools::spawnNoFileDescriptorInheritance(const std::string& prog, bool blocking)
{
#if defined ( D_OS_UNIX )
    globalSpawnSuccess = static_cast<bool*>(mmap(NULL, sizeof *globalSpawnSuccess, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    *globalSpawnSuccess = true;
    pid_t pid = fork();

    bool ret = true;
    if ( pid < 0 )
    {
        munmap( globalSpawnSuccess, sizeof *globalSpawnSuccess );
        return  false;
    }
    else if ( pid == 0 )
    {
        // Close all open file descriptors
        int maxfd = fileDescriptorTableSize();
        for( int fd = 3; fd < maxfd; fd++ )
        {
            close( fd );
        }

        execlp( prog.c_str(), prog.c_str(), NULL );
        *globalSpawnSuccess = false;
        _exit( 0 );
    }
    else
    {
        if ( blocking )
        {
            int status;
            waitpid(pid, &status, 0);
            ret = *globalSpawnSuccess;
        }
        munmap( globalSpawnSuccess, sizeof *globalSpawnSuccess );
    }
    
    return ret;
#else
    (void)blocking;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof( si );
    ZeroMemory( &pi, sizeof(pi) );

    // Win 32 does not inhert file descriptors
    // Start the child process. 
    if( !CreateProcessA( NULL,   // No module name (use command line)
        (LPSTR)prog.c_str(),        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi ) )         // Pointer to PROCESS_INFORMATION structure
    {
        return false;
    }
    
    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

    return true;
#endif
}

//----------------------------------------------------------------------------
/*! \fn     bool  DSystemTools::spawn( const std::string& prog, char** argv, pid_t& pid )
 *  \param  prog    Program to be spawned
 *  \param  argv    Program argument vector
 *  \param  pid     PID of spawned prosess
 *  \return success status
 *
 *  Spawn specified program.
 */

#if defined ( D_OS_UNIX )
#include <spawn.h>

extern char **environ;

bool  DSystemTools::spawn( const std::string& prog, char** argv, pid_t& pid )
{
    return posix_spawnp( &pid, prog.c_str(), NULL, NULL, argv, environ ) == 0;
}

#elif defined ( D_OS_WIN32 )

bool  DSystemTools::spawn(const std::string& /*prog*/, char** /*argv*/, pid_t& /*pid*/)
{
    return false;
}

#endif // D_OS_UNIX


//----------------------------------------------------------------------------
/*! \fn     pid_t  DSystemTools::processId()
 *  \return process ID
 *
 *  Determine and return process ID.
 */

pid_t  DSystemTools::processId()
{
#if defined ( D_OS_UNIX )
    return  getpid();
#elif defined ( D_OS_WIN32 )
    return _getpid();
#endif // D_OS_UNIX 
}

unsigned long DSystemTools::threadId()
{
#if defined ( D_OS_UNIX )
    return  static_cast<unsigned long>(syscall(SYS_gettid));
#elif defined ( D_OS_WIN32 )
    return static_cast<unsigned long>(GetCurrentThreadId());
#endif // D_OS_UNIX 
}

//----------------------------------------------------------------------------
/*! \fn     uint32_t  DSystemTools::systemId()
 *  \return unique system ID
 *
 *  Determine and return unique system ID.
 */

uint32_t  DSystemTools::systemId()
{
#if defined( D_OS_IRIX )
    return  0x08000212;
#elif defined( D_OS_LINUX )
    return  0x0;
#elif defined ( D_OS_WIN32 )
    return  0x1;
#else
    return  0xffffffff;
#endif
}


//----------------------------------------------------------------------------
/*! \fn     int  DSystemTools::fileDescriptorTableSize()
 *  \return file descriptor table size
 *
 *  Return the file descriptor table size. This value determines the maximum
 *  number of file descriptors the current process is allowed to handle.
 */

int  DSystemTools::fileDescriptorTableSize()
{
#if defined ( D_OS_UNIX )
    return  getdtablesize();
#elif defined ( D_OS_WIN32 )
    return  0;
#endif // D_OS_UNIX
}


//----------------------------------------------------------------------------
/*! \fn     long  DSystemTools::uptime()
 *  \return system uptime in seconds
 *
 *  Determine and return system uptime in seconds.
 */

long  DSystemTools::uptime()
{
#if defined( D_OS_IRIX )
    return  0L;
#elif defined( D_OS_WIN32 )
    return  0L;
#elif defined( D_OS_X )
    return 0L;
#else
    struct sysinfo  info;

    if ( sysinfo( &info ) != 0 )
    {
        return  0L;
    }
    else
    {
        return  info.uptime;
    }
#endif
}


//----------------------------------------------------------------------------
/*! \fn     const char * DSystemTools::environmentVal( const char* env )
 *  \param  env     Name of environment variable
 *  \return environment variable contents
 *
 *  Determine and return content of specified environment variable. If no
 *  such environment variable exists, 0 will be returned.
 */

const char * DSystemTools::environmentVal(const char* env)
{
    return  getenv( env );
}


//----------------------------------------------------------------------------
/*! \fn     std::string  DSystemTools::username()
 *  \return username of the currently logged on user
 *  \todo find a utf8 compatible version
 *
 *  Returns the username of the currently logged on user.
 */

string  DSystemTools::username()
{
    const char* name;
#if defined ( D_OS_WIN32 )
    name = getenv( "USERNAME" );
#else
    name = getenv( "USER" );
#endif
    return string( name );
}


//----------------------------------------------------------------------------
/*! 
 *  \param  filename     Resource name
 *  \return path to resource, empty if not found
 *
 *  On OS-X applications are distributed in a package called bundles. This
 *  function returns the path to a file in the resource folder in the OS-X
 *  bundle.
 */

std::string  DSystemTools::getOsXBundleResource(const std::string& filename)
{
#if defined (D_OS_X)
    CFStringRef cfStringRef;
    cfStringRef = CFStringCreateWithBytes( CFAllocatorGetDefault(), (UInt8*)filename.c_str(), filename.length(), CFStringGetSystemEncoding(), false  );
    
    CFURLRef appUrlRef = CFBundleCopyResourceURL( CFBundleGetMainBundle(), cfStringRef, NULL, NULL );

    string path;
    if ( appUrlRef )
    {
      CFStringRef filePathRef = CFURLCopyPath( appUrlRef );
      const char *s =  CFStringGetCStringPtr( filePathRef, kCFStringEncodingUTF8 );
      if ( s ) path = s;
      // Release references
      CFRelease( filePathRef );
      CFRelease( appUrlRef );
    }
    CFRelease( cfStringRef );
    
    return path;
#else
    ERROR_F( "DSystemTools::getOsXBundleResource" );
    (void)filename; // Only used on OSX
    error << ERRerror << "Not supported on this platform." << endl;
    return string();
#endif        
}


/********************************** EOF *************************************/
