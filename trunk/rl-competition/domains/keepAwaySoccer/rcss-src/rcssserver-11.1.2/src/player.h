// -*-c++-*-

/***************************************************************************
                          player.h  -  A class for field players and goalies
                             -------------------
    begin                : 26-NOV-2001
    copyright            : (C) 2001, 2002 by The RoboCup Soccer Server
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


#ifndef RCSSSERVER_PLAYER_H
#define RCSSSERVER_PLAYER_H

#include "arm.h"
#include "audio.h"
#include "bodysender.h"
#include "bodysensor.h"
#include "field.h"
#include "fullstatesender.h"
#include "hetroplayer.h"
#include "initsender.h"
#include "object.h"
#include "pcombuilder.h"
#include "pcomparser.h"
#include "playerparam.h"
#include "random.h"
#include "remoteclient.h"
#include "serializer.h"
#include "serverparam.h"
#include "visual.h"

#include <string>

class Player
    : public MPObject,
      public RemoteClient,
      public rcss::Listener,
      public rcss::Observer< rcss::VisualSenderPlayer >,
      public rcss::BodyObserver< rcss::BodySenderPlayer >,
      public rcss::FullStateObserver,
      public rcss::InitObserver< rcss::InitSenderPlayer >,
      public rcss::pcom::Builder
{
private:

    Team * M_team;
    const ID M_unum;  /* uniform number */

    std::string M_name_far;
    std::string M_name_toofar;
    /* th 19.05.00 */
    std::string M_short_name_far;
    std::string M_short_name_toofar;

    Value M_stamina;
    Value M_recovery;
    Value M_effort;

    Value M_vis_angle;
public:
    const Value defangle;
    const Value vis_distance;
private:
    Value M_inertia_moment;

    Value M_version; //!< client protocol version
#ifdef NEW_QSTEP
    Value dist_qstep_player; /* each Player quantize step of distance */
    Value land_qstep_player; /* each Player quantize step of distance for landmark */
    Value dir_qstep_player; /* each Player quantize step of direction */
#endif

    Angle M_angle_body; //!< temporal body angle
    Angle M_angle_body_committed;
    Angle M_angle_neck; //!< temporal neck angle
    Angle M_angle_neck_committed;

    // pfr 8/14/00: for RC2000 evaluation
    Value M_kick_rand;

    int M_vis_send;
    //int sendcnt;
    bool M_highquality;

public:
    int alive;

private:
    bool M_command_done;
    bool M_turn_neck_done;
    bool M_done_received; //pfr:SYNCH

    int M_hear_capacity_from_teammate;
    int M_hear_capacity_from_opponent;

    bool M_goalie;
    int M_goalie_catch_ban;
    //Value M_goalie_catch_probability;
    int M_goalie_moves_since_catch;
    //PVector relative_ball_ang;

    int M_kick_count;
    int M_dash_count;
    int M_turn_count;
    int M_catch_count;
    int M_move_count;
    int M_turn_neck_count;
    int M_change_view_count;
    int M_say_count;

    bool M_offside_mark;
    PVector M_offside_pos;

    const HetroPlayer * M_player_type;
    int M_player_type_id;

    Arm M_arm;
    unsigned int M_tackle_cycles;
    unsigned int M_tackle_count;

    char M_buffer[ MaxMesg ];

    unsigned int M_clang_min_ver;
    unsigned int M_clang_max_ver;

    rcss::pcom::Parser M_parser;

private:
    // not used
    Player();
    const Player & operator=( const Player & );

