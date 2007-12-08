/*
 * Copyright (c) 1999 - 2001, Artur Merke <amerke@ira.uka.de>
 *
 * This file is part of FrameView2d.
 *
 * FrameView2d is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * FrameView2d is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FrameView2d; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef _SMONITOR_DEV_H_
#define _SMONITOR_DEV_H_

#include "input_dev.h"
#include "rgbcolor.h"
#include "udpsocket.h"
#include "object2d.h"
#include "valueparser.h"

#include <vector>
#include <cstring>

#define MAX_PLAYER 11

std::ostream & operator<< (std::ostream & o, const RGBcolor & col);

class VisualPlayersViewArea: public VisualObject2d {
 public:
  static const int LOW;
  static const int HIGH;
  static const int FEEL_RANGE;
 private:
  VisualCircleArc2d  circlearc1;
  VisualCircle2d     circle;
  VisualCircleArc2d  circlearc2;
  RGBcolor c_exact;
  RGBcolor c_fuzzy;
  int view_quality;
  double view_width;
 public:
  VisualPlayersViewArea();
  void init(int my_key, int my_layer, const RGBcolor & my_exact, const RGBcolor & my_fuzzy);
  virtual ~VisualPlayersViewArea() { }

  void draw(DisplayBase * disp, const Area2d & area, const Frame2d & p_frame, bool chg);
  void actualize(const Frame2d& f, bool chg);
  void set_view_mode( int quality, double width );
};

class VisualBall: public VisualObject2d {
  RGBcolor c_ball;
  RGBcolor c_line_until_ball_stops;
  RGBcolor c_line_markers;
  RGBcolor c_font;

  VisualPoint2d  ball_point;;
  VisualCircle2d ball_circle;
  VisualCircle2d ball_margin;

  VisualLine2d  line_until_ball_stops;
  VisualLines2d line_markers;
  VisualString2d label;

  Frame2d vel_frame;
  bool vel_frame_chg;

  double ball_decay;
  bool show_vel;
  bool show_vel_string;
  bool changed;
  static const int MAX_LINE_MARKERS_NUM;
  static const int MAX_LABEL_LEN;
 public:
  VisualBall();

  void init(int my_key, int my_layer, double my_small_r, double my_large_r,
	    const RGBcolor & my_c_ball,
	    const RGBcolor & my_c_line_until_ball_stops,
	    const RGBcolor & my_c_line_unitl_ball_out_of_field,
	    const RGBcolor & my_c_font);

  virtual ~VisualBall() {}

  void draw(DisplayBase * disp, const Area2d & area, const Frame2d & p_frame, bool chg);
  void actualize(const Frame2d& f, bool chg);

  void unset_show_vel() { show_vel= false; }
  void set_show_vel(const Point2d & vel);

  void unset_show_vel_string() { show_vel_string= false; }
  void set_show_vel_string(const Point2d & vel);

  void set_label_pos(const Point2d & p) {
    label.rel= p;
    label.changed= true;
  }
  void set_ball_decay(double bdecay) {
    ball_decay= bdecay;
    //cout << "\nbdecay setting=" << bdecay << flush;
  }
};

class VisualPlayer: public VisualObject2d {
  RGBcolor c_invalid;
  RGBcolor c_player;
  RGBcolor c_goalie;
  RGBcolor c_font;
  RGBcolor c_black;
  RGBcolor c_white;
  RGBcolor c_red;
    RGBcolor c_blue;
    RGBcolor c_yellow;
    RGBcolor c_orange;
    RGBcolor c_pink;
    RGBcolor c_olive;

  VisualCircle2d body_bg;
  VisualCircle2d body;
  VisualCircle2d body_margin;
  VisualCircle2d body_margin_kick;
    VisualCircle2d body_margin_kick_fault;
    VisualCircle2d body_margin_tackle;
    VisualCircle2d body_margin_tackle_fault;
    VisualCircle2d body_margin_catch_fault;
  VisualCircle2d body_margin_ball_collision;

  VisualLine2d body_dir;
  VisualLine2d head_dir;
  VisualString2d label;

  Frame2d head_frame;
  bool head_frame_chg;

  bool use_number;
  bool show_kick;
    bool show_kick_fault;
    bool show_tackle;
    bool show_tackle_fault;
    bool show_catch_fault;
  bool ball_collision;
  bool is_active;
  bool is_goalie;
  bool changed;
  int type;
  static const int MAX_LABEL_LEN;

 public:
  VisualPlayer();

  void init(int my_key, int my_layer, char p_char,
	    const RGBcolor & my_c_invalid,
	    const RGBcolor & my_c_player,
	    const RGBcolor & my_c_goalie,
	    const RGBcolor & my_c_font);

  virtual ~VisualPlayer() {}

  void draw(DisplayBase * disp, const Area2d & area, const Frame2d & p_frame, bool chg);
  void actualize(const Frame2d& f, bool chg);

  void set_type(int type);
  void set_head_angle( const Angle & a);
  void set_active(bool flag= true);
  void set_label(const char * lab);   ///< copies the content of lab into his own label
  void set_use_number(bool flag= true) {
    use_number= flag;
    label.changed= true;
  }
  void set_goalie(bool flag= true);
  void set_kicking(bool flag= true) {
    show_kick= flag;
  }
    void set_kicking_fault( bool flag = true )
      {
          show_kick_fault = flag;
      }
    void set_tackling( bool flag = true )
      {
          show_tackle = flag;
      }
    void set_tackling_fault( bool flag = true )
      {
          show_tackle_fault = flag;
      }
    void set_catching_fault( bool flag = true )
      {
          show_catch_fault = flag;
      }
  void set_ball_collision( bool flag= true ) {
    ball_collision= flag;
  }
  void set_low_stamina_indicator(bool flag= true) { body.filled= flag; }

  void set_label_pos(const Point2d & p) {
    label.rel= p;
    label.changed= true;
  }
};

class VisualField: public VisualObject2d {

    static const double FIELD_LENGTH;
    static const double FIELD_WIDTH;
    //static const double FIELD_MARGIN;
    static const double FIELD_CENTER_CIRCLE_R;
    static const double FIELD_PENALTY_AREA_LENGTH;
    static const double FIELD_PENALTY_AREA_WIDTH;
    static const double FIELD_GOAL_AREA_LENGTH;
    static const double FIELD_GOAL_AREA_WIDTH;
    static const double FIELD_GOAL_WIDTH;
    static const double FIELD_GOAL_DEPTH;
    static const double FIELD_PENALTY_SPOT_DIST;
    static const double FIELD_CORNER_ARC_R;
    //static const double FIELD_CORNER_KICK_MARGIN;


   static const double HLEN;
   static const double HWID;

    static const double PEN_X;
    static const double PEN_Y;
    static const double GOAL_X;
    static const double GOAL_Y;

    static const double GPOST_RAD;



  RGBcolor c_line;
  RGBcolor c_goal;
  RGBcolor c_black;
  RGBcolor c_white;
  RGBcolor c_red;

  VisualPoints2d points;
  VisualLines2d  lines;
  VisualCircleArcs2d circlearcs;
  VisualPolygon2d goal_l;
  VisualPolygon2d goal_r;
  VisualPolygon2d vertical_stripe;
  VisualCircles2d goal_posts;

  bool keepaway;
  double keepaway_length;
  double keepaway_width;
  VisualPolygon2d keep_area;

  bool changed;
 public:
  VisualField();

  void init(int my_key, int my_layer,
	    const RGBcolor & my_c_line,
	    const RGBcolor & my_c_goal,
	    bool my_keepaway,
	    double my_keepaway_length,
	    double my_keepaway_width);

  virtual ~VisualField() {}

  void draw(DisplayBase * disp, const Area2d & area, const Frame2d & p_frame, bool chg);
  void actualize(const Frame2d& f, bool chg);
  void set_goal_width(double width);

  void use_vertical_stripe(bool flag= true) { vertical_stripe.visible= flag; }
  void set_vertical_stripe(double x1, double x2) {
    vertical_stripe.changed= true;
    vertical_stripe.rel.tab[0].x= x1;
    vertical_stripe.rel.tab[1].x= x2;
    vertical_stripe.rel.tab[2].x= x2;
    vertical_stripe.rel.tab[3].x= x1;
  }
};

class SMonitorDevice: public InputDevice {
  UDPsocket server;
  //UDPsocket coach;

    bool timeover;

    enum {
        BUFFER_MAX_SIZE= 8192,
        STRING_MAX_SIZE= 512,
    };
  char buffer1[BUFFER_MAX_SIZE];
  char buffer2[BUFFER_MAX_SIZE];

  static const int frame_canvas_left;
  static const int frame_canvas_right;
  static const int frame_ball;
  static const int frame_varea;
  static const int frame_shadow;

  static const int BUTTON_START;
  static const int BUTTON_RECONNECT;
  static const int BUTTON_SCALE_LEVEL;
  static const int BUTTON_INFO_LEVEL;
  static const int BUTTON_MODE;
  static const int BUTTON_UNZOOM;
  static const int BUTTON_QUIT;
  static const int BUTTON_NUMBER;

  static const int POPUP_BUTTON_DROP_BALL;
  static const int POPUP_BUTTON_FREE_KICK_LEFT;
  static const int POPUP_BUTTON_FREE_KICK_RIGHT;
  static const int POPUP_BUTTON_NUMBER;

  /****************************************************************************/
  /* begin of SMonitorDevice internal types declaration                       */
  /****************************************************************************/

  struct Options {
    Options();
    int server_port;
    char server_host[STRING_MAX_SIZE];
    int coach_port;
    int protocol_version;
    bool connect_on_start;

    bool keepaway;
    double keepaway_length;
    double keepaway_width;

    bool list_player_types;
    bool show_ball_collisions;
    bool just_edit;
    bool track_ball;
    double player_radius;
    int player_skin;
    Point2d player_num_pos;
    bool show_ball_vel;

    double ball_radius;
    int ball_skin;

    double kick_radius;

    void toggle_track_ball() { track_ball= !track_ball; }
    void toggle_show_ball_vel() { show_ball_vel= !show_ball_vel; }

    RGBcolor c_team_l, c_goalie_l, c_font_l, c_invalid_l;
    RGBcolor c_team_r, c_goalie_r, c_font_r, c_invalid_r;
    RGBcolor c_ball, c_field, c_marigin, c_line, c_goal;
    RGBcolor c_varea_exact, c_varea_fuzzy;

    int info_level;
    static const int info_level_min;
    static const int info_level_def;
    static const int info_level_max;

    int scale_level;
    static const int scale_level_min;
    static const int scale_level_def;
    static const int scale_level_max;
    static const double scale_level_inc;
    static const double scale_level_beg;
    double scale_factor() const;
    bool set_scale_level_from_double(double s);

    static const int MODE_STANDARD;
    static const int MODE_MOVE;
    static const int MODE_SHOW_VIEW_AREA;

    int mode;
    int active_in_mode;

    const char * get_mode_string() const;
    const char * get_mode_value_string() const;
    bool set_mode_from_string(const char * s);

    bool lt_show_spare_space_to_offside_line;
    bool rt_show_spare_space_to_offside_line;
  };

  struct ServerState {
    //long last_packet_ms_time;
    struct Charfield {
      char	name[512] ;
      int len;
      Charfield() { name[0]= '\0'; len = 0;}
      void set( const char* str ) { strncpy(name,str,16); len= strlen(name); }
      void reset() { name[0]= '\0'; }
    };
    ServerState();
    Charfield left_teamname;
    Charfield right_teamname;
    Charfield match_information;
    bool reconnected;
    void reset();
  };

  struct GuessState {
    GuessState() { reset(); }
    void reset() {
      use_stamina= false;
      use_type= false;
    }
    bool use_stamina;
    bool use_type;
  };

  struct Positions {
    struct Ball {
      Point2d pos;
      Point2d vel;
    };
    struct Player {
      Player() { alive= false; type= 0; view_quality= VisualPlayersViewArea::FEEL_RANGE, view_width= 0.0; };
      bool alive;
      int type;
      Point2d pos;
      Angle body_angle;
      Angle head_angle_rel;
      double view_width;
      int view_quality;
      double stamina;
    };

    Positions() {};
    Ball ball;
    Player player[MAX_PLAYER*2];

    Player & ref_player(int i) { return player[i]; };
    void set_player_alive(int i,bool flag=true) { player[i].alive= flag; }
    bool player_alive(int i) { return player[i].alive; }
    void set_player(int i,const Point2d & p,const Angle & a);
    void set_player_pos(int i,const Point2d & p) { player[i].pos= p; }
    void set_ball_pos(const Point2d & p) { ball.pos= p; }
    void set_ball(const Point2d & p, const Point2d & v) { ball.pos= p; ball.vel= v; }
    void print_inline(std::ostream & o);
    void print_formation(std::ostream & o, bool left_half, bool left_team);

    //some info messages (useful for example to recognize offside lines (considers players p1 <= p < p2)
    bool max_x_pos(int p1, int p2, double & x1) const;
    bool max_x_pos(int p1, int p2, double & x1, double & x2) const;
    bool min_x_pos(int p1, int p2, double & x1) const;
    bool min_x_pos(int p1, int p2, double & x1, double & x2) const;
    //lt= left_team, rt= right_team
    bool lt_max_x_pos(double & x1) const { return max_x_pos(0,MAX_PLAYER,x1); }
    bool lt_min_x_pos(double & x1) const { return min_x_pos(0,MAX_PLAYER,x1); }
    bool lt_max_x_pos(double & x1, double & x2) const { return max_x_pos(0,MAX_PLAYER,x1,x2); }
    bool lt_min_x_pos(double & x1, double & x2) const { return min_x_pos(0,MAX_PLAYER,x1,x2); }
    bool rt_max_x_pos(double & x1) const { return max_x_pos(MAX_PLAYER,MAX_PLAYER*2,x1); }
    bool rt_min_x_pos(double & x1) const { return min_x_pos(MAX_PLAYER,MAX_PLAYER*2,x1); }
    bool rt_max_x_pos(double & x1, double & x2) const { return max_x_pos(MAX_PLAYER,MAX_PLAYER*2,x1,x2); }
    bool rt_min_x_pos(double & x1, double & x2) const { return min_x_pos(MAX_PLAYER,MAX_PLAYER*2,x1,x2); }
  };

  struct CoachState {
    CoachState() { left_team= true; }
    bool left_team;
    void toggle_team() { left_team= !left_team; }
  };

    struct PenaltyState {
        int time_;
        int left_score_;
        int left_trial_;
        int right_score_;
        int right_trial_;
        PenaltyState()
            : time_( 0 )
            , left_score_( 0 )
            , left_trial_( 0 )
            , right_score_( 0 )
            , right_trial_( 0 )
          { }
    };
  /****************************************************************************/
  /* end of SMonitorDevice internal types declaration                         */
  /****************************************************************************/

  Options options;
  ServerState server_state;
  GuessState guess;
  Positions server_pos;
  CoachState coach_state;

  VisualBall vis_ball;
  VisualPlayer vis_player[MAX_PLAYER*2];
  VisualPlayersViewArea vis_view_area;
  VisualField vis_field;
  Area2d initial_area;

    std::vector< PenaltyState > M_penalty_state;

  void vis_ball_set_info_level(int lev);
  void vis_player_set_info_level(int lev, VisualPlayer & vis_p, const Positions::Player & p);
  void vis_player_set_info_level(int lev);

  int server_msg_type(void * ptr);
  bool server_interpret_showinfo_t(BuilderBase * build, void *ptr);
  bool server_interpret_msginfo_t(BuilderBase * build, void *ptr);
  bool server_interpret_drawinfo_t(BuilderBase * build, void *ptr);

  bool server_interpret_showinfo_t2(BuilderBase * build, void *ptr); //ver. 2
  bool server_interpret_player_type_t(BuilderBase * build, void *ptr); //ver. 2
  bool server_interpret_server_params_t(BuilderBase * build, void *ptr); //ver. 2

    void updatePenaltyState( const int current_time,
                             const char pmode );

  void show_parser_error_point(std::ostream & out, const char * origin, const char * parse_error_point);
  bool ins_simple_obj(const char * buf, int fref, BuilderBase * build, const char* & next);
  bool server_interpret_frameview_msg(BuilderBase * build, const char * msg, bool enforce_frame= false, int frame= -1); //art!

  bool server_interpret_command_msg(BuilderBase * build, const char * msg);

  /// the object ID of the ball
  static const int id_ball;
  static const int id_invalid;

  /// true if object ID means the ball or a valid player
  bool o_valid(int i) { return ( o_ball(i) || p_valid(i))?true:false; }
  /// true if object ID means the ball
  bool o_ball(int i) { return (i == id_ball)?true:false; }
  /// true if object ID means a player ball
  bool o_player(int i) { return p_valid(i); }
  /// true if the player ID is valid
  bool p_valid(int i) { return (i>= 0 && i < MAX_PLAYER*2)?true:false; }
  /// maps player ID to frame number of this player
  int p_frame(int i) { return i+1; }

  /// maps player ID to number of this player in his team
  int p_number(int i) { return i<MAX_PLAYER?i+1:i+1-MAX_PLAYER; }
  /// maps player ID to team ID
  int p_team(int i) { return i<MAX_PLAYER?0:1; }
  /// return true if player ID belongs to the left team
  bool p_left(int i) { return i<MAX_PLAYER?true:false; }
  /// return true if player ID belongs to the left team
  bool p_right(int i) { return i<MAX_PLAYER?false:true; }
  /// maps player ID to character in '1' ... '9' , 'A', 'B'
  char p_char(int i);

  ///sets player position,
  void set_object_pos(BuilderBase * build, int p_num, const Point2d & pos);

  void set_all_objects_scale(BuilderBase * build, double scale);

  void send_object_pos(int p_num, const Point2d & pos);
  void send_dispinit();
  void send_dispstart();
  void send_dispball(const Point2d & pos);
  void send_dispfoul_left(const Point2d & pos);
  void send_dispfoul_right(const Point2d & pos);
  void send_dispbye();

  bool process_options(const ValueParser & vp);

  template <class T>
  void print_option_entry(std::ostream & o,int mode, const char * option, const T & val, const char * description) const;
  void generic_description_of_options(std::ostream & o, int mode) const;
 public:
  /****************************************************************************/
  /* public interface of SMonitorDevice as defined in InputDevice             */
  /****************************************************************************/
  SMonitorDevice();
  virtual ~SMonitorDevice() { }

  bool set_initial_area(const Area2d & area) { initial_area= area; return true; }
  bool process_options(const char * fname);
  bool process_options(int argc, char const* const* argv);
  void help_options(std::ostream & o) const;
  void help_char_command(std::ostream & o) const;
  void generate_file_options(std::ostream & o) const;

  bool process_char_command(BuilderBase * build, MenuBase * menu, const InputEvent & event);
  bool process_mouse_button_event(BuilderBase * build, const InputEvent & event);
  bool process_menu_button(BuilderBase * build, MenuBase * menu, const InputEvent & event);
  bool process_popup_button(BuilderBase * build, MenuBase * popup, const InputEvent & event);

  bool init_menu(MenuBase * menu);
  bool init_popup(MenuBase * popup);
  bool init_frames(BuilderBase * build);
  bool init_connection();

  bool uses_mouse() const;
  bool uses_popup() const;

  bool destruct();

  bool process_input(fd_set * , BuilderBase * build);

  const char * status_line() { return server_state.match_information.name; }
  int set_fds(fd_set * set);
  bool got_fds(fd_set * set);

    virtual bool is_timeover() const { return timeover; }
};

template <class T>
void SMonitorDevice::print_option_entry(std::ostream & o,int mode, const char * option, const T & val, const char * description) const {
  if (mode == 0)
    o << "\n"
      << "\n# " << description << " (default " << val << ")"
      << "\n" << option << " = "; //<< val;
  else
    o << "\n"
      << "\n" << description
      << "\n-m_" << option << " [" << val << "]";
}



#endif
