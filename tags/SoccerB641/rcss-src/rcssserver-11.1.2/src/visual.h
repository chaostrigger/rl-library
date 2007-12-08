// -*-c++-*-

/***************************************************************************
                                   visual.h
                     Classes for building visual messages
                             -------------------
    begin                : 5-AUG-2002
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

#ifndef RCSSVISUAL_H
#define RCSSVISUAL_H

#include "sender.h"
#include "object.h"
#include "observer.h"
#include "random.h"
#include "utility.h"
#include <rcssbase/lib/factory.hpp>

class Stadium;
class Player;
class MPObject;
class PObject;
class PVector;

namespace rcss
{
class SerializerPlayer;

/*!
//===================================================================
//
//  CLASS: VisualSendor
//
//  DESC: Base class for the visual protocol.
//
//===================================================================
*/

class VisualSender
    : protected Sender
{
public:
    VisualSender( std::ostream& transport );

    virtual
    ~VisualSender();

    virtual
    void
    sendVisual() = 0;
};


/*!
//===================================================================
//
//  CLASS: Observer
//
//  DESC: Interface for an object that receives visual information.
//
//===================================================================
*/

template< typename S >
class Observer
    : protected BaseObserver< S >
{
public:
    typedef S VisualSender;

    Observer()
      {}

    Observer( VisualSender& sender )
        : BaseObserver< VisualSender >( sender )
      {}

    Observer( std::auto_ptr< VisualSender > sender )
        : BaseObserver< VisualSender >( sender )
      {}

    ~Observer()
      {}

    void
    setVisualSender( VisualSender& sender )
      {
          BaseObserver< VisualSender >::setSender( sender );
      }

    void
    setVisualSender( std::auto_ptr< VisualSender > sender )
      {
          BaseObserver< VisualSender >::setSender( sender );
      }

    void
    sendVisual()
      { BaseObserver< VisualSender >::sender().sendVisual(); }
};

/*!
//===================================================================
//
//  CLASS: VisualSenderPlayer
//
//  DESC: Base class for the visual protocol for players.
//
//===================================================================
*/

class VisualSenderPlayer
    : public VisualSender
{
public:
    typedef VisualSenderPlayer Base;

    class Params
    {
    public:
        std::ostream& m_transport;
        const Player& m_self;
        const Stadium& m_stadium;
        const SerializerPlayer& m_ser;

        Params( std::ostream& transport,
                const Player& self,
                const Stadium& stadium,
                const SerializerPlayer& ser )
            : m_transport( transport ),
              m_self( self ),
              m_stadium( stadium ),
              m_ser( ser )
          {}
    };


public:
    typedef std::auto_ptr< VisualSenderPlayer > Ptr;
    typedef Ptr(*Creator)( const VisualSenderPlayer::Params& );
    typedef rcss::lib::Factory< Creator, int > Factory;

    static
    Factory&
    factory();

    VisualSenderPlayer( const Params& params );

    ~VisualSenderPlayer();

protected:
    const SerializerPlayer&
    serializer() const
      { return m_ser; }

    const Player&
    self() const
      { return m_self; }

    const Stadium&
    stadium() const
      { return m_stadium; }

    int
    sendCount() const
      { return m_sendcnt; }

    void
    incSendCount()
      { m_sendcnt++; }

    void
    resetSendCount()
      { m_sendcnt = 0; }

private:
    const SerializerPlayer& m_ser;

    /*:TODO: M_self needs to be replaced with a reference to a
      InitObserver and InitObserver should have virtual functions for
      stuff like velocity, stamina, etc */
    const Player& m_self;
    const Stadium& m_stadium;

    int m_sendcnt;
};

/*!
//===================================================================
//
//  CLASS: VisualSensorPlayerV1
//
//  DESC: Class for the version 1* visual protocol.  This version is
//        completely unused as far as I am aware of, but it is here
//        none the less, just in case there is someone somewhere
//        still using it.
//
//        * It's version 1 to the simualtor in it's current form.
//        From what I know the original simulator was written in
//        lisp and the first C++ version was actually version 3.  I
//        don't know if the protocol was compatible with previous
//        versions, so this may well be the version 3 protocol.
//
//===================================================================
*/