public:
    Player( Team * team, Stadium *stad, ID number );
    ~Player();

    void init( Value version, int goalie_flag );

    /** This function is called in the begin of each cycle
     * and in case a player sends a sense_body command. */
    void sense_body();

    void setEnable();
    void disable();

    void send_visual_info();

    /* contributed by Artur Merke */
    void send_fullstate_information();

    /** Inline-Deklarations */
    inline void parseMsg( const char* msg, const size_t& len );

    inline void rotate( const Angle & a );
    inline void rotate_to( const Angle & a );

    inline Arm& getArm();
    inline const Arm& getArm() const;
    inline bool getArmDir( double & arm_dir ) const;
    inline unsigned int getTackleCycles() const;
    //inline void decrementTackleCycles();
    inline bool isTackling() const;
    inline unsigned int getTackleCount() const;
    inline unsigned int getClangMinVer() const;
    inline unsigned int getClangMaxVer() const;
    inline bool isGoalie() const;
    inline void setBackPasser();
    inline void setFreeKickFaulter();

    inline void send( const char* msg );

    inline void place( const PVector & location );
    void place( const PVector & pos,
                const Angle & angle,
                const PVector & vel,
                const PVector & accel )
      {
          M_pos = pos;
          M_angle_body_committed = angle;
          M_vel = vel;
          M_accel = accel;
      }

    const
    Team * team() const
      {
          return M_team;
      }

    int unum() const
      {
          return M_unum;
      }

    const
    std::string & nameFar() const
      {
          return M_name_far;
      }

    const
    std::string & nameTooFar() const
      {
          return M_name_toofar;
      }

    const
    std::string & shortNameFar() const
      {
          return M_short_name_far;
      }

    const
    std::string & shortNameTooFar() const
      {
          return M_short_name_toofar;
      }

    const
    Value & stamina() const
      {
          return M_stamina;
      }
    const
    Value & recovery() const
      {
          return M_recovery;
      }
    const
    Value & effort() const
      {
          return M_effort;
      }

    const
    Value & visibleAngle() const
      {
          return M_vis_angle;
      }

    const
    Value & version() const
      {
          return M_version;
      }

    double distQStep() const
      {
#ifndef NEW_QSTEP
          return ServerParam::instance().dist_qstep;
#else
          return dist_qstep_player;
#endif
      }

    double landDistQStep() const
      {
#ifndef NEW_QSTEP
          return ServerParam::instance().land_qstep;
#else
          return land_qstep_player;
#endif
      }

    double dirQStep() const
      {
#ifndef NEW_QSTEP
          return 0.1;
#else
          return dir_qstep_player;
#endif
      }

    const
    Angle & angleBodyCommitted() const
      {
          return M_angle_body_committed;
      }

    const
    Angle & angleNeckCommitted() const
      {
          return M_angle_neck_committed;
      }

    int visSend() const
      {
          return M_vis_send;
      }
    bool highquality() const
      {
          return M_highquality;
      }

    bool doneReceived() const
      {
          return M_done_received;
      }

    void decrementHearCapacity( const Player & sender )
      {
          if ( team() == sender.team() )
          {
              M_hear_capacity_from_teammate
                  -= ServerParam::instance().hearDecay();
          }
          else
          {
              M_hear_capacity_from_opponent
                  -= ServerParam::instance().hearDecay();
          }
      }
    bool canHearFullFrom( const Player & sender ) const
      {
          if ( team() == sender.team() )
          {
              return M_hear_capacity_from_teammate
                  >= (int)ServerParam::instance().hearDecay();
          }
          else
          {
              return M_hear_capacity_from_opponent
                  >= (int)ServerParam::instance().hearDecay();
          }
      }

    int kickCount() const { return M_kick_count; }
    int dashCount() const { return M_dash_count; }
    int turnCount() const { return M_turn_count; }
    int catchCount() const { return M_catch_count; }
    int moveCount() const { return M_move_count; }
    int turnNeckCount() const { return M_turn_neck_count; }
    int changeViewCount() const { return M_change_view_count; }
    int sayCount() const { return M_say_count; }

    Angle vangle( const PObject & obj ) const
      {
          return M_pos.vangle( obj.pos(), M_angle_body_committed );
      }

    bool hasOffsideMark() const
      {
          return M_offside_mark;
      }
    const
    PVector & offsidePos() const
      {
          return M_offside_pos;
      }

    void setPlayerType( const int );
    //    int getPlayerType() const;
    void substitute( const int& );
    const
    HetroPlayer * playerType() const
      {
          return M_player_type;
      }
    int playerTypeId() const
      {
          return M_player_type_id;
      }


    bool setSenders();

    void recoverAll();
    void updateStamina();
    void updateCapacity();

    void resetCommandFlags();

    void clearOffsideMark();
    void setOffsideMark( const double & offside_line );

