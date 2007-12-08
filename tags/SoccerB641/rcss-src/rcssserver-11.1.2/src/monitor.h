// -*-c++-*-

/***************************************************************************
                                 monitor.h
    A class providing the communication interface for remote monitors that
    connect to the server
                             -------------------
    begin                : 27-DEC-2001
    copyright            : (C) 2001 by The RoboCup Soccer Server
                           Maintainance Group.
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

#ifndef _MONITOR_H_
#define _MONITOR_H_

#include "remoteclient.h"
#include "types.h"
#include <netinet/in.h>

class Stadium;

class Monitor
    : public RemoteClient
{
protected:
    Stadium & M_stadium;
    double M_version;
public:
    Monitor ( Stadium & stadium,
              const double & version );
    ~Monitor ();

    void
    parseMsg( const char* msg, const size_t& len )
      {
          char* str = (char*)msg;
          if( str[ len - 1 ] != 0 )
          {
              if( M_version >= 3.0 )
                  send( "(warning message_not_null_terminated)" );
              str[ len ] = 0;
          }
          parseCommand( str );
      }

    void
    disable()
      {
          RemoteClient::close();
      }

    bool parseCommand( const char* message );


    int send( const char* msg )
      {
          if ( getVersion() == 1.0 )
          {
              dispinfo_t minfo;
              minfo.mode = htons( MSG_MODE );
              minfo.body.msg.board = htons(MSG_BOARD);
              std::strncpy( minfo.body.msg.message, msg, max_message_length_for_display );
              return RemoteClient::send( reinterpret_cast< const char * >( &minfo ),
                                         sizeof( dispinfo_t ) );
          }
          else
          {
              dispinfo_t2 minfo;
              minfo.mode = htons( MSG_MODE );
              minfo.body.msg.board = htons( MSG_BOARD );
              std::strncpy( minfo.body.msg.message, msg, max_message_length_for_display );
              return RemoteClient::send( reinterpret_cast< char * >( &minfo ),
                                         sizeof( dispinfo_t2 ) );
          }
      }

    double getVersion() const
      {
          return M_version;
      }
};


#endif
