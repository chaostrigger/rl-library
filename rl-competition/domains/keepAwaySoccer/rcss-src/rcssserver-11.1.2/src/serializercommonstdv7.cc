// -*-c++-*-

/***************************************************************************
                            serializercommonstdv7.cc
                  Class for serializing data to std v7 clients
                             -------------------
    begin                : 24-JAN-2003
    copyright            : (C) 2003 by The RoboCup Soccer Server
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

#include "serializercommonstdv7.h"
#include "clangmsg.h"

namespace rcss
{

SerializerCommonStdv7::SerializerCommonStdv7()
{}

SerializerCommonStdv7::~SerializerCommonStdv7()
{}

const SerializerCommonStdv7&
SerializerCommonStdv7::instance()
{
    static SerializerCommonStdv7 ser;
    return ser;
}

void
SerializerCommonStdv7::serializeServerParamBegin( std::ostream& strm ) const
{
    strm << "(server_param";
}

void
SerializerCommonStdv7::serializeServerParamEnd( std::ostream& strm ) const
{
    strm << ")";
}

void
SerializerCommonStdv7::serializePlayerParamBegin( std::ostream& strm ) const
{
    strm << "(player_param";
}

void
SerializerCommonStdv7::serializePlayerParamEnd( std::ostream& strm ) const
{
    strm << ")";
}

void
SerializerCommonStdv7::serializePlayerTypeBegin( std::ostream& strm ) const
{
    strm << "(player_type";
}

void
SerializerCommonStdv7::serializePlayerTypeEnd( std::ostream& strm ) const
{
    strm << ")";
}

void
SerializerCommonStdv7::serializeParam( std::ostream& strm, int param ) const
{
    strm << " " << param;
}

void
SerializerCommonStdv7::serializeParam( std::ostream& strm, unsigned int param ) const
{
    strm << " " << param;
}

void
SerializerCommonStdv7::serializeParam( std::ostream& strm, bool param ) const
{
    strm << " " << param;
}

void
SerializerCommonStdv7::serializeParam( std::ostream& strm, double param ) const
{
    strm << " " << param;
}

void
SerializerCommonStdv7::serializeParam( std::ostream& strm, const std::string& param ) const
{
    strm << " " << param;
}

namespace
{
const SerializerCommon&
create()
{ return SerializerCommonStdv7::instance(); }

lib::RegHolder v7 = SerializerCommon::factory().autoReg( &create, 7 );
}
}
