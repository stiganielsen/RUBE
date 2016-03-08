
const int shortLength = 150;
const int longLength = 150;

#define  c4     261
#define  d4     294
#define  e4     329
#define  f4     349
#define  g4     392
#define  a4     440
#define  b4     493

#define  c5     523
#define  d5     587
#define  e5     659
#define  f5     698
#define  g5     783
#define  a5     880
#define  b5     987

#define  c6     1046
#define  d6     1174
#define  e6     1318
#define  f6     1396
#define  g6     1567
#define  a6     1760
#define  b6     1975

void playSound(int freq, int duration){
	tone(speakerPin,freq,duration);
	delay(duration);
}

void soundStart(void){

	playSound(c5, shortLength);
	playSound(d5, shortLength);
	playSound(e5, shortLength);
	playSound(f5, shortLength);
	playSound(g5, shortLength);
}

void soundFinished(void){
	playSound(c5, shortLength);
	playSound(d5, shortLength);
	playSound(e5, shortLength);
	playSound(f5, shortLength);
	playSound(g5, shortLength);
}

void soundError(void){
	tone(speakerPin,1000,100);
	delay(150);
	tone(speakerPin,1000,500);
	delay(500);
}
