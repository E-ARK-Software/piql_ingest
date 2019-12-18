#ifndef     DCONFIG_H
#define     DCONFIG_H

/*****************************************************************************
**  $Id: dconfig.h,v 1.1 2004/03/22 18:11:32 swhbt Exp $
**
**  DLFR global platform specific definitions
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


/****************************************************************************/
/*  O P E R A T I N G   S Y S T E M   M A C R O S                           */
/****************************************************************************/

/***********/
/*  LINUX  */
/***********/

#if defined( linux ) || defined( __linux ) || defined( __linux__ )
#   define  D_OS_LINUX
#   define  D_OS_UNIX
#   define  _FILE_OFFSET_BITS 64
#   if __x86_64__ || __ppc64__
#      define D_OS_LINUX64
#   endif
/**********/
/*  ECOS  */
/**********/
#elif defined( __ECOS )
#define D_OS_ECOS


/***********/
/*  WIN32  */
/***********/

#elif defined( _WIN32 )
#   define  D_OS_WIN32
#   if defined( WIN64 )
#       define  D_OS_WIN64
#   endif

/*******************/
/*  CLANG64 - MAC  */
/*******************/

#elif defined( __APPLE_CC__ )
#   define  D_OS_LINUX
#   define  D_OS_UNIX
#   define  D_OS_X
#   define  _FILE_OFFSET_BITS 64

/********************/
/*  UNSUPPORTED OS  */
/********************/

#else
#   error   "*** UNSUPPORTED OS ***"
#endif  /* D_OS_xxx */



/****************************************************************************/
/*  S U P P O R T E D   C O M P I L E R                                     */
/****************************************************************************/

#if defined( __GNUC__ )
#   define  D_CC_GNU
#   if (__GNUC__ > 2) || (__GNUC__ == 2 && (__GNUC_MINOR__ > 6))
#       define  D_CC_ANSI
#   else
#       define  D_CC_STD
#   endif
#endif


#ifdef  __cplusplus
#   ifndef  D_CPLUSPLUS
#       define  D_CPLUSPLUS
#   endif
#else
#   undef   D_CPLUSPLUS
#endif



/****************************************************************************/
/*  V A R I O U S   M A C R O S                                             */
/****************************************************************************/

/******************/
/*  C PROTOTYPES  */
/******************/

#ifdef  D_CPLUSPLUS
#   define  D_C_FUNC            extern "C"
#   define  D_C_DECL_BEGIN      extern "C" {
#   define  D_C_DECL_END        }
#else
#   define  D_C_FUNC            extern
#   define  D_C_DECL_BEGIN
#   define  D_C_DECL_END
#endif


/***********************/
/*  NAMESPACE SUPPORT  */
/***********************/

#ifdef  __cplusplus
#define  D_NAMESPACE_BEGIN(NS)              namespace NS {
#define  D_NAMESPACE_END                    }
#define  D_NAMESPACE_USING(NS)              using namespace NS;
#define  D_NAMESPACE_USE(NS)                using NS;
#define  D_NAMESPACE_FWD(NS,CLS)            namespace NS { class CLS; }
#define  D_NAMESPACE_FWD2(NS1,NS2,CLS)      namespace NS1 { namespace NS2 { class CLS; } }
#define  D_NAMESPACE_FWDSTRUCT(NS,S)        namespace NS { struct S; }
#define  D_NAMESPACE_RENAME(NEW_NS,OLD_NS)  namespace NEW_NS = OLD_NS;
#define  D_NAMESPACE_FWDTEMPLATE(NS,CLS)    namespace NS { CLS; }
#else
#define  D_NAMESPACE_BEGIN(NS)    
#define  D_NAMESPACE_END          
#define  D_NAMESPACE_USING(NS)    
#define  D_NAMESPACE_USE(NS)      
#define  D_NAMESPACE_FWD(NS,CLS)
#define  D_NAMESPACE_FWDSTRUCT(NS,S)
#define  D_NAMESPACE_RENAME(NEW_NS,OLD_NS)
#define  D_NAMESPACE_FWDTEMPLATE(NS,CLS)
#endif

/*******************************/
/*  RETURN REFERENCE TO VALUE  */
/*******************************/

#ifdef  __cplusplus
template< class T >
T&  REF( T param )
{
    static T  temp;
    temp = param;

    return  temp;
}
#endif


/****************************************************************************/
/*  G E N E R A L   C O N S T A N T S                                       */
/****************************************************************************/

#if !defined( MAXNAMLEN )
#   define  MAXNAMLEN           255
#endif



/********************************** EOF *************************************/

#endif  /* DCONFIG_H */
