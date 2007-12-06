/* -*- Mode: c++ -*- */

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "serverparam.h"

#include "playerparam.h"
#include <rcssbase/conf/builder.hpp>
#include <rcssbase/conf/parser.hpp>
#include <rcssbase/conf/streamstatushandler.hpp>
#include <rcssbase/conf/paramsetter.hpp>
#include <rcssbase/conf/paramgetter.hpp>

#include <string>
#include <iostream>
#include <cstring>

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h> /* needed for htonl, htons, ... */
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_WINSOCK2_H
#include <Winsock2.h> /* needed for htonl, htons, ... */
#endif

ServerParam::Printer::Printer( std::ostream& out, unsigned int version )
    : M_out( out ),
      M_version( version )
{}

void
ServerParam::Printer::operator()( const std::pair< const std::string, unsigned int > item )
{
    if ( item.second <= M_version )
    {
        int ivalue;
        if( ServerParam::instance().getInt( item.first, ivalue ) )
        {
            M_out << "(" << item.first << " " << ivalue << ")";
            return;
        }

        bool bvalue;
        if( ServerParam::instance().getBool( item.first, bvalue ) )
        {
            M_out << "(" << item.first << " " << bvalue << ")";
            return;
        }

        double dvalue;
        if( ServerParam::instance().getDoub( item.first, dvalue ) )
        {
            M_out << "(" << item.first << " " << dvalue << ")";
            return;
        }

        std::string svalue;
        if( ServerParam::instance().getStr( item.first, svalue ) )
        {
            M_out << "(" << item.first << " " << svalue << ")";
            return;
        }
    }
}


#if defined(_WIN32) || defined(__WIN32__) || defined (WIN32) || defined (__CYGWIN__)
#  ifndef WIN32
#    define WIN32
#  endif
#endif


#ifdef WIN32
const char ServerParam::LANDMARK_FILE[] = "~\\.rcssserver-landmark.xml";
const char ServerParam::SERVER_CONF[] = "~\\.rcssserver\\server.conf";
const char ServerParam::OLD_SERVER_CONF[] = "~\\.rcssserver-server.conf";
#else
const char ServerParam::LANDMARK_FILE[] = "~/.rcssserver-landmark.xml";
const char ServerParam::SERVER_CONF[] = "~/.rcssserver/server.conf";
const char ServerParam::OLD_SERVER_CONF[] = "~/.rcssserver-server.conf";
#endif

const int ServerParam::SEND_COMMS = false;

const int ServerParam::TEXT_LOGGING = true;
const int ServerParam::GAME_LOGGING = true;
const int ServerParam::GAME_LOG_VERSION = 3;
const char ServerParam::TEXT_LOG_DIR[] = "./";
const char ServerParam::GAME_LOG_DIR[] = "./";
const char ServerParam::TEXT_LOG_FIXED_NAME[] = "rcssserver";
const char ServerParam::GAME_LOG_FIXED_NAME[] = "rcssserver";
const int ServerParam::TEXT_LOG_FIXED = false;
const int ServerParam::GAME_LOG_FIXED = false;
const int ServerParam::TEXT_LOG_DATED = true;
const int ServerParam::GAME_LOG_DATED = true;
const char ServerParam::LOG_DATE_FORMAT[] = "%Y%m%d%H%M-";
const int ServerParam::LOG_TIMES = false;
const int ServerParam::RECORD_MESSAGES = false;
const int ServerParam::TEXT_LOG_COMPRESSION = 0;
const int ServerParam::GAME_LOG_COMPRESSION = 0;
const bool ServerParam::PROFILE = false;

const int ServerParam::KAWAY_LOGGING = true;
const char ServerParam::KAWAY_LOG_DIR[] = "./";
const char ServerParam::KAWAY_LOG_FIXED_NAME[] = "rcssserver";
const int ServerParam::KAWAY_LOG_FIXED = false;
const int ServerParam::KAWAY_LOG_DATED = true;

const int ServerParam::KAWAY_START = -1;

const int ServerParam::POINT_TO_BAN = 5;
const int ServerParam::POINT_TO_DURATION= 20;
const unsigned int ServerParam::SAY_MSG_SIZE = 10;
const unsigned int ServerParam::HEAR_MAX = 1;
const unsigned int ServerParam::HEAR_INC = 1;
const unsigned int ServerParam::HEAR_DECAY = 1;

const double ServerParam::TACKLE_DIST = 2.0;
const double ServerParam::TACKLE_BACK_DIST = 0.5;
const double ServerParam::TACKLE_WIDTH = 1.0;
const double ServerParam::TACKLE_EXPONENT = 6.0;
const unsigned int ServerParam::TACKLE_CYCLES = 10;
const double ServerParam::TACKLE_POWER_RATE = 0.027;

const int ServerParam::NR_NORMAL_HALFS = 2;
const int ServerParam::NR_EXTRA_HALFS = 2;
const bool ServerParam::PENALTY_SHOOT_OUTS = true;

const int    ServerParam::PEN_BEFORE_SETUP_WAIT = 30;
const int    ServerParam::PEN_SETUP_WAIT = 100;
const int    ServerParam::PEN_READY_WAIT =  50;
const int    ServerParam::PEN_TAKEN_WAIT = 200;
const int    ServerParam::PEN_NR_KICKS = 5;
const int    ServerParam::PEN_MAX_EXTRA_KICKS = 10;
const double ServerParam::PEN_DIST_X = 42.5;
const bool   ServerParam::PEN_RANDOM_WINNER = false;
const double ServerParam::PEN_MAX_GOALIE_DIST_X = 14;
const bool   ServerParam::PEN_ALLOW_MULT_KICKS = true;
const bool   ServerParam::PEN_COACH_MOVES_PLAYERS = true;


const unsigned int ServerParam::FREEFORM_WAIT_PERIOD = 600;
const unsigned int ServerParam::FREEFORM_SEND_PERIOD = 20;

const bool ServerParam::FREE_KICK_FAULTS = true;
const bool ServerParam::BACK_PASSES = true;

const bool ServerParam::PROPER_GOAL_KICKS = false;
const double ServerParam::STOPPED_BALL_VEL = 0.01;
const int ServerParam::MAX_GOAL_KICKS = 3;

const int ServerParam::CLANG_DEL_WIN = 1;
const int ServerParam::CLANG_RULE_WIN = 1;

const bool ServerParam::S_AUTO_MODE = false;
const int ServerParam::S_KICK_OFF_WAIT = 100;
const int ServerParam::S_CONNECT_WAIT = 300;
const int ServerParam::S_GAME_OVER_WAIT = 100;

const char ServerParam::S_TEAM_L_START[] = "";
const char ServerParam::S_TEAM_R_START[] = "";

#ifdef MODULEDIR
const char ServerParam::S_MODULE_DIR[] = MODULEDIR;
#else
const char ServerParam::S_MODULE_DIR[] = "/usr/local/lib/rcssserver/modules";
#endif

