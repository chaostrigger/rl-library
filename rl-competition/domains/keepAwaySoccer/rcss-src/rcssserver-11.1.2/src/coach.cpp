// -*-c++-*-

/***************************************************************************
                              coach.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "coach.h"

#include "audio.h"
#include "clangparser.h"
#include "clangmsg.h"
#include "clangmsgbuilder.h"
#include "field.h"
//#include "monitor.h"
#include "object.h"
#include "player.h"
#include "serializer.h"
#include "xpmholder.h"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>

#include <netinet/in.h>

namespace {

PlayMode
PlayModeID( char *mode )
{
    static char *PlayModeString[] = PLAYMODE_STRINGS;

    for ( int n = 0; n < PM_MAX; ++n )
    {
        if( ! std::strcmp( PlayModeString[n], mode ) )
            return (PlayMode)n;
    }
    return PM_Null;
}

void
chop_last_parenthesis( char *str, int max_size )
{
    int l = std::strlen( str );

    if ( l > max_size )
    {
        str[max_size] = NULLCHAR;
    }
    else
    {
        --l;
        if( str[l] == ')' ) str[l] = NULLCHAR;
    }
}

}


void
Coach::disable()
{
    M_assignedp = false;
    RemoteClient::close();
    std::cout << "An offline coach disconnected\n";
}


int
Coach::parse_init( Stadium& stad,
                   char *command,
                   const rcss::net::Addr& addr )
{
    char com[MaxStringSize] ;
    int n ;
    n = std::sscanf( command, "(%[-0-9a-zA-Z.+*/?<>_]", com );
    if( n < 1 )
    {
        send( "(error illegal_command_form)" );
        return 0;
    }

    if ( ! std::strcmp ( com, "init" ) )
    {
        float ver = 0.0;
        n = std::sscanf( command, "(init (version %f))", &ver );
        if ( n < 1.0 )
        {
            send( "(error illegal_command_form)" );
            return 0;
        }

        if( ! connect( addr ) )
        {
            return 0;
        }

        if ( ! setSenders( ver ) )
        {
            std::cerr << "Error: Could not find serializer or sender for version"
                      << (int)version() << std::endl;
            send( "(error illegal_client_version)" );
            return 0;
        }

        setEnforceDedicatedPort( ver >= 8.0 );
        stad.addOfflineCoach( this );

        stad.addListener( this );

        sendInit();

        sendServerParams();
        sendPlayerParams();
        sendPlayerTypes();
        sendChangedPlayers();
        M_assignedp = true;
        return 1;
    }
    else
    {
        // old type of coach that doesn;t send an init message
        M_assignedp = true;
        return -1;
    }
}


bool
Coach::setSenders( const Value & client_version )
{
    M_version = client_version;

    rcss::SerializerCoach::Creator ser_cre;
    if ( ! rcss::SerializerCoach::factory().getCreator( ser_cre,
                                                        (int)client_version ) )
    {
        return false;
    }

    const rcss::SerializerCoach* ser = ser_cre();
    if( ! ser )
    {
        return false;
    }

    rcss::AudioSenderCoach::Params audio_params( *M_stadium,
                                                 getTransport(),
                                                 *this,
                                                 *ser );

    rcss::AudioSenderCoach::Creator audio_cre;
    if( ! rcss::AudioSenderCoach::factory().getCreator( audio_cre,
                                                        (int)client_version ) )
    {
        return false;
    }
    setAudioSender( audio_cre( audio_params ) );

    rcss::InitSenderOfflineCoach::Params init_params( getTransport(),
                                                      *this,
                                                      *M_stadium,
                                                      *ser );
    rcss::InitSenderOfflineCoach::Creator init_cre;
    if ( ! rcss::InitSenderOfflineCoach::factory().getCreator( init_cre,
                                                               (int)client_version ) )
    {
        return false;
    }

    rcss::InitSenderOfflineCoach::Ptr isoc = init_cre( init_params );
    rcss::InitObserver<rcss::InitSenderOfflineCoach>::setInitSender( isoc );

    return true;
}


