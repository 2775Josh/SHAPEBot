#include <Servo.h>
#include <IRremote.h>  

//For Knees: 90 degrees is parallel to the upper leg, 90 is perpendicular
//For Hips: 0 degrees is parallel to the body, 90 is straight down
//For Abs: 0 degrees is flat, and up is positive
//All length units are mm, and offsets are in degrees scaled down by a factor of 1.5


//Right Front
Servo RFAb;
int RFAb_pin = 2;
double RFAb_offset = 101;
double RFAb_scaling = -1.5;

Servo RFHip;
int RFHip_pin = 4;
double RFHip_offset = 52;
double RFHip_scaling = 1.5;

Servo RFKnee;
int RFKnee_pin = 3;
double RFKnee_offset = 112;
double RFKnee_scaling = -1.5;

//Right Back
Servo RBAb;
int RBAb_pin = 11;
double RBAb_offset = 154;
double RBAb_scaling = 1.5;

Servo RBHip;
int RBHip_pin = 13;
double RBHip_offset = 90;
double RBHip_scaling = 1.5;

Servo RBKnee;
int RBKnee_pin = 12;
double RBKnee_offset = 155;
double RBKnee_scaling = -1.5;

//Left Front
Servo LFAb;
int LFAb_pin = 5;
double LFAb_offset = 115;
double LFAb_scaling = 1.5;

Servo LFHip;
int LFHip_pin = 7;
double LFHip_offset = 104;
double LFHip_scaling = -1.5;

Servo LFKnee;
int LFKnee_pin = 6;
double LFKnee_offset = 76;
double LFKnee_scaling = 1.5;

//Left Back
Servo LBAb;
int LBAb_pin = 8;
double LBAb_offset = 94;
double LBAb_scaling = -1.5;

Servo LBHip;
int LBHip_pin = 9;
double LBHip_offset = 150;
double LBHip_scaling = -1.5;

Servo LBKnee;
int LBKnee_pin = 10;
double LBKnee_offset = 51;
double LBKnee_scaling = 1.5;

int IR_pin = A1;

//Apply to all legs
double upper_leg_length = 137.5;
double lower_leg_length = 142;
double ab_width = 60;

//Program-wide trackers
double pi = 3.1416;
int phase = 0;
//FOR PHASES

long starttime = 0;
long endtime = 0;
int command = 0; 
//FOR COMMANDS
//0 - stop
//1 - walk forward, 2 - walk backward, 3 - turn left, 4 - turn right
//5 - strafe left, 6 - strafe right, 7 - xy circle, 8 - xz circle
//9 - yz circle, 10 - ROM test, 11 - drag pickup, 12 - shake
//13 - lift hind leg, 14 - jump, 15 - headstand

//Standing-still constants
int detach_time = 100;
double standing_x = -30;
double standing_y = 160;

double trot_glide_time = 125;
double trot_air_time = 100;
double trot_dist = 60;
double trot_front = standing_x + trot_dist;
double trot_back = standing_x - trot_dist;

double turn_glide_time = 100;
double turn_air_time = 75;
double turn_dist = 15;
double turn_front = ab_width + turn_dist;
double turn_back = ab_width - turn_dist;

double strafe_glide_time = 175;
double strafe_air_time = 75;
double strafe_dist = 10;
double strafe_front = ab_width + strafe_dist;
double strafe_back = ab_width - strafe_dist;
double strafe_x = -5;

double circle_time = 1500;

//IR Button mappings
int fwd_button = 70;
int backward_button = 21;
int left_turn_button = 68;
int right_turn_button = 67;
int left_strafe_button = 7;
int right_strafe_button = 9;
int xy_circle_button = 12;
int xz_circle_button = 24;
int yz_circle_button = 94;
int tall_button = 8;
int drag_stand_button = 28;
int shake_button = 90;
int leg_lift_button = 66;
int jump_button = 82;
int handstand_button = 74;
int stop_button = 64;
int power_button = 69;


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  IrReceiver.begin(IR_pin);
  delay(250);
  while(true){
    starttime = millis();
    if (IrReceiver.decode()) {
      IrReceiver.resume();
      int cmd = IrReceiver.decodedIRData.command;
      if(cmd == power_button){
        break;
      }
    }
  }
  attachall();
}