const double ServerParam::BALL_STUCK_AREA = 3.0;

bool ServerParam::s_in_init = false;

ServerParam&
ServerParam::instance( const std::string& progname )
{ static ServerParam rval( progname ); return rval; }

ServerParam&
ServerParam::instance()
{ return ServerParam::instance( "" ); }

bool
ServerParam::init( const int& argc, const char * const *argv )
{
    s_in_init = true;
    instance( argv[ 0 ] );
    s_in_init = false;
#ifndef WIN32
    if ( system( ( "ls " + tildeExpand( ServerParam::OLD_SERVER_CONF ) + " > /dev/null 2>&1" ).c_str() ) == 0 )
    {
        if ( system( ( "ls " + tildeExpand( ServerParam::SERVER_CONF ) + " > /dev/null 2>&1" ).c_str() ) != 0 )
        {
            if( system( "which awk > /dev/null 2>&1" ) == 0 )
            {
                std::cout << "Trying to convert old configuration file '"
                          << ServerParam::OLD_SERVER_CONF
                          << "'\n";

                char filename[] = "/tmp/rcssserver-oldconf-XXXXXX";
                int fd = mkstemp( filename );
                if( fd != -1 )
                {
                    close( fd );
                    std::string command = "awk '/^[ \\t]*$/ {} ";
                    command += "/^[^#]+[:]/ { gsub(/:/, \"=\" ); $1 = \"server::\" $1; } ";
                    command += "/^[ \\t]*[^#:=]+$/ { $1 = \"server::\" $1 \" = true\"; }";
                    command += "{ print; }' ";
                    command +=  tildeExpand( ServerParam::OLD_SERVER_CONF );
                    command += " > ";
                    command += filename;
                    if( system( command.c_str() ) == 0 )
                    {
                        std::cout << "Conversion successful\n";
                        instance().m_conf_parser->parse( filename );
                    }
                    else
                    {
                        std::cout << "Conversion failed\n";
                    }
                }
                else
                {
                    std::cout << "Conversion failed\n";
                }
            }
        }
    }
#endif // not win32

    boost::filesystem::path conf_path( tildeExpand( ServerParam::SERVER_CONF ),
                                       boost::filesystem::portable_posix_name );
    if ( ! instance().m_conf_parser->parseCreateConf( conf_path, "server" ) )
    {
        std::cerr << "could not create or parse configuration file '"
                  << tildeExpand( ServerParam::SERVER_CONF )
                  << "'\n";
        instance().m_builder->displayHelp();
        instance().clear();
        std::exit( EXIT_FAILURE );
    }
    else
    {
        if ( ! PlayerParam::init( ServerParam::instance().m_builder.get() ) )
        {
            instance().m_builder->displayHelp();
            instance().clear();
            std::exit( EXIT_FAILURE );
        }
        if ( ! instance().m_conf_parser->parse( argc, argv ) )
        {
            instance().m_builder->displayHelp();
            instance().clear();
            std::exit( EXIT_FAILURE );
        }
        if ( ! instance().timer_loaded )
        {
            instance().setSynchMode( false );
        }
        if ( ! instance().timer_loaded )
        {
            std::cerr << "Could not load timer\n";
            std::cerr << "timer library not found in:\n";
            for ( std::vector< boost::filesystem::path >::const_iterator i = rcss::lib::Loader::getPath().begin();
                 i != rcss::lib::Loader::getPath().end();
                 ++i )
            {
                std::cerr << "\t - " << i->native_directory_string() << "\n";
            }
            instance().clear();
            std::exit( EXIT_FAILURE );
        }

        if ( instance().m_builder->genericHelpRequested() )
        {
            instance().m_builder->displayHelp();
            instance().clear();
            std::exit( EXIT_SUCCESS );
        }
    }

    return true;
}

ServerParam::ServerParam( const std::string& progname )
    : m_builder( new rcss::conf::Builder( progname, "server" ) ),
      m_conf_parser( new rcss::conf::Parser( *m_builder ) ),
      m_err_handler( new rcss::conf::StreamStatusHandler() )
{
    m_builder->addHandler( *m_err_handler );
    setDefaults();
    addParams();
    if( !ServerParam::s_in_init )
    {
        std::cerr << "Warning: ServerParam is being used before it has been initialised\n";
    }
}

ServerParam::~ServerParam()
{
    if ( m_builder )
        m_builder->removeHandler( *m_err_handler );
}

template< typename P >
void
ServerParam::addParam(  const std::string& name,
                        P& param,
                        const std::string& desc,
                        int version )
{
    m_builder->addParam( name, param, desc );
    m_ver_map[ name ] = version;
}

template< typename S, typename G >
void
ServerParam::addParam(  const std::string& name,
                        const S& setter,
                        const G& getter,
                        const std::string& desc,
                        int version )
{
    m_builder->addParam( name, setter, getter, desc );
    m_ver_map[ name ] = version;
}