void
Coach::parse_command( const char *command )
{
    char com[MaxStringSize];
    int n;

    n = std::sscanf( command, "(%[-0-9a-zA-Z.+*/?<>_]", com );
    if ( n < 1 )
    {
        send( "(error illegal_command_form)" );
        return ;
    }

    if ( ! std::strcmp ( com, "start" ) )
    {
        Stadium::_Start ( *M_stadium );
        send( "(ok start)" );
    }
    else if ( ! std::strcmp( com,"change_mode" ) )
    {
        char new_mode[MaxStringSize] ;
        PlayMode mode_id ;
        n = std::sscanf( command, "(change_mode %[-0-9a-zA-Z.+*/?<>_])", new_mode );
        if ( n < 1 )
        {
            send( "(error illegal_command_form)" );
            return;
        }
        mode_id = PlayModeID( new_mode ) ;

        if ( mode_id == PM_Null )
        {
            send( "(error illegal_mode)" );
            return;
        }
        M_stadium->change_play_mode( mode_id );
        send( "(ok change_mode)" );
    }
    else if ( ! std::strcmp( com,"move" ) )
    {
        char obj[MaxStringSize];
        double x = 0.0, y = 0.0, ang = 0.0, velx = 0.0, vely = 0.0;

        ++command;
        while ( *command != '(' )
        {
            if ( *command == NULLCHAR )
            {
                send( "(error illegal_object_form)" );
                return;
            }
            ++command;
        }

        int i = 0;
        while ( *command != ')' )
        {
            if ( *command == NULLCHAR )
            {
                send( "(error illegal_object_form)" );
                return ;
            }
            *(obj+i) = *command;
            ++i; ++command;
        }
        *(obj+i) = *command;
        ++i; ++command;
        *(obj+i) = NULLCHAR;

        if ( ! std::strcmp( obj, BALL_NAME ) )
        {
            M_stadium->clearBallCatcher();
        }

        n = std::sscanf( command," %lf %lf %lf %lf %lf)",
                         &x,&y,&ang,&velx,&vely );
        if ( std::isnan( x ) != 0 ||
             std::isnan( y ) != 0 ||
             std::isnan( ang ) != 0 ||
             std::isnan( velx ) != 0 ||
             std::isnan( vely ) != 0
             )
        {
            send( "(error illegal_command_form)" );
            return;
        }

        if ( n == 2 )
        {
            if ( ! std::strcmp( obj, BALL_NAME ) )
            {
                M_stadium->set_ball( LEFT, PVector( x, y ) );
            }
            else if ( M_stadium->movePlayer( obj, PVector( x, y ) ) )
            {

            }
            else
            {
                send( "(error illegal_object_form)" );
                return;
            }
        }
        else if ( n == 3 )
        {
            if ( ! std::strcmp( obj, BALL_NAME ) )
            {
                M_stadium->set_ball( LEFT, PVector( x, y ) );
            }
            else if ( M_stadium->movePlayer( obj,
                                             PVector( x, y ),
                                             Deg2Rad( std::max( std::min( ang,
                                                                          ServerParam::instance().maxm ),
                                                                ServerParam::instance().minm ) ) ) )
            {

            }
            else
            {
                send( "(error illegal_object_form)" );
                return;
            }
        }
        else if ( n == 5 )
        {
            if ( ! std::strcmp( obj, BALL_NAME ) )
            {
                M_stadium->set_ball( NEUTRAL, PVector( x, y ), PVector( velx, vely ) );
            }
            else if ( M_stadium->movePlayer( obj, PVector( x, y ),
                                             Deg2Rad( std::max( std::min( ang,
                                                                          ServerParam::instance().maxm ),
                                                                ServerParam::instance().minm ) ),
                                             PVector( velx, vely ) ) )
            {

            }
            else
            {
                send( "(error illegal_object_form)" );
                return;
            }
        }
        else
        {
            send( "(error illegal_command_form)" );
            return ;
        }
        send( "(ok move)" );
    }
    else if ( ! std::strcmp( com,"look" ) )
    {
        look( *M_stadium ) ;
    }
    else if ( ! std::strcmp( com,"team_names" ) )
    {
        team_names( *M_stadium ) ;
    }
    else if ( ! std::strcmp( com, "recover" ) )
    {
        recover( *M_stadium );
    }
    else if ( ! std::strcmp( com, "check_ball" ) )
    {
        check_ball( *M_stadium ) ;
    }
    else if( ! std::strcmp( com, "say" ) )
    {
        static char msg[MaxStringSize] ;
        n = std::sscanf( command, SAY_MESSAGE_SCAN_FORMAT, com, msg );
        if( n < 2 )
        {
            send( "(error illegal_command_form)" );
            return;
        }
        chop_last_parenthesis( msg, ServerParam::instance().SayCoachMsgSize );
        M_stadium->say( msg, false );
        send( "(ok say)" );
    }
    else if ( ! std::strcmp( com,"ear" ) )
    {
        char mode[16] ;
        n = std::sscanf( command,"(%s %s)", com, mode );
        if ( n < 2)
        {
            send( "(error illegal_command_form)" );
            return;
        }
        if ( ! std::strcmp( mode, "on)" ) )
        {
            M_hear = true;
            send( "(ok ear on)" );
        }
        else if ( ! std::strcmp( mode, "off)" ) )
        {
            M_hear = false;
            send( "(ok ear off)" );
        }
        else
        {
            send("(error illegal_command_form)");
            return ;
        }
    }
    else if ( ! std::strcmp( com, "eye" ) )
    {
        char mode[16] ;
        n = std::sscanf( command, "(%s %s)", com, mode );
        if ( n < 2)
        {
            send( "(error illegal_command_form)" );
            return;
        }

        if ( ! std::strcmp( mode, "on)" ) )
        {
            M_eye = true;
            send( "(ok eye on)" );
        }
        else if ( ! std::strcmp( mode, "off)" ) )
        {
            M_eye = false;
            send( "(ok eye off)" );
        }
        else
        {
            send( "(error illegal_command_form)" );
            return ;
        }
    }
    else if ( ! std::strcmp ( com, "change_player_type" ) )
    {
        char name[128];
        int unum, player_type;
        if ( std::sscanf( command, "(%s %s %d %d)",
                          com, name, &unum, &player_type ) < 4 )
        {
            send( "(error illegal_command_form)" );
            return ;
        }

        const Team* team = NULL;
        if ( M_stadium->teamLeft().name() == name )
        {
            team = &( M_stadium->teamLeft() );
        }

        if ( M_stadium->teamRight().name() == name )
        {
            team = &( M_stadium->teamRight() );
        }

        if ( team == NULL )
        {
            send( "(warning no_team_found)" );
            return;
        }

        if ( player_type < 0
             || player_type >= PlayerParam::instance().playerTypes() )
        {
            send( "(error out_of_range_player_type)" );
            return;
        }

        const Player * player = NULL;
        for ( int i = 0; i < team->size(); i++ )
        {
            const Player * p = team->player( i );
            if ( p && p->unum() == unum )
            {
                player = p;
                break;
            }
        }

        if ( player == NULL )
        {
            send( "(warning no_such_player)" );
            return;
        }

        M_stadium->substitute( player, player_type );

        char buf[64];
        std::snprintf ( buf, 64, "(ok change_player_type %s %d %d)",
                        name, unum, player_type );

        send( buf );

        return ;
    }
    //pfr:SYNCH
    else if ( ! std::strcmp( com, "done" ) )
    {
        //std::cerr << "Recv trainer done" << std::endl;
        M_done_received = true;
        return ;
    }
    else if ( ! std::strcmp( com, "compression" ) )
    {
        int level ;
        if ( std::sscanf( command, "(%s %d)", com, &level ) < 2)
        {
            send( "(error illegal_command_form)" );
            return ;
        }
#ifdef HAVE_LIBZ
        if( level > 9 )
        {
            send( "(error illegal_command_form)" );
            return ;
        }


#ifdef HAVE_SSTREAM
        std::ostringstream reply;
        reply << "(ok compression " << level << ")";
        send ( reply.str().c_str() );
#else
        std::ostrstream reply;

        reply << "(ok compression " << level << ")" << std::ends;
        send ( reply.str() );
        reply.freeze( false );
#endif
        setCompressionLevel ( level );

#else
        send ( "(warning compression_unsupported)" );
#endif
        return ;
    }
    else
    {
        send( "(error unknown_command)" );
        return ;
    }
}


