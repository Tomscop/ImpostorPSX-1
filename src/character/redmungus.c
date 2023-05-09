/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "redmungus.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Redmungus character structure
enum
{
  Redmungus_ArcMain_Idle0,
  Redmungus_ArcMain_Left0,
  Redmungus_ArcMain_Down0,
  Redmungus_ArcMain_Up0,
  Redmungus_ArcMain_Right0,
	
	Redmungus_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Redmungus_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Redmungus;

//Redmungus character definitions
static const CharFrame char_redmungus_frame[] = {
  {Redmungus_ArcMain_Idle0, {  0,  0, 70, 86}, {172,169}}, //0 idle 1
  {Redmungus_ArcMain_Idle0, { 71,  0, 70, 89}, {172,171}}, //1 idle 2
  {Redmungus_ArcMain_Idle0, {142,  0, 68, 86}, {171,172}}, //2 idle 3
  {Redmungus_ArcMain_Idle0, {  0, 90, 67, 84}, {171,172}}, //3 idle 4

  {Redmungus_ArcMain_Left0, {  0,  0,130, 79}, {203-1,170}}, //4 left 1
  {Redmungus_ArcMain_Left0, {  0, 80,128, 79}, {203-1,171}}, //5 left 2
  {Redmungus_ArcMain_Left0, {129, 80,106, 79}, {197-1,172}}, //6 left 3
  {Redmungus_ArcMain_Left0, {  0,160,104, 80}, {196-1,172}}, //7 left 4

  {Redmungus_ArcMain_Down0, {  0,  0, 89, 80}, {178+6,161}}, //8 down 1
  {Redmungus_ArcMain_Down0, { 90,  0, 89, 79}, {178+6,161}}, //9 down 2
  {Redmungus_ArcMain_Down0, {  0, 81, 88, 80}, {178+6,164}}, //10 down 3
  {Redmungus_ArcMain_Down0, { 89, 81, 87, 81}, {178+6,165}}, //11 down 4

  {Redmungus_ArcMain_Up0, {  0,  0,116, 84}, {200-6,177}}, //12 up 1
  {Redmungus_ArcMain_Up0, {117,  0,117, 84}, {200-6,177}}, //13 up 2
  {Redmungus_ArcMain_Up0, {  0, 85,123, 82}, {202-6,175}}, //14 up 3
  {Redmungus_ArcMain_Up0, {124, 85,124, 81}, {202-6,174}}, //15 up 4

  {Redmungus_ArcMain_Right0, {  0,  0, 84, 82}, {158+48,173}}, //16 right 1
  {Redmungus_ArcMain_Right0, { 85,  0, 83, 81}, {158+48,173}}, //17 right 2
  {Redmungus_ArcMain_Right0, {169,  0, 86, 80}, {156+48,173}}, //18 right 3
  {Redmungus_ArcMain_Right0, {  0, 83, 86, 80}, {155+48,173}}, //19 right 4
};

static const Animation char_redmungus_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 3, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4, 5, 6, 7, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 8, 9, 10, 11, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 12, 13, 14, 15, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 16, 17, 18, 19, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Redmungus character functions
void Char_Redmungus_SetFrame(void *user, u8 frame)
{
	Char_Redmungus *this = (Char_Redmungus*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_redmungus_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Redmungus_Tick(Character *character)
{
	Char_Redmungus *this = (Char_Redmungus*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Redmungus_SetFrame);
	if (stage.song_step >= 409)
		Character_DrawFlipped(character, &this->tex, &char_redmungus_frame[this->frame]);
}

void Char_Redmungus_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Redmungus_Free(Character *character)
{
	Char_Redmungus *this = (Char_Redmungus*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Redmungus_New(fixed_t x, fixed_t y)
{
	//Allocate redmungus object
	Char_Redmungus *this = Mem_Alloc(sizeof(Char_Redmungus));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Redmungus_New] Failed to allocate redmungus object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Redmungus_Tick;
	this->character.set_anim = Char_Redmungus_SetAnim;
	this->character.free = Char_Redmungus_Free;
	
	Animatable_Init(&this->character.animatable, char_redmungus_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFFE51919;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\OPPONENT\\REDMUNGS.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "left0.tim",
  "down0.tim",
  "up0.tim",
  "right0.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