void
ServerParam::addParams()
{
    addParam( "goal_width", gwidth, "The width of the goals", 7 );
    addParam( "player_size",
              rcss::conf::makeSetter( this, &ServerParam::setPlayerSize ),
              rcss::conf::makeGetter( psize ),
              "The size of the default player",
              7 );
    addParam( "player_decay", pdecay, "Players speed decay rate", 7 );
    addParam( "player_rand", prand, "Player random movement factor", 7 );
    addParam( "player_weight", pweight, "The weight of the player", 7 );
    addParam( "player_speed_max", pspeed_max, "The max speed of players", 7 );
    // th 6.3.00
    addParam( "player_accel_max", paccel_max, "The max acceleration of players", 7 );
    //
    addParam( "stamina_max", stamina_max, "The maximum stamina of players", 7 );
    addParam( "stamina_inc_max", stamina_inc, "The maximum player stamina increament", 7 );
    addParam( "recover_init", recover_init, "The intial recovery value for players", 9 );
    addParam( "recover_dec_thr", recover_dthr, "", 7 );
    addParam( "recover_min", recover_min, "", 7 );
    addParam( "recover_dec", recover_dec, "", 7 );
    addParam( "effort_init", effort_init, "", 7 );
    addParam( "effort_dec_thr", effort_dthr, "", 7 );
    addParam( "effort_min", effort_min, "", 7 );
    addParam( "effort_dec", effort_dec, "", 7 );
    addParam( "effort_inc_thr", effort_ithr, "", 7 );
    addParam( "effort_inc", effort_inc, "", 7 );
    // pfr 8/14/00: for RC2000 evaluation
    addParam( "kick_rand",           kick_rand, "", 7 );
    addParam( "team_actuator_noise", team_actuator_noise, "", 7 );
    addParam( "prand_factor_l",      prand_factor_l, "", 7 );
    addParam( "prand_factor_r",      prand_factor_r, "", 7 );
    addParam( "kick_rand_factor_l",  kick_rand_factor_l, "", 7 );
    addParam( "kick_rand_factor_r",  kick_rand_factor_r, "", 7 );

    addParam( "ball_size",
              rcss::conf::makeSetter( this, &ServerParam::setBallSize ),
              rcss::conf::makeGetter( bsize ),
              "",
              7 );
    addParam( "ball_decay", bdecay, "", 7 );
    addParam( "ball_rand", brand, "", 7 );
    addParam( "ball_weight", bweight, "", 7 );
    addParam( "ball_speed_max", bspeed_max, "", 7 );
    // th 6.3.00
    addParam( "ball_accel_max", baccel_max, "", 7 );
    //
    addParam( "dash_power_rate", dprate, "", 7 );
    addParam( "kick_power_rate", kprate, "", 7);
    addParam( "kickable_margin",
              rcss::conf::makeSetter( this, &ServerParam::setKickMargin ),
              rcss::conf::makeGetter( kmargin ),
              "",
              7 );
    addParam( "control_radius",
              rcss::conf::makeSetter( this, &ServerParam::setCTLRadius ),
              rcss::conf::makeGetter( ctlradius ),
              "",
              7 );

    addParam( "catch_probability", catch_prob, "", 7 );
    addParam( "catchable_area_l",    catch_area_l, "", 7 );
    addParam( "catchable_area_w",    catch_area_w, "", 7 );
    addParam( "goalie_max_moves",    goalie_max_moves, "", 7 );

    addParam( "maxpower", maxp, "", 7 );
    addParam( "minpower", minp, "", 7 );
    addParam( "maxmoment", maxm, "", 7 );
    addParam( "minmoment", minm, "", 7 );
    addParam( "maxneckmoment", maxnm, "", 7 );
    addParam( "minneckmoment", minnm, "", 7 );
    addParam( "maxneckang", maxn, "", 7 );
    addParam( "minneckang", minn, "", 7 );
    addParam( "visible_angle", visangle, "", 7 );
    addParam( "visible_distance", visdist, "", 7 );
    addParam( "audio_cut_dist", audio_dist, "", 7 );
    addParam( "quantize_step", dist_qstep, "", 7 );
    addParam( "quantize_step_l", land_qstep, "", 7 );
#ifdef NEW_QSTEP
    addParam( "quantize_step_dir", dir_qstep, "", 7 );
    addParam( "quantize_step_dist_team_l", dist_qstep_l, "", 7 );
    addParam( "quantize_step_dist_team_r", dist_qstep_r, "", 7 );
    addParam( "quantize_step_dist_l_team_l", land_qstep_l, "", 7 );
    addParam( "quantize_step_dist_l_team_r", land_qstep_r, "", 7 );
    addParam( "quantize_step_dir_team_l", dir_qstep_l, "", 7 );
    addParam( "quantize_step_dir_team_r", dir_qstep_r, "", 7 );
#endif
    addParam( "ckick_margin", ckmargin, "", 7 );
    addParam( "wind_dir", windir, "", 7 );
    addParam( "wind_force", winforce, "", 7 );
    addParam( "wind_ang", winang, "", 7 );
    addParam( "wind_rand", winrand, "", 7 );
    addParam( "inertia_moment", inertia_moment, "", 7 );
    addParam( "wind_none", win_no, "", 7 );
    addParam( "wind_random", win_random, "", 7 );
    addParam( "half_time",
              rcss::conf::makeSetter( this, &ServerParam::setHalfTime ),
              rcss::conf::makeGetter( this, &ServerParam::getRawHalfTime ),
              "",
              7 );
    addParam( "drop_ball_time", drop_time, "", 7 );
    addParam( "port", portnum, "", 8 );
    addParam( "coach_port", coach_pnum, "", 8 );
    addParam( "olcoach_port", olcoach_pnum, "", 8 );
    addParam( "say_coach_cnt_max", say_cnt_max, "", 7 );
    addParam( "say_coach_msg_size", SayCoachMsgSize, "", 7 );
    addParam( "simulator_step",
              rcss::conf::makeSetter( this, &ServerParam::setSimStep ),
              rcss::conf::makeGetter( this, &ServerParam::getRawSimStep ),
              "",
              7 );
    addParam( "send_step",
              rcss::conf::makeSetter( this, &ServerParam::setSendStep ),
              rcss::conf::makeGetter( this, &ServerParam::getRawSendStep ),
              "", 7 );
    addParam( "recv_step", recv_st, "", 7 );
    addParam( "sense_body_step",
              rcss::conf::makeSetter( this, &ServerParam::setSenseBodyStep ),
              rcss::conf::makeGetter( this, &ServerParam::getRawSenseBodyStep ),
              "", 7 );
    addParam( "say_msg_size", M_say_msg_size, "", 7 );
    addParam( "clang_win_size",      clang_win_size, "", 7 );
    addParam( "clang_define_win",    clang_define_win, "", 7 );
    addParam( "clang_meta_win",      clang_meta_win, "", 7 );
    addParam( "clang_advice_win",    clang_advice_win, "", 7 );
    addParam( "clang_info_win",      clang_info_win, "", 7 );
    addParam( "clang_del_win",      M_clang_del_win, "", 8 );
    addParam( "clang_rule_win",      M_clang_rule_win, "", 8 );
    addParam( "clang_mess_delay",    clang_mess_delay, "", 7 );
    addParam( "clang_mess_per_cycle",      clang_mess_per_cycle, "", 7 );
    addParam( "hear_max", M_hear_max, "", 7 );
    addParam( "hear_inc", M_hear_inc, "", 7 );
    addParam( "hear_decay", M_hear_decay, "", 7 );
    addParam( "catch_ban_cycle", cban_cycle, "", 7 );
    addParam( "coach", CoachMode, "", 7 );
    addParam( "coach_w_referee", CwRMode, "", 7 );
    addParam( "old_coach_hear", old_hear, "", 7 );
    addParam( "send_vi_step",
              rcss::conf::makeSetter( this, &ServerParam::setCoachVisualStep ),
              rcss::conf::makeGetter( this, &ServerParam::getRawCoachVisualStep ),
              "", 7 );
    addParam( "use_offside", useoffside, "", 7 );
    addParam( "offside_active_area_size", offside_area, "", 7 );
    addParam( "forbid_kick_off_offside", kickoffoffside, "", 7 );
    addParam( "verbose", verbose, "", 8 );
    addParam( "offside_kick_margin", offside_kick_margin, "", 7 );
    addParam( "slow_down_factor",
              rcss::conf::makeSetter( this, &ServerParam::setSlowDownFactor ),
              rcss::conf::makeGetter( slow_down_factor ), "", 7 );
    addParam( "synch_mode",
              rcss::conf::makeSetter( this, &ServerParam::setSynchMode ),
              rcss::conf::makeGetter( synch_mode ), "", 8 ); //pfr:SYNCH
    addParam( "synch_offset",
              rcss::conf::makeSetter( this, &ServerParam::setSynchOffset ),
              rcss::conf::makeGetter( this, &ServerParam::getRawSynchOffset ),
              "", 8 ); //pfr:SYNCH
    addParam( "synch_micro_sleep",       synch_micro_sleep, "", 8 ); //pfr:SYNCH

    addParam( "start_goal_l",            start_goal_l, "", 7 );
    addParam( "start_goal_r",            start_goal_r, "", 7 );

    addParam( "fullstate_l",             fullstate_l, "", 7 );
    addParam( "fullstate_r",             fullstate_r, "", 7 );
    addParam( "slowness_on_top_for_left_team", slowness_on_top_for_left_team, "", 8 );
    addParam( "slowness_on_top_for_right_team", slowness_on_top_for_right_team, "", 8 );
    addParam( "landmark_file",           landmark_file, "", 8 );
    addParam( "send_comms", send_comms, "", 8 );
    addParam( "text_logging", text_logging, "", 8 );
    addParam( "game_logging", game_logging, "", 8 );
    addParam( "game_log_version", game_log_version, "", 8 );
    addParam( "text_log_dir", text_log_dir, "", 8 );
    addParam( "game_log_dir", game_log_dir, "", 8 );
    addParam( "text_log_fixed_name", text_log_fixed_name, "", 8 );
    addParam( "game_log_fixed_name", game_log_fixed_name, "", 8 );
    addParam( "text_log_fixed", text_log_fixed, "", 8 );
    addParam( "game_log_fixed", game_log_fixed, "", 8 );
    addParam( "text_log_dated", text_log_dated, "", 8 );
    addParam( "game_log_dated", game_log_dated, "", 8 );
    addParam( "log_date_format", log_date_format, "", 8 );
    addParam( "log_times", log_times, "", 8 );
    addParam( "record_messages", record_messages, "", 8 );
    addParam( "text_log_compression", text_log_compression, "", 8 );
    addParam( "game_log_compression", game_log_compression, "", 8 );
    addParam( "profile", M_profile, "", 8 );
    addParam( "point_to_ban", M_point_to_ban, "", 8 );
    addParam( "point_to_duration", M_point_to_duration, "", 8 );

    addParam( "tackle_dist", M_tackle_dist, "", 8 );
    addParam( "tackle_back_dist", M_tackle_back_dist, "", 8 );
    addParam( "tackle_width", M_tackle_width, "", 8 );
    addParam( "tackle_exponent", M_tackle_exponent, "", 8 );
    addParam( "tackle_cycles", M_tackle_cycles, "", 8 );
    addParam( "tackle_power_rate", M_tackle_power_rate, "", 8 );

    addParam( "freeform_wait_period", M_freeform_wait_period, "", 8 );
    addParam( "freeform_send_period", M_freeform_send_period, "", 8 );

    addParam( "free_kick_faults", M_free_kick_faults, "", 8 );
    addParam( "back_passes", M_back_passes, "", 8 );

    addParam( "proper_goal_kicks", M_proper_goal_kicks, "", 8 );
    addParam( "stopped_ball_vel", M_stopped_ball_vel, "", 8 );
    addParam( "max_goal_kicks", M_max_goal_kicks, "", 8 );

    addParam( "auto_mode", M_auto_mode, "", 9 );
    addParam( "kick_off_wait", M_kick_off_wait, "", 9 );
    addParam( "connect_wait", M_connect_wait, "", 9 );
    addParam( "game_over_wait", M_game_over_wait, "", 9 );

    addParam( "team_l_start",
              rcss::conf::makeSetter( this, &ServerParam::setTeamLeftStart ),
              rcss::conf::makeGetter( this, &ServerParam::teamLeftStart ),
              "", 9 );
    addParam( "team_r_start",
              rcss::conf::makeSetter( this, &ServerParam::setTeamRightStart ),
              rcss::conf::makeGetter( this, &ServerParam::teamRightStart ),
              "", 9 );

    addParam( "keepaway", kaway, "", 9 );
    addParam( "keepaway_length", ka_length, "", 9 );
    addParam( "keepaway_width", ka_width, "", 9 );

    addParam( "keepaway_logging", kaway_logging, "", 9 );
    addParam( "keepaway_log_dir", kaway_log_dir, "", 9 );
    addParam( "keepaway_log_fixed_name", kaway_log_fixed_name, "", 9 );
    addParam( "keepaway_log_fixed", kaway_log_fixed, "", 9 );
    addParam( "keepaway_log_dated", kaway_log_dated, "", 9 );

    addParam( "keepaway_start", kaway_start, "", 9 );

    addParam( "nr_normal_halfs", nr_normal_halfs, "Number of normal halfs in a game", 9 );
    addParam( "nr_extra_halfs", nr_extra_halfs, "Number if extra-time periods in a game if it is drawn", 9 );
    addParam( "penalty_shoot_outs", penalty_shoot_outs,
              "Set to true to enable penalty shootouts after normal time and extra time if the game is drawn.\nTo have the game go straight into penalty shoot outs, set this to true and nr_normal_halfs and nr_extra_halfs to 0"
              , 9 );

    addParam( "pen_before_setup_wait", pen_before_setup_wait, "", 9 );
    addParam( "pen_setup_wait",        pen_setup_wait, "", 9 );
    addParam( "pen_ready_wait",        pen_ready_wait, "", 9 );
    addParam( "pen_taken_wait",        pen_taken_wait, "", 9 );
    addParam( "pen_nr_kicks",          pen_nr_kicks, "", 9 );
    addParam( "pen_max_extra_kicks",   pen_max_extra_kicks, "", 9 );
    addParam( "pen_dist_x"         ,   pen_dist_x         , "", 9 );
    addParam( "pen_random_winner",     pen_random_winner, "", 9 );
    addParam( "pen_max_goalie_dist_x", pen_max_goalie_dist_x, "", 9 );
    addParam( "pen_allow_mult_kicks",  pen_allow_mult_kicks,
              "Turn on to allow dribbling in penalty shootouts", 9 );
    addParam( "pen_coach_moves_players", pen_coach_moves_players,
              "Turn on to have the server automatically position players for peanlty shootouts", 9 );

    addParam( "ball_stuck_area", M_ball_stuck_area, "", 11 );
    addParam( "coach_msg_file", M_coach_msg_file, "", 11 );

}


