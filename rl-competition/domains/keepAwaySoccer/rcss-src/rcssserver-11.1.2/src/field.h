// -*-c++-*-
/*
 *Header:
 *File: field.h (for C++)
 *Author: Noda Itsuki
 *Date: 1996/02/16
 *EndHeader:
 */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000, 2001 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler

 This file is a part of SoccerServer.

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */


#ifndef RCSSSERVER_FIELD_H
#define RCSSSERVER_FIELD_H

#include "timeable.h"

#include "object.h"
#include "resultsaver.hpp"
#include "serverparam.h"

#include <rcssbase/gzip/gzfstream.hpp>

#include <string>
#include <fstream>
#include <deque>
#include <vector>
#include <algorithm>
#include <memory>
#include <cstdio>
#include <sys/time.h>

class HetroPlayer;

class Monitor;
class Coach;
class OnlineCoach;

class Referee;

namespace rcss
{
class Listener;
namespace clang
{
class Msg;
}
}

class Field {
private:
    const Field & operator=( const Field & );
public:
    const PObject line_l;
    const PObject line_r;
    const PObject line_t;
    const PObject line_b;

    Field();
    ~Field() {}
} ;

class Weather {
private:
    const Weather & operator=( const Weather & );
public:
    PVector	wind_vector;
    Value	wind_rand;

    void	init();
} ;


/*
 *===================================================================
 *Part: Stadium Class
 *===================================================================
 */
class Stadium : public virtual Timeable
{
public:
    // These are vectors and not lists, because we need to shuffle them
    typedef std::vector< OnlineCoach * > OnlineCoachCont;
    typedef std::vector< Player * >  PlayerCont;
    typedef std::vector< Monitor * > MonitorCont;
    typedef std::vector< Coach * >  OfflineCoachCont;
    typedef std::vector< rcss::Listener * > ListenerCont;

protected:
    // definitions of different timeable methods
    void doRecvFromClients( );
    void doNewSimulatorStep();
    void doSendSenseBody();
    void doSendVisuals();
    void doSendCoachMessages();
    bool doSendThink();
    void doQuit();

    static const std::string DEF_TEXT_NAME;
    static const std::string DEF_TEXT_SUFFIX;
    static const std::string DEF_GAME_NAME;
    static const std::string DEF_GAME_SUFFIX;
    static const std::string DEF_KAWAY_NAME;
    static const std::string DEF_KAWAY_SUFFIX;

protected:
    bool M_alive;

    std::vector< PObject * > M_landmarks;

    rcss::net::UDPSocket M_player_socket;
    rcss::net::UDPSocket M_online_coach_socket;
    rcss::net::UDPSocket M_offline_coach_socket;

    OnlineCoachCont M_remote_online_coaches;
    PlayerCont  M_remote_players; //!< connected player container
    MonitorCont M_monitors;
    OfflineCoachCont M_remote_offline_coaches;
    ListenerCont M_listeners;

public:

    const Field field;

private:

    Ball * M_ball;
    //Player * M_players[MAX_PLAYER*2];
    PlayerCont M_players; //!< player instance container
    Coach * M_coach;
    //OnlineCoach * M_olcoaches[2];
    OnlineCoachCont M_olcoaches;
    Team * M_team_l;
    Team * M_team_r;

    Weather	M_weather;

    MPObjectTable M_motable; /* Movable objects */
    //PObjectTable votable;	/* Visible objects */

    PlayMode M_playmode;

    TheNumber M_time;

    std::vector< HetroPlayer * > M_player_types;


    const Player * M_ball_catcher; /* goalie who has caught ball */

    Side M_kick_off_side;

    dispinfo_t M_dinfo;
    dispinfo_t2 M_dinfo2;
    dispinfo_t M_minfo;

    std::string M_text_log_name;
    std::string M_game_log_name;
    std::string M_kaway_log_name;