void loop() {
  long curtime = millis();

  if (IrReceiver.decode()) {
    IrReceiver.resume();
    int cmd = IrReceiver.decodedIRData.command;
    Serial.println(cmd);
    if(cmd == fwd_button){
      command = 1;
    }else if (cmd == backward_button){
      command = 2;
    }else if(cmd == left_turn_button){
      command = 3;
    }else if (cmd == right_turn_button){
      command = 4;
    }else if(cmd == left_strafe_button){
      command = 5;
    }else if (cmd == right_strafe_button){
      command = 6;
    }else if (cmd == xy_circle_button){
      command = 7;
    }else if (cmd == xz_circle_button){
      command = 8;
    }else if (cmd == yz_circle_button){
      command = 9;
    }else if (cmd == tall_button){
      command = 10;
    }else if (cmd == drag_stand_button){
      command = 11;
    }else if (cmd == shake_button){
      command = 12;
    }else if (cmd == leg_lift_button){
      command = 13;
    }else if (cmd == jump_button){
      command = 14;
    }else if (cmd == handstand_button){
      command = 15;
    }else if (cmd == stop_button || cmd == power_button){
      command = 0;
    }
  }

  if(phase == 0){
    RF_gotopos(standing_x, standing_y, ab_width);
    RB_gotopos(standing_x, standing_y, ab_width);
    LF_gotopos(standing_x, standing_y, ab_width);
    LB_gotopos(standing_x, standing_y, ab_width);
    if(command == 0 && curtime - starttime > detach_time){
      detachall();
    }else{
      attachall();
    }
    if(command == 1){
      phase = 1;
      starttime = curtime;
      endtime = curtime + 2 * trot_glide_time + 2 * trot_air_time;
    }else if(command == 2){
      phase = 2;
      starttime = curtime;
      endtime = curtime + 2 * trot_glide_time + 2 * trot_air_time;
    }else if(command == 3){
      phase = 3;
      starttime = curtime;
      endtime = curtime + 2 * turn_air_time + 2 * turn_glide_time;
    }else if(command == 4){
      phase = 4;
      starttime = curtime;
      endtime = curtime + 2 * turn_air_time + 2 * turn_glide_time;
    }else if(command == 5){
      phase = 5;
      starttime = curtime;
      endtime = curtime + 2 * strafe_air_time + 2 * strafe_glide_time;
    }else if(command == 6){
      phase = 6;
      starttime = curtime;
      endtime = curtime + 2 * strafe_air_time + 2 * strafe_glide_time;
    }else if(command == 7){
      phase = 7;
      starttime = curtime;
      endtime = curtime + circle_time;
    }else if(command == 8){
      phase = 8;
      starttime = curtime;
      endtime = curtime + circle_time;
    }else if(command == 9){
      phase = 9;
      starttime = curtime;
      endtime = curtime + circle_time;
    }else if(command == 10){
      phase = 10;
      starttime = curtime;
      endtime = curtime;
    }else if(command == 11){
      phase = 11;
      starttime = curtime;
      endtime = curtime;
    }else if(command == 12){
      phase = 12;
      starttime = curtime;
      endtime = curtime;
    }else if(command == 13){
      phase = 13;
      starttime = curtime;
      endtime = curtime;
    }else if(command == 14){
      phase = 14;
      starttime = curtime;
      endtime = curtime;
    }else if(command == 15){
      phase = 15;
      starttime = curtime;
      endtime = curtime;
    }
  }

  if(phase == 1){
    double z = ab_width;
    double x;
    double y;
    if(curtime < endtime - trot_air_time){
      x = smoothmove(starttime, endtime - trot_air_time, trot_front, trot_back);
      y = standing_y;
    } else {
      x = tocirclex(smoothmove(endtime - trot_air_time, endtime, 180, 0), standing_x, trot_dist);
      y = tocircley(smoothmove(endtime - trot_air_time, endtime, 180, 0), standing_x, trot_dist);
    }
    RF_gotopos(x, y, z);
    LB_gotopos(x, y, z);
    if(curtime < starttime + trot_glide_time){
      x = smoothmove(starttime - trot_air_time - trot_glide_time, starttime + trot_glide_time, trot_front, trot_back);
      y = standing_y;
    } else if (curtime - starttime > trot_glide_time && curtime - starttime < trot_glide_time + trot_air_time){
      x = tocirclex(smoothmove(starttime + trot_glide_time, starttime + trot_glide_time + trot_air_time, 180, 0), standing_x, trot_dist);
      y = tocircley(smoothmove(starttime + trot_glide_time, starttime + trot_glide_time + trot_air_time, 180, 0), standing_x, trot_dist);
    } else {
      x = smoothmove(starttime + trot_air_time + trot_glide_time, starttime + 2 * trot_air_time + 3* trot_glide_time, trot_front, trot_back);
      y = standing_y;
    }
    LF_gotopos(x, y, z);
    RB_gotopos(x, y, z);
    if(curtime >= endtime){
      starttime = curtime;
      endtime = curtime + 2 * trot_air_time + 2 * trot_glide_time;
      if(command != 1){
        phase = 0;
      }
    }
  }

  if(phase == 2){
    double z = ab_width;
    double x;
    double y;
    if(curtime < endtime - trot_air_time){
      x = smoothmove(starttime, endtime - trot_air_time, trot_back, trot_front);
      y = standing_y;
    } else {
      x = tocirclex(smoothmove(endtime - trot_air_time, endtime, 0, 180), standing_x, trot_dist);
      y = tocircley(smoothmove(endtime - trot_air_time, endtime, 0, 180), standing_x, trot_dist);
    }
    RF_gotopos(x, y, z);
    LB_gotopos(x, y, z);
    if(curtime < starttime + trot_glide_time){
      x = smoothmove(starttime - trot_air_time - trot_glide_time, starttime + trot_glide_time, trot_back, trot_front);
      y = standing_y;
    } else if (curtime - starttime > trot_glide_time && curtime - starttime < trot_glide_time + trot_air_time){
      x = tocirclex(smoothmove(starttime + trot_glide_time, starttime + trot_glide_time + trot_air_time, 0, 180), standing_x, trot_dist);
      y = tocircley(smoothmove(starttime + trot_glide_time, starttime + trot_glide_time + trot_air_time, 0, 180), standing_x, trot_dist);
    } else {
      x = smoothmove(starttime + trot_air_time + trot_glide_time, starttime + 2 * trot_air_time + 3* trot_glide_time, trot_back, trot_front);
      y = standing_y;
    }
    LF_gotopos(x, y, z);
    RB_gotopos(x, y, z);
    if(curtime >= endtime){
      starttime = curtime;
      endtime = curtime + 2 * trot_air_time + 2 * trot_glide_time;
      if(command != 2){
        phase = 0;
      }
    }
  }

  if(phase == 3){
    double z;
    double x = standing_x;
    double y;
    if(curtime < endtime - turn_air_time){
      z = smoothmove(starttime, endtime - turn_air_time, turn_back, turn_front);
      y = standing_y;
    } else {
      z = tocirclex(smoothmove(endtime - turn_air_time, endtime, 0, 180), ab_width, turn_dist);
      y = tocircley(smoothmove(endtime - turn_air_time, endtime, 0, 180), ab_width, turn_dist);
    }
    RF_gotopos(x, y, z);
    LB_gotopos(x, y, z);
    if(curtime < starttime + turn_glide_time){
      z = smoothmove(starttime - turn_air_time - turn_glide_time, starttime + turn_glide_time, turn_front, turn_back);
      y = standing_y;
    } else if (curtime - starttime > turn_glide_time && curtime - starttime < turn_glide_time + turn_air_time){
      z = tocirclex(smoothmove(starttime + turn_glide_time, starttime + turn_glide_time + turn_air_time, 180, 0), ab_width, turn_dist);
      y = tocircley(smoothmove(starttime + turn_glide_time, starttime + turn_glide_time + turn_air_time, 180, 0), ab_width, turn_dist);
    } else {
      z = smoothmove(starttime + turn_air_time + turn_glide_time, starttime + 2 * turn_air_time + 3* turn_glide_time, turn_front, turn_back);
      y = standing_y;
    }
    LF_gotopos(x, y, z);
    RB_gotopos(x, y, z);
    if(curtime >= endtime){
      starttime = curtime;
      endtime = curtime + 2 * turn_air_time + 2 * turn_glide_time;
      if(command != 3){
        phase = 0;
      }
    }
  }

  if(phase == 4){
    double z;
    double x = standing_x;
    double y;
    if(curtime < endtime - turn_air_time){
      z = smoothmove(starttime, endtime - turn_air_time, turn_front, turn_back);
      y = standing_y;
    } else {
      z = tocirclex(smoothmove(endtime - turn_air_time, endtime, 180, 0), ab_width, turn_dist);
      y = tocircley(smoothmove(endtime - turn_air_time, endtime, 180, 0), ab_width, turn_dist);
    }
    RF_gotopos(x, y, z);
    LB_gotopos(x, y, z);
    if(curtime < starttime + turn_glide_time){
      z = smoothmove(starttime - turn_air_time - turn_glide_time, starttime + turn_glide_time, turn_back, turn_front);
      y = standing_y;
    } else if (curtime - starttime > turn_glide_time && curtime - starttime < turn_glide_time + turn_air_time){
      z = tocirclex(smoothmove(starttime + turn_glide_time, starttime + turn_glide_time + turn_air_time, 0, 180), ab_width, turn_dist);
      y = tocircley(smoothmove(starttime + turn_glide_time, starttime + turn_glide_time + turn_air_time, 0, 180), ab_width, turn_dist);
    } else {
      z = smoothmove(starttime + turn_air_time + turn_glide_time, starttime + 2 * turn_air_time + 3* turn_glide_time, turn_back, turn_front);
      y = standing_y;
    }
    LF_gotopos(x, y, z);
    RB_gotopos(x, y, z);
    if(curtime >= endtime){
      starttime = curtime;
      endtime = curtime + 2 * turn_air_time + 2 * turn_glide_time;
      if(command != 4){
        phase = 0;
      }
    }
  }

  if(phase == 5){
    double leftz;
    double rightz;
    double x = strafe_x;
    double y;
    if(curtime < endtime - strafe_air_time){
      rightz = smoothmove(starttime, endtime - strafe_air_time, strafe_back, strafe_front);
      leftz = smoothmove(starttime, endtime - strafe_air_time, strafe_front, strafe_back);
      y = standing_y;
    } else {
      rightz = tocirclex(smoothmove(endtime - strafe_air_time, endtime, 0, 180), ab_width, strafe_dist);
      leftz = tocirclex(smoothmove(endtime - strafe_air_time, endtime, 180, 0), ab_width, strafe_dist);
      y = tocircley(smoothmove(endtime - strafe_air_time, endtime, 180, 0), ab_width, strafe_dist);
    }
    RF_gotopos(x, y, rightz);
    LB_gotopos(x, y, leftz);
    if(curtime < starttime + strafe_glide_time){
      rightz = smoothmove(starttime - strafe_air_time - strafe_glide_time, starttime + strafe_glide_time, strafe_back, strafe_front);
      leftz = smoothmove(starttime - strafe_air_time - strafe_glide_time, starttime + strafe_glide_time, strafe_front, strafe_back);
      y = standing_y;
    } else if (curtime - starttime > strafe_glide_time && curtime - starttime < strafe_glide_time + strafe_air_time){
      rightz = tocirclex(smoothmove(starttime + strafe_glide_time, starttime + strafe_glide_time + strafe_air_time, 0, 180), ab_width, strafe_dist);
      leftz = tocirclex(smoothmove(starttime + strafe_glide_time, starttime + strafe_glide_time + strafe_air_time, 180, 0), ab_width, strafe_dist);
      y = tocircley(smoothmove(starttime + strafe_glide_time, starttime + strafe_glide_time + strafe_air_time, 0, 180), ab_width, strafe_dist);
    } else {
      rightz = smoothmove(starttime + strafe_air_time + strafe_glide_time, starttime + 2 * strafe_air_time + 3* strafe_glide_time, strafe_back, strafe_front);
      leftz = smoothmove(starttime + strafe_air_time + strafe_glide_time, starttime + 2 * strafe_air_time + 3* strafe_glide_time, strafe_front, strafe_back);
      y = standing_y;
    }
    LF_gotopos(x, y, leftz);
    RB_gotopos(x, y, rightz);
    if(curtime >= endtime){
      starttime = curtime;
      endtime = curtime + 2 * strafe_air_time + 2 * strafe_glide_time;
      if(command != 5){
        phase = 0;
      }
    }
  }

  if(phase == 6){
    double leftz;
    double rightz;
    double x = strafe_x;
    double y;
    if(curtime < endtime - strafe_air_time){
      leftz = smoothmove(starttime, endtime - strafe_air_time, strafe_back, strafe_front);
      rightz = smoothmove(starttime, endtime - strafe_air_time, strafe_front, strafe_back);
      y = standing_y;
    } else {
      leftz = tocirclex(smoothmove(endtime - strafe_air_time, endtime, 0, 180), ab_width, strafe_dist);
      rightz = tocirclex(smoothmove(endtime - strafe_air_time, endtime, 180, 0), ab_width, strafe_dist);
      y = tocircley(smoothmove(endtime - strafe_air_time, endtime, 180, 0), ab_width, strafe_dist);
    }
    RF_gotopos(x, y, rightz);
    LB_gotopos(x, y, leftz);
    if(curtime < starttime + strafe_glide_time){
      leftz = smoothmove(starttime - strafe_air_time - strafe_glide_time, starttime + strafe_glide_time, strafe_back, strafe_front);
      rightz = smoothmove(starttime - strafe_air_time - strafe_glide_time, starttime + strafe_glide_time, strafe_front, strafe_back);
      y = standing_y;
    } else if (curtime - starttime > strafe_glide_time && curtime - starttime < strafe_glide_time + strafe_air_time){
      leftz = tocirclex(smoothmove(starttime + strafe_glide_time, starttime + strafe_glide_time + strafe_air_time, 0, 180), ab_width, strafe_dist);
      rightz = tocirclex(smoothmove(starttime + strafe_glide_time, starttime + strafe_glide_time + strafe_air_time, 180, 0), ab_width, strafe_dist);
      y = tocircley(smoothmove(starttime + strafe_glide_time, starttime + strafe_glide_time + strafe_air_time, 0, 180), ab_width, strafe_dist);
    } else {
      leftz = smoothmove(starttime + strafe_air_time + strafe_glide_time, starttime + 2 * strafe_air_time + 3* strafe_glide_time, strafe_back, strafe_front);
      rightz = smoothmove(starttime + strafe_air_time + strafe_glide_time, starttime + 2 * strafe_air_time + 3* strafe_glide_time, strafe_front, strafe_back);
      y = standing_y;
    }
    LF_gotopos(x, y, leftz);
    RB_gotopos(x, y, rightz);
    if(curtime >= endtime){
      starttime = curtime;
      endtime = curtime + 2 * strafe_air_time + 2 * strafe_glide_time;
      if(command != 6){
        phase = 0;
      }
    }
  }

  if(phase == 7){
    double x = tocirclex(smoothmove(starttime, endtime, 0, 360), standing_x, 20);
    double y = tocircley(smoothmove(starttime, endtime, 0, 360), standing_y, 30);
    double z = ab_width;
    
    RF_gotopos(x, y, z);
    LF_gotopos(x, y, z);
    RB_gotopos(x, y, z);
    LB_gotopos(x, y, z);
    if(curtime >= endtime){
      starttime = curtime;
      endtime = curtime + circle_time;
      if(command != 7){
        phase = 0;
      }
    }
  }

  if(phase == 8){
    double x = tocirclex(smoothmove(starttime, endtime, 0, 360), standing_x, 30);
    double y = standing_y;
    double rightz = tocircley(smoothmove(starttime, endtime, 0, 360), ab_width, 20);
    double leftz = tocircley(smoothmove(starttime, endtime, -180, 180), ab_width, 20);
    
    RF_gotopos(x, y, rightz);
    LF_gotopos(x, y, leftz);
    RB_gotopos(x, y, rightz);
    LB_gotopos(x, y, leftz);
    if(curtime >= endtime){
      starttime = curtime;
      endtime = curtime + circle_time;
      if(command != 8){
        phase = 0;
      }
    }
  }

  if(phase == 9){
    double x = standing_x;
    double y = tocircley(smoothmove(starttime, endtime, 0, 360), standing_y, 30);
    double leftz = tocirclex(smoothmove(starttime, endtime, 0, 360), ab_width, 20);
    double rightz = tocirclex(smoothmove(starttime, endtime, -180, 180), ab_width, 20);
    
    RF_gotopos(x, y, rightz);
    LF_gotopos(x, y, leftz);
    RB_gotopos(x, y, rightz);
    LB_gotopos(x, y, leftz);
    if(curtime >= endtime){
      starttime = curtime;
      endtime = curtime + circle_time;
      if(command != 9){
        phase = 0;
      }
    }
  }

  if(phase == 10){
    double x = standing_x;
    double y;
    double z = ab_width;
    double move_time = 225;
    double hold_time = move_time + 1000;
    double final_time = hold_time + 225;
    if(curtime - starttime < move_time){
      y = smoothmove(starttime, starttime + move_time, standing_y, 260);
    }else if (curtime - starttime < hold_time){
    }else if (curtime - starttime < final_time){
      y = smoothmove(starttime + hold_time, starttime + final_time, 260, standing_y);
    }else{
      phase = 0;
      command = 0;
    }
    RF_gotopos(x, y, z);
    LF_gotopos(x, y, z);
    RB_gotopos(x, y, z);
    LB_gotopos(x, y, z);
  }

  if(phase == 11){
    double x;
    double y;
    double z;
    double move_time = 500;
    double splay_time = move_time + 500;
    double final_time = splay_time + 1000;
    if(curtime - starttime < move_time){
      x = smoothmove(starttime, starttime + move_time, standing_x, -200);
      y = smoothmove(starttime, starttime + move_time, standing_y, -10);
      z = ab_width;
    }else if (curtime - starttime < splay_time){
      y = -10;
      x = -100;
      z = 250;
    }else if(curtime - starttime < final_time){
      x = smoothmove(starttime + splay_time, starttime + final_time, -100, standing_x);;
      y = smoothmove(starttime + splay_time, starttime + final_time, 0, standing_y);
      z = smoothmove(starttime + splay_time, starttime + final_time, 200, ab_width);
    }else{
      phase = 0;
      command = 0;
    }
    RF_gotopos(x, y, z);
    LF_gotopos(x, y, z);
    RB_gotopos(x, y, z);
    LB_gotopos(x, y, z);
  }

  if(phase == 12){
    double x;
    double y;
    double z;
    double lean_time = 300;
    double shake_time = lean_time + 2000;
    double final_time = shake_time + 300;
    if(curtime - starttime < lean_time){
      x = standing_x;
      y = smoothmove(starttime, starttime + lean_time, standing_y, 240);
      z = smoothmove(starttime, starttime + lean_time, ab_width, -30);
      LF_gotopos(x, y, z);
    }else if (curtime - starttime < shake_time){
      RF_gotopos(200, 0, ab_width);
    }else if(curtime - starttime < final_time){
      x = standing_x;
      y = smoothmove(starttime + shake_time, starttime + final_time, 240, standing_y);
      z = smoothmove(starttime + shake_time, starttime + final_time, -30, ab_width);
      LF_gotopos(x, y, z);
      RF_gotopos(x, standing_y, ab_width);
    }else{
      phase = 0;
      command = 0;
    }
  }

  if(phase == 13){
    double x;
    double y;
    double z;
    double lean_time = 300;
    double shake_time = lean_time + 2000;
    double final_time = shake_time + 300;
    if(curtime - starttime < lean_time){
      x = standing_x;
      y = smoothmove(starttime, starttime + lean_time, standing_y, 240);
      z = smoothmove(starttime, starttime + lean_time, ab_width, -30);
      RB_gotopos(x, y, z);
    }else if (curtime - starttime < shake_time){
      LB_gotopos(-100, 0, 160);
    }else if(curtime - starttime < final_time){
      x = standing_x;
      y = smoothmove(starttime + shake_time, starttime + final_time, 240, standing_y);
      z = smoothmove(starttime + shake_time, starttime + final_time, -30, ab_width);
      RB_gotopos(x, y, z);
      LB_gotopos(x, standing_y, ab_width);
    }else{
      phase = 0;
      command = 0;
    }
  }

  if(phase == 14){
    double x;
    double y;
    double z;
    double lean_time = 450;
    double jump_time = lean_time + 250;
    double retract_time = jump_time + 500;
    double final_time = retract_time + 250;
    if(curtime - starttime < lean_time){
      x = standing_x;
      y = smoothmove(starttime, starttime + lean_time, standing_y, 30);
      z = ab_width;
    }else if (curtime - starttime < jump_time){
      x = smoothmove(starttime, starttime + jump_time, -30, -70);
      y = smoothmove(starttime, starttime + jump_time, 50, 170);
      z = ab_width;
    }else if(curtime - starttime < retract_time){
      x = standing_x;
      y = 50;
      z = ab_width;
    }else if(curtime - starttime < final_time){
      x = standing_x;
      y = standing_y;
      z = ab_width;
    }else{
      phase = 0;
      command = 0;
    }

    RF_gotopos(x, y, z);
    LF_gotopos(x, y, z);
    RB_gotopos(x, y, z);
    LB_gotopos(x, y, z);
  }

  if(phase == 15){
    double x;
    double backx;
    double y;
    double backy;
    double z = ab_width;
    double move_time = 500;
    double push_time = move_time + 500;
    double stand_time = push_time + 1200;
    double hold_time = stand_time + 2000;
    double fall_time = hold_time + 800;
    double pull_time = fall_time + 600;
    double final_time = pull_time + 500;
    if(curtime - starttime < move_time){
      x = smoothmove(starttime, starttime + move_time, standing_x, -200);
      backx = x;
      y = smoothmove(starttime, starttime + move_time, standing_y, 0);
      backy = y;
    }else if (curtime - starttime < push_time){
      y = smoothmove(starttime + move_time, starttime + push_time, 0, 200);
      x = -200;
      backx = x;
      backy = y;
    }else if (curtime - starttime < stand_time){
      x = smoothmove(starttime + move_time, starttime + stand_time, -200, -10);
      backx = smoothmove(starttime + move_time, starttime + stand_time, -200, -210);
      y = 200;
      backy = smoothmove(starttime + move_time, starttime + stand_time, 200, 10);
    }else if (curtime - starttime < hold_time){
      x = -10;
      backx = -210;
      y = 200;
      backy = 10;
    }else if(curtime - starttime < fall_time){
      x = smoothmove(starttime + hold_time, starttime + fall_time, -10, -200);
      backx = smoothmove(starttime + hold_time, starttime + fall_time, -210, -200);
      y = 200;
      backy = smoothmove(starttime + hold_time, starttime + fall_time, 10, 200);
    }else if(curtime - starttime < pull_time){
      y = smoothmove(starttime + fall_time, starttime + pull_time, 200, 0);
      backy = y;
      x = -200;
      backx = x;
    }else if(curtime - starttime < final_time){
      x = smoothmove(starttime + pull_time, starttime + final_time, -200, standing_x);
      y = smoothmove(starttime + pull_time, starttime + final_time, 0, standing_y);
      backx = x;
      backy = y;
    }else{
      phase = 0;
      command = 0;
    }
    RF_gotopos(x, y, z);
    LF_gotopos(x, y, z);
    RB_gotopos(backx, backy, z);
    LB_gotopos(backx, backy, z);
  }

}