class VisualSenderPlayerV1
    : public VisualSenderPlayer
{
public:

    VisualSenderPlayerV1( const Params & params );

    virtual
    ~VisualSenderPlayerV1();

    virtual
    void
    sendVisual();


private:
    void
    sendFlag( const PObject& obj )
      { (this->*M_send_flag)( obj ); }

    void
    sendBall( const MPObject& obj )
      { (this->*M_send_ball)( obj ); }

    void
    sendPlayer( const Player& obj )
      { (this->*M_send_player)( obj ); }

    void
    serializeLine( const std::string & name,
                   const int dir,
                   const double & sight_2_line_ang,
                   const double & player_2_line )
      {
          (this->*M_serialize_line)( name, dir,
                                     sight_2_line_ang, player_2_line );
      }

    void
    sendFlags();

    void
    sendBalls();

    void
    sendPlayers();

    void
    sendLines();

    void
    sendLowFlag( const PObject& flag );

    void
    sendHighFlag( const PObject& flag );

    void
    sendLowBall( const MPObject& ball );

    void
    sendHighBall( const MPObject& ball );

    void
    sendLowPlayer( const Player& player );

    void
    sendHighPlayer( const Player& player );

    bool
    sendLine( const PObject& line );

    void
    serializeLowLine( const std::string & name,
                      const int dir,
                      const double &, const double & );

    void
    serializeHighLine( const std::string & name,
                       const int dir,
                       const double & sight_2_line_ang,
                       const double & player_2_line );

    double
    calcRadDir( const PObject& obj );

    int
    calcDegDir( const double & rad_dir ) const
      {
          return rad2Deg( rad_dir );
      }

    double
    calcLineRadDir( const double & line_normal ) const;

    int
    calcLineDir( const double & sight_2_line_ang ) const
      {
          if ( sight_2_line_ang > 0 )
              return rad2Deg( sight_2_line_ang - M_PI*0.5 );
          else
              return rad2Deg( sight_2_line_ang + M_PI*0.5 );
      }

    double
    calcUnQuantDist( const PObject& obj ) const;

    double
    calcQuantDist( const double& dist,
                   const double& qstep ) const
      {
          return Quantize( std::exp( Quantize( std::log( dist + EPS ), qstep ) ), 0.1 );
      }

    double
    calcLineDist( const double & sight_2_line_ang,
                  const double & player_2_line,
                  const double & qstep ) const
      {
          return Quantize( std::exp( Quantize( std::log( std::fabs( player_2_line
                                                                    / std::cos( sight_2_line_ang ) )
                                                         + EPS ),
                                               qstep ) ), 0.1 );
      }


    void
    calcVel( const PVector & obj_vel,
             const PVector & obj_pos,
             const double & un_quant_dist,
             const double & quant_dist,
             double & dist_chg,
             double & dir_chg ) const;

    bool
    decide( const double & prob )
      {
          if ( prob >= 1.0 ) return true;
          if ( prob <= 0.0 ) return false;
          return boost::bernoulli_distribution<>( prob )( random::DefaultRNG::instance() );
      }

protected:
    virtual
    void
    serializePlayer( const Player& player,
                     const std::string & name,
                     const double & dist,
                     const int dir,
                     const double & dist_chg,
                     const double & dir_chg );

    virtual
    void
    serializePlayer( const Player& player,
                     const std::string & name,
                     const double & dist,
                     const int dir );

    virtual
    int rad2Deg( const double & rad ) const
      {
          return Rad2IDeg( rad );
      }

    virtual
    const std::string &
    calcName( const PObject& obj ) const;

    virtual
    const std::string &
    calcCloseName( const PObject& obj ) const;

    virtual
    const std::string &
    calcTFarName( const Player& obj ) const;

    virtual
    const std::string &
    calcUFarName( const Player& obj ) const;

private:
    void (VisualSenderPlayerV1::*M_send_flag)( const PObject& );
    void (VisualSenderPlayerV1::*M_send_ball)( const MPObject& );
    void (VisualSenderPlayerV1::*M_send_player)( const Player& );
    void (VisualSenderPlayerV1::*M_serialize_line)( const std::string &,
                                                    const int,
                                                    const double &,
                                                    const double & );
};

