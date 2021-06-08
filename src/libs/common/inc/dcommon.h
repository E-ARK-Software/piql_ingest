#ifndef     DCOMMON_H
#define     DCOMMON_H

/*****************************************************************************
**  $Id: dcommon.h,v 1.1 2005-11-16 16:28:00 swhbt Exp $
**
**  DLFR common class global definitions
**
**  Creation date:  2004/03/22
**  Created by:     Han Timmerman
**
**  Target(s):      Linux 2.4.x / GCC 3.0
**
**  Namespace:      DLFR
**
**  Dependencies:   -
**  Restrictions:   -
**  ToDo:           -
**
**  Copyright (c) 2004 ANT Software Development. All rights reserved.
**
**  This file is part of the DLFR common library
**
*****************************************************************************/


/****************************************************************************/
/*  P L A T F O R M   S P E C I F I C   I N C L U D E S                     */
/****************************************************************************/

#if !defined( DCONFIG_H )
#   include  "dconfig.h"
#endif

#if !defined( DTYPES_H )
#   include  "dtypes.h"
#endif

/****************************************************************************/
/*  L I B R A R Y   N A M E S P A C E                                       */
/****************************************************************************/

#define     D_NAMESPACE             DLFR



/****************************************************************************/
/*  C O M M O N  N A M E S P A C E  D E F I N I T I O N                     */
/****************************************************************************/

D_NAMESPACE_BEGIN(D_NAMESPACE)
D_NAMESPACE_END    


/********************************** EOF *************************************/

#endif  /* DCOMMON_H */
