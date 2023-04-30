/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "blackdk.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//BlackDK character structure
enum
{
  BlackDK_ArcMain_Idle0,
  BlackDK_ArcMain_Idle1,
  BlackDK_ArcMain_Idle2,
  BlackDK_ArcMain_Idle3,
  BlackDK_ArcMain_Idle4,
  BlackDK_ArcMain_Idle5,
  BlackDK_ArcMain_Idle6,
  BlackDK_ArcMain_Idle7,
  BlackDK_ArcMain_Idle8,
  BlackDK_ArcMain_Idle9,
  BlackDK_ArcMain_Idle10,
  BlackDK_ArcMain_Left0,
  BlackDK_ArcMain_Left1,
  BlackDK_ArcMain_Left2,
  BlackDK_ArcMain_Down0,
  BlackDK_ArcMain_Down1,
  BlackDK_ArcMain_Down2,
  BlackDK_ArcMain_Up0,
  BlackDK_ArcMain_Up1,
  BlackDK_ArcMain_Up2,
  BlackDK_ArcMain_Right0,
  BlackDK_ArcMain_Right1,
  BlackDK_ArcMain_Right2,
  BlackDK_ArcMain_Right3,
	
	BlackDK_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BlackDK_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_BlackDK;

//BlackDK character definitions
static const CharFrame char_blackdk_frame[] = {
  {BlackDK_ArcMain_Idle0, {  0,  0,170, 94}, {159,141}}, //0 idle 1
  {BlackDK_ArcMain_Idle0, {  0, 94,171, 93}, {158,140}}, //1 idle 2
  {BlackDK_ArcMain_Idle1, {  0,  0,171, 93}, {158,140}}, //2 idle 3
  {BlackDK_ArcMain_Idle1, {  0, 93,170, 93}, {157,140}}, //3 idle 4
  {BlackDK_ArcMain_Idle2, {  0,  0,170, 93}, {157,140}}, //4 idle 5
  {BlackDK_ArcMain_Idle2, {  0, 93,167, 94}, {154,141}}, //5 idle 6
  {BlackDK_ArcMain_Idle3, {  0,  0,165, 95}, {152,142}}, //6 idle 7
  {BlackDK_ArcMain_Idle3, {  0, 95,166, 95}, {152,142}}, //7 idle 8
  {BlackDK_ArcMain_Idle4, {  0,  0,164, 98}, {150,145}}, //8 idle 9
  {BlackDK_ArcMain_Idle4, {  0, 98,165, 98}, {151,145}}, //9 idle 10
  {BlackDK_ArcMain_Idle5, {  0,  0,164, 98}, {149,145}}, //10 idle 11
  {BlackDK_ArcMain_Idle5, {  0, 98,164, 99}, {150,146}}, //11 idle 12
  {BlackDK_ArcMain_Idle6, {  0,  0,164, 99}, {150,146}}, //12 idle 13
  {BlackDK_ArcMain_Idle6, {  0, 99,165, 99}, {152,146}}, //13 idle 14
  {BlackDK_ArcMain_Idle7, {  0,  0,165, 99}, {151,146}}, //14 idle 15
  {BlackDK_ArcMain_Idle7, {  0, 99,166, 99}, {153,146}}, //15 idle 16
  {BlackDK_ArcMain_Idle8, {  0,  0,167, 99}, {154,146}}, //16 idle 17
  {BlackDK_ArcMain_Idle8, {  0, 99,167, 99}, {154,146}}, //17 idle 18
  {BlackDK_ArcMain_Idle9, {  0,  0,166, 98}, {155,145}}, //18 idle 19
  {BlackDK_ArcMain_Idle9, {  0, 98,166, 98}, {155,145}}, //19 idle 20
  {BlackDK_ArcMain_Idle10, {  0,  0,168, 96}, {157,144}}, //20 idle 21

  {BlackDK_ArcMain_Left0, {  0,  0,104,107}, {167,154}}, //21 left 1
  {BlackDK_ArcMain_Left0, {104,  0,103,104}, {165,151}}, //22 left 2
  {BlackDK_ArcMain_Left0, {  0,107,103,104}, {165,151}}, //23 left 3
  {BlackDK_ArcMain_Left0, {103,107,100,108}, {161,155}}, //24 left 4
  {BlackDK_ArcMain_Left1, {  0,  0,100,108}, {162,155}}, //25 left 5
  {BlackDK_ArcMain_Left1, {100,  0, 94,109}, {156,156}}, //26 left 6
  {BlackDK_ArcMain_Left1, {  0,109, 98,112}, {150,159}}, //27 left 7
  {BlackDK_ArcMain_Left1, { 98,109, 98,112}, {151,159}}, //28 left 8
  {BlackDK_ArcMain_Left2, {  0,  0, 99,112}, {150,159}}, //29 left 9
  {BlackDK_ArcMain_Left2, { 99,  0,100,112}, {150,159}}, //30 left 10
  {BlackDK_ArcMain_Left2, {  0,112,100,112}, {151,159}}, //31 left 11

  {BlackDK_ArcMain_Down0, {  0,  0,190, 99}, {170,135}}, //32 down 1
  {BlackDK_ArcMain_Down0, {  0, 99,180, 82}, {169,122}}, //33 down 2
  {BlackDK_ArcMain_Down1, {  0,  0,170, 82}, {166,126}}, //34 down 3
  {BlackDK_ArcMain_Down1, {  0, 82,164, 83}, {163,128}}, //35 down 4
  {BlackDK_ArcMain_Down1, {  0,165,161, 84}, {162,129}}, //36 down 5
  {BlackDK_ArcMain_Down2, {  0,  0,160, 83}, {161,129}}, //37 down 6

  {BlackDK_ArcMain_Up0, {  0,  0,137, 98}, {143,146}}, //38 up 1
  {BlackDK_ArcMain_Up0, {  0, 98,128, 96}, {141,144}}, //39 up 2
  {BlackDK_ArcMain_Up1, {  0,  0,128, 97}, {141,145}}, //40 up 3
  {BlackDK_ArcMain_Up1, {128,  0,121, 92}, {139,140}}, //41 up 4
  {BlackDK_ArcMain_Up1, {  0, 97,121, 94}, {139,142}}, //42 up 5
  {BlackDK_ArcMain_Up1, {121, 97,119, 94}, {138,142}}, //43 up 6
  {BlackDK_ArcMain_Up2, {  0,  0,118, 92}, {138,141}}, //44 up 7
  {BlackDK_ArcMain_Up2, {118,  0,117, 93}, {137,142}}, //45 up 8
  {BlackDK_ArcMain_Up2, {  0, 93,117, 94}, {137,143}}, //46 up 9
  {BlackDK_ArcMain_Up2, {117, 93,116, 93}, {137,142}}, //47 up 10

  {BlackDK_ArcMain_Right0, {  0,  0,171,101}, {115,148}}, //48 right 1
  {BlackDK_ArcMain_Right0, {  0,101,144,101}, {118,148}}, //49 right 2
  {BlackDK_ArcMain_Right1, {  0,  0,144,101}, {119,148}}, //50 right 3
  {BlackDK_ArcMain_Right1, {  0,101,134,100}, {119,147}}, //51 right 4
  {BlackDK_ArcMain_Right2, {  0,  0,132,100}, {120,147}}, //52 right 5
  {BlackDK_ArcMain_Right2, {  0,100,133,100}, {120,147}}, //53 right 6
  {BlackDK_ArcMain_Right3, {  0,  0,131,100}, {120,146}}, //54 right 7
  {BlackDK_ArcMain_Right3, {  0,100,131,100}, {120,146}}, //55 right 8
};

static const Animation char_blackdk_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 0, 1, 2, 3, 4, 5, 5, 6, 7, 8, 9, 10, 10, 11, 12, 13, 14, 15, 15, 16, 17, 18, 19, 20, 20, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{1, (const u8[]){ 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{1, (const u8[]){ 48, 49, 50, 51, 51, 52, 53, 54, 55, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//BlackDK character functions
void Char_BlackDK_SetFrame(void *user, u8 frame)
{
	Char_BlackDK *this = (Char_BlackDK*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_blackdk_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BlackDK_Tick(Character *character)
{
	Char_BlackDK *this = (Char_BlackDK*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_BlackDK_SetFrame);
	if (stage.song_step >= 3408)
		Character_DrawCol(character, &this->tex, &char_blackdk_frame[this->frame], 200, 128, 128);
	else
		Character_DrawCol(character, &this->tex, &char_blackdk_frame[this->frame], 150, 150, 150);
}

void Char_BlackDK_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BlackDK_Free(Character *character)
{
	Char_BlackDK *this = (Char_BlackDK*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BlackDK_New(fixed_t x, fixed_t y)
{
	//Allocate blackdk object
	Char_BlackDK *this = Mem_Alloc(sizeof(Char_BlackDK));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BlackDK_New] Failed to allocate blackdk object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BlackDK_Tick;
	this->character.set_anim = Char_BlackDK_SetAnim;
	this->character.free = Char_BlackDK_Free;
	
	Animatable_Init(&this->character.animatable, char_blackdk_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 3;

	//health bar color
	this->character.health_bar = 0xFF3B1A51;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(166,100);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\BLACKDK.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "idle3.tim",
  "idle4.tim",
  "idle5.tim",
  "idle6.tim",
  "idle7.tim",
  "idle8.tim",
  "idle9.tim",
  "idle10.tim",
  "left0.tim",
  "left1.tim",
  "left2.tim",
  "down0.tim",
  "down1.tim",
  "down2.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
  "right3.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
