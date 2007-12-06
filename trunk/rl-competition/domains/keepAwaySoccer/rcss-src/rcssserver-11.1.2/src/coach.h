// -*-c++-*-

/***************************************************************************
                              coach.h
                  A classes for trainer and coaches.
                             -------------------
    begin                : 2007-04-18
    copyright            : (C) 20072 by The RoboCup Soccer Server
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

#ifndef RCSSSERVER_COACH_H
#define RCSSSERVER_COACH_H

#include "audio.h"
#include "field.h"
#include "initsender.h"
#include "remoteclient.h"

/*!
  \class Coach
  \brief trainer class
 */
class Coach
    : public RemoteClient,
      public rcss::Listener,
      public rcss::InitObserver< rcss::InitSenderOfflineCoach >
{
protected:
    char M_buffer[ MaxMesg ];

    Stadium * M_stadium;

    bool M_assignedp;
    bool M_eye;
    bool M_hear;
    Value M_version;

    Side M_side;
    bool M_done_received; //pfr:SYNCH

public:

    explicit
    Coach( Stadium *stad )
        : M_stadium( stad ),
          M_assignedp( false ),
          M_eye( false ),
          M_hear( false ),
          M_version( 0.0 ),
          M_side( NEUTRAL ),
          M_done_received( false )
      { }

    int parse_init( Stadium& stad, char *command,
                    const rcss::net::Addr& addr );
    bool assignedp() const
      {
          return M_assignedp;
      }

    bool isEyeOn() const
      {
          return M_eye;
      }
    bool isEarOn() const
      {
          return M_hear;
      }

    const
    Value & version() const
      {
          return M_version;
      }

    Side side() const
      {
          return M_side;
      }

    bool doneReceived() const
      {
          return M_done_received;
      }

    virtual
    void parse_command( const char *command );

    void look( Stadium& stad );
    void team_names( Stadium& stad );
    void recover( Stadium& stad );
    void send_visual_info();
    void check_ball( Stadium& stad );

    virtual void send( const char* msg );

    virtual
    void
    parseMsg( const char* msg, const size_t& len )
      {
          char* str = (char*)msg;
          if( str[ len - 1 ] != 0 )
          {
              if( version() >= 8.0 )
                  send( "(warning message_not_null_terminated)" );
              str[ len ] = 0;
          }
          M_stadium->writeTextLog(  *this, str, RECV );
          parse_command( str );
      }


    void disable();

    void resetCommandFlags()
      {
          M_done_received = false;
      }

    void sendExternalMsg();

protected:

    virtual
    bool setSenders( const Value & client_version );

};

class XPMHolder;

/*!
  \class OnlineCoach
  \brief online coach class
 */
class OnlineCoach
    :	public Coach,
      public rcss::InitObserver< rcss::InitSenderOnlineCoach >
{
private:
    //Angle angle;

    char M_message_buf[MaxMesg];

    int M_freeform_messages_said;
    int M_freeform_messages_allowed;

    std::deque< rcss::clang::Msg* > M_message_queue;

    int M_define_messages_left;
    int M_advice_messages_left;
    int M_info_messages_left;
    int M_meta_messages_left;
    int M_del_messages_left;
    int M_rule_messages_left;

    TheNumber M_msg_left_update_time;

    const rcss::SerializerOnlineCoach* M_serializer;

    std::string M_coach_name;

public:

    OnlineCoach( Stadium *stad );
    ~OnlineCoach();

    void disable();
    void setSide( Side side )
      {
          M_side = side;
      }

    void awardFreeformMessageCount();

    virtual
    void parse_command( const char *command );

    void say( char *message, bool standard = false );
    void say( const rcss::clang::Msg & message );

    /* this deques and says any messages which should come out
       returns the number of messages se nt out */
    int check_message_queue( TheNumber time );


    //returns whether it updated anything
    bool update_messages_left( TheNumber time );

    virtual
    void send( const char* msg )
      {
          if ( RemoteClient::send( msg, std::strlen( msg ) + 1 ) != -1 )
          {
              M_stadium->writeTextLog( *this, msg, SEND );
          }
          else
          {
              std::cerr << __FILE__ << ": " << __LINE__ << ": ";
              perror( "Error sending to online coach" );
          }
      }

    void sendPlayerClangVer();
    void sendPlayerClangVer( const Player & player );

    virtual
    void
    parseMsg( const char* msg, const size_t& len )
      {
          char* str = (char*)msg;
          if( str[ len - 1 ] != 0 )
          {
              if( version() >= 8.0 )
                  send( "(warning message_not_null_terminated)" );
              str[ len ] = 0;
          }
          M_stadium->writeTextLog( *this, str, RECV );
          parse_command( str );
      }

private:

    const rcss::SerializerOnlineCoach&
    setSerializer( const rcss::SerializerOnlineCoach& ser )
      { return *(M_serializer = &ser); }

    const rcss::SerializerOnlineCoach&
    getSerializer() const
      { return *M_serializer; }

public:
    void setName( const std::string & name )
      {
          M_coach_name = name;
      }

    const
    std::string & getName() const
      {
          return M_coach_name;
      }

    virtual
    bool setSenders( const Value & client_version );
};

#endif
