//預設值
int melody_default[] = {NOTE_C7,NOTE_, NOTE_C7};
int melody_wrong[] = {NOTE_C7,NOTE_B7, NOTE_C7};
int noteDurations_default[] = {1,1,1};
void play(int *melody, int *noteDurations_default, int num)
{
for(int note = 0; note < num; note++)
{
int noteDuration = 200 / noteDurations_default[note];
tone(8, melody[note], noteDuration);
delay(noteDuration);
}
}

//2書儀
int melody_2[] = {NOTE_C6,NOTE_D7,NOTE_E7,NOTE_F7,NOTE_G7,NOTE_A7,NOTE_B7,NOTE_C8,NOTE_C6,NOTE_C8};
int noteDurations_2[] = {1,1,1,1,1,1,1,1,1,1};
void play2(int *melody, int *noteDurations_default, int num)
{
for(int note = 0; note < num; note++)
{
int noteDuration = 450 / noteDurations_default[note];
tone(8, melody[note], noteDuration);
delay(noteDuration);
}
}

//10易庭
int melody_10[] = {NOTE_G7,NOTE_C6,NOTE_A7,NOTE_B7,NOTE_C8,NOTE_D7,NOTE_C8};
int noteDurations_10[] = {1,1,1,1,1,1,1};
void play10(int *melody, int *noteDurations_default, int num)
{
for(int note = 0; note < num; note++)
{
int noteDuration = 320 / noteDurations_default[note];
tone(8, melody[note], noteDuration);
delay(noteDuration);
}
}

//13矮矮
int melody_13[] = {NOTE_C7,NOTE_C7,NOTE_G7,NOTE_G7,NOTE_A7,NOTE_A7,NOTE_G7};
int noteDurations_13[] = {1,1,1,1,1,1,1};
void play13(int *melody, int *noteDurations_default, int num)
{
for(int note = 0; note < num; note++)
{
int noteDuration = 320 / noteDurations_default[note];
tone(8, melody[note], noteDuration);
delay(noteDuration);
}
}
//17小方
int melody_17[] = {NOTE_G7,NOTE_A7,NOTE_G7,NOTE_A7,NOTE_G7,NOTE_A7,NOTE_C8};
int noteDurations_17[] = {1,1,1,1,1,1,1};
void play17(int *melody, int *noteDurations_default, int num)
{
for(int note = 0; note < num; note++)
{
int noteDuration = 320 / noteDurations_default[note];
tone(8, melody[note], noteDuration);
delay(noteDuration);
}
}

//警告音
int melody_warning[] = {NOTE_C7,NOTE_F7,NOTE_C7,NOTE_F7,NOTE_C7,NOTE_F7,NOTE_C7,NOTE_F7,NOTE_C7,NOTE_F7,NOTE_C7,NOTE_F7,NOTE_C7,NOTE_F7,NOTE_C7,NOTE_F7,NOTE_C7,NOTE_F7,NOTE_C7,NOTE_F7};
int noteDurations_warning[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
void play_warning(int *melody, int *noteDurations_default, int num)
{
for(int note = 0; note < num; note++)
{
int noteDuration = 1000 / noteDurations_default[note];
tone(8, melody[note], noteDuration);
delay(noteDuration);
}
}
