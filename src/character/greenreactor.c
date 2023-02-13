/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "greenreactor.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Green Reactor character structure
enum
{
  GreenReactor_ArcMain_Idle0,
  GreenReactor_ArcMain_Idle1,
  GreenReactor_ArcMain_Idle2,
  GreenReactor_ArcMain_Left0,
  GreenReactor_ArcMain_Left1,
  GreenReactor_ArcMain_Down0,
  GreenReactor_ArcMain_Down1,
  GreenReactor_ArcMain_Down2,
  GreenReactor_ArcMain_Up0,
  GreenReactor_ArcMain_Up1,
  GreenReactor_ArcMain_Up2,
  GreenReactor_ArcMain_Up3,
  GreenReactor_ArcMain_Up4,
  GreenReactor_ArcMain_Up5,
  GreenReactor_ArcMain_Up6,
  GreenReactor_ArcMain_Right0,
  GreenReactor_ArcMain_Right1,
	
	GreenReactor_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GreenReactor_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_GreenReactor;

//Green Reactor character definitions
static const CharFrame char_greenreactor_frame[] = {
  {GreenReactor_ArcMain_Idle0, {  0,  0,144, 95}, {172, 86}}, //0 idle 1
  {GreenReactor_ArcMain_Idle0, {  0, 95,143, 96}, {172, 88}}, //1 idle 2
  {GreenReactor_ArcMain_Idle1, {  0,  0,143, 96}, {173, 90}}, //2 idle 3
  {GreenReactor_ArcMain_Idle1, {  0, 96,143, 97}, {174, 91}}, //3 idle 4
  {GreenReactor_ArcMain_Idle2, {  0,  0,143, 98}, {174, 91}}, //4 idle 5

  {GreenReactor_ArcMain_Left0, {  0,  0,181, 95}, {219, 88}}, //5 left 1
  {GreenReactor_ArcMain_Left0, {  0, 95,182, 96}, {220, 88}}, //6 left 2
  {GreenReactor_ArcMain_Left1, {  0,  0,183, 96}, {221, 88}}, //7 left 3
  {GreenReactor_ArcMain_Left1, {  0, 96,183, 95}, {221, 87}}, //8 left 4

  {GreenReactor_ArcMain_Down0, {  0,  0,159,102}, {186, 80}}, //9 down 1
  {GreenReactor_ArcMain_Down0, {  0,102,158,100}, {185, 80}}, //10 down 2
  {GreenReactor_ArcMain_Down1, {  0,  0,157,106}, {185, 85}}, //11 down 3
  {GreenReactor_ArcMain_Down1, {  0,106,157,105}, {185, 85}}, //12 down 4
  {GreenReactor_ArcMain_Down2, {  0,  0,159,106}, {185, 85}}, //13 down 5
  {GreenReactor_ArcMain_Down2, {  0,106,158,106}, {185, 85}}, //14 down 6

  {GreenReactor_ArcMain_Up0, {  0,  0,150,110}, {179,100}}, //15 up 1
  {GreenReactor_ArcMain_Up0, {  0,110,150,101}, {178, 96}}, //16 up 2
  {GreenReactor_ArcMain_Up1, {  0,  0,147,101}, {170, 96}}, //17 up 3
  {GreenReactor_ArcMain_Up1, {  0,101,137,103}, {161, 98}}, //18 up 4
  {GreenReactor_ArcMain_Up2, {  0,  0,154,111}, {178,105}}, //19 up 5
  {GreenReactor_ArcMain_Up2, {  0,111,158,115}, {182,110}}, //20 up 6
  {GreenReactor_ArcMain_Up3, {  0,  0,157,126}, {181,120}}, //21 up 7
  {GreenReactor_ArcMain_Up3, {  0,126,163,119}, {187,113}}, //22 up 8
  {GreenReactor_ArcMain_Up4, {  0,  0,170,103}, {194, 98}}, //23 up 9
  {GreenReactor_ArcMain_Up4, {  0,103,169,103}, {193, 97}}, //24 up 10
  {GreenReactor_ArcMain_Up5, {  0,  0,175,102}, {199, 97}}, //25 up 11
  {GreenReactor_ArcMain_Up5, {  0,102,175,102}, {200, 97}}, //26 up 12
  {GreenReactor_ArcMain_Up6, {  0,  0,174,104}, {198, 98}}, //27 up 13
  {GreenReactor_ArcMain_Up6, {  0,104,172,104}, {195, 98}}, //28 up 14

  {GreenReactor_ArcMain_Right0, {  0,  0,162, 97}, {173, 87}}, //29 right 1
  {GreenReactor_ArcMain_Right0, {  0, 97,134, 92}, {136, 86}}, //30 right 2
  {GreenReactor_ArcMain_Right1, {  0,  0,133, 93}, {135, 87}}, //31 right 3
  {GreenReactor_ArcMain_Right1, {  0, 93,134, 94}, {136, 87}}, //32 right 4
};

static const Animation char_greenreactor_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, 7, 8, 8, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{1, (const u8[]){ 9, 10, 11, 12, 13, 13, 14, 14, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{1, (const u8[]){ 15, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 29, 30, 31, 32, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Green Reactor character functions
void Char_GreenReactor_SetFrame(void *user, u8 frame)
{
	Char_GreenReactor *this = (Char_GreenReactor*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_greenreactor_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GreenReactor_Tick(Character *character)
{
	Char_GreenReactor *this = (Char_GreenReactor*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GreenReactor_SetFrame);
	Character_Draw(character, &this->tex, &char_greenreactor_frame[this->frame]);
}

void Char_GreenReactor_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_GreenReactor_Free(Character *character)
{
	Char_GreenReactor *this = (Char_GreenReactor*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GreenReactor_New(fixed_t x, fixed_t y)
{
	//Allocate greenreactor object
	Char_GreenReactor *this = Mem_Alloc(sizeof(Char_GreenReactor));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GreenReactor_New] Failed to allocate greenreactor object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GreenReactor_Tick;
	this->character.set_anim = Char_GreenReactor_SetAnim;
	this->character.free = Char_GreenReactor_Free;
	
	Animatable_Init(&this->character.animatable, char_greenreactor_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFF1F6B2B;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GREENREC.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "left0.tim",
  "left1.tim",
  "down0.tim",
  "down1.tim",
  "down2.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "up3.tim",
  "up4.tim",
  "up5.tim",
  "up6.tim",
  "right0.tim",
  "right1.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