double degreetomotor(double degree, double offset, double scaling){
  return (degree/scaling) + offset;
}

void RFAb_spin(double degree){
  RFAb.write(degreetomotor(degree, RFAb_offset, RFAb_scaling));
}

void RFHip_spin(double degree){
  RFHip.write(degreetomotor(degree, RFHip_offset, RFHip_scaling));
}

void RFKnee_spin(double degree){
  RFKnee.write(degreetomotor(degree, RFKnee_offset, RFKnee_scaling));
}

void RBAb_spin(double degree){
  RBAb.write(degreetomotor(degree, RBAb_offset, RBAb_scaling));
}

void RBHip_spin(double degree){
  RBHip.write(degreetomotor(degree, RBHip_offset, RBHip_scaling));
}

void RBKnee_spin(double degree){
  RBKnee.write(degreetomotor(degree, RBKnee_offset, RBKnee_scaling));
}

void LFAb_spin(double degree){
  LFAb.write(degreetomotor(degree, LFAb_offset, LFAb_scaling));
}

void LFHip_spin(double degree){
  LFHip.write(degreetomotor(degree, LFHip_offset, LFHip_scaling));
}

void LFKnee_spin(double degree){
  LFKnee.write(degreetomotor(degree, LFKnee_offset, LFKnee_scaling));
}

