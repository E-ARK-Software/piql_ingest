#ifndef     DTYPES_H
#define     DTYPES_H

/*****************************************************************************
**  $Id: dtypes.h,v 1.7 2005/07/22 09:09:00 swhbt Exp $
**
**  DLFR common data type definitions
**
**  Creation date:  2004/03/22
**  Created by:     Han Timmerman
**
**  Project ID:     NO-04001-01S
**  Project name:   Piql DLFR
**  Project status: Create
**
**  Target(s):      Linux 2.4.x / GCC 3.0
**
**  Namespace:      DLFR
**
**  Dependencies:   -
**  Restrictions:   -
**  ToDo:           2004/03/22  Detect ECOS OS
**
**  Copyright (c) 2004 ANT Software Development. All rights reserved.
**
**  This file is part of the DLFR common library
**
*****************************************************************************/

#ifndef     DCOMMON_H
#   include "dcommon.h"
#endif


/****************************************************************************/
/*  O P E R A T I N G   S Y S T E M   D E P E N D E N T   T Y P E S         */
/****************************************************************************/

#if defined( D_OS_UNIX ) 
#   include <limits.h>
#   include <inttypes.h>
#   include <sys/types.h>

D_C_DECL_BEGIN

    typedef int8_t      INT8;
    typedef int16_t     INT16;
    typedef int32_t     INT32;
    typedef int64_t     INT64;

    typedef uint8_t     UINT8;
    typedef uint16_t    UINT16;
    typedef uint32_t    UINT32;
    typedef uint64_t    UINT64;

D_C_DECL_END


#elif defined( D_OS_ECOS ) 
#   include <limits.h>
#   include <sys/types.h>

D_C_DECL_BEGIN

    typedef int8_t      INT8;
    typedef int16_t     INT16;
    typedef int32_t     INT32;
    typedef int64_t     INT64;

    typedef uint8_t     UINT8;
    typedef uint16_t    UINT16;
    typedef uint32_t    UINT32;
    typedef uint64_t    UINT64;

D_C_DECL_END


#elif defined ( D_OS_WIN32 )
#   include <Winsock2.h>
#   include <windows.h>
#   include <inttypes.h>
#   define strcasecmp _stricmp
    typedef unsigned __int64    uint64_t;         
    typedef unsigned int        uint32_t;
    typedef unsigned __int16    uint16_t;
    typedef int pid_t;
#if defined( D_OS_WIN64 )
    typedef __int64 ssize_t;
#else
    typedef long ssize_t;
#endif
    typedef int mode_t;
#endif



/****************************************************************************/
/*  P R O J E C T   T Y P E S                                               */
/****************************************************************************/

D_C_DECL_BEGIN

    typedef UINT8       byte;

D_C_DECL_END



/****************************************************************************/
/*  P R O J E C T   C O N S T A N T S                                       */
/****************************************************************************/

/*********************************/
/*  DEVICE AND FILE DEFINITIONS  */
/*********************************/

