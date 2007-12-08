// -*-c++-*-

/***************************************************************************
                                   referee.h
                              Refereeing module
                             -------------------
    begin                : 16-May-2002
    copyright            : (C) 2001 by The RoboCup Soccer Server
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

#include "referee.h"

#include "field.h"
#include "player.h"

#include <limits>

char* KeepawayRef::trainingMsg = "training Keepaway 1";
const int KeepawayRef::TURNOVER_TIME = 4;

PVector
Referee::truncateToPitch( PVector ball_pos )
{
    ball_pos.x = std::min( ball_pos.x, +PITCH_LENGTH * 0.5 );
    ball_pos.x = std::max( ball_pos.x, -PITCH_LENGTH * 0.5 );
    ball_pos.y = std::min( ball_pos.y, +PITCH_WIDTH * 0.5 );
    ball_pos.y = std::max( ball_pos.y, -PITCH_WIDTH * 0.5 );

    return ball_pos;
}

PVector
Referee::moveOutOfPenalty( const Side side,
                           PVector ball_pos )
{
    if ( side != RIGHT )
    {
        if ( ball_pos.x < -PITCH_LENGTH * 0.5 + PENALTY_AREA_LENGTH
             && std::fabs( ball_pos.y ) < PENALTY_AREA_WIDTH * 0.5 )
        {
            ball_pos.x = -PITCH_LENGTH * 0.5 + PENALTY_AREA_LENGTH;
            if ( ball_pos.y > 0 )
            {
                ball_pos.y = +PENALTY_AREA_WIDTH * 0.5;
            }
            else
            {
                ball_pos.y = -PENALTY_AREA_WIDTH * 0.5;
            }
        }
    }

    if ( side != LEFT )
    {
        if ( ball_pos.x > PITCH_LENGTH * 0.5 - PENALTY_AREA_LENGTH
             && std::fabs( ball_pos.y ) < PENALTY_AREA_WIDTH * 0.5 )
        {
            ball_pos.x = PITCH_LENGTH * 0.5 - PENALTY_AREA_LENGTH;
            if( ball_pos.y > 0 )
            {
                ball_pos.y = +PENALTY_AREA_WIDTH * 0.5;
            }
            else
            {
                ball_pos.y = -PENALTY_AREA_WIDTH * 0.5;
            }
        }
    }

    return ball_pos;
}

PVector
Referee::moveIntoPenalty( Side side, PVector pos )
{
    if ( side != RIGHT )
    {
        if ( pos.x > -PITCH_LENGTH * 0.5 + PENALTY_AREA_LENGTH + ServerParam::instance().bsize )
        {
            pos.x = -PITCH_LENGTH * 0.5 + PENALTY_AREA_LENGTH + ServerParam::instance().bsize;
        }

        if ( std::fabs( pos.y ) > PENALTY_AREA_WIDTH * 0.5 + ServerParam::instance().bsize )
        {
            if ( pos.y > 0 )
            {
                pos.y = PENALTY_AREA_WIDTH * 0.5 + ServerParam::instance().bsize;
            }
            else
            {
                pos.y = -PENALTY_AREA_WIDTH * 0.5 - ServerParam::instance().bsize;
            }
        }
    }
    if ( side != LEFT )
    {
        if ( pos.x < PITCH_LENGTH * 0.5 - PENALTY_AREA_LENGTH - ServerParam::instance().bsize )
        {
            pos.x = PITCH_LENGTH * 0.5 - PENALTY_AREA_LENGTH - ServerParam::instance().bsize;
        }

        if ( std::fabs( pos.y ) > PENALTY_AREA_WIDTH * 0.5 + ServerParam::instance().bsize )
        {
            if ( pos.y > 0 )
            {
                pos.y = PENALTY_AREA_WIDTH * 0.5 + ServerParam::instance().bsize;
            }
            else
            {
                pos.y = -PENALTY_AREA_WIDTH * 0.5 - ServerParam::instance().bsize;
            }
        }
    }

    return pos;
}



void
Referee::awardFreeKick( const Side side,
                        PVector pos )
{
    pos = truncateToPitch( pos );
    pos = moveOutOfPenalty( (Side)(-side), pos );

    if( side == LEFT )
    {
        M_stadium.placeBall( PM_FreeKick_Left, LEFT, pos );
    }
    else if( side == RIGHT )
    {
        M_stadium.placeBall( PM_FreeKick_Right, RIGHT, pos );
    }
}

void
Referee::awardGoalKick( const Side side,
                        PVector pos )
{
    if ( pos.y > 0 )
    {
        pos.y = GOAL_AREA_WIDTH * 0.5;
    }
    else
    {
        pos.y = -GOAL_AREA_WIDTH * 0.5;
    }

    M_stadium.clearBallCatcher();
    if ( side == LEFT )
    {
        pos.x = -PITCH_LENGTH * 0.5 + GOAL_AREA_LENGTH;
        M_stadium.placeBall( PM_GoalKick_Left, LEFT, pos );
    }
    else
    {
        pos.x = PITCH_LENGTH * 0.5 - GOAL_AREA_LENGTH;
        M_stadium.placeBall( PM_GoalKick_Right, RIGHT, pos );
    }
}

void
Referee::awardDropBall( PVector pos )
{
    M_stadium.clearBallCatcher();

    pos = truncateToPitch( pos );
    pos = moveOutOfPenalty( NEUTRAL, pos );

    M_stadium.placeBall( PM_Drop_Ball, NEUTRAL, pos );
    M_stadium.placePlayersInField();

    if( ! isPenaltyShootOut( M_stadium.playmode() ) )
    {
        M_stadium.change_play_mode( PM_PlayOn );
    }
}

void
Referee::awardKickIn( const Side side,
                      PVector pos )
{
    M_stadium.clearBallCatcher();

    pos = truncateToPitch( pos );

    if ( side == LEFT )
    {
        M_stadium.placeBall( PM_KickIn_Left, LEFT, pos );
    }
    else
    {
        M_stadium.placeBall( PM_KickIn_Right, RIGHT, pos );
    }
}


void
Referee::awardCornerKick( const Side side,
                          PVector pos )
{
    M_stadium.clearBallCatcher();

    if ( pos.y > 0 )
    {
        pos.y = PITCH_WIDTH * 0.5 - ServerParam::instance().ckmargin;
    }
    else
    {
        pos.y = -PITCH_WIDTH * 0.5 + ServerParam::instance().ckmargin;
    }

    if ( side == LEFT )
    {
        pos.x = PITCH_LENGTH * 0.5 - ServerParam::instance().ckmargin;
        M_stadium.placeBall( PM_CornerKick_Left, LEFT, pos );
    }
    else
    {
        pos.x = -PITCH_LENGTH * 0.5 + ServerParam::instance().ckmargin;
        M_stadium.placeBall( PM_CornerKick_Right, RIGHT, pos );
    }
}


bool
Referee::inPenaltyArea( const Side side,
                        const PVector & pos )
{
    if ( side != RIGHT )
    {
        // according to FIFA the ball is catchable if it is at
        // least partly within the penalty area, thus we add ball size
        static RArea pen_area( PVector( -PITCH_LENGTH/2+PENALTY_AREA_LENGTH/2.0, 0.0 ),
                               PVector( PENALTY_AREA_LENGTH
                                        + ServerParam::instance().bsize * 2,
                                        PENALTY_AREA_WIDTH
                                        + ServerParam::instance().bsize * 2 ) ) ;
        if ( pen_area.inArea( pos ) )
        {
            return true;
        }
    }

    if ( side != LEFT )
    {
        // according to FIFA the ball is catchable if it is at
        // least partly within the penalty area, thus we add ball size
        static RArea pen_area( PVector( +PITCH_LENGTH/2-PENALTY_AREA_LENGTH/2.0, 0.0 ),
                               PVector( PENALTY_AREA_LENGTH
                                        + ServerParam::instance().bsize * 2,
                                        PENALTY_AREA_WIDTH
                                        + ServerParam::instance().bsize * 2 ) ) ;
        if ( pen_area.inArea( pos ) )
        {
            return true;
        }
    }

    return false;
}


bool
Referee::isPenaltyShootOut( const PlayMode pm,
                            const Side side )
{
    bool bLeft = false, bRight = true;
    switch( pm ) {
    case PM_PenaltySetup_Left:
    case PM_PenaltyReady_Left:
    case PM_PenaltyTaken_Left:
    case PM_PenaltyMiss_Left:
    case PM_PenaltyScore_Left:
        bLeft = true;
        break;
    case PM_PenaltySetup_Right:
    case PM_PenaltyReady_Right:
    case PM_PenaltyTaken_Right:
    case PM_PenaltyMiss_Right:
    case PM_PenaltyScore_Right:
        bRight = true;
        break;
    default:
        return false;
    }

    if ( side == NEUTRAL && ( bLeft == true || bRight == true ) )
    {
        return true;
    }
    else if ( side == LEFT && bLeft == true )
    {
        return true;
    }
    else if ( side == RIGHT && bRight == true )
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool
Referee::crossGoalLine( const Side side,
                        const PVector & prev_ball_pos )
{
    if ( prev_ball_pos.x == M_stadium.ball().pos().x )
    {
        // ball cannot have crossed gline
        //          std::cout << time << ": vertcal movement\n";
        return false;
    }

    if ( std::fabs( M_stadium.ball().pos().x )
         <= PITCH_LENGTH*0.5 + ServerParam::instance().bsize )
    {
        // ball hasn't crossed gline
        //          std::cout << time << ": hasn't crossed\n";
        return false;
    }

    if ( std::fabs( prev_ball_pos.x )
         > PITCH_LENGTH*0.5 + ServerParam::instance().bsize )
    {
        // ball already over the gline
        //          std::cout << time << ": already crossed\n";
        return false;
    }

    if ( ( side * M_stadium.ball().pos().x ) >= 0 )
    {
        //ball in wrong half
        //          std::cout << time << ": wrong_half\n";
        return false;
    }

    if ( std::fabs( prev_ball_pos.y ) > ( ServerParam::instance().gwidth*0.5
                                          + ServerParam::instance().goalPostRadius() )
         && std::fabs( prev_ball_pos.x ) > PITCH_LENGTH*0.5 )
    {
        // then the only goal that could have been scored would be
        // from going behind the goal post.  I'm pretty sure that
        // isn't possible anyway, but just in case this function acts
        // as a double check
        //          std::cout << time << ": behind_half\n";
        return false;
    }

    double delta_x = M_stadium.ball().pos().x - prev_ball_pos.x;
    double delta_y = M_stadium.ball().pos().y - prev_ball_pos.y;

    // we already checked above that ball->pos.x != prev_ball_pos.x, so delta_x cannot be zero.
    double gradient = delta_y / delta_x;
    double offset = prev_ball_pos.y - gradient * prev_ball_pos.x;

    // determine y for x = PITCH_LENGTH*0.5 + ServerParam::instance().bsize * -side
    double x = ( PITCH_LENGTH*0.5 + ServerParam::instance().bsize ) * -side;
    double y_intercept = gradient * x + offset;

    //      std::cout << time << ": prev = " << prev_ball_pos << std::endl;
    //      std::cout << time << ": curr = " << ball->pos << std::endl;
    //      std::cout << time << ": delta_x = " << delta_x << std::endl;
    //      std::cout << time << ": delta_y = " << delta_y << std::endl;
    //      std::cout << time << ": grad = " << gradient << std::endl;
    //      std::cout << time << ": off = " << offset << std::endl;
    //      std::cout << time << ": x = " << x << std::endl;
    //      std::cout << time << ": y_inter = " << y_intercept << std::endl;


    return std::fabs( y_intercept ) <= ( ServerParam::instance().gwidth*0.5
                                         + ServerParam::instance().goalPostRadius() );
}

//**********
// TimeRef
//**********

void
TimeRef::analyse()
{
    static int s_half_time_count = 0;

    const PlayMode pm = M_stadium.playmode();
    if ( pm == PM_BeforeKickOff
         || pm == PM_TimeOver
         || pm == PM_AfterGoal_Right
         || pm == PM_AfterGoal_Left
         || pm == PM_OffSide_Right
         || pm == PM_OffSide_Left
         || pm == PM_Back_Pass_Right
         || pm == PM_Back_Pass_Left
         || pm == PM_Free_Kick_Fault_Right
         || pm == PM_Free_Kick_Fault_Left
         || pm == PM_CatchFault_Right
         || pm == PM_CatchFault_Left )
    {
        return;
    }

    /* if a value of half_time is negative, then ignore time. */
    if ( ServerParam::instance().halfTime() > 0 )
    {
        /* check for penalty shoot-outs, half_time and extra_time. */
        if ( M_stadium.time() >= ( ServerParam::instance().halfTime()
                                   * ( ServerParam::instance().nr_normal_halfs
                                       + ServerParam::instance().nr_extra_halfs ) )
             )
        {
            if ( ServerParam::instance().penalty_shoot_outs
                 && M_stadium.teamLeft().point() == M_stadium.teamRight().point() )
            {
                return; // handled by PenaltyRef
            }
            else
            {
                M_stadium.say( "time_up" );
                M_stadium.change_play_mode( PM_TimeOver );
                return;
            }
        }
        else if ( M_stadium.time() >= ( ServerParam::instance().halfTime()
                                        * ServerParam::instance().nr_normal_halfs )
                  )
        {
            if ( ! M_stadium.teamLeft().enabled()
                 || ! M_stadium.teamRight().enabled() )
            {
                M_stadium.say( "time_up_without_a_team" );
                M_stadium.change_play_mode( PM_TimeOver );
                return;
            }
            else if ( M_stadium.teamLeft().point() != M_stadium.teamRight().point() )
            {
                M_stadium.say( "time_up" );
                M_stadium.change_play_mode( PM_TimeOver );
                return;
            }
            else if ( M_stadium.time() >= ( ServerParam::instance().halfTime()
                                            * ( s_half_time_count + 1 ) )
                      )
            {
                ++s_half_time_count;
                M_stadium.say( "time_extended" );
                Side kick_off_side = ( s_half_time_count % 2 == 0
                                       ? LEFT
                                       : RIGHT );
                M_stadium.setHalfTime( kick_off_side, s_half_time_count );
                placePlayersInTheirField();
                return;
            }
        }
        // if not in overtime, check whether halfTime() cycles have been passed
        else if ( M_stadium.time() >= ( ServerParam::instance().halfTime()
                                        * ( s_half_time_count + 1 ) )
                  )
        {
            ++s_half_time_count;
            M_stadium.say( "half_time" );
            M_stadium.setHalfTime( RIGHT, s_half_time_count );
            placePlayersInTheirField();
            return;
        }
    }