protected:

    virtual
    void turnImpl()
      {
          M_angle_body_committed = this->M_angle_body;
          M_angle_neck_committed = this->M_angle_neck;
          M_accel.assign( 0.0, 0.0 );
      }

    virtual
    void updateAngle()
      {
          M_angle_body_committed = this->M_angle_body;
          M_angle_neck_committed = this->M_angle_neck;
      }

    virtual
    double maxAccel() const
      {
          if ( pos().y < 0.0 )
          {
              return M_max_accel * ( M_team->side() == LEFT
                                     ? ServerParam::instance().slowness_on_top_for_left_team
                                     : ServerParam::instance().slowness_on_top_for_right_team );
          }
          return M_max_accel;
      }

    virtual
    double maxSpeed() const
      {
          if ( pos().y < 0.0 )
          {
              return M_max_speed * ( M_team->side() == LEFT
                                     ? ServerParam::instance().slowness_on_top_for_left_team
                                     : ServerParam::instance().slowness_on_top_for_right_team );
          }
          return M_max_speed;
      }

private:
    /** PlayerCommands */
    void dash( double power );
    void turn( double moment );
    void turn_neck( double moment );
    void kick( double power, double dir );
    void goalieCatch( double dir );
    void say( std::string message );
    void score();
    void move( double x, double y );
    void change_view( rcss::pcom::VIEW_WIDTH viewWidth, rcss::pcom::VIEW_QUALITY viewQuality );
    void compression( int level );
    void bye();
    void done();
    void pointto( bool on, double dist, double head );
    void attentionto( bool on, rcss::pcom::TEAM team_side, std::string team_name, int at_unum );
    void tackle( double power );
    void clang( int min, int max);
    void ear( bool on, rcss::pcom::TEAM team_side, std::string team_name, rcss::pcom::EAR_MODE mode );

};


/** Inline-Definitions */

inline
void
Player::parseMsg( const char* msg, const size_t& len )
{
    char* command = const_cast<char*>(msg);
    if ( command[ len - 1 ] != 0 ) {
        if( version() >= 8.0 ){
            send( "(warning message_not_null_terminated)" );
        }
        command[ len ] = 0;
    }
    M_stadium->writeTextLog( *this, command, RECV );

    /** Call the PlayerCommandParser */
    if ( M_parser.parse( command ) != 0 ){
        send( "(error illegal_command_form)" );
        std::cerr << "Error parsing >" << command << "<\n";
    }
}

inline
void
Player::rotate( const Angle & a )
{
    M_angle_body = normalize_angle( M_angle_body_committed + a );
}

inline
void
Player::rotate_to( const Angle & dir )
{
    M_angle_body = normalize_angle( dir );
}

inline
Arm &
Player::getArm()
{
    return M_arm;
}

inline
const
Arm &
Player::getArm() const
{
    return M_arm;
}

inline
bool
Player::getArmDir( double& arm_dir ) const
{
    return M_arm.getRelDir ( rcss::geom::Vector2D( pos().x, pos().y ),
                             angleBodyCommitted()+ angleNeckCommitted(),
                             arm_dir );
}

inline
unsigned int
Player::getTackleCycles() const
{
    return M_tackle_cycles;
}

// inline
// void
// Player::decrementTackleCycles()
// {
//     if( M_tackle_cycles > 0 )
//         M_tackle_cycles--;
// }

inline
bool
Player::isTackling() const
{
    return M_tackle_cycles > 0;
}

inline
unsigned int
Player::getTackleCount() const
{
    return M_tackle_count;
}

inline
unsigned int
Player::getClangMinVer() const
{
    return M_clang_min_ver;
}

inline
unsigned int
Player::getClangMaxVer() const
{
    return M_clang_max_ver;
}

inline
bool
Player::isGoalie() const
{
    return M_goalie;
}

inline
void
Player::setBackPasser()
{
    alive |= BACK_PASS;
}

inline
void
Player::setFreeKickFaulter()
{
    alive |= FREE_KICK_FAULT;
}

inline
void
Player::send( const char* msg )
{
    if ( RemoteClient::send( msg, std::strlen( msg ) + 1 ) != -1 )
    {
        M_stadium->writeTextLog( *this, msg, SEND );
    }
}

inline
void
Player::place( const PVector & location )
{
    M_accel = PVector( 0.0, 0.0 );
    M_vel = PVector( 0.0, 0.0 );
    M_pos = location;
}

#endif