D_C_DECL_BEGIN

    enum  IoDeviceType
    {
        D_DT_UNDEFINED = 0,
        D_DT_FS_FILE,
        D_DT_SERIAL
    };


    enum  IoFileType
    {
        D_FT_UNDEFINED  = 0,
        D_FT_FILE_ANY,
        D_FT_FILE_DIR,
        D_FT_FILE_REG,
        D_FT_FILE_CFG
    };


    enum  IoOpenMode
    {
        D_OM_DEFAULT    = -1,           /** Default open mode */
        D_OM_UNDEFINED  =  0,           /** Undefined mode */
        D_OM_RDONLY     =  0x0001,      /** Read only */
        D_OM_WRONLY     =  0x0002,      /** Write only */
        D_OM_RDWR       =  0x0003,      /** Read/Write */
        D_OM_APPEND     =  0x0008,      /** Append */
        D_OM_CREATE     =  0x0010,      /** Create */
        D_OM_TRUNCATE   =  0x0020,      /** Truncate */
        D_OM_BINARY     =  0x0040       /** Binary */
    };


    enum  IoPermissions
    {
        D_PERM_DEFAULT  = -1,           /** Default permission */
        D_PERM_NONE     =  0x0000,      /** No access */
        D_PERM_USR_X    =  0x0001,      /** User read access */
        D_PERM_USR_W    =  0x0002,      /** User write access */
        D_PERM_USR_R    =  0x0004,      /** User execute access */
        D_PERM_GRP_X    =  0x0010,      /** Group read access */
        D_PERM_GRP_W    =  0x0020,      /** Group write access */
        D_PERM_GRP_R    =  0x0040,      /** Group execute access */
        D_PERM_OTH_X    =  0x0100,      /** Other read access */
        D_PERM_OTH_W    =  0x0200,      /** Other write access */
        D_PERM_OTH_R    =  0x0400       /** Other execute access */
    };


    enum  IoParity
    {
        D_PARITY_UNDEFINED  = 0,
        D_PARITY_NONE,
        D_PARITY_EVEN,
        D_PARITY_ODD
    };


    enum  IoDuplex
    {
        D_DUPLEX_UNDEFINED  = 0,
        D_DUPLEX_FULL,
        D_DUPLEX_HALF
    };


    enum  IoFlow
    {
        D_FLOW_UNDEFINED    = 0,        /** Flow control undefined */
        D_FLOW_NONE,                    /** No flow control */
        D_FLOW_XONXOFF,                 /** Software flow control */
        D_FLOW_RTSCTS,                  /** Hardware flow control */
        D_FLOW_MODEM                    /** Full hardware flow control */
    };


    enum  IoModem
    {
        D_MODEM_LINE_ENABLE = 0x0001,   /** DSR / Line enable */
        D_MODEM_DCD         = 0x0002,   /** Data Carrier Detect */
        D_MODEM_DTR         = 0x0004,   /** Data Terminal Ready */
        D_MODEM_DSR         = 0x0008,   /** Data Set Ready */
        D_MODEM_RTS         = 0x0010,   /** Request To Send */
        D_MODEM_CTS         = 0x0020,   /** Clear To Send */
        D_MODEM_RING        = 0x0080    /** Ring detect */
    };


    enum  AppType
    {
        D_APP_UNDEFINED     = 0,        /** Undefined application type */
        D_APP_DAEMON,                   /** Daemon server application */
        D_APP_CONSOLE,                  /** Console based application */
        D_APP_GUI                       /** GUI application */
    };


    enum  UnitType
    {
        D_UNIT_DEFAULT      = -1,
        D_UNIT_UNDEFINED    =  0,
        D_UNIT_METER        =  1,
        D_UNIT_FEET         =  2,
        D_UNIT_FRAMES       =  3,
        D_UNIT_TIME         =  4
    };



    #define      D_IO_DEFAULT_PERMISSIONS       ((int)0x0666)
    #define      D_IO_DEFAULT_LINE_TERM_CHR     ((UINT8)'\n')
    #define      D_IO_MAX_BUFFER_LEN            ((size_t)1024)
    #define      D_IO_MAX_LOCK_COUNT            ((size_t)1024)
    #define      D_IO_INVALID_HANDLE            ((int)-1)

    #define      D_FRAMES_PER_METER             (52.80)
    #define      D_FRAMES_PER_FEET              (16.093)


D_C_DECL_END



/***********************************/
/*  PROTOCOL HANDLING DEFINITIONS  */
/***********************************/

D_C_DECL_BEGIN

    #define      D_PROTOCOL_DEFAULT_SEND_BUFFER_SIZE ((size_t)32)
    #define      D_PROTOCOL_DEFAULT_RECV_BUFFER_SIZE ((size_t)32)

D_C_DECL_END



/*********************/
/*  RPC DEFINITIONS  */
/*********************/

D_C_DECL_BEGIN

    #define      D_RPC_DEFAULT_SEND_BUFFER_SIZE  ((size_t)32768)
    #define      D_RPC_DEFAULT_RECV_BUFFER_SIZE  ((size_t)32768)
    #define      D_RPC_UNKNOWN_CAPABILITY        (0xffffffff)

D_C_DECL_END



/**********************************/
/*  SIGNAL AND EVENT DEFINITIONS  */
/**********************************/

D_C_DECL_BEGIN

    enum  SigType
    {
        D_SIG_NONE          = 0x0000,
        D_SIG_HUP           = 0x0001,
        D_SIG_INT           = 0x0002,
        D_SIG_QUIT          = 0x0004,
        D_SIG_TERM          = 0x0008,
        D_SIG_USR1          = 0x0010,
        D_SIG_USR2          = 0x0020,
        D_SIG_PIPE          = 0x0040,
        D_SIG_ALRM          = 0x0080,
        D_SIG_CHLD          = 0x0100,

        D_SIG_ALL           = 0x01ff
    };


    #define   D_SIGNAL_EVENT_RING_SIZE  ((size_t)100)

D_C_DECL_END



/****************************************************************************/
/*  P R O J E C T   E N V I R O N M E N T   V A R I A B L E S               */
/****************************************************************************/

#define D_ENV_DNS_ENABLED       "CV_DNS_ENABLED"
#define D_ENV_IPC_NAMEDIR       "CV_IPC_NAME_DIR"



/****************************************************************************/
/*  G E N E R A L   C O N S T A N T S                                       */
/****************************************************************************/

#if !defined( MAXNAMLEN )
#   define  MAXNAMLEN           255
#endif



/********************************** EOF *************************************/

#endif  /* DTYPES_H */