//     if ( ServerParam::instance().CoachMode && M_stadium.check_ball_pos() != BPI_InField )
//     {
//         // freeze the ball
//         M_stadium.ball->moveTo( M_stadium.ball->pos(),
//                                 0.0,
//                                 PVector( 0.0, 0.0 ),
//                                 PVector( 0.0, 0.0 ) );
//     }
}


void
Referee::placePlayersInTheirField()
{
    static const RArea fld_l( PVector( -PITCH_LENGTH/4, 0.0 ),
                              PVector( PITCH_LENGTH/2, PITCH_WIDTH ) );
    static const RArea fld_r( PVector( PITCH_LENGTH/4, 0.0),
                              PVector( PITCH_LENGTH/2, PITCH_WIDTH ) );

    const bool kick_off_offside ( ServerParam::instance().kickoffoffside
                                  && ( M_stadium.playmode() == PM_KickOff_Left
                                       || M_stadium.playmode() == PM_KickOff_Right ) );

    const Stadium::PlayerCont::iterator end = M_stadium.remotePlayers().end();
    for ( Stadium::PlayerCont::iterator it = M_stadium.remotePlayers().begin();
          it != end;
          ++it )
    {
        if ( (*it)->alive == DISABLE ) continue;

        switch ( (*it)->team()->side() ) {
        case LEFT:
            if ( (*it)->pos().x > 0 )
            {
                if ( kick_off_offside )
                {
                    (*it)->moveTo( PVector( -(*it)->size(), (*it)->pos().y ) );
                }
                else
                {
                    (*it)->moveTo( fld_l.randomize() );
                }
            }
            break;
        case RIGHT:
            if ( (*it)->pos().x < 0 )
            {
                if ( kick_off_offside )
                {
                    (*it)->moveTo( PVector( (*it)->size(), (*it)->pos().y ) );
                }
                else
                {
                    (*it)->moveTo( fld_r.randomize() );
                }
            }
            break;
        case NEUTRAL:
        default:
            break;
        }

        if ( (*it)->team()->side() != M_stadium.kickOffSide() )
        {
            CArea expand_c( PVector( 0.0, 0.0 ),
                            KICK_OFF_CLEAR_DISTANCE + (*it)->size() );

            if ( expand_c.inArea( (*it)->pos() ) )
            {
                (*it)->moveTo( expand_c.nearestEdge( (*it)->pos() ) );
            }
        }
    }
}


void
Referee::clearPlayersFromBall( const Side side )
{
    // I would really prefer if we did not teleport players around the field. In
    // my mind players should be given time to move away from the ball and given
    // yellow cards if they repeatedly fail to stay clear.  Two yellows and your
    // out of the game.

    const Stadium::PlayerCont::iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::iterator it = M_stadium.players().begin();
          it != end;
          ++it )
    {
        if ( (*it)->alive == DISABLE )
        {
            continue;
        }

        if ( side == NEUTRAL
             || (*it)->team()->side() == side )
        {
            CArea clear_area( M_stadium.ball().pos(),
                              KICK_OFF_CLEAR_DISTANCE + (*it)->size() );
            if ( clear_area.inArea( (*it)->pos() ) )
            {
                (*it)->place( clear_area.nearestEdge( (*it)->pos() ) );
            }
        }
    }
}

//**********
// BallStuckRef
//**********

void
BallStuckRef::analyse()
{
    if ( ServerParam::instance().ballStuckArea() <= 0.0
         || ServerParam::instance().drop_time <= 0 )
    {
        return;
    }

    if ( M_stadium.playmode() != PM_PlayOn )
    {
        M_last_ball_pos = M_stadium.ball().pos();
        M_counter = 0;
        return;
    }

    if ( M_stadium.ball().pos().distance( M_last_ball_pos )
         <= ServerParam::instance().ballStuckArea() )
    {
        ++M_counter;

        if ( M_counter >= ServerParam::instance().drop_time )
        {
            M_last_ball_pos = M_stadium.ball().pos();
            M_counter = 0;

            awardDropBall( M_stadium.ball().pos() );
        }
    }
    else
    {
        M_last_ball_pos = M_stadium.ball().pos();
        M_counter = 0;
    }

}


//**********
// OffsideRef
//**********

void
OffsideRef::kickTaken( const Player & kicker )
{
    if ( ! ServerParam::instance().useoffside )
    {
        return;
    }

    M_last_kicker = &kicker;

    setOffsideMark( kicker );
}

void
OffsideRef::ballTouched( const Player & player )
{
    if ( ! ServerParam::instance().useoffside )
    {
        return;
    }

    M_last_kicker = &player;

    setOffsideMark( player );
}

void
OffsideRef::analyse()
{
    if ( ! ServerParam::instance().useoffside )
    {
        return;
    }

    if ( isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    if ( M_stadium.playmode() == PM_BeforeKickOff
         || M_stadium.playmode() == PM_KickOff_Left
         || M_stadium.playmode() == PM_KickOff_Right )
    {
        if ( ServerParam::instance().kickoffoffside )
        {
            placePlayersInTheirField();
        }
        return;
    }

    if ( M_stadium.playmode() == PM_OffSide_Left )
    {
        clearPlayersFromBall( LEFT );
        checkPlayerAfterOffside();
        if ( ++M_after_offside_time > AFTER_OFFSIDE_WAIT )
        {
            M_stadium.change_play_mode( PM_FreeKick_Right );
        }
        return;
    }

    if ( M_stadium.playmode() == PM_OffSide_Right )
    {
        clearPlayersFromBall( RIGHT );
        checkPlayerAfterOffside();
        if ( ++M_after_offside_time > AFTER_OFFSIDE_WAIT )
        {
            M_stadium.change_play_mode( PM_FreeKick_Left );
        }
        return;
    }

    if ( M_stadium.playmode() != PM_PlayOn )
    {
        const Stadium::PlayerCont::iterator end = M_stadium.players().end();
        for ( Stadium::PlayerCont::iterator p = M_stadium.players().begin();
              p != end;
              ++p )
        {
            (*p)->clearOffsideMark();
        }
        return;
    }

    double dist = ServerParam::instance().offside_area;

    {
        const Stadium::PlayerCont::const_iterator end = M_stadium.players().end();
        for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
              p != end;
              ++p )
        {
            if ( (*p)->hasOffsideMark() )
            {
                double tmp = (*p)->pos().distance( M_stadium.ball().pos() );
                if ( tmp < dist )
                {
                    dist = tmp;
                    M_offside_pos = (*p)->offsidePos();
                }
            }
        }
    }

    if ( dist != ServerParam::instance().offside_area )
    {
        callOffside();
    }
}

void
OffsideRef::playModeChange( PlayMode pm )
{
    if ( pm != PM_PlayOn )
    {
        const Stadium::PlayerCont::const_iterator end = M_stadium.players().end();
        for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
              p != end;
              ++p )
        {
            (*p)->clearOffsideMark();
        }
    }
}

