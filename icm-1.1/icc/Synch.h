// -*- C++ -*-

//==========================================================================
/**
 *  @file    Synch.h
 *
 *  $Id: Synch.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *   Wrapper Facades for various synchronization mechanisms.
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 */
//==========================================================================

#ifndef ACE_SYNCH_H
#define ACE_SYNCH_H

#include /**/ "os/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if !defined (DO_NOT_INCLUDE_SYNCH_H)

/* All the classes have been moved out into their own headers as part of
   the compile-time and footprint reduction effort. */

//#include "Auto_Event.h"
//#include "Barrier.h"
#include "icc/ConditionThreadMutex.h"
#include "ConditionRecursiveThreadMutex.h"
//#include "Event.h"
#include "icc/Lock.h"
//#include "Manual_Event.h"
//#include "Mutex.h"
//#include "Null_Barrier.h"
#include "NullCondition.h"
#include "icc/NullMutex.h"
//#include "NullSemaphore.h"
//#include "RWMutex.h"
//#include "RW_Thread_Mutex.h"
#include "icc/RecursiveThreadMutex.h"
//#include "Semaphore.h"
#include "icc/ThreadMutex.h"
//#include "Thread_Semaphore.h"
//#include "TSS_Adapter.h"



#endif /* DO_NOT_INCLUDE_SYNCH_H */


#endif /* ACE_SYNCH_H */