    rcss::gz::gzofstream M_gz_text_log;
    rcss::gz::gzofstream M_gz_game_log;
    std::ofstream M_text_log; /* file for command log */
    std::ofstream M_game_log;   /* file for recording the game */
    std::ofstream M_kaway_log;  /* file for keepaway log */

    std::list< Referee * > M_referees;

    unsigned int M_last_playon_start;

    int M_kick_off_wait;
    int M_connect_wait;
    int M_game_over_wait;

    int M_left_child;
    int M_right_child;

    tm m_real_time;

    std::list< rcss::ResultSaver::Ptr > m_savers;

public:

    Stadium();

    virtual
    ~Stadium();

    void init();

    void finalize( const std::string & msg );

    virtual
    bool isAlive()
      {
          return M_alive;
      }

    PlayMode playmode() const
      {
          return M_playmode;
      }

    TheNumber time() const
      {
          return M_time;
      }

    const
    Weather & weather() const
      {
          return M_weather;
      }

    bool text_log_open()
      {
          return ( M_text_log.is_open()
                   || M_gz_text_log.is_open() );
      }

    std::ostream& text_log_stream()
      {
          if ( ServerParam::instance().textLogCompression () > 0 )
              return M_gz_text_log;
          else
              return M_text_log;
      }

    std::ostream & kawayLog()
      {
          return M_kaway_log;
      }

    bool game_log_open()
      {
          return ( M_game_log.is_open ()
                   || M_gz_game_log.is_open () );
      }

    void
    flushLogs()
      {
          M_text_log.flush();
          M_game_log.flush();
          M_kaway_log.flush();
          M_gz_text_log.flush();
          M_gz_game_log.flush();
      }

    const
    Ball & ball() const
      {
          return *M_ball;
      }

    const
    Team & teamLeft() const
      {
          return *M_team_l;
      }

    const
    Team & teamRight() const
      {
          return *M_team_r;
      }

    const
    std::vector< PObject * > & landmarks() const
      {
          return M_landmarks;
      }

    OnlineCoachCont & onlineCoaches()
      {
          return  M_remote_online_coaches;
      }

    PlayerCont & remotePlayers()
      {
          return M_remote_players;
      }
    PlayerCont & players()
      {
          return M_players;
      }
    const
    PlayerCont & players() const
      {
          return M_players;
      }

    MonitorCont & monitors()
      {
          return M_monitors;
      }
    OfflineCoachCont & offlineCoaches()
      {
          return  M_remote_offline_coaches;
      }
    ListenerCont& listeners()
      {
          return  M_listeners;
      }

    void removeListener( const rcss::Listener* listener )
      {
          M_listeners.erase( remove( M_listeners.begin(),
                                     M_listeners.end(),
                                     listener ),
                             M_listeners.end() );
      }
    void addListener( rcss::Listener* listener )
      {
          M_listeners.push_back( listener );
      }

    void addOfflineCoach( Coach* coach )
      {
          M_remote_offline_coaches.push_back( coach );
      }

    void addLandmark( PObject * new_obj );

    const
    HetroPlayer * playerType( int id ) const;


    const
    Player * ballCatcher() const
      {
          return M_ball_catcher;
      }

    void clearBallCatcher()
      {
          M_ball_catcher = NULL;
      }

    Side kickOffSide() const
      {
          return M_kick_off_side;
      }

private:
    void initObjects();

    void openTextLog();
    void openGameLog();
    void openKawayLog();

    template < class T >
    void recv( std::vector< T >& clients );

    void udp_recv_message();
    void udp_recv_from_coach();
    void udp_recv_from_online_coach();

    void parsePlayerInit( const char* message,
                          const rcss::net::Addr& cli_addr );
    bool parseMonitorInit( const char* message,
                           const rcss::net::Addr& cli_addr );
    void parseOnlineCoachInit( const char* message,
                               const rcss::net::Addr& cli_addr );

    Player * newPlayer( const rcss::net::Addr &,
                        const char* init_message );
    Player * newPlayer( const char * teamname,
                        Value version,
                        int goalie_flag,
                        const rcss::net::Addr& );
    Player * reconnectPlayer( const rcss::net::Addr&,
                             const char* init_message );
    OnlineCoach * newCoach( const rcss::net::Addr&, const char* );