void
Coach::send( const char* msg )
{
    if ( RemoteClient::send( msg, std::strlen( msg ) + 1 ) != -1 )
    {
        M_stadium->writeTextLog( *this, msg, SEND );
    }
}

void
Coach::sendExternalMsg()
{
    if ( ServerParam::instance().coachMsgFile().empty() )
    {
        return;
    }

    std::ifstream fin( ServerParam::instance().coachMsgFile().c_str() );
    if ( ! fin.is_open() )
    {
        return;
    }

    std::vector< char > buf;
    std::copy( std::istreambuf_iterator< char >( fin ),
               std::istreambuf_iterator< char >(),
               std::back_inserter( buf ) );

    if ( buf.empty() )
    {
        return;
    }

    if ( buf.size() >= MaxMesg - std::strlen( "(include 8192 )" ) )
    {
        std::cerr << "Too large messages in " << ServerParam::instance().coachMsgFile()
                  << std::endl;
        return;
    }

    std::string msg = "(include ";
    msg += boost::lexical_cast< std::string >( buf.size() );
    msg += ' ';
    msg.append( buf.begin(), buf.end() );
    msg += ')';

    std::cerr << "send external message in [" << ServerParam::instance().coachMsgFile()
              << "] " << msg
              << std::endl;

    send( msg.c_str() );
}

