/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "monotone.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Monotone character structure
enum
{
  Monotone_ArcMain_Idle0,
  Monotone_ArcMain_Idle1,
  Monotone_ArcMain_Idle2,
  Monotone_ArcMain_Left0,
  Monotone_ArcMain_Left1,
  Monotone_ArcMain_Left2,
  Monotone_ArcMain_Down0,
  Monotone_ArcMain_Down1,
  Monotone_ArcMain_Down2,
  Monotone_ArcMain_Down3,
  Monotone_ArcMain_Up0,
  Monotone_ArcMain_Up1,
  Monotone_ArcMain_Up2,
  Monotone_ArcMain_Right0,
  Monotone_ArcMain_Right1,
  Monotone_ArcMain_IdleD0,
  Monotone_ArcMain_IdleD1,
  Monotone_ArcMain_LeftD0,
  Monotone_ArcMain_DownD0,
  Monotone_ArcMain_UpD0,
  Monotone_ArcMain_RightD0,
  Monotone_ArcMain_IdleR0,
  Monotone_ArcMain_IdleR1,
  Monotone_ArcMain_IdleR2,
  Monotone_ArcMain_LeftR0,
  Monotone_ArcMain_LeftR1,
  Monotone_ArcMain_DownR0,
  Monotone_ArcMain_UpR0,
  Monotone_ArcMain_RightR0,
  Monotone_ArcMain_IdleG0,
  Monotone_ArcMain_IdleG1,
  Monotone_ArcMain_IdleG2,
  Monotone_ArcMain_LeftG0,
  Monotone_ArcMain_DownG0,
  Monotone_ArcMain_UpG0,
  Monotone_ArcMain_RightG0,
  Monotone_ArcMain_IdleB0,
  Monotone_ArcMain_IdleB1,
  Monotone_ArcMain_IdleB2,
  Monotone_ArcMain_IdleB3,
  Monotone_ArcMain_IdleB4,
  Monotone_ArcMain_IdleB5,
  Monotone_ArcMain_IdleB6,
  Monotone_ArcMain_LeftB0,
  Monotone_ArcMain_DownB0,
  Monotone_ArcMain_UpB0,
  Monotone_ArcMain_RightB0,