void
ServerParam::setSynchMode( bool value )
{
    if ( synch_mode != value || !timer_loaded )
    {
        if ( value )
        {
            rcss::lib::Loader module;
            if ( module.open( "librcsssynctimer" ) )
            {
                m_builder->manageModule( module );
                timer_loaded = true;
            }
            else
            {
                std::cerr << "Could not load synctimer" << std::endl;
            }
        }
        else
        {
            rcss::lib::Loader module;
            if ( module.open( "librcssstdtimer" ) )
            {
                m_builder->manageModule( module );
                timer_loaded = true;
            }
            else
            {
                std::cerr << "Could not load stdtimer" << std::endl;
            }
        }
        synch_mode = value ;
        lcm_st = lcm( sim_st,
                      lcm( send_st,
                           lcm( recv_st,
                                lcm( sb_step,
                                     lcm( sv_st,
                                          ( synch_mode ? synch_offset : 1 ) ) ) ) ) );
    }
}


void
ServerParam::setTeamLeftStart( std::string start )
{
    M_team_l_start = start;
    std::replace( M_team_l_start.begin(), M_team_l_start.end(), '\n', ' ' );
}

void
ServerParam::setTeamRightStart( std::string start )
{
    M_team_r_start = start;
    std::replace( M_team_r_start.begin(), M_team_r_start.end(), '\n', ' ' );
}