void
Coach::look( Stadium& stad )
{
#ifdef HAVE_SSTREAM
    std::ostringstream ost;
#else
    std::ostrstream ost;
#endif

    ost << "(ok look " << stad.time();
    if ( version() >= 7.0 )
        ost << " (" << GOAL_L_NAME_SHORT;
    else
        ost << " (" << GOAL_L_NAME;

    ost << " " << -(PITCH_LENGTH/2.0)
        << " " << 0.0
        << ")" ;

    if ( version() >= 7.0 )
        ost << " (" << GOAL_R_NAME_SHORT;
    else
        ost << " (" << GOAL_R_NAME;

    ost << " " << (PITCH_LENGTH/2.0)
        << " " << 0.0
        << ")" ;
    if ( version() >= 7.0 )
        ost << " (" << BALL_NAME_SHORT;
    else
        ost << " (" << BALL_NAME;

    ost << " " << stad.ball().pos().x
        << " " << stad.ball().pos().y
        << " " << stad.ball().vel().x
        << " " << stad.ball().vel().y
        << ")" ;

    const Stadium::PlayerCont::const_iterator end = stad.players().end();
    for ( Stadium::PlayerCont::const_iterator p = stad.players().begin();
          p != end;
          ++p )
    {
        if ( (*p)->alive == DISABLE )
            continue ;
        if ( version() >= 7.0 )
            ost << " (" << (*p)->shortName()
                << " " << (*p)->pos().x
                << " " << (*p)->pos().y
                << " " << (*p)->vel().x
                << " " << (*p)->vel().y
                << " " << Rad2IDegRound( (*p)->angleBodyCommitted() )
                << " " << Rad2IDegRound( (*p)->angleNeckCommitted() )
                << ")" ;
        else
            ost << " (" << (*p)->name()
                << " " << (*p)->pos().x
                << " " << (*p)->pos().y
                << " " << Rad2IDeg( (*p)->angleBodyCommitted() )
                << " " << Rad2IDeg( (*p)->angleNeckCommitted() )
                << " " << (*p)->vel().x
                << " " << (*p)->vel().y
                << ")" ;
    }
    ost << ")" << std::endl;
#ifdef HAVE_SSTREAM
    send(ost.str().c_str());
#else
    ost << std::ends;
    send(ost.str());
    ost.freeze();
#endif

}

void
Coach::team_names( Stadium & stad )
{
#ifdef HAVE_SSTREAM
    std::ostringstream ost;
#else
    std::ostrstream ost;
#endif

    ost << "(ok team_names";

    if ( ! stad.teamLeft().name().empty() )
    {
        ost << " (team l " << stad.teamLeft().name() << ")";
    }

    if ( ! stad.teamRight().name().empty() )
    {
        ost << " (team r " << stad.teamRight().name() << ")";
    }

    ost << ")" << std::endl;
#ifdef HAVE_SSTREAM
    send( ost.str().c_str() );
#else
    ost<< std::ends;
    send( ost.str() );
    ost.freeze();
#endif
}

void
Coach::recover( Stadium& stad )
{
    stad.recoveryPlayers();

#ifdef HAVE_SSTREAM
    std::ostringstream ost;
    ost << "(ok recover)" << std::endl;
    send(ost.str().c_str());
#else
    std::ostrstream ost;
    ost << "(ok recover)" << std::endl << std::ends;
    send(ost.str()) ;
    ost.freeze();
#endif
}



void
Coach::send_visual_info()
{
#ifdef HAVE_SSTREAM
    std::ostringstream ost;
#else
    std::ostrstream ost;
#endif

    if ( version() >= 7.0 )
        ost << "(see_global " << M_stadium->time();
    else
        ost << "(see " << M_stadium->time();

    if ( version() >= 7.0 )
        ost << " (" << GOAL_L_NAME_SHORT;
    else
        ost << " (" << GOAL_L_NAME;
    ost << " " << -(PITCH_LENGTH/2.0)
        << " " << 0.0
        << ")" ;

    if ( version() >= 7.0 )
        ost << " (" << GOAL_R_NAME_SHORT;
    else
        ost << " (" << GOAL_R_NAME;
    ost << " " << (PITCH_LENGTH/2.0)
        << " " << 0.0
        << ")" ;

    if ( version() >= 7.0 )
        ost << " (" << BALL_NAME_SHORT;
    else
        ost << " (" << BALL_NAME;
    ost << " " << M_stadium->ball().pos().x
        << " " << M_stadium->ball().pos().y
        << " " << M_stadium->ball().vel().x
        << " " << M_stadium->ball().vel().y
        << ")" ;

    const Stadium::PlayerCont::const_iterator end = M_stadium->players().end();
    for ( Stadium::PlayerCont::const_iterator p = M_stadium->players().begin();
          p != end;
          ++p )
    {
        if ( (*p)->alive == DISABLE )
            continue ;

        if ( version() >= 7.0 )
        {
            ost << " (" << (*p)->shortName()
                << " " << (*p)->pos().x
                << " " << (*p)->pos().y
                << " " << (*p)->vel().x
                << " " << (*p)->vel().y
                << " " << Rad2IDegRound( (*p)->angleBodyCommitted() )
                << " " << Rad2IDegRound( (*p)->angleNeckCommitted() );
            if ( version() >= 8.0 )
            {
                double arm_dir;
                if ( (*p)->getArm().getRelDir ( rcss::geom::Vector2D( (*p)->pos().x,
                                                                      (*p)->pos().y ),
                                                (*p)->angleBodyCommitted()
                                                + (*p)->angleNeckCommitted(),
                                                arm_dir ) )
                    ost << " " << Rad2IDegRound( normalize_angle( arm_dir
                                                                  + (*p)->angleNeckCommitted()
                                                                  + (*p)->angleBodyCommitted() ) );
                if ( (*p)->isTackling() )
                    ost << " t";
            }
            ost << ")" ;
        }
        else
        {
            ost << " (" << (*p)->name()
                << " " << (*p)->pos().x
                << " " << (*p)->pos().y
                << " " << Rad2IDeg( (*p)->angleBodyCommitted() )
                << " " << Rad2IDeg( (*p)->angleNeckCommitted() )
                << " " << (*p)->vel().x
                << " " << (*p)->vel().y
                << ")" ;
        }
    }
    ost << ")" << std::endl;
#ifdef HAVE_SSTREAM
    send( ost.str().c_str() );
#else
    ost << std::ends;

    send( ost.str() );
    ost.freeze( false );
#endif
}