void LBAb_spin(double degree){
  LBAb.write(degreetomotor(degree, LBAb_offset, LBAb_scaling));
}

void LBHip_spin(double degree){
  LBHip.write(degreetomotor(degree, LBHip_offset, LBHip_scaling));
}

void LBKnee_spin(double degree){
  LBKnee.write(degreetomotor(degree, LBKnee_offset, LBKnee_scaling));
}

double to_rad(double degree){
  return degree / 180.0 * pi;
}

double to_deg(double rad){
  return rad * 180.0 / pi;
}

void RF_gotopos(double x, double y){
  double cosphi = (upper_leg_length * upper_leg_length + lower_leg_length * lower_leg_length - x * x - y * y) / (2 * upper_leg_length * lower_leg_length);
  double phi = acos(cosphi);
  RFKnee_spin(to_deg(phi));

  double alpha = atan2(x, y);
  double beta = (pi - phi)*lower_leg_length/(upper_leg_length + lower_leg_length);
  double theta = pi / 2 + alpha - beta;
  RFHip_spin(to_deg(theta));
}

void RF_gotopos(double x, double y, double z){
  double sinsqphi = (z*z - ab_width*ab_width)/(y*y+z*z);
  double phi;
  if(sinsqphi < 0){
    phi = asin(-sqrt(-sinsqphi));
  }else{
    phi = asin(sqrt(sinsqphi));
  }
  RFAb_spin(to_deg(phi));

  double hiptofoot = sqrt(y*y + z*z - ab_width*ab_width);
  RF_gotopos(x, hiptofoot); 
}

