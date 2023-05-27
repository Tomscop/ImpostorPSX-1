/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "blackrun.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//BlackRun character structure
enum
{
  BlackRun_ArcMain_Idle0,
  BlackRun_ArcMain_Idle1,
  BlackRun_ArcMain_Idle2,
  BlackRun_ArcMain_Idle3,
  BlackRun_ArcMain_Left0,
  BlackRun_ArcMain_Left1,
  BlackRun_ArcMain_Down0,
  BlackRun_ArcMain_Down1,
  BlackRun_ArcMain_Up0,
  BlackRun_ArcMain_Right0,
  BlackRun_ArcMain_Right1,
  BlackRun_ArcMain_Scream0,
  BlackRun_ArcMain_Scream1,
  BlackRun_ArcMain_Scream2,
  BlackRun_ArcMain_Scream3,
  BlackRun_ArcMain_Scream4,
  BlackRun_ArcMain_Scream5,
  BlackRun_ArcMain_IdleA0,
  BlackRun_ArcMain_IdleA1,
  BlackRun_ArcMain_IdleA2,
  BlackRun_ArcMain_IdleA3,
  BlackRun_ArcMain_LeftA0,
  BlackRun_ArcMain_LeftA1,
  BlackRun_ArcMain_DownA0,
  BlackRun_ArcMain_DownA1,
  BlackRun_ArcMain_DownA2,
  BlackRun_ArcMain_DownA3,
  BlackRun_ArcMain_UpA0,
  BlackRun_ArcMain_UpA1,
  BlackRun_ArcMain_RightA0,
  BlackRun_ArcMain_RightA1,
	