    void removeDisconnectedClients();

    Player * get_player_by_name( const char * name );

    void step();
    void makeDispInfo();
    void resetCommandFlags();
    void resetPlayerFlags();

    void move_caught_ball(); // [2000.07.21: I.Noda]

public:
    void referee_get_foul( const double & x, const double & y,
                           const Side side );
    void referee_drop_ball( const double & x, const double & y,
                            const Side side );
    void discard_player( const Side side,
                         const int unum );

    void score( const Side side );
    void penaltyScore( const Side side,
                       const bool scored );
    void penaltyWinner( const Side side );

    void setHalfTime( const Side kick_off_side,
                      const int half_time_count );

    void set_ball( const Side kick_off_side,
                   const PVector & pos,
                   const PVector & vel = PVector(0.0,0.0) );
    /*!
      The side parameter is redundant, but it save us from having to look it up.
      Later I would like to make playmodes objects that have side as a member.
    */
    void placeBall( const PlayMode pm,
                    const Side side,
                    PVector location );

    bool movePlayer( const Side side,
                     const int unum,
                     const PVector & pos,
                     const Angle * ang,
                     const PVector * vel );
    bool movePlayer( const char * obj_name,
                     const PVector & pos,
                     const PVector & vel = PVector( 0.0, 0.0 ) );
    bool movePlayer( const char * obj_name,
                     const PVector & pos,
                     const Angle & ang,
                     const PVector & vel = PVector( 0.0, 0.0 ) );

    void placePlayersInField();

    void recoveryPlayers();

    void change_play_mode( const PlayMode pm );

    void say( const char *message, bool ref = true );

private:
    //! diretcly send message to player client that has cli_addr
    void sendToPlayer( const char *msg,
                       const rcss::net::Addr& cli_addr );
    //! diretcly send message to online coach client that has cli_addr
    void sendToOnlineCoach( const char *msg,
                            const rcss::net::Addr& cli_addr );

    void sendRefAudio( const std::string & msg );
public:
    void sendPlayerAudio( const Player & player,
                          const std::string & msg );
    void sendCoachAudio( const Coach & coach,
                         const std::string & msg );
    void sendCoachStdAudio( const OnlineCoach &,
                            const rcss::clang::Msg & msg );

    void sendGraphic( Side side,
                      unsigned int x,
                      unsigned int y,
                      std::auto_ptr< XPMHolder > holder );


    BallPosInfo ballPosInfo();

    void writeTextLog( const char *message, int flag );
    void writeTextLog( Player & p, const char *message, int flag );
    void writeTextLog( Coach &, const char *message, int flag );
    void writeTextLog( OnlineCoach & p, const char *message, int flag );

private:
    std::ostream & writeGameLog( const char * str,
                                 const std::streamsize n );

    void writeGameLog( const dispinfo_t * msg );
    void writeGameLog( const dispinfo_t2 * msg );

    void write_times( timeval, timeval );
    void write_profile( timeval, timeval, char* );

public:
    void substitute( const Player * player,
                     const int player_type_id );

private:
    void broadcastSubstitution( const int side,
                                const int unum,
                                const int player_type );
public:

    //! \todo reimplement
    bool detectCollision( MPObject * a, MPObject * b );


    void collisions()
      {
          M_motable.collisions( playmode() );
      }

    bool canSendFreeform();

    void setPlayerState( const Side side,
                         const int unum,
                         const int state );

    void kickTaken( const Player & kicker,
                    const PVector & accel );

    void ballCaught( const Player & catcher );

private:

    const
    tm & realTime() const
      {
          return m_real_time;
      }

    void startTeams();

    int startTeam( const std::string& start );

    bool teamConnected( Side side );

    void killTeams();

    void saveResults();

    void disable();

    void renameLogs();

    void closeLogs();

public:
    static
    void _Start( Stadium& stad );

};

#endif