void
Coach::check_ball( Stadium & stad )
{
#ifdef HAVE_SSTREAM
    std::ostringstream ost;
#else
    std::ostrstream ost;
#endif

    static char* BallPosInfoStr[] = BALL_POS_INFO_STRINGS;
    BallPosInfo info = stad.ballPosInfo();

    ost << "(ok check_ball " << stad.time() << " " ;

    ost << BallPosInfoStr[info] << ")";

    ost << std::ends;

#ifdef HAVE_SSTREAM
    send( ost.str().c_str() );
#else
    ost << std::ends;

    send( ost.str() );
    ost.freeze( false );
#endif
}



OnlineCoach::OnlineCoach( Stadium *stad )
    : Coach( stad )
{
    //angle = Deg2Rad( 90 );
    M_side = NEUTRAL;
    M_freeform_messages_said = 0;
    M_freeform_messages_allowed = ServerParam::instance().say_cnt_max;
    M_define_messages_left = -1;
    M_advice_messages_left = -1;
    M_info_messages_left = -1;
    M_meta_messages_left = -1;
    M_del_messages_left = -1;
    M_rule_messages_left = -1;
    M_msg_left_update_time = -1;
}

OnlineCoach::~OnlineCoach()
{

}

void
OnlineCoach::disable()
{
    M_assignedp = false;
    RemoteClient::close();
    std::cout << "An online coach disconnected\n";
}

void
OnlineCoach::awardFreeformMessageCount()
{
    if ( M_freeform_messages_allowed > 0 )
    {
        M_freeform_messages_allowed += ServerParam::instance().say_cnt_max;
    }
}