void RB_gotopos(double x, double y){
  double cosphi = (upper_leg_length * upper_leg_length + lower_leg_length * lower_leg_length - x * x - y * y) / (2 * upper_leg_length * lower_leg_length);
  double phi = acos(cosphi);
  RBKnee_spin(to_deg(phi));

  double alpha = atan2(x, y);
  double beta = (pi - phi)*lower_leg_length/(upper_leg_length + lower_leg_length);
  double theta = pi / 2 + alpha - beta;
  RBHip_spin(to_deg(theta));
}

void RB_gotopos(double x, double y, double z){
  double sinsqphi = (z*z - ab_width*ab_width)/(y*y+z*z);
  double phi;
  if(sinsqphi < 0){
    phi = asin(-sqrt(-sinsqphi));
  }else{
    phi = asin(sqrt(sinsqphi));
  }
  RBAb_spin(to_deg(phi));

  double hiptofoot = sqrt(y*y + z*z - ab_width*ab_width);
  RB_gotopos(x, hiptofoot); 
}

void LF_gotopos(double x, double y){
  double cosphi = (upper_leg_length * upper_leg_length + lower_leg_length * lower_leg_length - x * x - y * y) / (2 * upper_leg_length * lower_leg_length);
  double phi = acos(cosphi);
  LFKnee_spin(to_deg(phi));

  double alpha = atan2(x, y);
  double beta = (pi - phi)*lower_leg_length/(upper_leg_length + lower_leg_length);
  double theta = pi / 2 + alpha - beta;
  LFHip_spin(to_deg(theta));
}

