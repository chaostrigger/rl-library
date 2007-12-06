// -*-c++-*-

/***************************************************************************
                            serializercoachstdv1.cc
               Class for serializing data to std v1 offline coaches
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

#include "serializercoachstdv1.h"
#include "clangmsg.h"
#include "param.h"

namespace rcss
{
SerializerCoachStdv1::SerializerCoachStdv1( const SerializerCommon& common )
    : SerializerCoach( common )
{}


SerializerCoachStdv1::~SerializerCoachStdv1()
{}

const SerializerCoachStdv1*
SerializerCoachStdv1::instance()
{
    rcss::SerializerCommon::Creator cre;
    if( !rcss::SerializerCommon::factory().getCreator( cre, 1 ) )
        return NULL;
    static SerializerCoachStdv1 ser( cre() );
    return &ser;
}

void
SerializerCoachStdv1::serializeRefAudio( std::ostream& strm,
                                         const int& time,
                                         const char* msg ) const
{
    strm << "(hear " << REFEREE_NAME << " " << time
         << " " << msg << ")";
}

void
SerializerCoachStdv1::serializeCoachAudio( std::ostream& strm,
                                           const int& time,
                                           const std::string& name,
                                           const char* msg ) const
{
    strm << "(hear " << name << " " << time
         << " " << msg << ")";
}

void
SerializerCoachStdv1::serializeCoachStdAudio( std::ostream& strm,
                                              const int& time,
                                              const std::string& name,
                                              const rcss::clang::Msg& msg ) const
{
    strm << "(hear " << name << " " << time
         << " " << msg << ")";
}

void
SerializerCoachStdv1::serializePlayerAudio( std::ostream& strm,
                                            const int& time,
                                            const std::string& name,
                                            const char* msg ) const
{
    strm << "(hear " << name << " " << time
         << " " << msg << ")";
}

void
SerializerCoachStdv1::serializeInit( std::ostream& strm ) const
{
    strm << "(init ok)";
}


namespace
{
const SerializerCoach*
create()
{ return SerializerCoachStdv1::instance(); }

lib::RegHolder v1 = SerializerCoach::factory().autoReg( &create, 1 );
lib::RegHolder v2 = SerializerCoach::factory().autoReg( &create, 2 );
lib::RegHolder v3 = SerializerCoach::factory().autoReg( &create, 3 );
lib::RegHolder v4 = SerializerCoach::factory().autoReg( &create, 4 );
lib::RegHolder v5 = SerializerCoach::factory().autoReg( &create, 5 );
lib::RegHolder v6 = SerializerCoach::factory().autoReg( &create, 6 );
}
}
