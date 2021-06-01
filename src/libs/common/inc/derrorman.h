#ifndef     DERRORMAN_H
#define     DERRORMAN_H

/*****************************************************************************
**  $Id$
**
**  Definition of the Piql DErrorMan class
**
**  Creation date:  2004-04-05
**  Created by:     Han Timmerman
**
**  Platform(s):    SGI, Irix 6.5.x, MipsPro 7.3
**                  Intel, Linux 2.4.x, g++ 3.0
**                  Intel, Windows 2K/XP, VC++ 6.0
**
**  Dependencies:   -
**  Restrictions:   -
**  ToDo:           -
**
**  Copyright (c) 2004 ANT Software Development. All rights reserved.
**
**  This file is part of the "cvbase" library
**
*****************************************************************************/

//  PROJECT INCLUDES
//
#include    "dcommon.h"
#include    "derror.h"

//  SYSTEM INCLUDES
//
#include    <iosfwd>
#include    <string>
#include    <vector>

//  NAMESPACE
//
D_NAMESPACE_BEGIN( D_NAMESPACE )


//============================================================================
//  CLASS:  DErrorMan

class  DErrorMan
{

public:

    class FilterItem
    {
    public:
        FilterItem() {};
        FilterItem( const std::string& str, const DError::Type& level ) : m_String( str ), m_Level( level ) {};

        std::string  m_String;
        DError::Type m_Level;
    };


    typedef std::vector<FilterItem> FilterVector;

public:

    DErrorMan( const std::string& mod="" );
    virtual ~DErrorMan();

    operator bool ();

    void                setOn();
    void                setOff();
    void                setLogLevel( const DError::Type& logLevel );
    DError::Type        logLevel() const;
    void                addExcludeFilter( const FilterItem& item );
    void                addIncludeFilter( const FilterItem& item );
    void                clearExcludeFilter();
    void                clearIncludeFilter();
    bool                shouldPrint( const DError& error ) const;

    void                setModuleName( const std::string& pname );
    bool                setErrorStream( std::ostream& estrm );
    bool                setLogStream( const std::string& fname );
    void                setLogEnabled( bool state );
    void                clearLogStream();
    bool                includeTimeStamp();
    bool                loggingToFile() const;
    
    void                printMessage( const std::string& message );
    
    const std::string&  moduleName() const;

    void                enableIndenting( bool enable );
    bool                isIndentingOn() const;
    void                pushIndentLevel();
    void                popIndentLevel();
    const std::string&  indentLevel() const;

    void                enableEntryExitLogging( bool enable );
    bool                isEntryExitLogging() const;

public:

    static DErrorMan*   Instance();

private:
    std::ostream&       stream();

private:

    bool                m_ErrOn;
    bool                m_LogOn;
    std::ostream*       m_DefaultStream;
    std::ostream*       m_ErrStream;
    std::ostream*       m_LogStream;
    std::string         m_ModuleName;
    std::string         m_LogFile;
    DError::Type        m_LogLevel;
    FilterVector        m_IncludeFilter;
    FilterVector        m_ExcludeFilter; 
    bool                m_IndentingOn;
    std::string         m_CurrentIndent;
    bool                m_EntryExitLogging;

private:    // DISABLE

    DErrorMan( const DErrorMan& );
    DErrorMan&  operator = ( const DErrorMan& );

};


//----------------------------------------------------------------------------
//  INLINE: DErrorMan

inline  DErrorMan::operator bool()
{
    return  m_ErrOn;
}


inline void  DErrorMan::setOn()
{
    m_ErrOn = true;
}


inline void  DErrorMan::setOff()
{
    m_ErrOn = false;
}


inline void  DErrorMan::enableIndenting( bool enable )
{
    m_IndentingOn = enable;
}


inline bool DErrorMan::isIndentingOn() const
{
    return m_IndentingOn;
}

    
inline const std::string& DErrorMan::indentLevel() const
{
    return m_CurrentIndent;
}


inline void  DErrorMan::setLogLevel( const DError::Type& logLevel )
{
    m_LogLevel = logLevel;
}


inline DError::Type  DErrorMan::logLevel() const
{
    return m_LogLevel;
}


inline std::ostream&  DErrorMan::stream()
{
    return  *m_ErrStream;
}


inline const std::string&  DErrorMan::moduleName() const
{
    return  m_ModuleName;
}


inline void  DErrorMan::enableEntryExitLogging( bool enable )
{
    m_EntryExitLogging = enable;
}

inline bool  DErrorMan::isEntryExitLogging() const
{
    return m_EntryExitLogging;
}


//============================================================================

D_NAMESPACE_END


//
//===================================EOF======================================

#endif  /* DERRORMAN_H */