void
OnlineCoach::parse_command( const char *command )
{
    char com[MaxStringSize];
    int n;

    n = std::sscanf( command, "(%[-0-9a-zA-Z.+*/?<>_]", com );
    if ( n < 1 )
    {
        send( "(error illegal_command_form)" );
        return;
    }

    if ( ! std::strcmp( com,"check_ball" ) )
    {
        check_ball( *M_stadium ) ;
    }
    else if ( ! std::strcmp( com, "look" ) )
    {
        look( *M_stadium ) ;
    }
    else if ( ! std::strcmp( com, "team_names" ) )
    {
        team_names( *M_stadium ) ;
    }
    else if ( ! std::strcmp( com, "say" ) )
    {
        if ( version() >= 7.0 )
        {
            /* parsing for new coach language */

            rcss::clang::MsgBuilder builder;
            rcss::clang::Parser parser( builder );

            int ret = parser.parse( command );
            if ( ret == 0 && builder.getMsg() != NULL )
            {
                //succeful parse
                bool should_queue = false;

                rcss::clang::Msg* msg = builder.detatchMsg().release();

                msg->setTimeRecv( M_stadium->time() );

                switch( msg->getType() )
                {
                case rcss::clang::Msg::META:
                    if ( M_stadium->playmode() != PM_PlayOn )
                    {
                        should_queue = true;
                    }
                    else if ( M_meta_messages_left != 0 )
                    {
                        //remember, negative implies we can send as many as we want
                        --M_meta_messages_left;
                        should_queue = true;
                    }
                    else
                    {
                        send( "(error said_too_many_meta_messages)" );
                    }
                    break;
                case rcss::clang::Msg::FREEFORM:
                    if ( M_stadium->playmode() != PM_PlayOn
                         || M_stadium->canSendFreeform() )
                    {
                        if ( M_freeform_messages_said < M_freeform_messages_allowed
                             || M_freeform_messages_allowed < 0 )
                        {
                            should_queue = true;
                            ++M_freeform_messages_said;
                        }
                        else
                        {
                            send( "(error said_too_many_freeform_messages)" );
                        }
                    }
                    else
                    {
                        send( "(error cannot_say_freeform_while_playon)" );
                    }
                    break;
                case rcss::clang::Msg::INFO:
                    if( M_stadium->playmode() != PM_PlayOn )
                    {
                        should_queue = true;
                    }
                    else if ( M_info_messages_left != 0 )
                    {
                        //remember, negative implies we can send as many as we want
                        --M_info_messages_left;
                        should_queue = true;
                    }
                    else
                    {
                        send( "(error said_too_many_info_messages)" );
                    }
                    break;
                case rcss::clang::Msg::ADVICE:
                    if( M_stadium->playmode() != PM_PlayOn )
                    {
                        should_queue = true;
                    }
                    else if ( M_advice_messages_left != 0 )
                    {
                        //remember, negative implies we can send as many as we want
                        --M_advice_messages_left;
                        should_queue = true;
                    }
                    else
                    {
                        send( "(error said_too_many_advice_messages)" );
                    }
                    break;
                case rcss::clang::Msg::DEFINE:
                    if ( M_stadium->playmode() != PM_PlayOn )
                    {
                        should_queue = true;
                    }
                    else if ( M_define_messages_left != 0 )
                    {
                        //remember, negative implies we can send as many as we want
                        --M_define_messages_left;
                        should_queue = true;
                    }
                    else
                    {
                        send( "(error said_too_many_define_messages)" );
                    }
                    break;
                case rcss::clang::Msg::DEL:
                    if( M_stadium->playmode() != PM_PlayOn )
                    {
                        should_queue = true;
                    }
                    else if ( M_del_messages_left != 0 )
                    {
                        //remember, negative implies we can send as many as we want
                        --M_del_messages_left;
                        should_queue = true;
                    }
                    else
                    {
                        send( "(error said_too_many_del_messages)" );
                    }
                    break;
                case rcss::clang::Msg::RULE:
                    if( M_stadium->playmode() != PM_PlayOn )
                    {
                        should_queue = true;
                    }
                    else if ( M_rule_messages_left != 0 )
                    {
                        //remember, negative implies we can send as many as we want
                        --M_rule_messages_left;
                        should_queue = true;
                    }
                    else
                    {
                        send( "(error said_too_many_rule_messages)" );
                    }
                    break;
                default:
                    std::cerr << __FILE__ << ": " << __LINE__
                              << ": Coach msg has an unknown type: "
                              << msg << std::endl;
                }

                if ( should_queue )
                {
                    M_message_queue.push_back( msg );
                    send( "(ok say)" );
                }
                else
                {
                    delete msg;
                }
            }
            else
            {
                send("(error could_not_parse_say)");
            }
        }
        else
        {
            //pre v7.0
            if( M_stadium->playmode() != PM_PlayOn )
            {
                if( M_freeform_messages_said < M_freeform_messages_allowed
                    || M_freeform_messages_allowed < 0 )
                {
                    static char msg[MaxStringSize] ;
                    n = std::sscanf( command, SAY_MESSAGE_SCAN_FORMAT, com, msg );
                    if( n < 2 )
                    {
                        send( "(error illegal_command_form)" );
                        return ;
                    }
                    chop_last_parenthesis( msg, ServerParam::instance().SayCoachMsgSize );
                    say( msg );
                    send( "(ok say)" );
                }
                else
                {
                    send( "(error said_too_many_messages)" );
                    return;
                }
            }
            else
            {
                send( "(warning cannot_say_while_playon)" );
                return ;
            }
        }
    }
    else if ( ! std::strcmp( com, "bye" ) )
    {
        M_assignedp = false;
        return ;
    }
    else if ( ! std::strcmp( com, "eye" ) )
    {
        char mode[16] ;
        n = std::sscanf( command, "(%s %s)", com, mode );
        if ( n < 2 )
        {
            send( "(error illegal_command_form)" );
            return;
        }
        if ( ! std::strcmp( mode, "on)" ) )
        {
            M_eye = true;
            send( "(ok eye on)" );
        }
        else if ( ! std::strcmp( mode, "off)" ) )
        {
            M_eye = false;
            send( "(ok eye off)" );
        }
        else
        {
            send( "(error illegal_command_form)" );
            return ;
        }
    }
    else if ( ! std::strcmp ( com, "change_player_type" ) )
    {
        if ( M_stadium->playmode() == PM_PlayOn )
        {
            send( "(warning cannot_sub_while_playon)" );
            return ;
        }
        // when time elapsed, do not allow substitutions anymore (for penalties)
        else if ( M_stadium->time() >= ServerParam::instance().halfTime() *
                  ( ServerParam::instance().nr_normal_halfs +
                    ServerParam::instance().nr_extra_halfs ) )
        {
            send( "(warning no_subs_left)" );
            return ;
        }

        const Team * team = NULL;

        if ( side() == LEFT )
        {
            team = &( M_stadium->teamLeft() );
        }
        if ( side() == RIGHT )
        {
            team = &( M_stadium->teamRight() );
        }

        if ( team == NULL )
        {
            send( "(warning no_team_found)" );
            return ;
        }

        if ( team->subsCount() >= PlayerParam::instance().subsMax() )
        {
            send( "(warning no_subs_left)" );
            return ;
        }


        int unum, player_type;
        if ( std::sscanf( command, "(%s %d %d)", com, &unum, &player_type ) < 3 )
        {
            send( "(error illegal_command_form)" );
            return ;
        }

        if ( player_type < 0
             || player_type >= PlayerParam::instance().playerTypes() )
        {
            send( "(error out_of_range_player_type)" );
            return;
        }

        const Player * player = NULL;
        for ( int i = 0; i < team->size(); ++i )
        {
            const Player * p = team->player( i );
            if ( p && p->unum() == unum )
            {
                player = p;
                break;
            }
        }

        if ( player == NULL )
        {
            send( "(warning no_such_player)" );
            return ;
        }

        if ( player->isGoalie() && player_type != 0 )
        {
            send( "(warning cannot_change_goalie)" );
            return ;
        }

        if ( player_type != 0
             //&& team->ptype_count[ player_type ] == PlayerParam::instance().ptMax()
             && team->ptypeCount( player_type ) >= PlayerParam::instance().ptMax()
             && player_type != player->playerTypeId() )
        {
            send( "(warning max_of_that_type_on_field)" );
            return ;
        }

        //         team->ptype_count[ player->player_type_id ] -= 1;
        //         player->substitute( player_type );
        //         if ( M_stadium->mode != PM_BeforeKickOff )
        //         {
        //             team->M_subs_count++;
        //         }
        //         team->ptype_count[ player_type ] += 1;
        M_stadium->substitute( player, player_type );

        char buf[64];
        std::snprintf( buf, 64, "(ok change_player_type %d %d)\n",
                       unum, player_type );
        send( buf );

        return ;
        //pfr:SYNCH
    }
    else if ( ! std::strcmp( com, "done" ) )
    {
        //std::cerr << "Recv olc done" << std::endl;
        M_done_received = true;
        return ;
    }
    else if ( ! std::strcmp( com, "compression" ) )
    {
        int level ;
        if ( std::sscanf( command, "(%s %d)", com, &level ) < 2 )
        {
            send( "(error illegal_command_form)" );
            return ;
        }
#ifdef HAVE_LIBZ
        if ( level > 9 )
        {
            send( "(error illegal_command_form)" );
            return ;
        }

#ifdef HAVE_SSTREAM
        std::ostringstream reply;

        reply << "(ok compression " << level << ")";
        send ( reply.str().c_str() );
#else
        std::ostrstream reply;

        reply << "(ok compression " << level << ")" << std::ends;
        send ( reply.str() );
        reply.freeze( false );
#endif

        setCompressionLevel ( level );

#else
        send ( "(warning compression_unsupported)" );
#endif
        return ;
    }
    else if ( ! std::strcmp( com, "team_graphic" ) )
    {
        if( M_stadium->playmode() != PM_BeforeKickOff )
        {
            send( "warning only_before_kick_off" );
            return;
        }

        unsigned int x, y;
        n = std::sscanf( command, "(%s (%u %u", com, &x, &y );
        if ( n < 3 )
        {
            send( "(error illegal_command_form)" );
            return;
        }

        if ( x >= 32 || y >= 8 )
            // needs to be parameterised
        {
            send( "(warning invalid_tile_location)" );
        }

        std::auto_ptr< XPMHolder > holder( new XPMHolder( command ) );
        if ( !holder->valid() )
        {
            send( "(error illegal_command_form)" );
            return;
        }

        if ( holder->getWidth() != 8 || holder->getHeight() != 8 )
            // needs to be parameterised
        {
            send( "(warning invalid_tile_size)" );
            return;
        }

        M_stadium->sendGraphic( side(), x, y, holder );

#ifdef HAVE_SSTREAM
        std::ostringstream msg;
        msg << "(ok team_graphic " << x << " " << y << ")";
        send( msg.str().c_str() );
#else
        std::ostrstream msg;
        msg << "(ok team_graphic " << x << " " << y << ")" << std::ends;
        send( msg.str() );
        msg.freeze( false );
#endif
    }
    else
    {
        send( "(error unknown_command)" );
        return;
    }
}

