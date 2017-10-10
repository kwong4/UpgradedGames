// Pocket Trivia
// Kevin Wong

#include "spritehandler.h"

#ifndef _GALACTICDEFENSE_H
#define _GALACTICDEFENSE_H

#define BLACK makecol(0,0,0)
#define WHITE makecol(255,255,255)
#define YELLOW makecol(255,216,0)
#define RED makecol(255,0,0)
#define ORANGE makecol(255,127.5,0)
#define BLUE makecol(0,191,255)

#define PI 3.1415926535
#define ACCELERATION 0.1f
#define STEP 3

#define NUM 20
#define WIDTH 800
#define HEIGHT 600
#define MODE GFX_AUTODETECT_WINDOWED
#define CHAR_PER_LENGTH 8
#define BULLET_CAP 15
#define BULLETSPEED 6
#define BULLETCOOLDOWN 10;
#define SMALLASTEROID_COUNT 7
#define LARGEASTEROID_COUNT 3
#define ASTEROID_COUNT SMALLASTEROID_COUNT + LARGEASTEROID_COUNT
#define ASTEROID_SPEED 
#define PULSE_DURATION 10
#define PULSE_COOLDOWN 100
#define DATA_FILE "data.dat"

// Gameover variable
int gameover = 0;
int bullet_index = 0;
int bullet_cooldown = 0;
int score = 0;
int hardmode = 0;
int selection = 0;
int max_selection = 2;
int active_pulse = 0;
int pulse_cooldown = 0;
int sound = 1;
int done = 0;

//create a back buffer
BITMAP *title;
BITMAP *buffer;
BITMAP *background;
BITMAP *explosion;
SAMPLE *background_music;
SAMPLE *click_sound;
SAMPLE *bullet_sound;
SAMPLE *pulse_sound;
sprite *pulse;
sprite *spaceship;
spritehandler *bullets;
spritehandler *asteroids;
DATAFILE *data;
pthread_mutex_t threadsafe;

// Print in correct format
void print_formated(const char* text, int x1, int x2, int y, int col, int bg);

// Print insturction screen
void instructions();

// Get Game menu input
int getmenuinput();

// Print the welcome screen menu
void welcome_screen();

// Draw initial start screen instructions
void draw_startscreen();

//calculate X movement value based on direction angle
double calcAngleMoveX(int angle);

//calculate Y movement value based on direction angle
double calcAngleMoveY(int angle);

// Warp sprite if reaches end of screen
void warpsprite(sprite *spr);

// Respawn asteroid if destoryed
void restart_asteroid(int num);

// Check collisions with other asteroids. Only in hard mode
void checkcollisions_asteroid(int num);

// Check if bullet collided with any asteroids
void checkcollisions_bullet(int num);

// Check collisions with ship
void checkcollisions_ship();

// Active pulse feature
void activate_pulse();

// Used to move ship forward or backwards
void thrusters(int dir);

// Turn ship left
void turnleft();

// Turn ship right
void turnright();

// Fire weapon
void fireweapon();

// Update bullet
void updatebullet(int num);

// Update asteroid
void updateasteroid(int num);

// Update health bar
void updatehealth();

// Update pulse cooldown bar
void updatepulse();

// Main update function
void update();

// Get game input from user
void getinput();

// Setup initial files
void setupscreen();

// Setup game
void setupgame();

#endif