/*!
//===================================================================
//
//  CLASS: VisualSenderPlayerV4
//
//  DESC: Class for the version 4 visual protocol.  This version
//        introduced body directions of players. Everything else is
//        the same.
//
//===================================================================
*/


class VisualSenderPlayerV4
    : public VisualSenderPlayerV1
{
public:
    VisualSenderPlayerV4( const Params & params );

    virtual
    ~VisualSenderPlayerV4();

protected:
    virtual
    void
    serializePlayer( const Player & player,
                     const double & dist,
                     const int dir,
                     const double & dist_chg,
                     const double & dir_chg );


    int
    calcBodyDir( const Player & player ) const;
};

/*!
//===================================================================
//
//  CLASS: VisualSensorPlayerV5
//
//  DESC: Class for the version 5 visual protocol.  This version
//        introduced head directions of players. Everything else is
//        the same
//
//===================================================================
*/

class VisualSenderPlayerV5
    : public VisualSenderPlayerV4
{
public:
    VisualSenderPlayerV5( const Params & params );

    virtual
    ~VisualSenderPlayerV5();

protected:
    virtual
    void
    serializePlayer( const Player & player,
                     const double & dist,
                     const int dir,
                     const double & dist_chg,
                     const double & dir_chg );

    int
    calcHeadDir( const Player & player ) const;

};


/*!
//===================================================================
//
//  CLASS: VisualSensorPlayerV6
//
//  DESC: Class for the version 6 visual protocol.  This version
//        introduced shortened names for all the objects meaning the
//        name calculation must be redefined.
//
//===================================================================
*/

class VisualSenderPlayerV6
    : public VisualSenderPlayerV5
{
public:
    VisualSenderPlayerV6( const Params & params );

    virtual
    ~VisualSenderPlayerV6();

    virtual
    const std::string &
    calcName( const PObject & obj ) const;

    virtual
    const std::string &
    calcCloseName( const PObject & obj ) const;

    virtual
    const std::string &
    calcUFarName( const Player & obj ) const;

    virtual
    const std::string &
    calcTFarName( const Player & obj ) const;
};

/*!
//===================================================================
//
//  CLASS: VisualSensorPlayerV7
//
//  DESC: Class for the version 7 visual protocol.  This version
//        fixed a bug in the generation of directions in that they
//        were truncated to int, rather than rounded.  This meant
//        error in the direction pointed was at most times between
//        -0.5 and +0.5 degrees, but occationally between -1.0 and
//        +1.0 degrees and at other times exact.  With the new method
//        of rounding, the error is always between -0.5 and +0.5.
//
//===================================================================
*/

class VisualSenderPlayerV7
    : public VisualSenderPlayerV6
{
public:
    VisualSenderPlayerV7( const Params & params );

    virtual
    ~VisualSenderPlayerV7();

protected:
    virtual
    int
    rad2Deg( const double & rad ) const
      {
          return Rad2IDegRound( rad );
      }
};

/*!
//===================================================================
//
//  CLASS: VisualSensorPlayerV8
//
//  DESC: Class for the version 8 visual protocol.  This version
//        introduced observation of the a new arm feature, that
//        allows plays to point to different spots, however, on
//        direction the arm is pointing to is visable, not the
//        distance to the spot the arm is pointing to.
//
//===================================================================
*/

class VisualSenderPlayerV8
    : public VisualSenderPlayerV7
{
public:
    VisualSenderPlayerV8( const Params & params );

    virtual
    ~VisualSenderPlayerV8();

protected:
    int
    calcPointDir( const Player & player );

    virtual
    void
    serializePlayer( const Player & player,
                     const std::string & name,
                     const double & dist,
                     const int dir,
                     const double & dist_chg,
                     const double & dir_chg );

    virtual
    void
    serializePlayer( const Player& player,
                     const std::string & name,
                     const double & dist,
                     const int dir );
};
}
#endif