void
ServerParam::clear()
{
    m_conf_parser.reset();
    m_builder.reset();
}

void
ServerParam::setDefaults()
{
    /* set default parameter */
    gwidth = GOAL_WIDTH ;
    psize = PLAYER_SIZE ;
    pdecay = PLAYER_DECAY ;
    prand = PLAYER_RAND ;
    pweight = PLAYER_WEIGHT ;
    stamina_max = STAMINA_MAX ;
    stamina_inc = STAMINA_INC_MAX ;
    recover_init = 1.0 ;
    recover_dthr = RECOVERY_DEC_THR ;
    recover_min = RECOVERY_MIN ;
    recover_dec = RECOVERY_DEC ;
    effort_init = 1.0 ;
    effort_dthr = EFFORT_DEC_THR ;
    effort_min = EFFORT_MIN ;
    effort_dec = EFFORT_DEC ;
    effort_ithr = EFFORT_INC_THR ;
    effort_inc= EFFORT_INC ;
    inertia_moment = IMPARAM ;
    // pfr 8/14/00: for RC2000 evaluation
    kick_rand = KICK_RAND;
    team_actuator_noise = false;
    prand_factor_l = PRAND_FACTOR_L;
    prand_factor_r = PRAND_FACTOR_R;
    kick_rand_factor_l = KICK_RAND_FACTOR_L;
    kick_rand_factor_r = KICK_RAND_FACTOR_R;

    bsize = BALL_SIZE ;
    bdecay = BALL_DECAY ;
    brand = BALL_RAND ;
    bweight = BALL_WEIGHT ;
    bspeed_max = BALL_SPEED_MAX ;
    pspeed_max = PLAYER_SPEED_MAX ;
    // th 6.3.00
    baccel_max = BALL_ACCEL_MAX ;
    paccel_max = PLAYER_ACCEL_MAX ;
    //
    dprate = DASHPOWERRATE ;
    kprate = KICKPOWERRATE ;
    kmargin = KICKABLE_MARGIN ;
    ctlradius = CONTROL_RADIUS ;
    ckmargin = CORNER_KICK_MARGIN ;
    catch_prob = GOALIE_CATCHABLE_POSSIBILITY ;
    catch_area_l = GOALIE_CATCHABLE_AREA_LENGTH ;
    catch_area_w = GOALIE_CATCHABLE_AREA_WIDTH ;
    goalie_max_moves = GOALIE_MAX_MOVES;
    maxp = MAXPOWER ;
    minp = MINPOWER ;
    maxm = MAXMOMENT ;
    minm = MINMOMENT ;
    maxnm = MAX_NECK_MOMENT ;
    minnm = MIN_NECK_MOMENT ;
    maxn = MAX_NECK_ANGLE ;
    minn = MIN_NECK_ANGLE ;
    visangle = VisibleAngle ;
    visdist = VisibleDistance ;
    drop_time = DROP_TIME ;
    portnum = DEFAULT_PORT_NUMBER ;
    coach_pnum = COACH_PORT_NUMBER ;
    olcoach_pnum = OLCOACH_PORT_NUMBER ;
    say_cnt_max = DEF_SAY_COACH_CNT_MAX ;
    SayCoachMsgSize = DEF_SAY_COACH_MSG_SIZE ;
    sim_st = SIMULATOR_STEP_INTERVAL_MSEC ;
    send_st = UDP_SEND_STEP_INTERVAL_MSEC ;
    recv_st = UDP_RECV_STEP_INTERVAL_MSEC ;
    sb_step = SENSE_BODY_INTERVAL_MSEC ;
    clang_win_size = 300;
    clang_define_win = 1;
    clang_meta_win = 1;
    clang_advice_win = 1;
    clang_info_win = 1;
    M_clang_del_win = CLANG_DEL_WIN;
    M_clang_rule_win = CLANG_RULE_WIN;
    clang_mess_delay = 50;
    clang_mess_per_cycle = 1;
    CoachMode = false;
    CwRMode = false;
    old_hear = false;
    sv_st = SEND_VISUALINFO_INTERVAL_MSEC ;
    audio_dist = AUDIO_CUT_OFF_DIST ;
    cban_cycle = GOALIE_CATCH_BAN_CYCLE ;
    slow_down_factor = 1;
    verbose = false;

    dist_qstep = DIST_QSTEP ;
    land_qstep = LAND_QSTEP ;
#ifdef NEW_QSTEP
    dir_qstep  = DIR_QSTEP ;
#endif

    windir = WIND_DIR ;
    winforce = WIND_FORCE ;
    winrand = WIND_RAND ;
    win_no = false;
    win_random = false;
    useoffside = true;
    offside_area = OFFSIDE_ACTIVE_AREA_SIZE ;
    kickoffoffside = true;
    offside_kick_margin = OFFSIDE_KICK_MARGIN ;

    kaway = false;
    ka_length = KEEPAWAY_LENGTH ;
    ka_width = KEEPAWAY_WIDTH ;

    nr_normal_halfs = NR_NORMAL_HALFS;
    nr_extra_halfs = NR_EXTRA_HALFS;
    penalty_shoot_outs = PENALTY_SHOOT_OUTS;

    pen_before_setup_wait = PEN_BEFORE_SETUP_WAIT;
    pen_setup_wait        = PEN_SETUP_WAIT;
    pen_ready_wait        = PEN_READY_WAIT;
    pen_taken_wait        = PEN_TAKEN_WAIT;
    pen_nr_kicks          = PEN_NR_KICKS;
    pen_max_extra_kicks   = PEN_MAX_EXTRA_KICKS;
    pen_dist_x            = PEN_DIST_X;
    pen_random_winner     = PEN_RANDOM_WINNER;
    pen_max_goalie_dist_x = PEN_MAX_GOALIE_DIST_X;
    pen_allow_mult_kicks  = PEN_ALLOW_MULT_KICKS;
    pen_coach_moves_players = PEN_COACH_MOVES_PLAYERS;

    timer_loaded = false;

    synch_offset = 60;
    synch_micro_sleep = 1;

    start_goal_l = 0;
    start_goal_r = 0;

    fullstate_l = false;
    fullstate_r = false;


#ifdef NEW_QSTEP
    bool defined_qstep_l = false;
    bool defined_qstep_r = false;
    bool defined_qstep_l_l = false;
    bool defined_qstep_l_r = false;
    bool defined_qstep_dir_l = false;
    bool defined_qstep_dir_r = false;
#endif

    slowness_on_top_for_left_team = 1.0;
    slowness_on_top_for_right_team = 1.0;

    landmark_file = ServerParam::LANDMARK_FILE;


    send_comms = ServerParam::SEND_COMMS;
    text_logging = ServerParam::TEXT_LOGGING;
    game_logging = ServerParam::GAME_LOGGING;
    game_log_version = ServerParam::GAME_LOG_VERSION;
    text_log_dir = ServerParam::TEXT_LOG_DIR;
    game_log_dir = ServerParam::GAME_LOG_DIR;
    text_log_fixed_name = ServerParam::TEXT_LOG_FIXED_NAME;
    game_log_fixed_name = ServerParam::GAME_LOG_FIXED_NAME;
    text_log_fixed = ServerParam::TEXT_LOG_FIXED;
    game_log_fixed = ServerParam::GAME_LOG_FIXED;
    text_log_dated = ServerParam::TEXT_LOG_DATED;
    game_log_dated = ServerParam::GAME_LOG_DATED;
    log_date_format = ServerParam::LOG_DATE_FORMAT;
    log_times = ServerParam::LOG_TIMES;
    record_messages = ServerParam::RECORD_MESSAGES;
    text_log_compression = ServerParam::TEXT_LOG_COMPRESSION;
    game_log_compression = ServerParam::GAME_LOG_COMPRESSION;
    M_profile = ServerParam::PROFILE;

    kaway_logging = ServerParam::KAWAY_LOGGING;
    kaway_log_dir = ServerParam::KAWAY_LOG_DIR;
    kaway_log_fixed_name = ServerParam::KAWAY_LOG_FIXED_NAME;
    kaway_log_fixed = ServerParam::KAWAY_LOG_FIXED;
    kaway_log_dated = ServerParam::KAWAY_LOG_DATED;

    kaway_start = ServerParam::KAWAY_START;

    M_point_to_ban = ServerParam::POINT_TO_BAN;
    M_point_to_duration = ServerParam::POINT_TO_DURATION;
    M_say_msg_size = ServerParam::SAY_MSG_SIZE ;
    M_hear_max = ServerParam::HEAR_MAX ;
    M_hear_inc = ServerParam::HEAR_INC ;
    M_hear_decay = ServerParam::HEAR_DECAY ;

    M_tackle_dist = TACKLE_DIST;
    M_tackle_back_dist = TACKLE_BACK_DIST;
    M_tackle_width = TACKLE_WIDTH;
    M_tackle_exponent = TACKLE_EXPONENT;
    M_tackle_cycles = TACKLE_CYCLES;
    M_tackle_power_rate = TACKLE_POWER_RATE;

    M_freeform_wait_period = FREEFORM_WAIT_PERIOD;
    M_freeform_send_period = FREEFORM_SEND_PERIOD;

    M_free_kick_faults = FREE_KICK_FAULTS;
    M_back_passes = BACK_PASSES;

    M_proper_goal_kicks = PROPER_GOAL_KICKS;
    M_stopped_ball_vel = STOPPED_BALL_VEL;
    M_max_goal_kicks = MAX_GOAL_KICKS;

    M_auto_mode = S_AUTO_MODE;
    M_kick_off_wait = S_KICK_OFF_WAIT;
    M_connect_wait = S_CONNECT_WAIT;
    M_game_over_wait = S_GAME_OVER_WAIT;

    M_team_l_start = S_TEAM_L_START;
    M_team_r_start = S_TEAM_R_START;

    M_ball_stuck_area = BALL_STUCK_AREA;

    M_coach_msg_file = "";

    setHalfTime( HALF_TIME );

    std::string module_dir = S_MODULE_DIR;
    for ( std::string::size_type pos = module_dir.find( "//" );
          pos != std::string::npos;
          pos = module_dir.find( "//" ) )
    {
        module_dir.replace( pos, 2, "/" );
    }

    rcss::lib::Loader::setPath( module_dir );
}