	Monotone_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Monotone_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Monotone;

//Monotone character definitions
static const CharFrame char_monotone_frame[] = {
  {Monotone_ArcMain_Idle0, {  0,  0,106, 92}, {155,122}}, //0 idle 1
  {Monotone_ArcMain_Idle0, {106,  0,106, 88}, {154,118}}, //1 idle 2
  {Monotone_ArcMain_Idle0, {  0, 92,107, 88}, {156,118}}, //2 idle 3
  {Monotone_ArcMain_Idle0, {107, 92,107, 91}, {154,121}}, //3 idle 4
  {Monotone_ArcMain_Idle1, {  0,  0,107, 84}, {155,114}}, //4 idle 5
  {Monotone_ArcMain_Idle1, {107,  0,105, 89}, {152,119}}, //5 idle 6
  {Monotone_ArcMain_Idle1, {  0, 89,103, 92}, {151,123}}, //6 idle 7
  {Monotone_ArcMain_Idle1, {103, 89,103, 86}, {151,117}}, //7 idle 8
  {Monotone_ArcMain_Idle2, {  0,  0,102, 87}, {151,119}}, //8 idle 9
  {Monotone_ArcMain_Idle2, {102,  0,102, 88}, {151,120}}, //9 idle 10
  {Monotone_ArcMain_Idle2, {  0, 88,101, 92}, {150,123}}, //10 idle 11
  {Monotone_ArcMain_Idle2, {101, 88,102, 85}, {150,116}}, //11 idle 12

  {Monotone_ArcMain_Left0, {  0,  0,135, 81}, {200,121}}, //12 left 1
  {Monotone_ArcMain_Left0, {  0, 81,135, 81}, {200,122}}, //13 left 2
  {Monotone_ArcMain_Left0, {  0,162,130, 88}, {188,122}}, //14 left 3
  {Monotone_ArcMain_Left1, {  0,  0,130, 88}, {188,123}}, //15 left 4
  {Monotone_ArcMain_Left1, {130,  0,125, 88}, {181,121}}, //16 left 5
  {Monotone_ArcMain_Left1, {  0, 88,125, 88}, {181,122}}, //17 left 6
  {Monotone_ArcMain_Left1, {125, 88,124, 87}, {180,120}}, //18 left 7
  {Monotone_ArcMain_Left2, {  0,  0,124, 87}, {180,120}}, //19 left 8
  {Monotone_ArcMain_Left2, {124,  0,123, 87}, {180,120}}, //20 left 9
  {Monotone_ArcMain_Left2, {  0, 87,124, 87}, {180,120}}, //21 left 10

  {Monotone_ArcMain_Down0, {  0,  0,143, 75}, {172,107}}, //22 down 1
  {Monotone_ArcMain_Down0, {  0, 75,143, 75}, {172,107}}, //23 down 2
  {Monotone_ArcMain_Down0, {  0,150,134, 72}, {168,109}}, //24 down 3
  {Monotone_ArcMain_Down1, {  0,  0,134, 71}, {168,108}}, //25 down 4
  {Monotone_ArcMain_Down1, {  0, 71,132, 72}, {168,110}}, //26 down 5
  {Monotone_ArcMain_Down1, {  0,143,132, 72}, {168,110}}, //27 down 6
  {Monotone_ArcMain_Down2, {  0,  0,132, 71}, {167,110}}, //28 down 7
  {Monotone_ArcMain_Down2, {  0, 71,132, 71}, {167,110}}, //29 down 8
  {Monotone_ArcMain_Down2, {  0,142,132, 71}, {167,110}}, //30 down 9
  {Monotone_ArcMain_Down3, {  0,  0,132, 71}, {167,109}}, //31 down 10

  {Monotone_ArcMain_Up0, {  0,  0, 86,100}, {137,138}}, //32 up 1
  {Monotone_ArcMain_Up0, { 86,  0, 85, 94}, {136,132}}, //33 up 2
  {Monotone_ArcMain_Up0, {  0,100, 97, 93}, {146,128}}, //34 up 3
  {Monotone_ArcMain_Up0, { 97,100, 97, 92}, {146,128}}, //35 up 4
  {Monotone_ArcMain_Up1, {  0,  0,101, 96}, {150,132}}, //36 up 5
  {Monotone_ArcMain_Up1, {101,  0,102, 89}, {151,125}}, //37 up 6
  {Monotone_ArcMain_Up1, {  0, 96,104, 91}, {153,127}}, //38 up 7
  {Monotone_ArcMain_Up1, {104, 96,104, 90}, {152,126}}, //39 up 8
  {Monotone_ArcMain_Up2, {  0,  0,104, 95}, {153,132}}, //40 up 9
  {Monotone_ArcMain_Up2, {104,  0,104, 90}, {153,127}}, //41 up 10

  {Monotone_ArcMain_Right0, {  0,  0,128, 72}, {151,114}}, //42 right 1
  {Monotone_ArcMain_Right0, {  0, 72,129, 72}, {151,114}}, //43 right 2
  {Monotone_ArcMain_Right0, {129, 72,126, 74}, {154,115}}, //44 right 3
  {Monotone_ArcMain_Right0, {  0,146,126, 73}, {153,114}}, //45 right 4
  {Monotone_ArcMain_Right0, {126,146,127, 75}, {155,115}}, //46 right 5
  {Monotone_ArcMain_Right1, {  0,  0,127, 75}, {156,115}}, //47 right 6
  {Monotone_ArcMain_Right1, {127,  0,127, 75}, {156,115}}, //48 right 7
  {Monotone_ArcMain_Right1, {  0, 75,127, 75}, {156,115}}, //49 right 8
  {Monotone_ArcMain_Right1, {127, 75,127, 75}, {155,115}}, //50 right 9
  
  {Monotone_ArcMain_IdleD0, {  0,  0, 96,107}, {156,110}}, //51 idled 1
  {Monotone_ArcMain_IdleD0, { 96,  0, 96,107}, {156,111}}, //52 idled 2
  {Monotone_ArcMain_IdleD0, {  0,107, 95,109}, {156,113}}, //53 idled 3
  {Monotone_ArcMain_IdleD0, { 95,107, 95,111}, {157,115}}, //54 idled 4
  {Monotone_ArcMain_IdleD1, {  0,  0, 96,112}, {157,115}}, //55 idled 5

  {Monotone_ArcMain_LeftD0, {  0,  0,101,110}, {163,113}}, //56 leftd 1
  {Monotone_ArcMain_LeftD0, {101,  0, 98,110}, {160,114}}, //57 leftd 2

  {Monotone_ArcMain_DownD0, {  0,  0, 96, 99}, {156,104}}, //58 downd 1
  {Monotone_ArcMain_DownD0, { 96,  0, 97,101}, {158,106}}, //59 downd 2

  {Monotone_ArcMain_UpD0, {  0,  0, 91,114}, {154,119}}, //60 upd 1
  {Monotone_ArcMain_UpD0, { 91,  0, 93,112}, {155,117}}, //61 upd 2

  {Monotone_ArcMain_RightD0, {  0,  0, 89,107}, {148,111}}, //62 rightd 1
  {Monotone_ArcMain_RightD0, { 89,  0, 88,108}, {148,112}}, //63 rightd 2
  
  {Monotone_ArcMain_IdleR0, {  0,  0,146, 85}, {169, 76}}, //64 idler 1
  {Monotone_ArcMain_IdleR0, {  0, 86,147, 84}, {170, 75}}, //65 idler 2
  {Monotone_ArcMain_IdleR1, {  0,  0,146, 85}, {169, 76}}, //66 idler 3
  {Monotone_ArcMain_IdleR1, {  0, 86,145, 90}, {169, 81}}, //67 idler 4
  {Monotone_ArcMain_IdleR2, {  0,  0,143, 92}, {168, 83}}, //68 idler 5
  
  {Monotone_ArcMain_LeftR0, {  0,  0,164, 88}, {212, 79}}, //69 leftr 1
  {Monotone_ArcMain_LeftR0, {  0, 89,164, 88}, {208, 79}}, //70 leftr 2
  {Monotone_ArcMain_LeftR1, {  0,  0,165, 88}, {209, 79}}, //71 leftr 3
	
  {Monotone_ArcMain_DownR0, {  0,  0,116, 91}, {149, 77}}, //72 downr 1
  {Monotone_ArcMain_DownR0, {117,  0,129,100}, {159, 91}}, //73 downr 2
  {Monotone_ArcMain_DownR0, {  0,101,129,100}, {159, 91}}, //74 downr 3
	
  {Monotone_ArcMain_UpR0, {  0,  0, 98,101}, {146, 92}}, //75 upr 1
  {Monotone_ArcMain_UpR0, { 99,  0, 96, 91}, {143, 82}}, //76 upr 2
  {Monotone_ArcMain_UpR0, {  0,102, 96, 92}, {143, 83}}, //77 upr 3
	
  {Monotone_ArcMain_RightR0, {  0,  0, 97, 95}, {125, 77}}, //78 rightr 1
  {Monotone_ArcMain_RightR0, { 98,  0,105, 93}, {132, 77}}, //79 rightr 2
  {Monotone_ArcMain_RightR0, {  0, 96,106, 93}, {132, 77}}, //80 rightr 3
  
  {Monotone_ArcMain_IdleG0, {  0,  0,100, 81}, {204,171}}, //81 idleg 1
  {Monotone_ArcMain_IdleG0, {100,  0,101, 80}, {205,170}}, //82 idleg 2
  {Monotone_ArcMain_IdleG0, {  0, 81,107, 80}, {208,170}}, //83 idleg 3
  {Monotone_ArcMain_IdleG0, {107, 81,102, 80}, {203,170}}, //84 idleg 4
  {Monotone_ArcMain_IdleG0, {  0,162, 94, 79}, {200,169}}, //85 idleg 5
  {Monotone_ArcMain_IdleG0, { 94,162, 90, 80}, {200,170}}, //86 idleg 6
  {Monotone_ArcMain_IdleG1, {  0,  0, 91, 80}, {201,170}}, //87 idleg 7
  {Monotone_ArcMain_IdleG1, { 91,  0, 95, 80}, {204,170}}, //88 idleg 8
  {Monotone_ArcMain_IdleG1, {  0, 80,101, 80}, {207,170}}, //89 idleg 9
  {Monotone_ArcMain_IdleG1, {101, 80,102, 82}, {204,172}}, //90 idleg 10
  {Monotone_ArcMain_IdleG1, {  0,162,100, 81}, {204,171}}, //91 idleg 11
  {Monotone_ArcMain_IdleG1, {100,162,101, 80}, {207,170}}, //92 idleg 12
  {Monotone_ArcMain_IdleG2, {  0,  0, 90, 79}, {200,169}}, //93 idleg 13
  {Monotone_ArcMain_IdleG2, { 90,  0, 90, 79}, {200,169}}, //94 idleg 14
  {Monotone_ArcMain_IdleG2, {  0, 79, 89, 80}, {200,170}}, //95 idleg 15
  {Monotone_ArcMain_IdleG2, { 89, 79, 94, 80}, {200,170}}, //96 idleg 16
  {Monotone_ArcMain_IdleG2, {  0,159,103, 79}, {204,170}}, //97 idleg 17
  {Monotone_ArcMain_IdleG2, {103,159,104, 80}, {208,170}}, //98 idleg 18

  {Monotone_ArcMain_LeftG0, {  0,  0,100, 81}, {221-7,162+8}}, //99 leftg 1
  {Monotone_ArcMain_LeftG0, {100,  0, 99, 81}, {220-7,162+8}}, //100 leftg 2
  {Monotone_ArcMain_LeftG0, {  0, 81,100, 81}, {219-7,161+8}}, //101 leftg 3
  {Monotone_ArcMain_LeftG0, {100, 81,100, 81}, {219-7,161+8}}, //102 leftg 4
  {Monotone_ArcMain_LeftG0, {  0,162,100, 81}, {218-7,161+8}}, //103 leftg 5
  {Monotone_ArcMain_LeftG0, {100,162,100, 81}, {218-7,161+8}}, //104 leftg 6

  {Monotone_ArcMain_DownG0, {  0,  0, 87, 71}, {184+10,140+18}}, //105 downg 1
  {Monotone_ArcMain_DownG0, { 87,  0, 86, 71}, {183+10,140+18}}, //106 downg 2
  {Monotone_ArcMain_DownG0, {  0, 71, 86, 71}, {183+10,142+18}}, //107 downg 3
  {Monotone_ArcMain_DownG0, { 86, 71, 86, 71}, {184+10,142+18}}, //108 downg 4
  {Monotone_ArcMain_DownG0, {  0,142, 87, 71}, {185+10,142+18}}, //109 downg 5
  {Monotone_ArcMain_DownG0, { 87,142, 86, 72}, {185+10,143+18}}, //110 downg 6

  {Monotone_ArcMain_UpG0, {  0,  0, 78, 85}, {185+9,177-3}}, //111 upg 1
  {Monotone_ArcMain_UpG0, { 78,  0, 77, 87}, {184+9,179-3}}, //112 upg 2
  {Monotone_ArcMain_UpG0, {155,  0, 77, 83}, {184+9,175-3}}, //113 upg 3
  {Monotone_ArcMain_UpG0, {  0, 87, 78, 83}, {185+9,175-3}}, //114 upg 4
  {Monotone_ArcMain_UpG0, { 78, 87, 77, 81}, {185+9,173-3}}, //115 upg 5
  {Monotone_ArcMain_UpG0, {155, 87, 77, 81}, {184+9,173-3}}, //116 upg 6

  {Monotone_ArcMain_RightG0, {  0,  0, 96, 80}, {169+19,158+12}}, //117 rightg 1
  {Monotone_ArcMain_RightG0, { 96,  0, 95, 80}, {169+19,158+12}}, //118 rightg 2
  {Monotone_ArcMain_RightG0, {  0, 80, 92, 79}, {169+19,158+12}}, //119 rightg 3
  {Monotone_ArcMain_RightG0, { 92, 80, 93, 79}, {170+19,158+12}}, //120 rightg 4
  {Monotone_ArcMain_RightG0, {  0,160, 89, 79}, {168+19,158+12}}, //121 rightg 5
  {Monotone_ArcMain_RightG0, { 89,160, 89, 80}, {168+19,159+12}}, //122 rightg 6
  
  {Monotone_ArcMain_IdleB0, {  0,  0,170, 94}, {159,141}}, //123 idleb 1
  {Monotone_ArcMain_IdleB0, {  0, 94,171, 93}, {158,140}}, //124 idleb 2
  {Monotone_ArcMain_IdleB1, {  0,  0,170, 93}, {157,140}}, //125 idleb 3
  {Monotone_ArcMain_IdleB1, {  0, 93,167, 94}, {154,141}}, //126 idleb 4
  {Monotone_ArcMain_IdleB2, {  0,  0,165, 95}, {152,142}}, //127 idleb 5
  {Monotone_ArcMain_IdleB2, {  0, 95,164, 98}, {150,145}}, //128 idleb 6
  {Monotone_ArcMain_IdleB3, {  0,  0,164, 98}, {149,145}}, //129 idleb 7
  {Monotone_ArcMain_IdleB3, {  0, 98,164, 99}, {150,146}}, //130 idleb 8
  {Monotone_ArcMain_IdleB4, {  0,  0,165, 99}, {152,146}}, //131 idleb 9
  {Monotone_ArcMain_IdleB4, {  0, 99,166, 99}, {153,145}}, //132 idleb 10
  {Monotone_ArcMain_IdleB5, {  0,  0,167, 99}, {154,145}}, //133 idleb 11
  {Monotone_ArcMain_IdleB5, {  0, 99,166, 98}, {155,144}}, //134 idleb 12
  {Monotone_ArcMain_IdleB6, {  0,  0,168, 96}, {157,143}}, //135 idleb 13

  {Monotone_ArcMain_LeftB0, {  0,  0,104,107}, {167,154}}, //136 leftb 1
  {Monotone_ArcMain_LeftB0, {104,  0,103,104}, {165,151}}, //137 leftb 2
  {Monotone_ArcMain_LeftB0, {  0,107,103,104}, {165,151}}, //138 leftb 3
  {Monotone_ArcMain_LeftB0, {103,107,100,108}, {161,155}}, //139 leftb 4

  {Monotone_ArcMain_DownB0, {  0,  0,190, 99}, {170,135}}, //140 downb 1
  {Monotone_ArcMain_DownB0, {  0, 99,180, 82}, {169,122}}, //141 downb 2

  {Monotone_ArcMain_UpB0, {  0,  0,137, 98}, {143,146}}, //142 upb 1
  {Monotone_ArcMain_UpB0, {  0, 98,128, 96}, {141,144}}, //143 upb 2

  {Monotone_ArcMain_RightB0, {  0,  0,171,101}, {115,148}}, //144 rightb 1
  {Monotone_ArcMain_RightB0, {  0,101,144,101}, {118,148}}, //145 rightb 2
};

static const Animation char_monotone_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, ASCR_BACK, 1}}, //CharAnim_Idle
	{1, (const u8[]){ 12, 13, 14, 15, 16, 17, 18, 18, 19, 20, 21, 21, 19, 20, 21, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 22, 23, 24, 25, 26, 27, 28, 29, 30, 28, 28, 29, 30, 31, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 38, 39, 40, 41, 38, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 42, 43, 44, 45, 46, 47, 48, 48, 49, 50, 48, 48, 49, 50, 48, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

static const Animation char_bfd_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 51, 52, 53, 54, 55, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 56, 57, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 58, 59, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 60, 61, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 62, 63, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
};

static const Animation char_redd_anim[CharAnim_Max] = {
	{2, (const u8[]){ 64, 65, 66, 67, 68, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 69, 70, 71, 71, 71, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 72, 73, 74, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 75, 76, 77, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 78, 79, 80, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

static const Animation char_greend_anim[CharAnim_Max] = {
	{2, (const u8[]){ 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{1, (const u8[]){ 99, 100, 101, 102, 103, 104, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 105, 106, 107, 108, 109, 110, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 111, 112, 113, 114, 115, 116, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 117, 118, 119, 120, 121, 122, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

static const Animation char_blackd_anim[CharAnim_Max] = {
	{2, (const u8[]){ 123, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{1, (const u8[]){ 136, 137, 138, 139, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 140, 141, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 142, 143, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 144, 145, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Monotone character functions
void Char_Monotone_SetFrame(void *user, u8 frame)
{
	Char_Monotone *this = (Char_Monotone*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_monotone_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Monotone_Tick(Character *character)
{
	Char_Monotone *this = (Char_Monotone*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_IdentityCrisis)
	{
		
		
		//Switches
		if (stage.song_step == -29) //bf disguise
		{
			Animatable_Init(&this->character.animatable, char_bfd_anim);
			this->character.health_bar = 0xFF29B5D6;
			this->character.size = FIXED_DEC(1,1);
			this->character.health_i = 0;
		}
		if ((stage.song_step == 384) || (stage.song_step == 896) || (stage.song_step == 1500) || (stage.song_step == 2272) || (stage.song_step == 3328)) //normal
		{
			Animatable_Init(&this->character.animatable, char_monotone_anim);
			this->character.health_bar = 0xFF392C49;
			this->character.size = FIXED_DEC(1667,1000);
			this->character.health_i = 1;
		}
		if ((stage.song_step == 636) || (stage.song_step == 2817) || (stage.song_step == 3198)) //red disguise
		{
			Animatable_Init(&this->character.animatable, char_redd_anim);
			this->character.health_bar = 0xFFBB2D30;
			this->character.size = FIXED_DEC(1,1);
			this->character.health_i = 2;
		}
		if ((stage.song_step == 1168) || (stage.song_step == 2878) || (stage.song_step == 3280)) //green disguise
		{
			Animatable_Init(&this->character.animatable, char_greend_anim);
			this->character.health_bar = 0xFF003315;
			this->character.size = FIXED_DEC(334,100);
			this->character.health_i = 3;
		}
		if ((stage.song_step == 1972) || (stage.song_step == 3072) || (stage.song_step == 3296)) //black disguise
		{
			Animatable_Init(&this->character.animatable, char_blackd_anim);
			this->character.health_bar = 0xFF3B1A51;
			this->character.size = FIXED_DEC(1667,1000);
			this->character.health_i = 4;
		}
	}
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Monotone_SetFrame);
	Character_Draw(character, &this->tex, &char_monotone_frame[this->frame]);
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_IdentityCrisis: //switches
				if (stage.song_step == 384)
					character->set_anim(character, CharAnim_Down);
				if (stage.song_step == 636)
					character->set_anim(character, CharAnim_Idle);
				if (stage.song_step == 896)
					character->set_anim(character, CharAnim_Up);
				if (stage.song_step == 1168)
					character->set_anim(character, CharAnim_Idle);
				if (stage.song_step == 1500)
					character->set_anim(character, CharAnim_Idle);
				if (stage.song_step == 1972)
					character->set_anim(character, CharAnim_Idle);
				if (stage.song_step == 2272)
					character->set_anim(character, CharAnim_Idle);
				if (stage.song_step == 2817)
					character->set_anim(character, CharAnim_Idle);
				if (stage.song_step == 2878)
					character->set_anim(character, CharAnim_Up);
				if (stage.song_step == 3072)
					character->set_anim(character, CharAnim_Idle);
				if (stage.song_step == 3198)
					character->set_anim(character, CharAnim_Down);
				if (stage.song_step == 3280)
					character->set_anim(character, CharAnim_Up);
				if (stage.song_step == 3296)
					character->set_anim(character, CharAnim_Up);
				if (stage.song_step == 3328)
					character->set_anim(character, CharAnim_Up);
				break;
			default:
				break;
		}
}

void Char_Monotone_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Monotone_Free(Character *character)
{
	Char_Monotone *this = (Char_Monotone*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Monotone_New(fixed_t x, fixed_t y)
{
	//Allocate monotone object
	Char_Monotone *this = Mem_Alloc(sizeof(Char_Monotone));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Monotone_New] Failed to allocate monotone object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Monotone_Tick;
	this->character.set_anim = Char_Monotone_SetAnim;
	this->character.free = Char_Monotone_Free;
	
	Animatable_Init(&this->character.animatable, char_bfd_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 0;

	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\MONOTONE.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "left0.tim",
  "left1.tim",
  "left2.tim",
  "down0.tim",
  "down1.tim",
  "down2.tim",
  "down3.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "right0.tim",
  "right1.tim",
  "idled0.tim",
  "idled1.tim",
  "leftd0.tim",
  "downd0.tim",
  "upd0.tim",
  "rightd0.tim",
  "idler0.tim",
  "idler1.tim",
  "idler2.tim",
  "leftr0.tim",
  "leftr1.tim",
  "downr0.tim",
  "upr0.tim",
  "rightr0.tim",
  "idleg0.tim",
  "idleg1.tim",
  "idleg2.tim",
  "leftg0.tim",
  "downg0.tim",
  "upg0.tim",
  "rightg0.tim",
  "idleb0.tim",
  "idleb1.tim",
  "idleb2.tim",
  "idleb3.tim",
  "idleb4.tim",
  "idleb5.tim",
  "idleb6.tim",
  "leftb0.tim",
  "downb0.tim",
  "upb0.tim",
  "rightb0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