void
OffsideRef::setOffsideMark( const Player & kicker )
{
    if ( kicker.hasOffsideMark() )
    {
        M_offside_pos = kicker.offsidePos();
        callOffside();
        return;
    }

    const Stadium::PlayerCont::const_iterator end = M_stadium.players().end();

    for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {

        (*p)->clearOffsideMark();
    }

    //if (m_no_offsides )
    if ( M_stadium.playmode() == PM_GoalKick_Left
         || M_stadium.playmode() == PM_GoalKick_Right
         || M_stadium.playmode() == PM_KickIn_Left
         || M_stadium.playmode() == PM_KickIn_Right
         || M_stadium.playmode() == PM_CornerKick_Left
         || M_stadium.playmode() == PM_CornerKick_Right )
    {
        return;
    }

    double fast = 0;
    double second = 0;
    double offside_line;

    switch ( kicker.team()->side() ) {
    case LEFT:
        for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
              p != end;
              ++p )
        {
            if ( (*p)->alive == DISABLE )
            {
                continue;
            }

            if ( (*p)->team()->side() == RIGHT )
            {
                if ( (*p)->pos().x > second )
                {
                    second = (*p)->pos().x;
                    if ( second > fast )
                    {
                        std::swap( fast, second );
                    }
                }
            }
        }

        if ( second > M_stadium.ball().pos().x )
        {
            offside_line = second;
        }
        else
        {
            offside_line = M_stadium.ball().pos().x;
        }

        for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
              p != end;
              ++p )
        {
            if ( (*p)->alive == DISABLE )
            {
                continue;
            }

            if ( (*p)->team()->side() == LEFT
                 && (*p)->pos().x > offside_line
                 && (*p)->unum() != kicker.unum() )
            {
                (*p)->setOffsideMark( offside_line );
            }
        }
        break;

    case RIGHT:
        for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
              p != end;
              ++p )
        {
            if ( (*p)->alive == DISABLE )
            {
                continue;
            }

            if ( (*p)->team()->side() == LEFT )
            {
                if ( (*p)->pos().x < second )
                {
                    second = (*p)->pos().x;
                    if ( second < fast )
                    {
                        std::swap( fast, second );
                    }
                }
            }
        }

        if ( second < M_stadium.ball().pos().x )
        {
            offside_line = second;
        }
        else
        {
            offside_line = M_stadium.ball().pos().x;
        }

        for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
              p != end;
              ++p )
        {
            if ( (*p)->alive == DISABLE )
                continue;
            if ( (*p)->team()->side() == RIGHT
                 && (*p)->pos().x < offside_line
                 && (*p)->unum() != kicker.unum() )
            {
                (*p)->setOffsideMark( offside_line );
            }
        }
        break;
    case NEUTRAL:
    default:
        break;
    }
}

