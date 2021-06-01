#ifndef DTCLAP_H
#define DTCLAP_H

/*****************************************************************************
**
**  Tclap type definitions
**
**  Creation date:  2013/09/10
**  Created by:     Haakon Larsson
**
**
**  Copyright (c) 2013 Piql AS. All rights reserved.
**
**  This file is part of the DLFR common library
**
*****************************************************************************/

//  INCLUDES
//
#include "dcommon.h"

// TCLAP_NAMESTARTSTRING added for backward compatibility 
#define TCLAP_NAMESTARTSTRING "-" 
#include "tclap/CmdLine.h"
#include <memory>

//  NAMESPACE
//
D_NAMESPACE_BEGIN( D_NAMESPACE )


//  TYPEDEFS
//

typedef TCLAP::ValueArg<std::string> DStringArg;
typedef std::auto_ptr< DStringArg > DStringArgP;
typedef TCLAP::SwitchArg DSwitchArg;
typedef std::auto_ptr< DSwitchArg > DSwitchArgP;
typedef TCLAP::ValueArg<unsigned int> DUIntArg;
typedef std::auto_ptr< DUIntArg > DUIntArgP;


//============================================================================
//  CLASS:  DTclapStdOutput

class DTclapStdOutput : public TCLAP::StdOutput 
{
public:
    inline virtual void version( TCLAP::CmdLineInterface& c )
    {
        std::cout << c.getProgramName() << " " << c.getVersion() << std::endl;
    };
    
};

//============================================================================
//  CLASS:  DTclapRangeConstraint

template <class T>
class DTclapRangeConstraint : public TCLAP::Constraint< T >
{
public:
    DTclapRangeConstraint(T min, T max, std::string description = "", std::string shortId = "");
    virtual     ~DTclapRangeConstraint ();
    virtual std::string     description () const;
    virtual std::string     shortID () const;
    virtual bool     check (const T & value) const;
    void setIncludeMin(bool include);
    void setIncludeMax(bool include);
    void setIgnoreMin(bool include);
    void setIgnoreMax(bool include);
private:
    T           m_min;
    T           m_max;
    bool        m_includeMin;
    bool        m_includeMax;
    bool        m_ignoreMin;
    bool        m_ignoreMax;
    std::string m_description;
    std::string m_shortId;
};

template <class T>
DTclapRangeConstraint<T>::DTclapRangeConstraint(T min, T max, std::string description, std::string shortId) 
    : m_min(min), m_max(max), 
      m_includeMin(true), m_includeMax(true),
      m_ignoreMin(false), m_ignoreMax(false),
      m_description(description), m_shortId(shortId)
{
}

template <class T>
DTclapRangeConstraint<T>::~DTclapRangeConstraint () 
{
}

template <class T>
std::string DTclapRangeConstraint<T>::description () const 
{ 
    return m_description; 
}

template <class T>
std::string DTclapRangeConstraint<T>::shortID () const 
{ 
    return m_shortId; 
}

template <class T>
bool DTclapRangeConstraint<T>::check (const T & value) const 
{ 
    return (m_ignoreMin || (m_includeMin ? (value >= m_min) : (value > m_min))) && 
           (m_ignoreMax || (m_includeMax ? (value <= m_max) : (value < m_max))); 
}

template <class T>
void DTclapRangeConstraint<T>::setIncludeMin(bool include) 
{ 
    m_includeMin = include; 
}

template <class T>
void DTclapRangeConstraint<T>::setIncludeMax(bool include) 
{ 
    m_includeMax = include; 
}

template <class T>
void DTclapRangeConstraint<T>::setIgnoreMin(bool include) 
{ 
    m_ignoreMin = include; 
}

template <class T>
void DTclapRangeConstraint<T>::setIgnoreMax(bool include) 
{ 
    m_ignoreMax = include; 
}

D_NAMESPACE_END

#endif /* DTCLAP_H */

/********************************** EOF *************************************/

