#include <gb/gb.h>
#include "bangmap01.c"
#include "bangtiles01.c"
#include "pioport.h"

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


#define POKE(addr, b)  (*(unsigned char *)(addr) = (b))

unsigned char tiles[] ={NOTEOFF,NOTEON,NOTEOFFOVER,MUTEON,MUTEOFF,SPEEUP,SPEEDWN,RIGHTEND,RIGHTMARK,LEFTMARK,LEFTEND,EMPTY,POINTER,NOTEONOVER,MUTEONOVER,MUTEOFFOVER};

UBYTE matrix[8][16];                              // drum grid
UBYTE channel[8];                                 // state of channels of play position
UBYTE ch_mute[8];                                // channel mutes on or off
UBYTE addr;                                          // value to be passed to 

void move(int s_x,int s_y, UINT8 s_tile) {
    bangmap01[s_x+s_y*20]=s_tile;
    set_bkg_tiles(s_x,s_y,1,1,tiles+s_tile);
}
    
void counter(int c_x,int c_y, UINT8 c_tile) {
    set_bkg_tiles(c_x,c_y,1,1,tiles+c_tile);
}

void gb_out(UBYTE B)  {
    GBWR_REG = B;
}


void main(){

UBYTE ch_1,ch_2,ch_3,ch_4,ch_5,ch_6,ch_7,ch_8,key;
int x,y,a,speed,c,d;
SHOW_BKG;
set_bkg_data(0,50,bangtiles);
set_bkg_tiles(0,0,20,18,bangmap01);
x=1;y=1;a=1;speed=100;c=0;d=0;
ch_1=0x00;ch_2=0x00;ch_3=0x00;ch_4=0x00;ch_5=0x00;ch_6=0x00;ch_7=0x00;ch_8=0x00;  // channel output values

move(x,y,NOTEOFFOVER);

for( c=0; c<9; c++ ) {              // empty matrix , clear grid - should be function on key combo
    for( d=0; d<17; d++ ) {
     matrix[c][d]=0;
    }
}

for( c=0; c<9; c++ ) {              // set all mute vales to on - 1
     ch_mute[c]=0;
}

gb_out(0x00);          // set pio all out off

for(;;){                     // main program loop
  delay(speed);        // set speed
  addr = 0x00;         // set addr all off
  gb_out(addr);        // set output to addr - all off
  key = joypad();     // mirror for joypad input
   
     
   if (a==16){                 // pointer scrolling function
     move(a,9,EMPTY);
     a = 0; 
   }
   if(a>1){move(a,9,EMPTY);}  // hide pointer after move
   a=a+1;
   move(a,9,POINTER);
   
    if(matrix[1][a]==1 && ch_mute[1]==0){move(17,1,MUTEON); ch_1=0x01;}else{ch_1=0x00;}
    if(matrix[2][a]==1 && ch_mute[2]==0){move(17,2,MUTEON); ch_2=0x02;}else{ch_2=0x00;}
    if(matrix[3][a]==1 && ch_mute[3]==0){move(17,3,MUTEON); ch_3=0x04;}else{ch_3=0x00;}
    if(matrix[4][a]==1 && ch_mute[4]==0){move(17,4,MUTEON); ch_4=0x08;}else{ch_4=0x00;}
    if(matrix[5][a]==1 && ch_mute[5]==0){move(17,5,MUTEON); ch_5=0x10;}else{ch_5=0x00;}
    if(matrix[6][a]==1 && ch_mute[6]==0){move(17,6,MUTEON); ch_6=0x20;}else{ch_6=0x00;}
    if(matrix[7][a]==1 && ch_mute[7]==0){move(17,7,MUTEON); ch_7=0x40;}else{ch_7=0x00;}
    if(matrix[8][a]==1 && ch_mute[8]==0){move(17,8,MUTEON); ch_8=0x80;}else{ch_8=0x00;}
    addr = ch_1 + ch_2 + ch_3 + ch_4 + ch_5 + ch_6 + ch_7 + ch_8;  // add channel hex values to make output port binary value
    gb_out(addr);        // set pio out to addr
    
  delay(50);  // min system delay for master loops, implicates speed and output - on - time
  
  for( c=1; c<9; c++ ) {                // turn leds off
    move(17,c,MUTEOFF);        
  }
  
    
  if(key & J_UP) {                    // combo moves for speed up - down, start - select
        if (key & J_START ){
            if(speed>=10){
                speed=speed-5;
            };
        }
    }
  if(key & J_DOWN) {
        if (key & J_START ){
          if(speed<=200){
              speed=speed+10;
           };
        }
    }    
  if(key & J_UP) {
        if (key & J_SELECT ){
            move(18,y,SPEEUP);
            ch_mute[y]=1;
        }
    }
  if(key & J_DOWN) {
        if (key & J_SELECT ){
            move(18,y,SPEEDWN);
            ch_mute[y]=0;
        }
    }  
  
    switch (joypad(0))              // master loops actions for key inputs
    {
      case (J_RIGHT): 
        if(x<=16){   
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
      
      case (J_LEFT):       
        if(x>=2){
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
     
      case (J_UP):
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
      
      case (J_DOWN):
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
   
      case (J_A):
        move(x,y,NOTEOFFOVER);
        matrix[y][x]=0;
      break;  // end start  
      
      case (J_B):
        move(x,y,NOTEON);
        matrix[y][x]=1;
      break;  // end select
   
    }    // end button switch         
  }      // end constant loop
}        // end void