void
OffsideRef::callOffside()
{
    if ( isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    PVector pos;

    static RArea pt( PVector( 0.0, 0.0 ),
                     PVector( PITCH_LENGTH - 2.0*ServerParam::instance().ckmargin,
                              PITCH_WIDTH - 2.0*ServerParam::instance().ckmargin ) );
    static RArea g_l( PVector( -PITCH_LENGTH/2.0 + GOAL_AREA_LENGTH/2.0, 0.0 ),
                      PVector( GOAL_AREA_LENGTH, GOAL_AREA_WIDTH ) );
    static RArea g_r( PVector( +PITCH_LENGTH/2.0 - GOAL_AREA_LENGTH/2.0, 0.0 ),
                      PVector( GOAL_AREA_LENGTH, GOAL_AREA_WIDTH ) );

    M_after_offside_time = 0;

    if ( M_offside_pos.x > PITCH_LENGTH/2.0 || g_r.inArea( M_offside_pos ) )
    {
        pos.x = +PITCH_LENGTH/2.0-GOAL_AREA_LENGTH;
        pos.y = ( M_offside_pos.y > 0 ? 1 : -1 ) * GOAL_AREA_WIDTH/2.0;
    }
    else if ( M_offside_pos.x < -PITCH_LENGTH/2.0 || g_l.inArea( M_offside_pos ) )
    {
        pos.x = -PITCH_LENGTH/2.0+GOAL_AREA_LENGTH;
        pos.y = ( M_offside_pos.y > 0 ? 1 : -1 ) * GOAL_AREA_WIDTH/2.0;
    }
    else if ( ! pt.inArea( M_offside_pos ) )
    {
        pos = pt.nearestEdge( M_offside_pos );
    }
    else
    {
        pos = M_offside_pos;
    }

    if ( M_last_kicker->team()->side() == LEFT )
    {
        M_stadium.placeBall( PM_OffSide_Left, RIGHT, pos );
    }
    else
    {
        M_stadium.placeBall( PM_OffSide_Right, LEFT, pos );
    }

    M_stadium.clearBallCatcher();

    const Stadium::PlayerCont::const_iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {
        (*p)->clearOffsideMark();
    }

    M_stadium.placePlayersInField();
    clearPlayersFromBall( M_last_kicker->team()->side() );
}


void
OffsideRef::checkPlayerAfterOffside()
{
    CArea c( M_offside_pos, ServerParam::instance().offside_area );

    Side offsideside = NEUTRAL;
    PVector ce, si;

    if ( M_stadium.playmode() == PM_OffSide_Right )
    {
        ce += PVector( PITCH_LENGTH/4 + M_offside_pos.x/2, 0.0 );
        si += PVector( PITCH_LENGTH/2 - M_offside_pos.x, PITCH_WIDTH );
        offsideside = RIGHT;
    }
    else if ( M_stadium.playmode() == PM_OffSide_Left )
    {
        ce += PVector( -PITCH_LENGTH/4 + M_offside_pos.x/2, 0.0 );
        si += PVector( PITCH_LENGTH/2 + M_offside_pos.x, PITCH_WIDTH );
        offsideside = LEFT;
    }
    else
    {
        return;
    }

    RArea fld( ce, si );

    const Stadium::PlayerCont::iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {
        if ( (*p)->alive == DISABLE )
        {
            continue;
        }

        if ( (*p)->team()->side() == offsideside )
        {
            if ( c.inArea( (*p)->pos() ) )
            {
                (*p)->moveTo( c.nearestEdge( (*p)->pos() ) );
            }
            if ( ! fld.inArea( (*p)->pos() ))
            {
                if ( M_stadium.playmode() == PM_OffSide_Right )
                {
                    (*p)->moveTo( fld.nearestVEdge( (*p)->pos() )
                                                    + PVector( ServerParam::instance().offside_kick_margin, 0 ) );
                }
                else
                {
                    (*p)->moveTo( fld.nearestVEdge( (*p)->pos() )
                                                    - PVector( ServerParam::instance().offside_kick_margin, 0 ) );
                }
            }
        }
    }
}

//**********
// FreeKickRef
//**********

void
FreeKickRef::kickTaken( const Player & kicker )
{
    if ( isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    if ( goalKick( M_stadium.playmode() ) )
    {
        if ( ( M_stadium.playmode() == PM_GoalKick_Left
               && kicker.team()->side() != LEFT )
             || ( M_stadium.playmode() == PM_GoalKick_Right
                  && kicker.team()->side() != RIGHT )
             )
        {
            // opponent player kicks tha ball while ball is in penalty areas.
            awardGoalKick( (Side)( - kicker.team()->side() ), M_stadium.ball().pos() );
            M_goal_kick_count = -1;
            M_kick_taken = false;
            return;
        }

        if ( M_kick_taken )
        {
            // ball was not kicked directly into play (i.e. out of penalty area
            // without touching another player
            if ( &kicker != M_kick_taker )
            {
                if ( ServerParam::instance().properGoalKicks() )
                {
                    awardGoalKick( M_kick_taker->team()->side(), M_stadium.ball().pos() );
                }
            }
            else if ( M_kick_taker_dashes != M_kick_taker->dashCount() )
            {
                if ( ServerParam::instance().freeKickFaults() )
                {
                    callFreeKickFault( kicker.team()->side(), M_stadium.ball().pos() );
                }
                else if ( ServerParam::instance().properGoalKicks() )
                {
                    awardGoalKick( M_kick_taker->team()->side(), M_stadium.ball().pos() );
                }
            }
            // else it's part of a compound kick
        }
        //else
        //{
        M_kick_taken = true;
        M_kick_taker = &kicker;
        M_kick_taker_dashes = M_kick_taker->dashCount();
        //}
    }
    else if ( M_stadium.playmode() != PM_PlayOn )
    {
        M_kick_taken = true;
        M_kick_taker = &kicker;
        M_kick_taker_dashes = M_kick_taker->dashCount();
        M_stadium.change_play_mode( PM_PlayOn );
    }
    else // PM_PlayOn
    {
        if ( M_kick_taken )
        {
            if ( M_kick_taker == &kicker
                 && ServerParam::instance().freeKickFaults() )
            {
                if ( M_kick_taker->dashCount() > M_kick_taker_dashes )
                {
                    callFreeKickFault( M_kick_taker->team()->side(),
                                       M_stadium.ball().pos() );
                }
            }
            else
            {
                M_kick_taken = false;
            }
        }
    }
}

void
FreeKickRef::ballTouched( const Player & player )
{
    if ( ( M_stadium.playmode() == PM_GoalKick_Left
           && player.team()->side() != LEFT )
         || ( M_stadium.playmode() == PM_GoalKick_Right
              && player.team()->side() != RIGHT )
         )
    {
        // opponent player kicks tha ball while ball is in penalty area.
        awardGoalKick( (Side)( - player.team()->side() ), M_stadium.ball().pos() );
        M_goal_kick_count = -1;
        M_kick_taken = false;
        return;
    }

    if ( M_kick_taken )
    {
        if ( M_kick_taker == &player
             && ServerParam::instance().freeKickFaults() )
        {
            if ( M_kick_taker->dashCount() > M_kick_taker_dashes )
            {
                callFreeKickFault( M_kick_taker->team()->side(),
                                   M_stadium.ball().pos() );
            }
            /// else do nothing yet as the player just colided with the ball instead of dashing into it
        }
        else
        {
            M_kick_taken = false;
        }
    }
}

void
FreeKickRef::analyse()
{
    const PlayMode pm = M_stadium.playmode();

    if ( isPenaltyShootOut( pm ) )
    {
        return;
    }

    if ( pm == PM_Free_Kick_Fault_Left )
    {
        clearPlayersFromBall( LEFT );
        if ( ++M_after_free_kick_fault_time > AFTER_FREE_KICK_FAULT_WAIT )
        {
            M_stadium.change_play_mode( PM_FreeKick_Right );
        }
        return;
    }

    if ( pm == PM_Free_Kick_Fault_Right )
    {
        clearPlayersFromBall( RIGHT );
        if ( ++M_after_free_kick_fault_time > AFTER_FREE_KICK_FAULT_WAIT )
        {
            M_stadium.change_play_mode( PM_FreeKick_Left );
        }
        return;
    }

    if ( goalKick( pm ) )
    {
        placePlayersForGoalkick();
        M_stadium.placePlayersInField();

        if ( ! inPenaltyArea( NEUTRAL, M_stadium.ball().pos() ) )
        {
            M_stadium.change_play_mode( PM_PlayOn );
        }
        else
        {
            if ( M_kick_taken
                 && ServerParam::instance().properGoalKicks() )
            {
                if ( ballStopped() )
                {
                    if ( tooManyGoalKicks() )
                    {
                        awardFreeKick( (Side)( -M_kick_taker->team()->side() ),
                                       M_stadium.ball().pos() );
                    }
                    else
                    {
                        awardGoalKick( M_kick_taker->team()->side(),
                                       M_stadium.ball().pos() );
                    }
                }
            }
            else
            {
                if ( M_timer > -1 )
                {
                    M_timer--;
                }

                if ( M_timer == 0 )
                {
                    awardDropBall( M_stadium.ball().pos() );
                }
            }
        }

        return;
    }

    if ( pm != PM_PlayOn
         && pm != PM_BeforeKickOff
         && pm != PM_TimeOver
         && pm != PM_AfterGoal_Right
         && pm != PM_AfterGoal_Left
         && pm != PM_OffSide_Right
         && pm != PM_OffSide_Left
         && pm != PM_Back_Pass_Right
         && pm != PM_Back_Pass_Left
         && pm != PM_Free_Kick_Fault_Right
         && pm != PM_Free_Kick_Fault_Left
         && pm != PM_CatchFault_Right
         && pm != PM_CatchFault_Left )
    {
        if ( M_stadium.ball().vel().x != 0.0
             || M_stadium.ball().vel().y != 0.0 )
        {
            M_stadium.change_play_mode( PM_PlayOn );
        }
    }

    M_stadium.placePlayersInField();

    if ( pm != PM_PlayOn
         && pm != PM_TimeOver
         && pm != PM_GoalKick_Left
         && pm != PM_GoalKick_Right )
    {
        clearPlayersFromBall( (Side)(-M_stadium.kickOffSide() ) );
    }

    if ( freeKick( pm ) )
    {
        if ( M_timer > -1 )
        {
            M_timer--;
        }

        if ( M_timer == 0 )
        {
            awardDropBall( M_stadium.ball().pos() );
        }
    }
}


void
FreeKickRef::playModeChange( PlayMode pm )
{
    if ( pm != PM_PlayOn )
    {
        M_kick_taken = false;
    }

    if ( pm == PM_KickOff_Left
         || pm == PM_KickIn_Left
         || pm == PM_FreeKick_Left
         || pm == PM_IndFreeKick_Left
         || pm == PM_CornerKick_Left
         || pm == PM_GoalKick_Left )
    {
        clearPlayersFromBall( RIGHT );
    }
    else if ( pm == PM_KickOff_Right
              || pm == PM_KickIn_Right
              || pm == PM_FreeKick_Right
              || pm == PM_IndFreeKick_Right
              || pm == PM_CornerKick_Right
              || pm == PM_GoalKick_Right )
    {
        clearPlayersFromBall( LEFT );
    }
    else if ( pm == PM_Drop_Ball )
    {
        clearPlayersFromBall( NEUTRAL );
    }

    if ( goalKick( pm ) )
    {
        M_timer = ServerParam::instance().drop_time;

        if ( ! goalKick( M_stadium.playmode() ) )
        {
            M_goal_kick_count = 0;
        }
        else
        {
            M_goal_kick_count++;
        }
    }
    else
    {
        M_goal_kick_count = 0;
    }

    if ( freeKick( pm ) )
    {
        M_timer = ServerParam::instance().drop_time;
    }

    if ( ! freeKick( pm ) && ! goalKick( pm ) )
    {
        M_timer = -1;
    }
}


void
FreeKickRef::callFreeKickFault( Side side, PVector pos )
{
    pos = truncateToPitch( pos );
    pos = moveOutOfPenalty( side, pos );

    M_stadium.clearBallCatcher();

    if ( side == LEFT )
    {
        M_stadium.placeBall( PM_Free_Kick_Fault_Left, RIGHT, pos );
    }
    else if ( side == RIGHT )
    {
        M_stadium.placeBall( PM_Free_Kick_Fault_Right, LEFT, pos );
    }

    M_after_free_kick_fault_time = 0;
}

bool
FreeKickRef::goalKick( PlayMode pm )
{
    return ( pm == PM_GoalKick_Right
             || pm == PM_GoalKick_Left );
}


bool
FreeKickRef::freeKick( PlayMode pm )
{
    switch( pm )
    {
    case PM_KickOff_Right:
    case PM_KickIn_Right:
    case PM_FreeKick_Right:
    case PM_CornerKick_Right:
    case PM_IndFreeKick_Right:
    case PM_KickOff_Left:
    case PM_KickIn_Left:
    case PM_FreeKick_Left:
    case PM_CornerKick_Left:
    case PM_IndFreeKick_Left:
        return true;
    default:
        return false;
    }
}

bool
FreeKickRef::ballStopped()
{
    return M_stadium.ball().vel().r() < ServerParam::instance().stoppedBallVel();
}

bool
FreeKickRef::tooManyGoalKicks()
{
    return M_goal_kick_count >= ServerParam::instance().maxGoalKicks();
}


void
FreeKickRef::placePlayersForGoalkick()
{
    static RArea p_l( PVector( -PITCH_LENGTH/2+PENALTY_AREA_LENGTH/2.0, 0.0 ),
                      PVector( PENALTY_AREA_LENGTH, PENALTY_AREA_WIDTH ) );
    static RArea p_r( PVector( +PITCH_LENGTH/2-PENALTY_AREA_LENGTH/2.0, 0.0 ),
                      PVector( PENALTY_AREA_LENGTH, PENALTY_AREA_WIDTH ) );
    RArea* p_area;

    int oppside;

    if ( M_stadium.playmode() == PM_GoalKick_Left )
    {
        oppside = RIGHT;
        p_area = &p_l;
    }
    else
    {
        oppside = LEFT;
        p_area = &p_r;
    }

    const Stadium::PlayerCont::iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {
        if ( (*p)->alive == DISABLE )
            continue;
        if ( (*p)->team()->side() == oppside )
        {
            const Value size = (*p)->playerType()->playerSize();
            RArea expand_area( p_area->left - size,
                               p_area->right + size,
                               p_area->top - size,
                               p_area->bottom + size );

            if ( expand_area.inArea( (*p)->pos() ) )
            {
                PVector new_pos = expand_area.nearestEdge( (*p)->pos() );
                if ( new_pos.x * oppside >= PITCH_LENGTH/2 )
                {
                    new_pos.x = (PITCH_LENGTH/2 - PENALTY_AREA_LENGTH) * oppside;
                }

                (*p)->moveTo( new_pos );
            }
        }
    }

//     if ( ! p_area->inArea( M_stadium.ball().pos() ) )
//     {
//         M_stadium.check_player_in_field();
//     }
}


//**********
// TouchRef
//**********


void
TouchRef::analyse()
{
    analyseImpl();

    M_prev_ball_pos = M_stadium.ball().pos();
}

void
TouchRef::analyseImpl()
{
    if ( isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    if ( M_stadium.playmode() == PM_AfterGoal_Left )
    {
        if ( ++M_after_goal_time > AFTER_GOAL_WAIT )
        {
            M_stadium.placeBall( PM_KickOff_Right, RIGHT, PVector(0.0,0.0) );
            placePlayersInTheirField();
        }
        return;
    }

    if ( M_stadium.playmode() == PM_AfterGoal_Right )
    {
        if ( ++M_after_goal_time > AFTER_GOAL_WAIT )
        {
            M_stadium.placeBall( PM_KickOff_Left, LEFT, PVector(0.0,0.0) );
            placePlayersInTheirField();
        }
        return;
    }

    if ( checkGoal() )
    {
        return;
    }

    if ( M_stadium.playmode() != PM_AfterGoal_Left
         && M_stadium.playmode() != PM_AfterGoal_Right
         && M_stadium.playmode() != PM_TimeOver )
    {
        if ( std::fabs( M_stadium.ball().pos().x )
             > PITCH_LENGTH * 0.5 + ServerParam::instance().bsize )
        {
            // check for goal kick or corner kick
            Side side = NEUTRAL;
            if ( M_last_touched != NULL )
            {
                side = M_last_touched->team()->side();
            }

            if ( M_stadium.ball().pos().x
                 > PITCH_LENGTH * 0.5 + ServerParam::instance().bsize )
            {
                if ( side == RIGHT )
                {
                    awardCornerKick( LEFT, M_stadium.ball().pos() );
                }
                else if ( M_stadium.ballCatcher() == NULL )
                {
                    awardGoalKick( RIGHT, M_stadium.ball().pos() );
                }
                else
                {
                    // the ball is caught and the goalie must have
                    // moved to a position beyond the opponents goal
                    // line.  Let the catch ref clean up the mess
                    return;
                }
            }
            else if ( M_stadium.ball().pos().x
                      < PITCH_LENGTH * 0.5 - ServerParam::instance().bsize )
            {
                if ( side == LEFT )
                {
                    awardCornerKick( RIGHT, M_stadium.ball().pos() );
                }
                else if ( M_stadium.ballCatcher() == NULL )
                {
                    awardGoalKick( LEFT, M_stadium.ball().pos() );
                }
                else
                {
                    // the ball is caught and the goalie must have
                    // moved to a position beyond the opponents goal
                    // line.  Let the catch ref clean up the mess
                    return;
                }
            }
        }
        else if ( std::fabs( M_stadium.ball().pos().y )
                  > PITCH_WIDTH * 0.5 + ServerParam::instance().bsize )
        {
            // check for kick in.

            Side side = NEUTRAL;
            if ( M_last_touched != NULL )
            {
                side = M_last_touched->team()->side();
            }

            if ( side == NEUTRAL )
            {
                // somethings gone wrong but give a drop ball
                awardDropBall( M_stadium.ball().pos() );
            }
            else
            {
                awardKickIn( (Side)( -side ), M_stadium.ball().pos() );
            }
        }
    }
}

void
TouchRef::kickTaken( const Player & kicker )
{
    if ( M_stadium.playmode() == PM_PlayOn
         && M_last_indirect_kicker
         && M_last_indirect_kicker != &kicker )
    {
        M_last_indirect_kicker = NULL;
        M_indirect_mode = false;
    }

    if ( M_indirect_mode )
    {
        M_last_indirect_kicker = &kicker;
    }

    M_last_touched = &kicker;
}

void
TouchRef::ballTouched( const Player & kicker )
{
    if ( M_stadium.playmode() == PM_PlayOn
         && M_last_indirect_kicker
         && M_last_indirect_kicker != &kicker )
    {
        M_last_indirect_kicker = NULL;
        M_indirect_mode = false;
    }

    if ( M_indirect_mode )
    {
        M_last_indirect_kicker = &kicker;
    }

    M_last_touched = &kicker;
}


void
TouchRef::playModeChange( PlayMode pm )
{
    if ( pm != PM_PlayOn )
    {
        M_last_touched = NULL;
    }

    if ( indirectFreeKick( pm ) )
    {
        M_last_indirect_kicker = NULL;
        M_indirect_mode = true;
    }
    else if ( pm != PM_PlayOn && pm != PM_Drop_Ball )
    {
        M_last_indirect_kicker = NULL;
        M_indirect_mode = false;
    }
}


bool
TouchRef::checkGoal()
{
    if ( M_stadium.playmode() == PM_AfterGoal_Left
         || M_stadium.playmode() == PM_AfterGoal_Right
         || M_stadium.playmode() == PM_TimeOver )
    {
        return false;
    }

    if ( M_indirect_mode )
    {
        return false;
    }

    // FIFA rules:  Ball has to be completely outside of the pitch to be considered out
    //  	static RArea pt( PVector(0.0,0.0),
    //                       PVector( PITCH_LENGTH
    //                                + ServerParam::instance().bsize * 2,
    //                                PITCH_WIDTH
    //                                + ServerParam::instance().bsize * 2 ) );

    if ( std::fabs( M_stadium.ball().pos().x )
         <= PITCH_LENGTH * 0.5 + ServerParam::instance().bsize )
    {
        return false;
    }


    if ( ( ! M_stadium.ballCatcher()
           || M_stadium.ballCatcher()->team()->side() == LEFT )
         && crossGoalLine( LEFT, M_prev_ball_pos )
         && ! isPenaltyShootOut( M_stadium.playmode() ) )
    {
        M_stadium.score( RIGHT );
        announceGoal( M_stadium.teamRight() );
        M_after_goal_time = 0;
        M_stadium.set_ball( LEFT, M_stadium.ball().pos() );
        if ( ServerParam::instance().halfTime() >= 0
             && M_stadium.time()
             >= ServerParam::instance().halfTime()
             * ServerParam::instance().nr_normal_halfs )
        {
            M_stadium.change_play_mode( PM_TimeOver );
        }
        else
        {
            M_stadium.change_play_mode( PM_AfterGoal_Right );
        }
        return true;
    }
    else if ( ( ! M_stadium.ballCatcher()
                || M_stadium.ballCatcher()->team()->side() == RIGHT )
              && crossGoalLine( RIGHT, M_prev_ball_pos )
              && ! isPenaltyShootOut( M_stadium.playmode() )  )
    {
        M_stadium.score( LEFT );
        announceGoal( M_stadium.teamLeft() );
        M_after_goal_time = 0;
        M_stadium.set_ball( RIGHT, M_stadium.ball().pos() );
        if ( ServerParam::instance().halfTime() >= 0
             && M_stadium.time()
             >= ServerParam::instance().halfTime()
             * ServerParam::instance().nr_normal_halfs )
        {
            M_stadium.change_play_mode( PM_TimeOver );
        }
        else
        {
            M_stadium.change_play_mode( PM_AfterGoal_Left );
        }
        return true;
    }

    return false;
}


void
TouchRef::announceGoal( const Team & team )
{
#ifdef HAVE_SSTREAM
    std::ostringstream msg;
    msg << "goal_" << SideStr( team.side() ) << "_" << team.point();
    M_stadium.say( msg.str().c_str() );
#else
    std::ostrstream msg;
    msg << "goal_" << SideStr( team.side() ) << "_" << team.point() << std::ends;
    M_stadium.say( msg.str() );
    msg.freeze( false );
#endif
}


bool
TouchRef::indirectFreeKick( const PlayMode pm )
{
    switch( pm ) {
    case PM_IndFreeKick_Right:
    case PM_IndFreeKick_Left:
        return true;
    default:
        return false;
    }
}


//************
// KeepawayRef
//************


void
KeepawayRef::analyse()
{
    if( ServerParam::instance().kaway )
    {
        if ( M_stadium.playmode() == PM_PlayOn )
        {
            if( !ballInKeepawayArea() )
            {
                logEpisode( "o" );
                M_stadium.say( trainingMsg );
                resetField();
            }
            else if( M_take_time >= TURNOVER_TIME )
            {
                logEpisode( "t" );
                M_stadium.say( trainingMsg );
                resetField();
            }
            else
            {
                bool keeperPoss = false;

                const Stadium::PlayerCont::const_iterator end = M_stadium.players().end();
                for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
                      p != end;
                      ++p )
                {
                    PVector ppos = (*p)->pos();

                    if ( (*p)->alive != DISABLE &&
                         ppos.distance( M_stadium.ball().pos() ) <
                         ServerParam::instance().kmargin +
                         ServerParam::instance().psize +
                         ServerParam::instance().bsize )
                    {
                        if ( (*p)->team()->side() == LEFT )
                        {
                            keeperPoss = true;
                        }
                        else if ( (*p)->team()->side() == RIGHT )
                        {
                            keeperPoss = false;
                            M_take_time++;
                            break;
                        }
                    }
                }
                if ( keeperPoss )
                    M_take_time = 0;
            }
        }
        else if ( ServerParam::instance().kawayStart() >= 0 )
        {
            if ( difftime( time( NULL ), M_start_time ) >
                 ServerParam::instance().kawayStart() )
                M_stadium.change_play_mode( PM_PlayOn );
        }
    }
}

void
KeepawayRef::kickTaken( const Player & )
{
}

void
KeepawayRef::ballTouched( const Player & )
{
}

void
KeepawayRef::playModeChange( PlayMode pm )
{
    if ( ServerParam::instance().kaway )
    {
        if ( pm == PM_PlayOn && M_episode == 0 )
        {
            M_episode = 1;

            const Stadium::PlayerCont::const_iterator end = M_stadium.players().end();
            for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
                  p != end;
                  ++p )
            {
                if ( (*p)->alive != DISABLE )
                {
                    if ( (*p)->team()->side() == LEFT )
                        M_keepers++;
                    else if ( (*p)->team()->side() == RIGHT )
                        M_takers++;
                }
            }

            logHeader();
            resetField();
        }
    }
}

bool
KeepawayRef::ballInKeepawayArea()
{
    PVector ball_pos = M_stadium.ball().pos();
    return ( std::fabs( ball_pos.x ) < ServerParam::instance().ka_length * 0.5 &&
             std::fabs( ball_pos.y ) < ServerParam::instance().ka_width * 0.5 );
}

void
KeepawayRef::logHeader()
{
    if ( M_stadium.kawayLog() )
    {
        M_stadium.kawayLog() << "# Keepers: " << M_keepers << '\n'
                             << "# Takers:  " << M_takers << '\n'
                             << "# Region:  " << ServerParam::instance().ka_length
                             << " x " << ServerParam::instance().ka_width << std::endl;

        M_stadium.kawayLog() << "#\n";

        M_stadium.kawayLog() << "# Description of Fields:\n"
                             << "# 1) Episode number\n"
                             << "# 2) Start time in simulator steps (100ms)\n"
                             << "# 3) End time in simulator steps (100ms)\n"
                             << "# 4) Duration in simulator steps (100ms)\n"
                             << "# 5) (o)ut of bounds / (t)aken away\n";

        M_stadium.kawayLog() << "#\n";
    }
}

void
KeepawayRef::logEpisode( const char *endCond )
{
    if ( M_stadium.kawayLog() )
    {
        M_stadium.kawayLog() << M_episode++ << "\t"
                             << M_time << "\t"
                             << M_stadium.time() << "\t"
                             << M_stadium.time() - M_time << "\t"
                             << endCond << std::endl;
    }

    M_time = M_stadium.time();
}

void
KeepawayRef::resetField()
{
    int keeper_pos = irand( M_keepers );
    //int keeper_pos = boost::uniform_smallint<>( 0, M_keepers - 1 )( rcss::random::DefaultRNG::instance() );

    const Stadium::PlayerCont::iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {
        if( (*p)->alive != DISABLE )
        {
            double x, y;
            if ( (*p)->team()->side() == LEFT )
            {
                switch( keeper_pos )
                {
                case 0:
                    x = -ServerParam::instance().ka_length * 0.5 + drand( 0, 3 );
                    y = -ServerParam::instance().ka_width * 0.5 + drand( 0, 3 );
                    break;
                case 1:
                    x = ServerParam::instance().ka_length * 0.5 - drand( 0, 3 );
                    y = -ServerParam::instance().ka_width * 0.5 + drand( 0, 3 );
                    break;
                case 2:
                    x = ServerParam::instance().ka_length * 0.5 - drand( 0, 3 );
                    y = ServerParam::instance().ka_width * 0.5 - drand( 0, 3 );
                    break;
                default:
                    x = drand( -1, 1 ); y = drand( -1, 1 );
                    break;
                }

                (*p)->place( PVector( x, y ) );

                keeper_pos = ( keeper_pos + 1 ) % M_keepers;
            }
            else if ( (*p)->team()->side() == RIGHT )
            {
                x = -ServerParam::instance().ka_length * 0.5 + drand( 0, 3 );
                y = ServerParam::instance().ka_width * 0.5 - drand( 0, 3 );

                (*p)->place( PVector( x, y ) );
            }
        }
    }

    M_stadium.set_ball( NEUTRAL,
                        PVector( -ServerParam::instance().ka_length * 0.5 + 4.0,
                                 -ServerParam::instance().ka_width * 0.5 + 4.0 ) );
    M_take_time = 0;
}


//************
// CatchRef
//************


void
CatchRef::kickTaken( const Player & kicker )
{
    if ( ! kicker.isGoalie() )
    {
        if ( kicker.team()->side() == LEFT )
        {
            M_team_l_touched = true;
        }
        else if ( kicker.team()->side() == RIGHT )
        {
            M_team_r_touched = true;
        }

        if ( M_team_l_touched && M_team_r_touched )
        {
            M_last_back_passer = NULL;
        }
        else
        {
            M_last_back_passer = &kicker;
        }
    }
    else if ( M_last_back_passer != NULL
              && M_last_back_passer->team() != kicker.team() )
    {
        M_last_back_passer = NULL;
        // The else if above is to handle rare situations where a player from team
        // A kicks the ball, the goalie from team B kicks it, and then the goalie
        // from team A cacthes it.  This should not be concidered a back pass and
        // the else if make sure of that.
    }
}

void
CatchRef::ballTouched( const Player & player )
{
    // If ball is not kicked, back pass violation is never taken.
    if ( ! player.isGoalie() )
    {
        if ( player.team()->side() == LEFT )
        {
            M_team_l_touched = true;
        }
        else if ( player.team()->side() == RIGHT )
        {
            M_team_r_touched = true;
        }

        if ( M_team_l_touched && M_team_r_touched )
        {
            M_last_back_passer = NULL;
        }
    }
}

void
CatchRef::ballCaught( const Player & catcher )
{
    if ( isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    // check handling violation
    if ( M_stadium.playmode() != PM_AfterGoal_Left
         && M_stadium.playmode() != PM_AfterGoal_Right
         && M_stadium.playmode() != PM_TimeOver
         && ! inPenaltyArea( catcher.team()->side(), M_stadium.ball().pos() ) )
    {
        callCatchFault( catcher.team()->side(), M_stadium.ball().pos() );
        return;
    }

    // check backpass violation
    if ( M_stadium.playmode() != PM_AfterGoal_Left
         && M_stadium.playmode() != PM_AfterGoal_Right
         && M_stadium.playmode() != PM_TimeOver
         //         && ! M_stadium.ball().wasMultiKicked()
         && M_last_back_passer != NULL
         && M_last_back_passer != &catcher
         && M_last_back_passer->team() == catcher.team()
         && ServerParam::instance().backPasses() )
    {
        //M_last_back_passer->alive |= BACK_PASS;
        M_stadium.setPlayerState( M_last_back_passer->team()->side(),
                                  M_last_back_passer->unum(),
                                  BACK_PASS );
        callBackPass( catcher.team()->side() );

        return;
    }

    //M_stadium.M_ball_catcher = &catcher;
    awardFreeKick( catcher.team()->side(), M_stadium.ball().pos() );
}


void
CatchRef::analyse()
{
    M_team_l_touched = false;
    M_team_r_touched = false;

    if ( isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    if ( M_stadium.playmode() == PM_Back_Pass_Left )
    {
        clearPlayersFromBall( LEFT );
        if ( ++M_after_back_pass_time > AFTER_BACKPASS_WAIT)
        {
            M_stadium.change_play_mode( PM_FreeKick_Right );
        }
        return;
    }

    if ( M_stadium.playmode() == PM_Back_Pass_Right )
    {
        clearPlayersFromBall( RIGHT );
        if ( ++M_after_back_pass_time > AFTER_BACKPASS_WAIT )
        {
            M_stadium.change_play_mode( PM_FreeKick_Left );
        }
        return;
    }

    if ( M_stadium.playmode() == PM_CatchFault_Left )
    {
        clearPlayersFromBall( LEFT );
        if( ++M_after_catch_fault_time > AFTER_CATCH_FAULT_WAIT )
        {
            M_stadium.change_play_mode( PM_IndFreeKick_Right );
            //M_stadium.change_play_mode( PM_FreeKick_Right );
        }
        return;
    }

    if ( M_stadium.playmode() == PM_CatchFault_Right )
    {
        clearPlayersFromBall( RIGHT );
        if( ++M_after_catch_fault_time > AFTER_CATCH_FAULT_WAIT )
        {
            M_stadium.change_play_mode( PM_IndFreeKick_Left );
            //M_stadium.change_play_mode( PM_FreeKick_Left );
        }
        return;
    }


    if ( M_stadium.ballCatcher()
         && M_stadium.playmode() != PM_AfterGoal_Left
         && M_stadium.playmode() != PM_AfterGoal_Right
         && M_stadium.playmode() != PM_TimeOver
         && ! inPenaltyArea( M_stadium.ballCatcher()->team()->side(),
                             M_stadium.ball().pos() ) )
    {
        callCatchFault( M_stadium.ballCatcher()->team()->side(),
                        M_stadium.ball().pos() );
    }

}

void
CatchRef::playModeChange( PlayMode pmode )
{
    if ( pmode != PM_PlayOn )
    {
        M_last_back_passer = NULL;
    }
}


void
CatchRef::callBackPass( const Side side )
{
    PVector pos = truncateToPitch( M_stadium.ball().pos() );
    pos = moveOutOfPenalty( side, pos );

    M_stadium.clearBallCatcher();

    if ( side == LEFT )
    {
        M_stadium.placeBall( PM_Back_Pass_Left, RIGHT, pos );
    }
    else
    {
        M_stadium.placeBall( PM_Back_Pass_Right, LEFT, pos );
    }

    M_last_back_passer = NULL;
    M_after_back_pass_time = 0;
}

void
CatchRef::callCatchFault( Side side, PVector pos )
{
    pos = truncateToPitch( pos );
    //pos = moveIntoPenalty( side, pos );
    pos = moveOutOfPenalty( side, pos );

    M_stadium.clearBallCatcher();

    if ( side == LEFT )
    {
        M_stadium.placeBall( PM_CatchFault_Left, RIGHT, pos );
    }
    else if ( side == RIGHT )
    {
        M_stadium.placeBall( PM_CatchFault_Right, LEFT, pos );
    }

    M_after_catch_fault_time = 0;
}


//************
// PenaltyRef
//************

void
PenaltyRef::analyse()
{
    analyseImpl();

    M_prev_ball_pos = M_stadium.ball().pos();
}

void
PenaltyRef::analyseImpl()
{
    static char *PlayModeString[] = PLAYMODE_STRINGS;
    static bool bFirstTime = true;

    // if normal and extra time are over -> start the penalty procedure or quit
    if ( bFirstTime
         && ServerParam::instance().penalty_shoot_outs
         && ( ( ServerParam::instance().halfTime() < 0
                && ( ServerParam::instance().nr_normal_halfs
                     + ServerParam::instance().nr_extra_halfs == 0 ) )
              || ( M_stadium.time() >= ServerParam::instance().halfTime()
                   * ( ServerParam::instance().nr_normal_halfs
                       + ServerParam::instance().nr_extra_halfs ) )
              )
         && M_stadium.teamLeft().point() == M_stadium.teamRight().point()
         && M_stadium.playmode() != PM_BeforeKickOff )
    {
        if ( drand( 0, 1 ) < 0.5 )       // choose random side of the playfield
        {
            M_pen_side = LEFT;            // and inform players
        }
        else
        {
            M_pen_side = RIGHT;
        }

        M_stadium.say( M_pen_side == LEFT
                       ? "penalty_onfield_l"
                       : "penalty_onfield_r" );
        // choose at random who starts (note that in penalty_init, actually the
        // opposite player is chosen since there the playMode changes)
        M_cur_pen_taker = ( drand( 0, 1 ) < 0.5 ) ? LEFT : RIGHT;

        // place the goalkeeper of the opposite field close to the penalty goal
        // otherwise it is hard to get there before pen_setup_wait cycles
        Side side = ( M_pen_side == LEFT ) ? RIGHT : LEFT;
        const Stadium::PlayerCont::iterator end = M_stadium.players().end();
        for ( Stadium::PlayerCont::iterator p = M_stadium.players().begin();
              p != end;
              ++p )
        {
            if ( (*p)->alive != DISABLE
                 && (*p)->team()->side() == side
                 && (*p)->isGoalie() )
            {
                (*p)->moveTo( PVector( -M_pen_side * (PITCH_LENGTH/2-10), 10 ) );
            }
        }

        penalty_init();
        bFirstTime = false;
    }

    if ( ! isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    const PlayMode pm = M_stadium.playmode();

    bool bCheckLeft  = penalty_check_players( LEFT );
    bool bCheckRight = penalty_check_players( RIGHT );

    if ( M_bDebug )
    {
        std::cerr << "timer " << PlayModeString[M_stadium.playmode()] << " "
                  << M_stadium.time() << " " << M_timer << " " << bCheckLeft
                  << " " << bCheckRight << std::endl;
    }

    // if ready or taken make sure all players keep well-positioned
    if ( ServerParam::instance().pen_coach_moves_players
         && ( pm == PM_PenaltyReady_Left || pm == PM_PenaltyReady_Right
              || pm == PM_PenaltyTaken_Left || pm == PM_PenaltyTaken_Right )
         )
    {
        if ( ! bCheckLeft )
        {
            penalty_place_all_players( LEFT );
        }

        if ( ! bCheckRight )
        {
            penalty_place_all_players( RIGHT );
        }

        bCheckLeft = bCheckRight = true;
    }

    if ( M_bDebug )
        std::cerr << "after placement" << std::endl;

    if ( M_timer < 0 )
    {
        std::cerr << "(PenaltyRef::analyse) timer cannot be negative?" << std::endl;
    }
    else if ( M_timer == 0 )
    {
        handleTimeout( bCheckLeft, bCheckRight );
    }
    else // M_timer > 0
    {
        handleTimer( bCheckLeft, bCheckRight );
    }
}

void
PenaltyRef::handleTimeout( bool left_move_check,
                           bool right_move_check )
{
    const PlayMode pm = M_stadium.playmode();

    if ( M_bDebug )
        std::cerr << M_stadium.time() << " timer reached zero" << std::endl;

    // when setup has finished and still players are positioned incorrectly
    // replace them and go to ready mode.
    if ( ServerParam::instance().pen_coach_moves_players
         && ( pm == PM_PenaltySetup_Left
              || pm == PM_PenaltySetup_Right )
         )
    {
        if ( ! left_move_check )
        {
            penalty_place_all_players( LEFT );
        }

        if ( ! right_move_check )
        {
            penalty_place_all_players( RIGHT );
        }

        left_move_check = right_move_check = true;
    }


    if ( pm == PM_PenaltyMiss_Left
         || pm == PM_PenaltyScore_Left
         || pm == PM_PenaltyMiss_Right
         || pm == PM_PenaltyScore_Right )
    {
        penalty_init();
    }
    else if ( left_move_check
              && right_move_check )
    {
        if ( pm == PM_PenaltySetup_Left )
        {
            M_stadium.change_play_mode( PM_PenaltyReady_Left );
        }
        else if ( pm == PM_PenaltySetup_Right )
        {
            M_stadium.change_play_mode( PM_PenaltyReady_Right );
        }
        // time elapsed -> missed goal
        else if ( pm == PM_PenaltyTaken_Left
                  || pm == PM_PenaltyReady_Left )
        {
            penalty_miss( LEFT );
        }
        else if ( pm == PM_PenaltyTaken_Right
                  || pm == PM_PenaltyReady_Right )
        {
            penalty_miss( RIGHT );
        }
    }
    // if incorrect positioned , place them correctly
    else if ( M_cur_pen_taker == LEFT )
    {
        penalty_foul( ( left_move_check == false ) ? LEFT : RIGHT );
    }
    else if ( M_cur_pen_taker == RIGHT )
    {
        penalty_foul( ( right_move_check == false ) ? RIGHT : LEFT );
    }
}

void
PenaltyRef::handleTimer( const bool left_move_check,
                         const bool right_move_check )
{
    const PlayMode pm = M_stadium.playmode();

    --M_timer;

    if ( pm == PM_PenaltyScore_Left
         || pm == PM_PenaltyScore_Right
         || pm == PM_PenaltyMiss_Left
         || pm == PM_PenaltyMiss_Right )
    {
        // freeze the ball
//         M_stadium.ball().moveTo( M_stadium.ball().pos(),
//                                 //0.0,
//                                 PVector( 0.0, 0.0 ),
//                                 PVector( 0.0, 0.0 ) );
        M_stadium.set_ball( M_cur_pen_taker,
                            M_stadium.ball().pos() );

        return;
    }

    if ( left_move_check
         && right_move_check )
    {
        // if ball crossed goalline, process goal and set ball on goalline
        if ( crossGoalLine( M_pen_side, M_prev_ball_pos ) )
        {
            if ( pm == PM_PenaltyTaken_Left )
            {
                penalty_score( LEFT );
            }
            else if ( pm == PM_PenaltyTaken_Right )
            {
                penalty_score( RIGHT );
            }
            // freeze the ball at the current position.
            M_stadium.set_ball( M_pen_side, M_stadium.ball().pos() );
        }
        else if ( std::fabs( M_stadium.ball().pos().x )
                 > PITCH_LENGTH * 0.5 + ServerParam::instance().bsize
                 || std::fabs( M_stadium.ball().pos().y )
                 > PITCH_WIDTH * 0.5 + ServerParam::instance().bsize )
        {
            if ( M_bDebug )
                std::cerr << "MISSED" << std::endl;

            M_stadium.set_ball( M_pen_side, M_stadium.ball().pos() );
            if ( pm == PM_PenaltyTaken_Left )
            {
                penalty_miss( LEFT );
            }
            else if ( pm == PM_PenaltyTaken_Right )
            {
                penalty_miss( RIGHT );
            }
        }
    }
    // if someone makes foul and we are not in setup -> replace the players
    else if ( pm == PM_PenaltyReady_Left
              || pm == PM_PenaltyReady_Right
              || pm == PM_PenaltyTaken_Left
              || pm == PM_PenaltyTaken_Right )
    {
        if ( ServerParam::instance().pen_coach_moves_players )
        {
            if ( left_move_check == false )
            {
                penalty_place_all_players( LEFT );
            }
            if ( right_move_check == false )
            {
                penalty_place_all_players( RIGHT );
            }
        }
        else if ( M_cur_pen_taker == LEFT )
        {
            penalty_foul( ( left_move_check == false ) ? LEFT : RIGHT );
        }
        else if ( M_cur_pen_taker == RIGHT )
        {
            penalty_foul( ( right_move_check == false ) ? RIGHT : LEFT );
        }
    }
}



void
PenaltyRef::ballCaught( const Player & catcher )
{
    if ( ! isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    if ( M_bDebug )
        std::cerr << "GOALIE CATCH" << std::endl;

    if ( M_stadium.playmode() == PM_PenaltyTaken_Left
         || M_stadium.playmode() == PM_PenaltyTaken_Right )
    {
        if ( catcher.team()->side() == M_cur_pen_taker )
        {
            std::cerr << "catch by taker side goalie. foul "
                      << -M_cur_pen_taker
                      << std::endl;
            // taker team's goalie catches the ball
            penalty_foul( (Side)( -M_cur_pen_taker ) );
        }
        else if ( ! inPenaltyArea( M_pen_side, M_stadium.ball().pos() ) )
        {
            std::cerr << "outside the penalty area. foul  "
                      << -M_cur_pen_taker
                      << std::endl;
            // goalie catches the ball outside the penalty area
            penalty_foul( (Side)( -M_cur_pen_taker ) );
        }
        else
        {
            std::cerr << "legal catch. miss of the side " << M_cur_pen_taker
                      << std::endl;
            // legal catch
            penalty_miss( M_cur_pen_taker );
        }
    }
    else if ( M_stadium.playmode() == PM_PenaltyReady_Left
              || M_stadium.playmode() == PM_PenaltyReady_Right )
    {
        std::cerr << "ball caught in ready mode. foul " << -M_cur_pen_taker
                  << std::endl;
        penalty_foul( catcher.team()->side() );
    }

    // freeze the ball
//     M_stadium.ball().moveTo( M_stadium.ball().pos(),
//                             //0.0,
//                             PVector( 0.0, 0.0 ),
//                             PVector( 0.0, 0.0 ) );
    M_stadium.set_ball( NEUTRAL, M_stadium.ball().pos() );

    //M_stadium.ball().vel.x = M_stadium.ball().vel.y = 0;
}

void
PenaltyRef::kickTaken( const Player & kicker )
{
    if ( ! isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    if ( M_bDebug )
        std::cerr << "kick taken in mode " << M_stadium.playmode() << std::endl;

    if ( M_stadium.ballCatcher() != NULL )
    {
        std::cerr << "player kicked and goalie catched at the same time" << std::endl;
    }
    // if in setup it is not allowed to kick the ball
    else if ( M_stadium.playmode() == PM_PenaltySetup_Left
              || M_stadium.playmode() == PM_PenaltySetup_Right )
    {
        penalty_foul( kicker.team()->side() );
    }
    // cannot kick second time after penalty was taken
    else if ( ServerParam::instance().pen_allow_mult_kicks == false
              && ( M_stadium.playmode() == PM_PenaltyTaken_Left
                   || M_stadium.playmode() == PM_PenaltyTaken_Right )
              && kicker.team()->side() == M_cur_pen_taker )
    {
        penalty_foul( M_cur_pen_taker );
    }
    else if ( M_stadium.playmode() == PM_PenaltyReady_Left
              || M_stadium.playmode() == PM_PenaltyTaken_Left
              || M_stadium.playmode() == PM_PenaltyReady_Right
              || M_stadium.playmode() == PM_PenaltyTaken_Right )
    {
        if ( ( M_stadium.playmode() == PM_PenaltyReady_Left
               || M_stadium.playmode() == PM_PenaltyReady_Right )
             && kicker.team()->side() == M_cur_pen_taker
             && ( ( LEFT == M_cur_pen_taker
                    && M_sLeftPenTaken.find( kicker.unum() ) != M_sLeftPenTaken.end() )
                  || ( RIGHT == M_cur_pen_taker
                       && M_sRightPenTaken.find( kicker.unum() ) != M_sRightPenTaken.end() )
                  )
             )
        {
            // this kicker has already taken the kick
            penalty_foul( M_cur_pen_taker );
        }
        else if ( M_last_taker
                  && M_last_taker->team()->side() == M_cur_pen_taker
                  && M_last_taker != &kicker )
        {
            // not a taker player in the same team must not kick the ball.
            penalty_foul( M_cur_pen_taker );
        }
        else if ( kicker.team()->side() != M_cur_pen_taker
                  && ! kicker.isGoalie() )
        {
            // field player in the defending team must not kick the ball.
            penalty_foul( (Side)( -M_cur_pen_taker ) );
        }
        else
        {
            M_last_taker = &kicker;
        }
    }

    // if we were ready for penalty -> change play mode
    if ( M_stadium.playmode() == PM_PenaltyReady_Left )
    {
        // when penalty is taken, add player, multiple copies are deleted
        M_sLeftPenTaken.insert( kicker.unum() );
        if ( M_sLeftPenTaken.size() == MAX_PLAYER )
        {
            M_sLeftPenTaken.clear();
        }
        M_stadium.change_play_mode( PM_PenaltyTaken_Left );
    }
    else if ( M_stadium.playmode() == PM_PenaltyReady_Right )
    {
        M_sRightPenTaken.insert( kicker.unum() );
        if ( M_sRightPenTaken.size() == MAX_PLAYER )
        {
            M_sRightPenTaken.clear();
        }
        M_stadium.change_play_mode( PM_PenaltyTaken_Right );
    }
    // if it was not allowed to kick, don't move ball
    else if ( M_stadium.playmode() != PM_PenaltyTaken_Left
              && M_stadium.playmode() != PM_PenaltyTaken_Right )
    {
        M_stadium.set_ball( M_pen_side, M_stadium.ball().pos() );
    }
}

void
PenaltyRef::playModeChange( PlayMode pm )
{
    // if mode changes, reset the timer
    if ( ! isPenaltyShootOut( M_stadium.playmode() ) )
    {
        return;
    }

    if ( pm == PM_PenaltySetup_Left || pm == PM_PenaltySetup_Right )
    {
        M_last_taker = NULL;
        M_timer = ServerParam::instance().pen_setup_wait;
    }
    else if ( pm == PM_PenaltyReady_Left || pm == PM_PenaltyReady_Right )
    {
        M_last_taker = NULL;
        M_timer = ServerParam::instance().pen_ready_wait;
    }
    else if ( pm == PM_PenaltyTaken_Left || pm == PM_PenaltyTaken_Right )
    {
        M_timer = ServerParam::instance().pen_taken_wait;
    }
    else if ( pm == PM_PenaltyMiss_Left   || pm == PM_PenaltyMiss_Right ||
              pm == PM_PenaltyScore_Right || pm == PM_PenaltyScore_Right )
    {
        M_last_taker = NULL;
        M_timer = ServerParam::instance().pen_before_setup_wait;
    }
    else
    {
        M_last_taker = NULL;
    }
}


void
PenaltyRef::penalty_init()
{
    if ( M_bDebug )
        std::cerr << "init penalty" << std::endl;

    // change the play mode such that the other side can take the penalty
    // and place the ball at the penalty spot
    M_cur_pen_taker = ( M_cur_pen_taker == LEFT
                        ? RIGHT
                        : LEFT );
    PlayMode pm = ( M_cur_pen_taker == LEFT
                    ? PM_PenaltySetup_Left
                    : PM_PenaltySetup_Right );
    M_stadium.placeBall( pm,
                         NEUTRAL,
                         PVector( - M_pen_side * ( PITCH_LENGTH/2 - ServerParam::instance().pen_dist_x ),
                                  0.0 ) );
}

void
PenaltyRef::penalty_score( Side side )
{
    M_stadium.change_play_mode( side == RIGHT
                                ? PM_PenaltyScore_Right
                                : PM_PenaltyScore_Left );

    if ( side == RIGHT )
    {
        M_stadium.penaltyScore( RIGHT, true );
    }
    else
    {
        M_stadium.penaltyScore( LEFT, true );
    }
    M_pen_nr_taken++;
    penalty_check_score();
}

void
PenaltyRef::penalty_miss( Side side )
{
    M_stadium.change_play_mode( side == LEFT
                                ? PM_PenaltyMiss_Left
                                : PM_PenaltyMiss_Right );
    M_pen_nr_taken++;

    if ( side == RIGHT )
    {
        M_stadium.penaltyScore( RIGHT, false );
    }
    else
    {
        M_stadium.penaltyScore( LEFT, false );
    }

    penalty_check_score();
}

void
PenaltyRef::penalty_foul( const Side side )
{
    if ( M_bDebug )
        std::cerr << "penalty_foul " << side << std::endl;

    M_stadium.say( ( side == LEFT ) ?  "penalty_foul_l" : "penalty_foul_r" );

    // if team takes penalty and makes mistake -> miss, otherwise -> score
    if ( side == LEFT && M_cur_pen_taker == LEFT )
    {
        penalty_miss( LEFT );
    }
    else if ( side == RIGHT && M_cur_pen_taker == RIGHT )
    {
        penalty_miss( RIGHT );
    }
    else if ( side == LEFT )
    {
        penalty_score( RIGHT );
    }
    else
    {
        penalty_score( LEFT );
    }
}

void
PenaltyRef::penalty_check_score()
{
    if ( M_bDebug ) std::cerr << "penalty_check_score" << std::endl;

    std::cerr << "Score: " << ( (M_cur_pen_taker == LEFT) ? "*" : " " )
              << M_stadium.teamLeft().penaltyPoint() << "-"
              << M_stadium.teamRight().penaltyPoint()
              << ((M_cur_pen_taker == RIGHT) ? "*" : " ") << " after "
              << M_pen_nr_taken << " penalties." << std::endl;

    // if both players have taken more than nr_kicks penalties -> check for winner
    if ( M_pen_nr_taken > 2 * ServerParam::instance().pen_nr_kicks )
    {
        if( M_pen_nr_taken % 2 == 0
            && ( M_stadium.teamLeft().penaltyPoint()
                 != M_stadium.teamRight().penaltyPoint() ) )
        {
            std::cerr << "Final score: "
                      << M_stadium.teamLeft().penaltyPoint() << "-"
                      << M_stadium.teamRight().penaltyPoint() << std::endl;
            if ( M_stadium.teamLeft().penaltyPoint() > M_stadium.teamRight().penaltyPoint() )
            {
                M_stadium.say( "penalty_winner_l" );
            }
            else
            {
                M_stadium.say( "penalty_winner_r" );
            }
            M_stadium.change_play_mode( PM_TimeOver );
        }
    }
    // if both players have taken nr_kicks and max_extra_kicks penalties -> quit
    else if ( M_pen_nr_taken > 2 * ( ServerParam::instance().pen_max_extra_kicks
                                     + ServerParam::instance().pen_nr_kicks )
              )
    {
        std::cerr << "Final score: "
                  << M_stadium.teamLeft().penaltyPoint() << "-"
                  << M_stadium.teamRight().penaltyPoint() << std::endl;
        if ( ServerParam::instance().pen_random_winner )
        {
            if ( drand( 0, 1 ) < 0.5 )
            {
                M_stadium.say( "penalty_winner_l" );
                M_stadium.penaltyWinner( LEFT );
                std::cerr << "Left team has won the coin toss!" << std::endl;
            }
            else
            {
                M_stadium.say( "penalty_winner_r" );
                M_stadium.penaltyWinner( RIGHT );
                std::cerr << "Right team has won the coin toss!" << std::endl;
            }
        }
        else
        {
            M_stadium.say( "penalty_draw" );
        }
        M_stadium.change_play_mode( PM_TimeOver );
    }
    // during normal kicks, check whether one team cannot win anymore
    else
    {
        // first calculate how many penalty kick sessions are left
        // and add this to the current number of points of both teams
        // finally, subtract 1 point from the team that has already shot this turn
        int iPenLeft = ServerParam::instance().pen_nr_kicks - M_pen_nr_taken/2;
        int iMaxExtraLeft  = M_stadium.teamLeft().penaltyPoint() + iPenLeft;
        int iMaxExtraRight = M_stadium.teamRight().penaltyPoint() + iPenLeft;
        if ( M_pen_nr_taken % 2 == 1 )
        {
            if ( M_cur_pen_taker == LEFT )
            {
                iMaxExtraLeft--;
            }
            else if ( M_cur_pen_taker == RIGHT )
            {
                iMaxExtraRight--;
            }
        }

        if ( iMaxExtraLeft < M_stadium.teamRight().penaltyPoint() )
        {
            std::cerr << "Final score: "
                      << M_stadium.teamLeft().penaltyPoint() << "-"
                      << M_stadium.teamRight().penaltyPoint() << std::endl;
            M_stadium.say( "penalty_winner_r" );
            M_stadium.change_play_mode( PM_TimeOver );
        }
        else if ( iMaxExtraRight < M_stadium.teamLeft().penaltyPoint() )
        {
            std::cerr << "Final score: "
                      << M_stadium.teamLeft().penaltyPoint() << "-"
                      << M_stadium.teamRight().penaltyPoint() << std::endl;
            M_stadium.say( "penalty_winner_l" );
            M_stadium.change_play_mode( PM_TimeOver );
        }
    }
}

bool
PenaltyRef::penalty_check_players( const Side side )
{
    PlayMode pm = M_stadium.playmode();
    int     iOutsideCircle = 0;
    bool    bCheck         = true;
    PVector posGoalie;
    //int     iPlayerOutside = -1, iGoalieNr=-1;
    const Player * outside_player = NULL;
    const Player * goalie = NULL;

    if ( pm == PM_PenaltyMiss_Left  || pm == PM_PenaltyMiss_Right
         || pm == PM_PenaltyScore_Left || pm == PM_PenaltyScore_Right )
    {
        return true;
    }

    // for all players from side 'side' get the goalie pos and count how many
    // players are outside the center circle.
    const Stadium::PlayerCont::const_iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {
        if ( (*p)->alive == DISABLE )
            continue;
        if ( (*p)->team()->side() == side )
        {
            if ( (*p)->isGoalie() )
            {
                goalie = *p;
                posGoalie = (*p)->pos();
                continue;
            }

            CArea c( PVector( 0.0, 0.0 ),
                     KICK_OFF_CLEAR_DISTANCE
                     - (*p)->size() );
            if ( ! c.inArea( (*p)->pos() ) )
            {
                iOutsideCircle++;
                outside_player = *p;
            }
        }
    }

    if ( ! goalie )
    {
        return false;
    }

    // if the 'side' equals the one that takes the penalty shoot out
    if ( side == M_cur_pen_taker )
    {
        // in case that goalie takes penalty kick
        // or taker goes into the center circle
        if ( iOutsideCircle == 0 )
        {
            if ( pm == PM_PenaltySetup_Left || pm == PM_PenaltySetup_Right )
            {
                if ( goalie->pos().distance( M_stadium.ball().pos() ) > 2.0 )
                {
                    // bCheck = false;
                }
                else
                {
                    outside_player = goalie;
                }
            }
        }
        // if goalie not outside field, check fails
        else if ( std::fabs( posGoalie.x ) < PITCH_LENGTH/2.0 - 1.5
                  || std::fabs( posGoalie.y ) < PENALTY_AREA_WIDTH/2.0 - 1.5 )
        {
            if ( M_bDebug )
                std::cerr << "side " << side << " goalie not correct "
                          <<  posGoalie.x << " " << posGoalie.y << std::endl;
            bCheck = false;
        }
        // only one should be outside the circle -> player that takes penalty
        else if ( iOutsideCircle > 1 )
        {
            if ( M_bDebug )
                std::cerr << "side " << side << " not 1 player outside " << std::endl;
            bCheck = false;
        }
        // in setup, player outside circle should be close to ball
        else if ( ( pm == PM_PenaltySetup_Left || pm == PM_PenaltySetup_Right )
                  && iOutsideCircle == 1 )
        {
            if ( outside_player
                 && outside_player->pos().distance( M_stadium.ball().pos() ) > 2.0 )
            {
                if ( M_bDebug )
                    std::cerr << "side " << side << " attacker not correct " << std::endl;
                bCheck = false;
            }
        }
    }
    else //other team
    {
        // goalie does not stand in front of goal line
        if ( M_stadium.playmode() != PM_PenaltyTaken_Left
             && M_stadium.playmode() != PM_PenaltyTaken_Right )
        {
            if ( std::fabs( posGoalie.x )
                 < PITCH_LENGTH/2.0 - ServerParam::instance().pen_max_goalie_dist_x
                 || std::fabs( posGoalie.y ) > GOAL_WIDTH/2.0 )
            {
                if ( M_bDebug )
                    std::cerr << "side " << side << " goalie to catch not correct "
                              << posGoalie << std::endl;
                bCheck = false;
            }
        }
        // when receiving the penalty every player should be in center circle
        if ( iOutsideCircle != 0 )
        {
            if ( M_bDebug )
                std::cerr << "side " << side << " not everyone in circle " << std::endl;
            bCheck = false;
        }
    }

    if ( bCheck
         && outside_player )
    {
        // if in setup and already in set -> check fails
        if( ( side == LEFT
              && M_stadium.playmode() == PM_PenaltySetup_Left
              && M_sLeftPenTaken.find( outside_player->unum() )
              != M_sLeftPenTaken.end() )
            || ( side == RIGHT
                 && M_stadium.playmode() == PM_PenaltySetup_Right
                 && M_sRightPenTaken.find( outside_player->unum() )
                 != M_sRightPenTaken.end() )
            )
        {
            if ( M_bDebug )
                std::cerr << "side " << side << " same player" << std::endl;
            bCheck = false;
        }
    }

    return bCheck;
}

void
PenaltyRef::penalty_place_all_players( const Side side )
{
    if ( side == M_cur_pen_taker )
    {
        placeTakerTeamPlayers();
    }
    else // other team
    {
        placeOtherTeamPlayers();
    }
}

void
PenaltyRef::placeTakerTeamPlayers()
{
    const bool bPenTaken = ( M_stadium.playmode() == PM_PenaltyTaken_Right
                             || M_stadium.playmode() == PM_PenaltyTaken_Left );

    const Player * taker = ( M_last_taker
                             ? M_last_taker
                             : getCandidateTaker() );

    const PVector goalie_wait_pos_b( -M_pen_side * ( PITCH_LENGTH / 2 + 2.0 ), +25.0 );
    const PVector goalie_wait_pos_t( -M_pen_side * ( PITCH_LENGTH / 2 + 2.0 ), -25.0 );

    // then replace the players from the specified side
    const Stadium::PlayerCont::iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {
        if ( (*p)->alive == DISABLE )
            continue;
        if ( (*p)->team()->side() != M_cur_pen_taker )
            continue;

        if ( (*p) == taker )
        {
            if ( ! bPenTaken
                 && taker->pos().distance( M_stadium.ball().pos() ) > 2.0 )
            {
                //PVector new_pos( -M_pen_side * ( PITCH_LENGTH/2 - ServerParam::instance().pen_dist_x - 2.0 ),
                //0.0 );
                CArea c( M_stadium.ball().pos(), 2.0 );
                //(*p)->moveTo( new_pos );
                (*p)->moveTo( c.nearestEdge( taker->pos() ) );
            }
        }
        else
        {
            if ( (*p)->isGoalie() )
            {
                CArea c( ( (*p)->pos().y > 0.0 ? goalie_wait_pos_b : goalie_wait_pos_t ),
                         2.0 );
                if ( ! c.inArea( (*p)->pos() ) )
                {
                    (*p)->moveTo( c.nearestEdge( (*p)->pos() ) );
                }
            }
            else // not goalie
            {
                CArea center( PVector( 0.0, 0.0 ),
                              KICK_OFF_CLEAR_DISTANCE
                              - (*p)->size()
                              //- ServerParam::instance().pspeed_max
                              );
                if ( ! center.inArea( (*p)->pos() ) )
                {
                    //(*p)->moveTo( PVector::fromPolar( 6.5, Deg2Rad( i*15 ) ) );
                    (*p)->moveTo( center.nearestEdge( (*p)->pos() ) );
                }
            }
        }
    }
}

void
PenaltyRef::placeOtherTeamPlayers()
{
    const bool bPenTaken = ( M_stadium.playmode() == PM_PenaltyTaken_Right
                             || M_stadium.playmode() == PM_PenaltyTaken_Left );
    const double goalie_line
        = ( M_pen_side == LEFT
            ? -PITCH_LENGTH/2.0 + ServerParam::instance().pen_max_goalie_dist_x
            : +PITCH_LENGTH/2.0 - ServerParam::instance().pen_max_goalie_dist_x );

    const Stadium::PlayerCont::iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {
        if ( (*p)->alive == DISABLE )
            continue;
        if ( (*p)->team()->side() == M_cur_pen_taker )
            continue;

        // only move goalie in case the penalty has not been started yet.
        if ( (*p)->isGoalie() )
        {
            if ( ! bPenTaken )
            {
                if ( M_pen_side == LEFT )
                {
                    if ( (*p)->pos().x - goalie_line > 0.0 )
                    {
                        (*p)->moveTo( PVector( goalie_line - 1.5, 0.0 ) );
                    }
                }
                else
                {
                    if ( (*p)->pos().x - goalie_line < 0.0 )
                    {
                        (*p)->moveTo( PVector( goalie_line + 1.5, 0.0 ) );
                    }
                }
            }
        }
        else // not goalie
        {
            CArea center( PVector( 0.0, 0.0 ),
                          KICK_OFF_CLEAR_DISTANCE
                          - (*p)->size()
                          //- ServerParam::instance().pspeed_max
                          );
            if( ! center.inArea( (*p)->pos() ) )
            {
                // place other players in circle in penalty area
                //(*p)->moveTo( PVector::fromPolar( 6.5, Deg2Rad( i*15 ) ) );
                (*p)->moveTo( center.nearestEdge( (*p)->pos() ) );
            }
        }
    }
}

const Player *
PenaltyRef::getCandidateTaker()
{
    const std::set< int > & sPenTaken = ( M_cur_pen_taker == LEFT
                                          ? M_sLeftPenTaken
                                          : M_sRightPenTaken );

    const Player * candidate = NULL;
    const Player * goalie = NULL;
    double min_dist = std::numeric_limits< double >::max();

    // first find the closest player to the ball
    const Stadium::PlayerCont::const_iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {
        if ( (*p)->alive == DISABLE )
            continue;
        if ( (*p)->team()->side() != M_cur_pen_taker )
            continue;

        if ( sPenTaken.find( (*p)->unum() )
             != sPenTaken.end() )
        {
            // players that have already taken a kick cannot be
            // counted as a potential kicker.
            continue;
        }

        if ( (*p)->isGoalie() )
        {
            goalie = (*p);
            continue;
        }

        double d = (*p)->pos().distance( M_stadium.ball().pos() );
        if( d < min_dist )
        {
            min_dist = d;
            candidate = (*p);
        }
    }

    if ( candidate == NULL )
    {
        return goalie;
    }

    return candidate;
}
