/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "greenst.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Green ST character structure
enum
{
  GreenST_ArcMain_Idle0,
  GreenST_ArcMain_Idle1,
  GreenST_ArcMain_Left0,
  GreenST_ArcMain_Left1,
  GreenST_ArcMain_Left2,
  GreenST_ArcMain_Down0,
  GreenST_ArcMain_Down1,
  GreenST_ArcMain_Up0,
  GreenST_ArcMain_Up1,
  GreenST_ArcMain_Up2,
  GreenST_ArcMain_Up3,
  GreenST_ArcMain_Right0,
  GreenST_ArcMain_Right1,
  GreenST_ArcMain_StimL0,
  GreenST_ArcMain_StimL1,
  GreenST_ArcMain_StimR0,
  GreenST_ArcMain_StimR1,
	
	GreenST_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GreenST_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_GreenST;

//Green ST character definitions
static const CharFrame char_greenst_frame[] = {
  {GreenST_ArcMain_Idle0, {  0,  0,118, 91}, {130, 91}}, //0 idle 1
  {GreenST_ArcMain_Idle0, {119,  0,119, 92}, {130, 92}}, //1 idle 2
  {GreenST_ArcMain_Idle0, {  0, 93,111, 95}, {129, 95}}, //2 idle 3
  {GreenST_ArcMain_Idle0, {112, 93,108, 95}, {130, 95}}, //3 idle 4
  {GreenST_ArcMain_Idle1, {  0,  0,108, 95}, {130, 95}}, //4 idle 5
  {GreenST_ArcMain_Idle1, {109,  0,108, 95}, {130, 95}}, //5 idle 6

  {GreenST_ArcMain_Left0, {  0,  0,164, 95}, {190, 96}}, //6 left 1
  {GreenST_ArcMain_Left0, {  0, 96,165, 96}, {188, 96}}, //7 left 2
  {GreenST_ArcMain_Left1, {  0,  0,165, 96}, {188, 96}}, //8 left 3
  {GreenST_ArcMain_Left1, {  0, 97,165, 96}, {188, 96}}, //9 left 4
  {GreenST_ArcMain_Left2, {  0,  0,166, 96}, {189, 96}}, //10 left 5

  {GreenST_ArcMain_Down0, {  0,  0,155, 99}, {161, 89}}, //11 down 1
  {GreenST_ArcMain_Down0, {  0,100,155, 97}, {161, 90}}, //12 down 2
  {GreenST_ArcMain_Down1, {  0,  0,155, 97}, {161, 90}}, //13 down 3
  {GreenST_ArcMain_Down1, {  0, 98,155, 97}, {161, 90}}, //14 down 4

  {GreenST_ArcMain_Up0, {  0,  0,189,125}, {163,126}}, //15 up 1
  {GreenST_ArcMain_Up0, {  0,126,201,120}, {165,121}}, //16 up 2
  {GreenST_ArcMain_Up1, {  0,  0,204,121}, {165,122}}, //17 up 3
  {GreenST_ArcMain_Up1, {  0,122,205,121}, {165,122}}, //18 up 4
  {GreenST_ArcMain_Up2, {  0,  0,204,120}, {165,121}}, //19 up 5
  {GreenST_ArcMain_Up2, {  0,121,204,120}, {165,121}}, //20 up 6
  {GreenST_ArcMain_Up3, {  0,  0,158,113}, {165,114}}, //21 up 7
  {GreenST_ArcMain_Up3, {  0,114,158,113}, {165,114}}, //22 up 8

  {GreenST_ArcMain_Right0, {  0,  0,144, 93}, {126, 93}}, //23 right 1
  {GreenST_ArcMain_Right0, {  0, 94,142, 93}, {128, 93}}, //24 right 2
  {GreenST_ArcMain_Right1, {  0,  0,142, 93}, {128, 93}}, //25 right 3
  {GreenST_ArcMain_Right1, {  0, 94,141, 93}, {128, 93}}, //26 right 4

  {GreenST_ArcMain_StimL0, {  0,  0,108, 99}, {130,100}}, //27 stiml 1
  {GreenST_ArcMain_StimL0, {109,  0,107, 99}, {131,100}}, //28 stiml 2
  {GreenST_ArcMain_StimL0, {  0,100,107, 99}, {130, 99}}, //29 stiml 3
  {GreenST_ArcMain_StimL0, {108,100,103, 93}, {130, 94}}, //30 stiml 4
  {GreenST_ArcMain_StimL1, {  0,  0,102, 94}, {130, 95}}, //31 stiml 5
  {GreenST_ArcMain_StimL1, {103,  0,102, 94}, {129, 95}}, //32 stiml 6

  {GreenST_ArcMain_StimR0, {  0,  0, 90, 99}, {124,100}}, //33 stimr 1
  {GreenST_ArcMain_StimR0, { 91,  0, 92, 99}, {125,100}}, //34 stimr 2
  {GreenST_ArcMain_StimR0, {  0,100, 93, 98}, {125, 99}}, //35 stimr 3
  {GreenST_ArcMain_StimR0, { 94,100, 98, 93}, {125, 94}}, //36 stimr 4
  {GreenST_ArcMain_StimR1, {  0,  0,103, 94}, {130, 95}}, //37 stimr 5
  {GreenST_ArcMain_StimR1, {104,  0,103, 94}, {130, 95}}, //38 stimr 6
};

static const Animation char_greenst_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, ASCR_BACK, 1}}, //CharAnim_Idle
	{1, (const u8[]){ 6, 6, 7, 7, 8, 8, 9, 9, 10, ASCR_BACK, 1}},         //CharAnim_Left
	{1, (const u8[]){ 27, 27, 28, 28, 29, 30, 30, 31, 31, 32, 32, ASCR_BACK, 1}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 11, 12, 13, 14, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 15, 15, 16, 17, 18, 19, 20, 21, 22, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 23, 24, 25, 26, ASCR_BACK, 1}},         //CharAnim_Right
	{1, (const u8[]){ 33, 33, 34, 34, 35, 36, 36, 37, 37, 38, 38, ASCR_BACK, 1}},   //CharAnim_RightAlt
};

//Green ST character functions
void Char_GreenST_SetFrame(void *user, u8 frame)
{
	Char_GreenST *this = (Char_GreenST*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_greenst_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GreenST_Tick(Character *character)
{
	Char_GreenST *this = (Char_GreenST*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GreenST_SetFrame);
	Character_Draw(character, &this->tex, &char_greenst_frame[this->frame]);
}

void Char_GreenST_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_GreenST_Free(Character *character)
{
	Char_GreenST *this = (Char_GreenST*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GreenST_New(fixed_t x, fixed_t y)
{
	//Allocate greenst object
	Char_GreenST *this = Mem_Alloc(sizeof(Char_GreenST));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GreenST_New] Failed to allocate greenst object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GreenST_Tick;
	this->character.set_anim = Char_GreenST_SetAnim;
	this->character.free = Char_GreenST_Free;
	
	Animatable_Init(&this->character.animatable, char_greenst_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	this->character.idle2 = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFFAD63D6;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GREENST.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "left0.tim",
  "left1.tim",
  "left2.tim",
  "down0.tim",
  "down1.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "up3.tim",
  "right0.tim",
  "right1.tim",
  "stiml0.tim",
  "stiml1.tim",
  "stimr0.tim",
  "stimr1.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