	BlackRun_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BlackRun_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_BlackRun;

//BlackRun character definitions
static const CharFrame char_blackrun_frame[] = {
  {BlackRun_ArcMain_Idle0, {  0,  0,147, 82}, {174,210}}, //0 idle 1
  {BlackRun_ArcMain_Idle0, {  0, 82,147, 85}, {174,211}}, //1 idle 2
  {BlackRun_ArcMain_Idle0, {  0,167,145, 86}, {174,212}}, //2 idle 3
  {BlackRun_ArcMain_Idle1, {  0,  0,135, 81}, {170,208}}, //3 idle 4
  {BlackRun_ArcMain_Idle1, {  0, 81,121, 83}, {161,204}}, //4 idle 5
  {BlackRun_ArcMain_Idle1, {121, 81,101, 83}, {149,202}}, //5 idle 6
  {BlackRun_ArcMain_Idle1, {  0,164, 92, 77}, {149,203}}, //6 idle 7
  {BlackRun_ArcMain_Idle1, { 92,164, 92, 75}, {150,205}}, //7 idle 8
  {BlackRun_ArcMain_Idle2, {  0,  0, 93, 80}, {150,210}}, //8 idle 9
  {BlackRun_ArcMain_Idle2, { 93,  0, 93, 80}, {150,210}}, //9 idle 10
  {BlackRun_ArcMain_Idle2, {  0, 80, 95, 83}, {149,212}}, //10 idle 11
  {BlackRun_ArcMain_Idle2, { 95, 80,107, 86}, {149,212}}, //11 idle 12
  {BlackRun_ArcMain_Idle2, {  0,166,118, 87}, {148,210}}, //12 idle 13
  {BlackRun_ArcMain_Idle2, {118,166,124, 88}, {151,208}}, //13 idle 14
  {BlackRun_ArcMain_Idle3, {  0,  0,143, 78}, {170,209}}, //14 idle 15

  {BlackRun_ArcMain_Left0, {  0,  0,132, 83}, {166-1,202+2}}, //15 left 1
  {BlackRun_ArcMain_Left0, {  0, 83,130, 82}, {166-1,201+2}}, //16 left 2
  {BlackRun_ArcMain_Left0, {  0,166,134, 81}, {166-1,200+2}}, //17 left 3
  {BlackRun_ArcMain_Left1, {  0,  0,145, 76}, {171-1,200+2}}, //18 left 4
  {BlackRun_ArcMain_Left1, {  0, 76,162, 77}, {177-1,203+2}}, //19 left 5

  {BlackRun_ArcMain_Down0, {  0,  0,151, 87}, {143+1,203}}, //20 down 1
  {BlackRun_ArcMain_Down0, {  0, 87,152, 87}, {143+1,203}}, //21 down 2
  {BlackRun_ArcMain_Down1, {  0,  0,142, 85}, {144+1,204}}, //22 down 3
  {BlackRun_ArcMain_Down1, {  0, 85,138, 81}, {146+1,206}}, //23 down 4
  {BlackRun_ArcMain_Down1, {  0,170,140, 80}, {153+1,207}}, //24 down 5

  {BlackRun_ArcMain_Up0, {  0,  0,111,115}, {166,225-6}}, //25 up 1
  {BlackRun_ArcMain_Up0, {111,  0,111,116}, {166,226-6}}, //26 up 2
  {BlackRun_ArcMain_Up0, {  0,116,113,113}, {166,224-6}}, //27 up 3
  {BlackRun_ArcMain_Up0, {113,116,126,104}, {166,220-6}}, //28 up 4
  {BlackRun_ArcMain_Up0, {113,116,126,104}, {166,220-6}}, //29 up 4

  {BlackRun_ArcMain_Right0, {  0,  0,174, 91}, {185-3,210-1}}, //30 right 1
  {BlackRun_ArcMain_Right0, {  0, 91,174, 92}, {186-3,210-1}}, //31 right 2
  {BlackRun_ArcMain_Right1, {  0,  0,168, 95}, {182-3,210-1}}, //32 right 3
  {BlackRun_ArcMain_Right1, {  0, 95,158, 93}, {177-3,211-1}}, //33 right 4
  {BlackRun_ArcMain_Right1, {  0, 95,158, 93}, {177-3,211-1}}, //34 right 4

  {BlackRun_ArcMain_Scream0, {  0,  0,124, 92}, {168,218-22}}, //35 scream 1
  {BlackRun_ArcMain_Scream0, {  0,  0,124, 92}, {168,218-22}}, //36 scream 1
  {BlackRun_ArcMain_Scream0, {124,  0,122, 89}, {164,215-22}}, //37 scream 2
  {BlackRun_ArcMain_Scream0, {124,  0,122, 89}, {164,215-22}}, //38 scream 2
  {BlackRun_ArcMain_Scream0, {  0, 92,121, 83}, {163,215-22}}, //39 scream 3
  {BlackRun_ArcMain_Scream0, {  0, 92,121, 83}, {163,215-22}}, //40 scream 3
  {BlackRun_ArcMain_Scream0, {121, 92,121, 83}, {163,215-22}}, //41 scream 4
  {BlackRun_ArcMain_Scream0, {121, 92,121, 83}, {163,215-22}}, //42 scream 4
  {BlackRun_ArcMain_Scream1, {  0,  0,121, 87}, {163,215-22}}, //43 scream 5
  {BlackRun_ArcMain_Scream1, {  0,  0,121, 87}, {163,215-22}}, //44 scream 5
  {BlackRun_ArcMain_Scream1, {121,  0,124, 86}, {164,215-22}}, //45 scream 6
  {BlackRun_ArcMain_Scream1, {121,  0,124, 86}, {164,215-22}}, //46 scream 6
  {BlackRun_ArcMain_Scream1, {  0, 87,129, 85}, {166,216-22}}, //47 scream 7
  {BlackRun_ArcMain_Scream1, {  0, 87,129, 85}, {166,216-22}}, //48 scream 7
  {BlackRun_ArcMain_Scream2, {  0,  0,139, 84}, {168,217-22}}, //49 scream 8
  {BlackRun_ArcMain_Scream2, {  0,  0,139, 84}, {168,217-22}}, //50 scream 8
  {BlackRun_ArcMain_Scream2, {  0, 84,153, 90}, {172,220-22}}, //51 scream 9
  {BlackRun_ArcMain_Scream2, {  0, 84,153, 90}, {172,220-22}}, //52 scream 9
  {BlackRun_ArcMain_Scream3, {  0,  0,130,165}, {165,304-22}}, //53 scream 10
  {BlackRun_ArcMain_Scream3, {  0,  0,130,165}, {165,304-22}}, //54 scream 10
  {BlackRun_ArcMain_Scream4, {  0,  0,137,108}, {168,250-22}}, //55 scream 11
  {BlackRun_ArcMain_Scream4, {  0,  0,137,108}, {168,250-22}}, //56 scream 11
  {BlackRun_ArcMain_Scream4, {  0,  0,137,108}, {168,250-22}}, //57 scream 11
  {BlackRun_ArcMain_Scream4, {  0,  0,137,108}, {168,250-22}}, //58 scream 11
  {BlackRun_ArcMain_Scream4, {  0,108,127,102}, {164,249-22}}, //59 scream 12
  {BlackRun_ArcMain_Scream4, {  0,108,127,102}, {164,249-22}}, //60 scream 12
  {BlackRun_ArcMain_Scream4, {  0,108,127,102}, {164,249-22}}, //61 scream 12
  {BlackRun_ArcMain_Scream4, {  0,108,127,102}, {164,249-22}}, //62 scream 12
  {BlackRun_ArcMain_Scream4, {127,108,127,103}, {165,250-22}}, //63 scream 13
  {BlackRun_ArcMain_Scream4, {127,108,127,103}, {165,250-22}}, //64 scream 13
  {BlackRun_ArcMain_Scream4, {127,108,127,103}, {165,250-22}}, //65 scream 13
  {BlackRun_ArcMain_Scream4, {127,108,127,103}, {165,250-22}}, //66 scream 13
  {BlackRun_ArcMain_Scream5, {  0,  0,128,103}, {165,250-22}}, //67 scream 14
  {BlackRun_ArcMain_Scream5, {  0,  0,128,103}, {165,250-22}}, //68 scream 14
  {BlackRun_ArcMain_Scream5, {  0,  0,128,103}, {165,250-22}}, //69 scream 14
  {BlackRun_ArcMain_Scream5, {  0,  0,128,103}, {165,250-22}}, //70 scream 14
  {BlackRun_ArcMain_Scream5, {128,  0,127,104}, {164,251-22}}, //71 scream 15

  {BlackRun_ArcMain_IdleA0, {  0,  0,135, 89}, {178-12,147+63}}, //72 idlea 1
  {BlackRun_ArcMain_IdleA0, {  0, 89,133, 90}, {178-12,147+63}}, //73 idlea 2
  {BlackRun_ArcMain_IdleA1, {  0,  0,130, 87}, {177-12,148+63}}, //74 idlea 3
  {BlackRun_ArcMain_IdleA1, {130,  0,122, 81}, {175-12,150+63}}, //75 idlea 4
  {BlackRun_ArcMain_IdleA1, {  0, 87,115, 89}, {169-12,150+63}}, //76 idlea 5
  {BlackRun_ArcMain_IdleA1, {115, 87, 98, 92}, {159-12,149+63}}, //77 idlea 6
  {BlackRun_ArcMain_IdleA2, {  0,  0, 88, 83}, {159-12,147+63}}, //78 idlea 7
  {BlackRun_ArcMain_IdleA2, { 88,  0, 87, 83}, {159-12,147+63}}, //79 idlea 8
  {BlackRun_ArcMain_IdleA2, {  0, 83, 88, 83}, {160-12,148+63}}, //80 idlea 9
  {BlackRun_ArcMain_IdleA2, { 88, 83, 88, 82}, {160-12,147+63}}, //81 idlea 10
  {BlackRun_ArcMain_IdleA2, {  0,166, 96, 82}, {160-12,148+63}}, //82 idlea 11
  {BlackRun_ArcMain_IdleA2, { 96,166,104, 86}, {161-12,150+63}}, //83 idlea 12
  {BlackRun_ArcMain_IdleA3, {  0,  0,117, 88}, {161-12,150+63}}, //84 idlea 13
  {BlackRun_ArcMain_IdleA3, {117,  0,126, 85}, {165-12,149+63}}, //85 idlea 14
  {BlackRun_ArcMain_IdleA3, {  0, 88,136, 89}, {178-12,149+63}}, //86 idlea 15

  {BlackRun_ArcMain_LeftA0, {  0,  0,110, 90}, {174+1-12,145+63}}, //87 lefta 1
  {BlackRun_ArcMain_LeftA0, {110,  0,126, 85}, {178+1-12,141+63}}, //88 lefta 2
  {BlackRun_ArcMain_LeftA0, {  0, 90,133, 85}, {180+1-12,140+63}}, //89 lefta 3
  {BlackRun_ArcMain_LeftA1, {  0,  0,135, 86}, {180+1-12,141+63}}, //90 lefta 4
  {BlackRun_ArcMain_LeftA1, {  0, 86,136, 86}, {180+1-12,142+63}}, //91 lefta 5
  {BlackRun_ArcMain_LeftA1, {  0, 86,136, 86}, {180+1-12,142+63}}, //92 lefta 5

  {BlackRun_ArcMain_DownA0, {  0,  0,187, 94}, {194-2-12,127+3+63}}, //93 downa 1
  {BlackRun_ArcMain_DownA0, {  0, 94,169, 95}, {191-2-12,131+3+63}}, //94 downa 2
  {BlackRun_ArcMain_DownA1, {  0,  0,161, 91}, {189-2-12,132+3+63}}, //95 downa 3
  {BlackRun_ArcMain_DownA1, {  0, 91,161, 91}, {189-2-12,132+3+63}}, //96 downa 4
  {BlackRun_ArcMain_DownA2, {  0,  0,158, 90}, {189-2-12,133+3+63}}, //97 downa 5
  {BlackRun_ArcMain_DownA2, {  0, 90,157, 91}, {189-2-12,133+3+63}}, //98 downa 6
  {BlackRun_ArcMain_DownA3, {  0,  0,157, 90}, {189-2-12,133+3+63}}, //99 downa 7
  {BlackRun_ArcMain_DownA3, {  0, 90,157, 89}, {189-2-12,133+3+63}}, //100 downa 8
  {BlackRun_ArcMain_DownA3, {  0, 90,157, 89}, {189-2-12,133+3+63}}, //101 downa 8

  {BlackRun_ArcMain_UpA0, {  0,  0,114,102}, {181+1-12,168-3+63}}, //102 upa 1
  {BlackRun_ArcMain_UpA0, {114,  0,117, 92}, {180+1-12,162-3+63}}, //103 upa 2
  {BlackRun_ArcMain_UpA0, {  0,102,117, 92}, {180+1-12,162-3+63}}, //104 upa 3
  {BlackRun_ArcMain_UpA0, {117,102,116, 90}, {178+1-12,162-3+63}}, //105 upa 4
  {BlackRun_ArcMain_UpA1, {  0,  0,116, 90}, {178+1-12,162-3+63}}, //106 upa 5
  {BlackRun_ArcMain_UpA1, {116,  0,116, 90}, {178+1-12,161-3+63}}, //107 upa 6
  {BlackRun_ArcMain_UpA1, {  0, 90,115, 90}, {178+1-12,161-3+63}}, //108 upa 7
  {BlackRun_ArcMain_UpA1, {115, 90,114, 90}, {177+1-12,161-3+63}}, //109 upa 8
  {BlackRun_ArcMain_UpA1, {115, 90,114, 90}, {177+1-12,161-3+63}}, //110 upa 8

  {BlackRun_ArcMain_RightA0, {  0,  0,118,102}, {148+5-12,161-6+63}}, //111 righta 1
  {BlackRun_ArcMain_RightA0, {118,  0,107, 97}, {151+5-12,159-6+63}}, //112 righta 2
  {BlackRun_ArcMain_RightA0, {  0,102,106, 99}, {150+5-12,159-6+63}}, //113 righta 3
  {BlackRun_ArcMain_RightA0, {106,102,105,100}, {153+5-12,158-6+63}}, //114 righta 4
  {BlackRun_ArcMain_RightA1, {  0,  0,105, 94}, {154+5-12,158-6+63}}, //115 righta 5
  {BlackRun_ArcMain_RightA1, {105,  0,105, 92}, {154+5-12,157-6+63}}, //116 righta 6
  {BlackRun_ArcMain_RightA1, {  0, 94,107, 91}, {156+5-12,157-6+63}}, //117 righta 7
  {BlackRun_ArcMain_RightA1, {107, 94,106, 91}, {156+5-12,157-6+63}}, //118 righta 8
};

static const Animation char_blackrun_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{1, (const u8[]){ 15, 16, 17, 18, 19, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 20, 21, 22, 23, 24, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 25, 26, 27, 28, 29, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 30, 31, 32, 33, 34, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	
	{1, (const u8[]){ 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, ASCR_BACK, 1}},         //CharAnim_Special1
	{1, (const u8[]){ 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, ASCR_BACK, 1}},         //CharAnim_Special2
};

static const Animation char_blackrun_anim2[CharAnim_Max] = {
	{1, (const u8[]){ 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{1, (const u8[]){ 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 93, 94, 94, 95, 96, 97, 98, 99, 100, 101, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 102, 103, 104, 105, 106, 107, 108, 109, 109, 110, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 111, 112, 113, 114, 115, 116, 116, 117, 117, 118, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	
	{1, (const u8[]){ 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, ASCR_BACK, 1}},         //CharAnim_Special1
	{1, (const u8[]){ 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, ASCR_BACK, 1}},         //CharAnim_Special2
};

//BlackRun character functions
void Char_BlackRun_SetFrame(void *user, u8 frame)
{
	Char_BlackRun *this = (Char_BlackRun*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_blackrun_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BlackRun_Tick(Character *character)
{
	Char_BlackRun *this = (Char_BlackRun*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_Danger)
	{
		if (stage.song_step == 1)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(407,1024);
		}
		if (stage.song_beat == 64)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(543,1024);
		}
		if (stage.song_beat == 96)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(814,1024);
		}
		if (stage.song_beat == 128)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(543,1024);
		}
		if (stage.song_beat == 155)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(924,1024);
		}
		if (stage.song_beat == 160)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(543,1024);
		}
		if (stage.song_beat == 192)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(814,1024);
		}
		if (stage.song_beat == 256)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(543,1024);
		}
		if (stage.song_beat == 288)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(814,1024);
		}
		if (stage.song_beat == 320)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(407,1024);
		}
		if (stage.song_beat == 384)
		{
			this->character.focus_x = FIXED_DEC(-226,1);
			this->character.focus_y = FIXED_DEC(-192,1);
			this->character.focus_zoom = FIXED_DEC(814,1024);
		}
	}
	
	//Perform idle dance
	if ((character->animatable.anim != CharAnim_Special1) && (character->animatable.anim != CharAnim_Special2))
	{
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	}
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_Danger: //AAAAAAAAAAAAAAA
				if (stage.song_step == 624)
					character->set_anim(character, CharAnim_Special1);
				if (stage.song_step == 632)
					character->set_anim(character, CharAnim_Special2);
				break;
			default:
				break;
		}
	
	//switch character
	if (stage.song_step == 639)
		Animatable_Init(&this->character.animatable, char_blackrun_anim2);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_BlackRun_SetFrame);
	Character_Draw(character, &this->tex, &char_blackrun_frame[this->frame]);
}

