#ifndef _SPRITE_H
#define _SPRITE_H 1

#include <allegro.h>

class sprite {
private:
public:
    int alive;
	int state;
	int objecttype;
	int direction;
    double x,y;
    double pointer_x, pointer_y;
    int width,height;
    double speed;
    double velx, vely;
    int xdelay,ydelay;
    int xcount,ycount;
    int curframe,totalframes,animdir;
    int framecount,framedelay;
	int animcolumns;
	int animstartx, animstarty;
	int faceAngle, moveAngle;
	int health;
	BITMAP *image;
	BITMAP *image2;
	BITMAP *image3;

public:
	sprite();
	~sprite();
	int load(BITMAP * bitmap_image);
	int load2(BITMAP * bitmap_image);
	int load3(BITMAP * bitmap_image);
	void draw(BITMAP *dest);
	void drawframe(BITMAP *dest);
	void updatePosition();
	void updateAnimation();
	int inside(int x,int y,int left,int top,int right,int bottom);
	int pointInside(int px,int py);
	int collided(sprite *other = NULL, int shrink = 5);
};

#endif