void LF_gotopos(double x, double y, double z){
  double sinsqphi = (z*z - ab_width*ab_width)/(y*y+z*z);
  double phi;
  if(sinsqphi < 0){
    phi = asin(-sqrt(-sinsqphi));
  }else{
    phi = asin(sqrt(sinsqphi));
  }
  LFAb_spin(to_deg(phi));

  double hiptofoot = sqrt(y*y + z*z - ab_width*ab_width);
  LF_gotopos(x, hiptofoot); 
}

void LB_gotopos(double x, double y){
  double cosphi = (upper_leg_length * upper_leg_length + lower_leg_length * lower_leg_length - x * x - y * y) / (2 * upper_leg_length * lower_leg_length);
  double phi = acos(cosphi);
  LBKnee_spin(to_deg(phi));

  double alpha = atan2(x, y);
  double beta = (pi - phi)*lower_leg_length/(upper_leg_length + lower_leg_length);
  double theta = pi / 2 + alpha - beta;
  LBHip_spin(to_deg(theta));
}

void LB_gotopos(double x, double y, double z){
  double sinsqphi = (z*z - ab_width*ab_width)/(y*y+z*z);
  double phi;
  if(sinsqphi < 0){
    phi = asin(-sqrt(-sinsqphi));
  }else{
    phi = asin(sqrt(sinsqphi));
  }
  LBAb_spin(to_deg(phi));

  double hiptofoot = sqrt(y*y + z*z - ab_width*ab_width);
  LB_gotopos(x, hiptofoot); 
}