void Char_BlackRun_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BlackRun_Free(Character *character)
{
	Char_BlackRun *this = (Char_BlackRun*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BlackRun_New(fixed_t x, fixed_t y)
{
	//Allocate blackrun object
	Char_BlackRun *this = Mem_Alloc(sizeof(Char_BlackRun));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BlackRun_New] Failed to allocate blackrun object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BlackRun_Tick;
	this->character.set_anim = Char_BlackRun_SetAnim;
	this->character.free = Char_BlackRun_Free;
	
	Animatable_Init(&this->character.animatable, char_blackrun_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 3;

	//health bar color
	this->character.health_bar = 0xFF3B1A51;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(924,1024);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(21671,10000);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\BLACKRUN.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "down1.tim",
  "up0.tim",
  "right0.tim",
  "right1.tim",
  "scream0.tim",
  "scream1.tim",
  "scream2.tim",
  "scream3.tim",
  "scream4.tim",
  "scream5.tim",
  "idlea0.tim",
  "idlea1.tim",
  "idlea2.tim",
  "idlea3.tim",
  "lefta0.tim",
  "lefta1.tim",
  "downa0.tim",
  "downa1.tim",
  "downa2.tim",
  "downa3.tim",
  "upa0.tim",
  "upa1.tim",
  "righta0.tim",
  "righta1.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
