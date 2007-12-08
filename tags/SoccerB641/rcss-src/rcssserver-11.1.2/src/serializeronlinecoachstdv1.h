// -*-c++-*-

/***************************************************************************
                            serializeronlinecoachstdv1.h
               Class for serializing data to std v1 online coaches
                             -------------------
    begin                : 27-MAY-2002
    copyright            : (C) 2002 by The RoboCup Soccer Server
                           Maintenance Group.
    email                : sserver-admin@lists.sourceforge.net
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef SERIALIZERONLINECOACHSTDV1_H
#define SERIALIZERONLINECOACHSTDV1_H

#include "serializer.h"

namespace rcss
{
class SerializerOnlineCoachStdv1
    : public SerializerOnlineCoach
{
protected:
    SerializerOnlineCoachStdv1( const SerializerCommon& common );

public:
    virtual
    ~SerializerOnlineCoachStdv1();

    static
    const SerializerOnlineCoachStdv1*
    instance();

    virtual
    void
    serializeRefAudio( std::ostream& strm,
                       const int& time,
                       const std::string& name,
                       const char* msg ) const;

    virtual
    void
    serializePlayerAudio( std::ostream& strm,
                          const int& time,
                          const std::string& name,
                          const char* msg ) const;

    virtual
    void
    serializePlayerClangVer( std::ostream& strm,
                             const std::string& name,
                             const unsigned int& min,
                             const unsigned int& max ) const;

    virtual
    void
    serializeInit( std::ostream&,
                   int side = 0 ) const;

    virtual
    void
    serializeChangedPlayer( std::ostream &,
                            int unum,
                            int type = -1 ) const;


};
}

#endif