server_params_t ServerParam::convertToStruct ()
{
    server_params_t tmp;

    tmp.gwidth = htonl( (Int32)(SHOWINFO_SCALE2 * gwidth) );
    tmp.inertia_moment = htonl( (Int32)(SHOWINFO_SCALE2 * inertia_moment) );
    tmp.psize = htonl( (Int32)(SHOWINFO_SCALE2 * psize) );
    tmp.pdecay = htonl( (Int32)(SHOWINFO_SCALE2 * pdecay) );
    tmp.prand = htonl( (Int32)(SHOWINFO_SCALE2 * prand) );
    tmp.pweight = htonl( (Int32)(SHOWINFO_SCALE2 * pweight) );
    tmp.pspeed_max = htonl( (Int32)(SHOWINFO_SCALE2 * pspeed_max) );
    tmp.paccel_max = htonl( (Int32)(SHOWINFO_SCALE2 * paccel_max) );
    tmp.stamina_max = htonl( (Int32)(SHOWINFO_SCALE2 * stamina_max) );
    tmp.stamina_inc = htonl( (Int32)(SHOWINFO_SCALE2 * stamina_inc) );
    tmp.recover_init = htonl( (Int32)(SHOWINFO_SCALE2 * recover_init) );
    tmp.recover_dthr = htonl( (Int32)(SHOWINFO_SCALE2 * recover_dthr) );
    tmp.recover_min = htonl( (Int32)(SHOWINFO_SCALE2 * recover_min) );
    tmp.recover_dec = htonl( (Int32)(SHOWINFO_SCALE2 * recover_dec) );
    tmp.effort_init = htonl( (Int32)(SHOWINFO_SCALE2 * effort_init) );
    tmp.effort_dthr = htonl( (Int32)(SHOWINFO_SCALE2 * effort_dthr) );
    tmp.effort_min = htonl( (Int32)(SHOWINFO_SCALE2 * effort_min) );
    tmp.effort_dec = htonl( (Int32)(SHOWINFO_SCALE2 * effort_dec) );
    tmp.effort_ithr = htonl( (Int32)(SHOWINFO_SCALE2 * effort_ithr) );
    tmp.effort_inc = htonl( (Int32)(SHOWINFO_SCALE2 * effort_inc) );
    tmp.kick_rand = htonl( (Int32)(SHOWINFO_SCALE2 * kick_rand) );
    tmp.team_actuator_noise = htons( (Int16) team_actuator_noise );
    tmp.prand_factor_l = htonl( (Int32)(SHOWINFO_SCALE2 * prand_factor_l ) );
    tmp.prand_factor_r = htonl( (Int32)(SHOWINFO_SCALE2 * prand_factor_r ) );
    tmp.kick_rand_factor_l = htonl( (Int32)(SHOWINFO_SCALE2 * kick_rand_factor_l ) );
    tmp.kick_rand_factor_r = htonl( (Int32)(SHOWINFO_SCALE2 * kick_rand_factor_r ) );
    tmp.bsize = htonl( (Int32)(SHOWINFO_SCALE2 * bsize) );
    tmp.bdecay = htonl( (Int32)(SHOWINFO_SCALE2 * bdecay) );
    tmp.brand = htonl( (Int32)(SHOWINFO_SCALE2 * brand) );
    tmp.bweight = htonl( (Int32)(SHOWINFO_SCALE2 * bweight) );
    tmp.bspeed_max = htonl( (Int32)(SHOWINFO_SCALE2 * bspeed_max) );
    tmp.baccel_max = htonl( (Int32)(SHOWINFO_SCALE2 * baccel_max ) );
    tmp.dprate = htonl( (Int32)(SHOWINFO_SCALE2 * dprate) );
    tmp.kprate = htonl( (Int32)(SHOWINFO_SCALE2 * kprate) );
    tmp.kmargin = htonl( (Int32)(SHOWINFO_SCALE2 * kmargin) );
    tmp.ctlradius = htonl( (Int32)(SHOWINFO_SCALE2 * ctlradius) );
    tmp.ctlradius_width = htonl( (Int32)(SHOWINFO_SCALE2 * ctlradius_width) );
    tmp.maxp = htonl( (Int32)(SHOWINFO_SCALE2 * maxp) );
    tmp.minp = htonl( (Int32)(SHOWINFO_SCALE2 * minp) );
    tmp.maxm = htonl( (Int32)(SHOWINFO_SCALE2 * maxm) );
    tmp.minm = htonl( (Int32)(SHOWINFO_SCALE2 * minm) );
    tmp.maxnm = htonl( (Int32)(SHOWINFO_SCALE2 * maxnm) );
    tmp.minnm = htonl( (Int32)(SHOWINFO_SCALE2 * minnm) );
    tmp.maxn = htonl( (Int32)(SHOWINFO_SCALE2 * maxn) );
    tmp.minn = htonl( (Int32)(SHOWINFO_SCALE2 * minn) );
    tmp.visangle = htonl( (Int32)(SHOWINFO_SCALE2 * visangle) );
    tmp.visdist = htonl( (Int32)(SHOWINFO_SCALE2 * visdist) );
    tmp.windir = htonl( (Int32)(SHOWINFO_SCALE2 * windir) );
    tmp.winforce = htonl( (Int32)(SHOWINFO_SCALE2 * winforce) );
    tmp.winang = htonl( (Int32)(SHOWINFO_SCALE2 * winang) );
    tmp.winrand = htonl( (Int32)(SHOWINFO_SCALE2 * winrand) );
    tmp.kickable_area = htonl( (Int32)(SHOWINFO_SCALE2 * kickable_area) );
    tmp.catch_area_l = htonl( (Int32)(SHOWINFO_SCALE2 * catch_area_l) );
    tmp.catch_area_w = htonl( (Int32)(SHOWINFO_SCALE2 * catch_area_w) );
    tmp.catch_prob = htonl( (Int32)(SHOWINFO_SCALE2 * catch_prob) );
    tmp.goalie_max_moves = htons( (Int16) goalie_max_moves );
    tmp.ckmargin = htonl( (Int32)(SHOWINFO_SCALE2 * ckmargin) );
    tmp.offside_area = htonl( (Int32)(SHOWINFO_SCALE2 * offside_area) );
    tmp.win_no = htons( (Int16) win_no);
    tmp.win_random = htons( (Int16) win_random) ;
    tmp.say_cnt_max = htons( (Int16) say_cnt_max) ;
    tmp.SayCoachMsgSize = htons( (Int16) SayCoachMsgSize) ;
    tmp.clang_win_size = htons( (Int16) clang_win_size );
    tmp.clang_define_win = htons( (Int16) clang_define_win );
    tmp.clang_meta_win = htons( (Int16) clang_meta_win );
    tmp.clang_advice_win = htons( (Int16) clang_advice_win );
    tmp.clang_info_win = htons( (Int16) clang_info_win );
    tmp.clang_mess_delay = htons( (Int16) clang_mess_delay );
    tmp.clang_mess_per_cycle = htons( (Int16) clang_mess_per_cycle );
    tmp.half_time = htons( (Int16) half_time) ;
    tmp.sim_st = htons( (Int16) sim_st) ;
    tmp.send_st = htons( (Int16) send_st) ;
    tmp.recv_st = htons( (Int16) recv_st) ;
    tmp.sb_step = htons( (Int16) sb_step) ;
    tmp.lcm_st = htons( (Int16) lcm_st) ;
    tmp.M_say_msg_size = htons( (Int16) M_say_msg_size) ;
    tmp.M_hear_max = htons( (Int16) M_hear_max) ;
    tmp.M_hear_inc = htons( (Int16) M_hear_inc) ;
    tmp.M_hear_decay = htons( (Int16) M_hear_decay) ;
    tmp.cban_cycle = htons( (Int16) cban_cycle) ;
    tmp.slow_down_factor = htons( (Int16) slow_down_factor) ;
    tmp.useoffside = htons( (Int16) useoffside) ;
    tmp.kickoffoffside = htons( (Int16) kickoffoffside) ;
    tmp.offside_kick_margin = htonl( (Int32)(SHOWINFO_SCALE2 * offside_kick_margin) );
    tmp.audio_dist = htonl( (Int32)(SHOWINFO_SCALE2 * audio_dist) );
    tmp.dist_qstep = htonl( (Int32)(SHOWINFO_SCALE2 * dist_qstep) );
    tmp.land_qstep = htonl( (Int32)(SHOWINFO_SCALE2 * land_qstep) );
#ifdef NEW_QSTEP
    tmp.dir_qstep = htonl( (Int32)(SHOWINFO_SCALE2 * dir_qstep) );
    tmp.dist_qstep_l = htonl( (Int32)(SHOWINFO_SCALE2 * dist_qstep_l) );
    tmp.dist_qstep_r = htonl( (Int32)(SHOWINFO_SCALE2 * dist_qstep_r) );
    tmp.land_qstep_l = htonl( (Int32)(SHOWINFO_SCALE2 * land_qstep_l) );
    tmp.land_qstep_r = htonl( (Int32)(SHOWINFO_SCALE2 * land_qstep_r) );
    tmp.dir_qstep_l = htonl( (Int32)(SHOWINFO_SCALE2 * dir_qstep_l) );
    tmp.dir_qstep_r = htonl( (Int32)(SHOWINFO_SCALE2 * dir_qstep_r) );
#else
    tmp.dir_qstep = htonl( (Int32)(SHOWINFO_SCALE2 * -1) );
    tmp.dist_qstep_l = htonl( (Int32)(SHOWINFO_SCALE2 * -1) );
    tmp.dist_qstep_r = htonl( (Int32)(SHOWINFO_SCALE2 * -1) );
    tmp.land_qstep_l = htonl( (Int32)(SHOWINFO_SCALE2 * -1) );
    tmp.land_qstep_r = htonl( (Int32)(SHOWINFO_SCALE2 * -1) );
    tmp.dir_qstep_l = htonl( (Int32)(SHOWINFO_SCALE2 * -1) );
    tmp.dir_qstep_r = htonl( (Int32)(SHOWINFO_SCALE2 * -1) );
#endif
    tmp.CoachMode = htons( (Int16) CoachMode) ;
    tmp.CwRMode = htons( (Int16) CwRMode) ;
    tmp.old_hear = htons( (Int16) old_hear) ;
    tmp.sv_st = htons( (Int16) sv_st) ;

    tmp.synch_mode   = htons( (Int16)synch_mode);//pfr:SYNCH
    tmp.synch_offset = htons( (Int16)synch_offset);//pfr:SYNCH
    tmp.synch_micro_sleep = htons( (Int16)synch_micro_sleep);//pfr:SYNCH

    tmp.start_goal_l = htons( (Int16)start_goal_l );
    tmp.start_goal_r = htons( (Int16)start_goal_r );

    tmp.fullstate_l = htons( (Int16)fullstate_l );
    tmp.fullstate_r = htons( (Int16)fullstate_r );

    tmp.drop_time = htons( (Int16) drop_time) ;

    tmp.slowness_on_top_for_left_team = htonl( (Int32)(SHOWINFO_SCALE2 * slowness_on_top_for_left_team) );
    tmp.slowness_on_top_for_right_team = htonl( (Int32)(SHOWINFO_SCALE2 * slowness_on_top_for_left_team) );

    tmp.ka_length = htonl( (Int32)(SHOWINFO_SCALE2 * ka_length) );
    tmp.ka_width = htonl( (Int32)(SHOWINFO_SCALE2 * ka_width) );

    tmp.ball_stuck_area = htonl( (Int32)(SHOWINFO_SCALE2 * M_ball_stuck_area) );

    tmp.point_to_ban =  htons( (Int16)M_point_to_ban );
    tmp.point_to_duration =  htons( (Int16)M_point_to_duration );

    return tmp;
}

