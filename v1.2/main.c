/*****************************************************************************************
 *
 *
 *  Copyright 2008 by Jowan Sebastian
 *  No Rights Reserved.
 *
 *
 *  Version     Date            Author              Log
 *  --------------------------------------------------------------------------------------
 *  0.2         01.05.08        Jowan Sebastian     new features - yay
 *
 ****************************************************************************************/

/*
 *general includes
**/
#include <gb/gb.h>
/*
 * graphics and hardware config
**/
#include "bangmap01.c"
#include "bangtiles01.c"
#include "pioport.h"
/*
 * decalring sprites
**/
#define NOTEOFF 0
#define NOTEON 1
#define NOTEOFFOVER 2
#define MUTEON 3
#define MUTEOFF 4
#define SPEEUP 5
#define SPEEDWN 6
#define RIGHTEND 7
#define RIGHTMARK 8
#define LEFTMARK 9
#define LEFTEND 10
#define EMPTY 11
#define POINTER 12
#define NOTEONOVER 13
#define MUTEONOVER 14
#define MUTEOFFOVER 15
#define LBEG 16
#define LEND 17

#define POKE(addr, b)  (*(unsigned char *)(addr) = (b))

/*
 * global variables
**/
unsigned char tiles[] ={NOTEOFF,NOTEON,NOTEOFFOVER,MUTEON,MUTEOFF,SPEEUP,SPEEDWN,RIGHTEND,RIGHTMARK,LEFTMARK,LEFTEND,EMPTY,POINTER,NOTEONOVER,MUTEONOVER,LBEG,LEND};
UBYTE matrix[8][32];                                // drum grid
UBYTE channel[8];                                   // state of channels of play position
UBYTE ch_mute[8];                                   // channel mutes on or off
UBYTE addr;                                         // value to be passed to
/*
 * move sprites function
**/
void move(int s_x,int s_y, UINT8 s_tile) {
    bangmap01[s_x+s_y*20]=s_tile;
    set_bkg_tiles(s_x,s_y,1,1,tiles+s_tile);
}
void counter(int c_x,int c_y, UINT8 c_tile) {
    set_bkg_tiles(c_x,c_y,1,1,tiles+c_tile);
}
/*
 * make hardware address x value y function
**/
void gb_out(UBYTE B)  {
    GBWR_REG = B;
}
/*
 * main loop
 *
**/
void main(){
// LOCAL VARIABLES
UBYTE ch_1,ch_2,ch_3,ch_4,ch_5,ch_6,ch_7,ch_8,key;
int x,y,c,d;
int p;
unsigned int len;
unsigned int lcnt;
unsigned int speed;
unsigned int scrl;
SHOW_BKG;                           // needed for gameboy original
set_bkg_data(0,70,bangtiles);       // imports the sprites
set_bkg_tiles(0,0,32,18,bangmap01); // sets the tiles
gb_out(0x00);                       // set all pins low

x=0;y=1;                            // set coordinates to 0,1 top left of screen
c=0;d=0;
ch_1=0x00;ch_2=0x00;                // give channel variables hex value of 0
ch_3=0x00;ch_4=0x00;                // "
ch_5=0x00;ch_6=0x00;                // "
ch_7=0x00;ch_8=0x00;                // "

lcnt=0;                             // play position in loop
speed=100;                          // initial delay in milisecs -> speed
len=15;                             // initial length of loop value
scrl=1;                             // initial scroll value

move(x,y,NOTEOFFOVER);              // NOTEOFFOVER is labeled wrong, it is actually the position pointer,
move(len,10,LBEG);                   // places loop end sprite

for( c=0; c<9; c++ ) {              // empty matrix
    for( d=0; d<31; d++ ) {
     matrix[c][d]=0;
    }
}

for( c=0; c<9; c++ ) {              // set all mute values to off - 1
     ch_mute[c]=0;
}
// infinate loop for main app
while(1){
    
    addr = 0x00;                    // set addr all off
    gb_out(addr);                   // set output to addr - all off
    delay(speed);                   // delay by speed value
    key = joypad();                 // copy joypad value to key

    if(lcnt>=len){                  // increment play position counter to the right,
        move(lcnt,9,EMPTY);         // until reaches loop end, then back to start
        lcnt=-1;
    }
    lcnt++;
    move(lcnt,9,POINTER);           // places counter sprite
    move(lcnt-1,9,EMPTY);           // hides trailing sprite with blank
    
    
    // checks to see if there is a value in the matrix at position x by y and if the mute is off
    // if neither true, adds a hex value representing the physcal pin on hardware
    // this is much easier with binary but C has no support for that fot some reason
    if(matrix[1][lcnt]==1 && ch_mute[1]==0){  move(1,0,MUTEON);  ch_1=0x01;}else{ch_1=0x00;}
    if(matrix[2][lcnt]==1 && ch_mute[2]==0){  move(5,0,MUTEON);  ch_2=0x02;}else{ch_2=0x00;}
    if(matrix[3][lcnt]==1 && ch_mute[3]==0){  move(9,0,MUTEON);  ch_3=0x04;}else{ch_3=0x00;}
    if(matrix[4][lcnt]==1 && ch_mute[4]==0){  move(13,0,MUTEON);  ch_4=0x08;}else{ch_4=0x00;}
    if(matrix[5][lcnt]==1 && ch_mute[5]==0){  move(17,0,MUTEON);  ch_5=0x10;}else{ch_5=0x00;}
    if(matrix[6][lcnt]==1 && ch_mute[6]==0){  move(21,0,MUTEON);  ch_6=0x20;}else{ch_6=0x00;}
    if(matrix[7][lcnt]==1 && ch_mute[7]==0){  move(25,0,MUTEON);  ch_7=0x40;}else{ch_7=0x00;}
    if(matrix[8][lcnt]==1 && ch_mute[8]==0){  move(29,0,MUTEON);  ch_8=0x80;}else{ch_8=0x00;}
    // add channel values together in hex
    addr = ch_1 + ch_2 + ch_3 + ch_4 + ch_5 + ch_6 + ch_7 + ch_8;
    // set pins high / low according to value
    gb_out(addr);        
    // min system delay for master loops
    delay(50);

    p=1;
    // turns all channel indicator led sprites off on top of screen
    while(p<30){
       move(p,0,MUTEOFF);
       p+=4;
    }
    p=y*4-2;
    // combination moves picked up slightly differnently
    if(key & J_UP) {
        if (key & J_START ){                // start and up -> speeds up
            if(speed>=10){                  // 10 min
                speed=speed-5;              // decrease speed / delay value
            };
        }
    }
    if(key & J_DOWN) {
        if (key & J_START ){                // start and up -> speeds down
          if(speed<=200){                   // 200 max
              speed=speed+10;               // increases speed / delay value
           };
        }
    }
    if(key & J_UP) {                        // select and up -> mutes channel cursor is on
        if (key & J_SELECT ){               // SPEEUP wrongly labled -> muteon
            move(p,0,SPEEUP);               // place muteon sprite in place
            ch_mute[y]=1;                   // give channel mute value 1
        }
    }
    if(key & J_DOWN) {                      // select and down -> unmutes channel cursor is on
        if (key & J_SELECT ){               // SPEEDWN wrongly labled -> muteoff
            move(p,0,SPEEDWN);              // place muteoff sprite in place
            ch_mute[y]=0;                   // give channel mute value 0
        }
    }
    if(key & J_RIGHT) {                     // start and right -> increase loop length
        if (key & J_START ){
            if (len < 31){                  // max is 31 -> 2 bar 32 steps
                len++;                      // increment
                move(len,10,LBEG);          // move sprite
                move(len-1,10,EMPTY);       // hide trail
            }
        }
    }
    if(key & J_LEFT) {                      // start and left -> decrease loop length
        if (key & J_START ){
            if (len > 2){                   // min is 2
                len--;                      // decrement
                move(len,10,LBEG);          // move sprite
                move(len+1,10,EMPTY);       // hide trail
            }
        }
    }
    if(key & J_RIGHT) {                     // select and right -> scroll screen to right
        if (key & J_SELECT ){
           if(scrl<7){                      // only 7 steps
                scroll_bkg(+16,0);          // each step is 16 pixels, move by 16 in x axis
                scrl++;                     // increment
            }
        }
    }
    if(key & J_LEFT) {                      // select and right -> scroll screen to right
        if (key & J_SELECT ){
            if(scrl>2){                     
               scroll_bkg(-16,0);
               scrl--;                      // decrement
           }
        }
    }
    // single button moves picked up by switch method
    switch (joypad(0))
    {
      case (J_RIGHT):                       // move cursor right
        if(x<=31){
          if(matrix[y][x]==1){
            move(x,y,NOTEON);
            x=x+1;
          }else{
            move(x,y,NOTEOFF);
            x=x+1;
          }
          if(matrix[y][x]==1){
            move(x,y,NOTEONOVER);
          }else{
            move(x,y,NOTEOFFOVER);
          }
        }
      break;  // end right

      case (J_LEFT):                        // move cursor left
        if(x>=1){
          if(matrix[y][x]==1){
             move(x,y,NOTEON);
             x=x-1;
          }else{
            move(x,y,NOTEOFF);
            x=x-1;
          }
          if(matrix[y][x]==1){
            move(x,y,NOTEONOVER);
          }else{
            move(x,y,NOTEOFFOVER);
          }
        }
      break;  // end left

      case (J_UP):                          // move cursor up
        if(y>=2){
          if(matrix[y][x]==1){
            move(x,y,NOTEON);
            y=y-1;
          }else{
            move(x,y,NOTEOFF);
            y=y-1;
          }
          if(matrix[y][x]==1){
            move(x,y,NOTEONOVER);
          }else{
            move(x,y,NOTEOFFOVER);
          }
        }
      break;  // end up

      case (J_DOWN):                        // move cursor down
        if(y<=8){
          if(matrix[y][x]==1){
            move(x,y,NOTEON);
            y=y+1;
            move(x,y,NOTEONOVER);
          }else{
            move(x,y,NOTEOFF);
            y=y+1;
            move(x,y,NOTEOFFOVER);
          }
          if(matrix[y][x]==1){
            move(x,y,NOTEONOVER);
          }else{
            move(x,y,NOTEOFFOVER);
          }
        };
      break;  // end down

      case (J_A):                           // note off at x y
        move(x,y,NOTEOFFOVER);              // place noteoff sprite
        matrix[y][x]=0;                     // give matrix x y value of 0
      break;  // end start

      case (J_B):                           // note on at x y
        move(x,y,NOTEON);                   // place noteon sprite
        matrix[y][x]=1;                     // give matrix x y value of 1
      break;  // end select

    }    // end button switch
  }      // end constant loop
}        // end void
