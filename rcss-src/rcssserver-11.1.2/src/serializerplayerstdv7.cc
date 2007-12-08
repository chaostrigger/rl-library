// -*-c++-*-

/***************************************************************************
                            serializerplayerstdv7.cc
                  Class for serializing data to std v7 players
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

#include "serializerplayerstdv7.h"
#include "clangmsg.h"

namespace rcss
{

SerializerPlayerStdv7::SerializerPlayerStdv7( const SerializerCommon& common )
    : SerializerPlayerStdv1( common )
{}

SerializerPlayerStdv7::~SerializerPlayerStdv7()
{}

const SerializerPlayerStdv7*
SerializerPlayerStdv7::instance()
{
    rcss::SerializerCommon::Creator cre;
    if( !rcss::SerializerCommon::factory().getCreator( cre, 7 ) )
        return NULL;
    static SerializerPlayerStdv7 ser( cre() );
    return &ser;
}

void
SerializerPlayerStdv7::serializeCoachAudio( std::ostream& strm,
                                            const int& time,
                                            const std::string& name,
                                            const char* msg ) const
{
    strm << "(hear " << time << " "
         << name << " \"" << msg << "\")";
}

void
SerializerPlayerStdv7::serializeCoachStdAudio( std::ostream& strm,
                                               const int& time,
                                               const std::string& name,
                                               const rcss::clang::Msg& msg ) const
{
    strm << "(hear " << time << " "
         << name << " " << msg << ")";
}

void
SerializerPlayerStdv7::serializeSelfAudio( std::ostream& strm,
                                           const int& time,
                                           const char* msg ) const
{
    strm << "(hear " << time << " self \""
         << msg << "\")";
}

void
SerializerPlayerStdv7::serializePlayerAudio( std::ostream& strm,
                                             const int& time,
                                             const double& dir,
                                             const char* msg ) const
{
    strm << "(hear " << time << " "
         << dir << " \"" << msg << "\")";
}

void
SerializerPlayerStdv7::serializeServerParamBegin( std::ostream& strm ) const
{
    strm << "(server_param";
}

void
SerializerPlayerStdv7::serializeServerParamEnd( std::ostream& strm ) const
{
    strm << ")";
}

void
SerializerPlayerStdv7::serializePlayerParamBegin( std::ostream& strm ) const
{
    strm << "(player_param";
}

void
SerializerPlayerStdv7::serializePlayerParamEnd( std::ostream& strm ) const
{
    strm << ")";
}

void
SerializerPlayerStdv7::serializePlayerTypeBegin( std::ostream& strm ) const
{
    strm << "(player_type";
}

void
SerializerPlayerStdv7::serializePlayerTypeEnd( std::ostream& strm ) const
{
    strm << ")";
}

void
SerializerPlayerStdv7::serializeParam( std::ostream& strm, int param ) const
{
    strm << " " << param;
}

void
SerializerPlayerStdv7::serializeParam( std::ostream& strm, unsigned int param ) const
{
    strm << " " << param;
}

void
SerializerPlayerStdv7::serializeParam( std::ostream& strm, bool param ) const
{
    strm << " " << param;
}

void
SerializerPlayerStdv7::serializeParam( std::ostream& strm, double param ) const
{
    strm << " " << param;
}

void
SerializerPlayerStdv7::serializeParam( std::ostream& strm, const std::string& param ) const
{
    strm << " " << param;
}

void
SerializerPlayerStdv7::serializeChangePlayer( std::ostream& strm,
                                              int unum ) const
{
    strm << "(change_player_type "
         << unum << ")";
}

void
SerializerPlayerStdv7::serializeChangePlayer( std::ostream& strm,
                                              int unum,
                                              int type ) const
{
    strm << "(change_player_type "
         << unum << " " << type << ")";
}

namespace
{
const SerializerPlayer*
create()
{ return SerializerPlayerStdv7::instance(); }

lib::RegHolder v7 = SerializerPlayer::factory().autoReg( &create, 7 );
}
}