void
OnlineCoach::sendPlayerClangVer()
{
    for ( Stadium::PlayerCont::iterator i = M_stadium->remotePlayers().begin();
          i != M_stadium->remotePlayers().end();
          ++i )
    {
        if ( (*i)->getClangMinVer() != 0
             || (*i)->getClangMaxVer() != 0 )
        {
            sendPlayerClangVer( **i );
        }
    }
}

void
OnlineCoach::sendPlayerClangVer( const Player& player )
{
#ifdef HAVE_SSTREAM
    std::ostringstream msg;
    getSerializer().serializePlayerClangVer( msg,
                                             player.shortName(),
                                             player.getClangMinVer(),
                                             player.getClangMaxVer() );
    send( msg.str().c_str() );
#else
    std::ostrstream msg;
    getSerializer().serializePlayerClangVer( msg,
                                             player.shortName(),
                                             player.getClangMinVer(),
                                             player.getClangMaxVer() );
    msg << std::ends;
    send( msg.str() );
    msg.freeze( false );
#endif
}


int
OnlineCoach::check_message_queue( TheNumber time )
{
    if ( M_message_queue.size() <= 0 )
    {
        return 0;
    }

    int messages_sent = 0;
    for ( int i = ServerParam::instance().clang_mess_per_cycle;
          i > 0 && M_message_queue.size() > 0;
          ++i )
    {
        rcss::clang::Msg* msg = *( M_message_queue.begin() );

        if ( M_stadium->playmode() != PM_PlayOn ||
             time - msg->getTimeRecv() >= ServerParam::instance().clang_mess_delay)
        {
            msg->setTimeSend( M_stadium->time() );
            msg->setSide( side() );
            say( *msg );

            M_message_queue.pop_front();
            delete msg;
            ++messages_sent;
        }
        else
        {
            break;
        }
    }
    return messages_sent;
}


