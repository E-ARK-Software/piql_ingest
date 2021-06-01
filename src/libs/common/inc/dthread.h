#ifndef     DTHREAD_H
#define     DTHREAD_H

/*****************************************************************************
**
**  Thread type definitions
**
**  Creation date:  2011/05/06
**  Created by:     Espen Iveland
**
**
**  Copyright (c) 2011 Piql AS. All rights reserved.
**
**  This file is part of the DLFR common library
**
*****************************************************************************/


#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition.hpp>

typedef boost::thread                              DThread;
typedef boost::thread_group                        DThreadGroup;
typedef boost::recursive_mutex                     DMutex;
typedef boost::shared_mutex                        DSharedMutex;
typedef boost::condition                           DCondition;
typedef boost::unique_lock<boost::recursive_mutex> DUniqueLock;
typedef boost::shared_lock<boost::shared_mutex>    DReadLock;
typedef boost::unique_lock<boost::shared_mutex>    DWriteLock;



/********************************** EOF *************************************/

#endif  /* DTHREAD_H */
