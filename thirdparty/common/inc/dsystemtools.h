#ifndef     DSYSTEMTOOLS_H
#define     DSYSTEMTOOLS_H

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
#include    "dcommon.h"

//  SYSTEM INCLUDES
//
#include    <string>

D_NAMESPACE_BEGIN(D_NAMESPACE)


//============================================================================
//  CLASS:  DSystemTools

class  DSystemTools
{
public:
    static int           sleep(long sec, long usec, void* mask = 0, int maxfd = -1);
    static int           sleepMs(long ms, void* mask = 0, int maxfd = -1);

    static int           doShellCommand(const std::string& cmd);
    static bool          doShellCommand(std::string& ret, const std::string& cmd);
    static bool          doShellCommand(bool(*progressCB)(std::string& progress, void* data), bool(*cancelCB)(void* data), const std::string& cmd, void* data);

    static bool          spawn(const std::string& prog, char** argv, pid_t& pid);
    static bool          spawnNoFileDescriptorInheritance(const std::string& prog, bool blocking);

    static pid_t         processId();
    static unsigned long threadId();
    static uint32_t      systemId();

    static int           fileDescriptorTableSize();
    static long          uptime();
    static const char *  environmentVal(const char* env);
    static std::string   username();
    static std::string   getOsXBundleResource(const std::string& filename);
};

D_NAMESPACE_END

//
//===================================EOF======================================

#endif  /* DSYSTEMTOOLS_H */