void
OnlineCoach::say( const rcss::clang::Msg& message )
{
    M_stadium->sendCoachStdAudio( *this, message );
}


void
OnlineCoach::say( char *message, bool )
{
    M_stadium->sendCoachAudio( *this, message );
}


bool
OnlineCoach::update_messages_left( TheNumber time )
{
    if ( M_msg_left_update_time >= 0 &&
         time - M_msg_left_update_time < ServerParam::instance().clang_win_size )
    {
        return false;
    }

    M_define_messages_left = ServerParam::instance().clang_define_win;
    M_meta_messages_left = ServerParam::instance().clang_meta_win;
    M_advice_messages_left = ServerParam::instance().clang_advice_win;
    M_info_messages_left = ServerParam::instance().clang_info_win;
    M_del_messages_left = ServerParam::instance().clangDelWin();
    M_rule_messages_left = ServerParam::instance().clangRuleWin();

    M_msg_left_update_time = time;

    return true;
}


bool
OnlineCoach::setSenders( const Value & client_version )
{
    M_version = client_version;

    rcss::SerializerOnlineCoach::Creator creator;

    if ( ! rcss::SerializerOnlineCoach::factory().getCreator( creator,
                                                              (int)client_version ) )
    {
        return false;
    }

    const rcss::SerializerOnlineCoach* ser = creator();
    if ( ! ser )
    {
        return false;
    }
    this->setSerializer( *ser );

    rcss::AudioSenderOnlineCoach::Params audio_params( *M_stadium,
                                                       getTransport(),
                                                       *this,
                                                       *ser );
    rcss::AudioSenderOnlineCoach::Creator audio_cre;
    if ( ! rcss::AudioSenderOnlineCoach::factory().getCreator( audio_cre,
                                                               (int)client_version ) )
    {
        return false;
    }
    this->setAudioSender( audio_cre( audio_params ) );

    rcss::InitSenderOnlineCoach::Params init_params( getTransport(),
                                                     *this,
                                                     *M_stadium,
                                                     *ser );
    rcss::InitSenderOnlineCoach::Creator init_cre;
    if ( ! rcss::InitSenderOnlineCoach::factory().getCreator( init_cre,
                                                              (int)client_version ) )
    {
        return false;
    }
    rcss::InitSenderOnlineCoach::Ptr isoc = init_cre( init_params );
    rcss::InitObserver< rcss::InitSenderOnlineCoach >::setInitSender( isoc );

    M_assignedp = true;
    return true;
}