bool
ServerParam::getInt( const std::string& param, int& value ) const
{
    return m_builder->get( param, value );
}

bool
ServerParam::getBool( const std::string& param, bool& value ) const
{ return m_builder->get( param, value ); }

bool
ServerParam::getDoub( const std::string& param, double& value ) const
{ return m_builder->get( param, value ); }

bool
ServerParam::getStr( const std::string& param, std::string& value ) const
{ return m_builder->get( param, value ); }


// std::ostream& toStr ( std::ostream& o, const ServerParamSensor_v7::data_t& data )
// {
//   return o << "(server_param "
//            << data.M_gwidth << " "
//            << data.M_inertia_moment << " "
//            << data.M_psize << " "
//            << data.M_pdecay << " "
//            << data.M_prand << " "
//            << data.M_pweight << " "
//            << data.M_pspeed_max << " "
//            << data.M_paccel_max << " "
//            << data.M_stamina_max << " "
//            << data.M_stamina_inc << " "
//            << data.M_recover_init << " "
//            << data.M_recover_dthr << " "
//            << data.M_recover_min << " "
//            << data.M_recover_dec << " "
//            << data.M_effort_init << " "
//            << data.M_effort_dthr << " "
//            << data.M_effort_min << " "
//            << data.M_effort_dec << " "
//            << data.M_effort_ithr << " "
//            << data.M_effort_inc << " "
//            << data.M_kick_rand << " "
//            << data.M_team_actuator_noise << " "
//            << data.M_prand_factor_l << " "
//            << data.M_prand_factor_r << " "
//            << data.M_kick_rand_factor_l << " "
//            << data.M_kick_rand_factor_r << " "
//            << data.M_bsize << " "
//            << data.M_bdecay << " "
//            << data.M_brand << " "
//            << data.M_bweight << " "
//            << data.M_bspeed_max << " "
//            << data.M_baccel_max << " "
//            << data.M_dprate << " "
//            << data.M_kprate << " "
//            << data.M_kmargin << " "
//            << data.M_ctlradius << " "
//            << data.M_ctlradius_width << " "
//            << data.M_maxp << " "
//            << data.M_minp << " "
//            << data.M_maxm << " "
//            << data.M_minm << " "
//            << data.M_maxnm << " "
//            << data.M_minnm << " "
//            << data.M_maxn << " "
//            << data.M_minn << " "
//            << data.M_visangle << " "
//            << data.M_visdist << " "
//            << data.M_windir << " "
//            << data.M_winforce << " "
//            << data.M_winang << " "
//            << data.M_winrand << " "
//            << data.M_kickable_area << " "
//            << data.M_catch_area_l << " "
//            << data.M_catch_area_w << " "
//            << data.M_catch_prob << " "
//            << data.M_goalie_max_moves << " "
//            << data.M_ckmargin << " "
//            << data.M_offside_area << " "
//            << data.M_win_no << " "
//            << data.M_win_random << " "
//            << data.M_say_cnt_max << " "
//            << data.M_SayCoachMsgSize << " "
//            << data.M_clang_win_size << " "
//            << data.M_clang_define_win << " "
//            << data.M_clang_meta_win << " "
//            << data.M_clang_advice_win << " "
//            << data.M_clang_info_win << " "
//            << data.M_clang_mess_delay << " "
//            << data.M_clang_mess_per_cycle << " "
//            << data.M_half_time << " "
//            << data.M_sim_st << " "
//            << data.M_send_st << " "
//            << data.M_recv_st << " "
//            << data.M_sb_step << " "
//            << data.M_lcm_st << " "
//            << data.M_say_msg_size << " "
//            << data.M_hear_max << " "
//            << data.M_hear_inc << " "
//            << data.M_hear_decay << " "
//            << data.M_cban_cycle << " "
//            << data.M_slow_down_factor << " "
//            << data.M_useoffside << " "
//            << data.M_kickoffoffside << " "
//            << data.M_offside_kick_margin << " "
//            << data.M_audio_dist << " "
//            << data.M_dist_qstep << " "
//            << data.M_land_qstep << " "
//            << data.M_dir_qstep << " "
//            << data.M_dist_qstep_l << " "
//            << data.M_dist_qstep_r << " "
//            << data.M_land_qstep_l << " "
//            << data.M_land_qstep_r << " "
//            << data.M_dir_qstep_l << " "
//            << data.M_dir_qstep_r << " "
//            << data.M_CoachMode << " "
//            << data.M_CwRMode << " "
//            << data.M_old_hear << " "
//            << data.M_sv_st << " "
//            << data.M_start_goal_l << " "
//            << data.M_start_goal_r << " "
//            << data.M_fullstate_l << " "
//            << data.M_fullstate_r << " "
//            << data.M_drop_time << ")";
// }


// std::ostream& toStr ( std::ostream& o, const ServerParamSensor_v8::data_t& data )
// {

//   o << "(server_param ";
//   std::for_each( data.m_sp.verMap().begin(), data.m_sp.verMap().end(), ServerParam::Printer( o, 8 ) );
//   o << ")";

//   return o;
// }