double smoothmove(long starttime, long endtime, double startpos, double endpos){
  long curtime = millis();
  if(curtime > endtime){
    return endpos;
  }
  if(curtime < starttime){
    return startpos;
  }
  return startpos + (endpos - startpos) * (curtime - starttime) / (endtime - starttime);
}

double tocirclex(double angle, double centerx, double radius){
  return centerx + radius * cos(to_rad(angle));
}

double tocircley(double angle, double centery, double radius){
  return centery + radius * sin(to_rad(angle));
}

void detachall(){
  RFAb.detach();
  RFHip.detach();
  RFKnee.detach();

  RBAb.detach();
  RBHip.detach();
  RBKnee.detach();

  LFAb.detach();
  LFHip.detach();
  LFKnee.detach();

  LBAb.detach();
  LBHip.detach();
  LBKnee.detach();
}

void attachall(){
  RFAb.attach(RFAb_pin);
  RFHip.attach(RFHip_pin);
  RFKnee.attach(RFKnee_pin);

  RBAb.attach(RBAb_pin);
  RBHip.attach(RBHip_pin);
  RBKnee.attach(RBKnee_pin);

  LFAb.attach(LFAb_pin);
  LFHip.attach(LFHip_pin);
  LFKnee.attach(LFKnee_pin);

  LBAb.attach(LBAb_pin);
  LBHip.attach(LBHip_pin);
  LBKnee.attach(LBKnee_pin);
}