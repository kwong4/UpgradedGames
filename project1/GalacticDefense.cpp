// Author: Kevin Wong

#include <pthread.h>
#include <stdio.h>
#include <allegro.h>
#include "math.h"
#include "sprite.h"
#include "GalacticDefense.h"
#include "spritehandler.h"
#include "defines.h"

//create a new thread mutex to protect variables
pthread_mutex_t threadsafe = PTHREAD_MUTEX_INITIALIZER;

// Print in correct format
void print_formated(const char* text, int x1, int x2, int y, int col, int bg) {
	
	// Buffer and initial variables
	char line[256];
	int end_position = strcspn(text, "\0");
	int current_y = y;
	int length = x2 - x1;
	
	// Check if we need to format
	if ((end_position * CHAR_PER_LENGTH) > length) {
		
		// Inital variables
		int start = 0;
		int end;
		int curr_space = 0;
		int next_space = 0;
		
		// Cycle through spaces and print on next line if doesn't fit
		while(next_space < end_position - 1) {
			next_space += strcspn(&text[curr_space], " ") + 1;
			if (((next_space - start) *  CHAR_PER_LENGTH) < length) {
				curr_space = next_space;
			}
			else {
				strncpy(line, &text[start], curr_space - start);
				strncpy(&line[curr_space - 1 - start], "\0", 1);
				textprintf_ex(screen, font, x1, y, col, bg, "%s", line);
				start = curr_space;
				y += 10;
			}
		}
		strncpy(line, &text[start], end_position - start);
		strncpy(&line[end_position - start], "\0", 1);
		textprintf_ex(screen, font, x1, y, col, bg, "%s", line);
	}
	else {
		textprintf_ex(screen, font, x1, y, col, bg, "%s", text);
	}
}

// Print insturction screen
void instructions() {
	
	// Clear screen
	rectfill(screen, 0, 0, WIDTH, HEIGHT, BLACK);
	
	// Print game info
	textout_centre_ex(screen, font, "GAME INFO:", WIDTH/2, HEIGHT/4, WHITE, BLACK);
	textout_ex(screen, font, "- You are tasked with the defense of the Galaxy.", WIDTH/8, HEIGHT/4 + 20, WHITE, BLACK);
	textout_ex(screen, font, "- Your goal is to destory as much asteroids before your ship needs repairs", WIDTH/8, HEIGHT/4 + 30, WHITE, BLACK);
	textout_ex(screen, font, "- You can take a total of 3 hits before your ship needs repairs", WIDTH/8, HEIGHT/4 + 40, WHITE, BLACK);
	textout_ex(screen, font, "- Small Asteroids take 1 hit to destory. But Big Asteroids require 3 hits to destory.", WIDTH/8, HEIGHT/4 + 50, WHITE, BLACK);
	textout_ex(screen, font, "- Easy gamemode will allow asteroids to pass by each other in predictable movement", WIDTH/8, HEIGHT/4 + 70, WHITE, BLACK);
	textout_ex(screen, font, "- Hard gamemode will make asteroids have unpredictable movements due to magnetic", WIDTH/8, HEIGHT/4 + 80, WHITE, BLACK);
	textout_ex(screen, font, "    properties of each asteroid", WIDTH/8, HEIGHT/4 + 90, WHITE, BLACK);
	
	// Print game instructions
	textout_centre_ex(screen, font, "INSTRUCTIONS:", WIDTH/2, HEIGHT/2 + 40, WHITE, BLACK);
    textout_ex(screen, font, "1. Use the arrow keys to navigate your space ship", WIDTH/8, HEIGHT/2 + 60, WHITE, BLACK);
    textout_ex(screen, font, "   Use the UP and DOWN key to accelerate and decelerate", WIDTH/8, HEIGHT/2 + 70, YELLOW, BLACK);
    textout_ex(screen, font, "   Use the LEFT and RIGHT key to TURN", WIDTH/8, HEIGHT/2 + 80, YELLOW, BLACK);
    textout_ex(screen, font, "2. Use the SPACE bar key to shoot", WIDTH/8, HEIGHT/2 + 90, WHITE, BLACK);
    print_formated("3. Use the 'g' key to activate a pulse that pushes all asteroids within a small area away. Make sure not to activate before asteroid is too close.", WIDTH/8, WIDTH - 10, HEIGHT/2 + 100, WHITE, BLACK);
    textout_ex(screen, font, "4. Press Ctrl + h to bring up the instructions at any time!", WIDTH/8, HEIGHT/2 + 120, WHITE, BLACK);
    textout_ex(screen, font, "5. Press Ctrl + m to toggle the background music at any time!", WIDTH/8, HEIGHT/2 + 130, WHITE, BLACK);
    textout_ex(screen, font, "6. Press Esc to exit the game!", WIDTH/8, HEIGHT/2 + 140, WHITE, BLACK);
    
    // ENTER to return
    textout_centre_ex(screen, font, "Press ENTER to return", WIDTH/2, HEIGHT/2 + 200, WHITE, BLACK);
    
    // Slow down game
    rest(250);
    
    // Check input from user
    while(1) {
    	if (key[KEY_ENTER]) {
    		play_sample(click_sound, 128, 128, 1000, FALSE);
			break;
		}
		
		// Background music toggle
		if (key[KEY_LCONTROL] && key[KEY_M]) {
			if (sound == 1) {
				sound = 0;
				stop_sample(background_music);
			}
			else {
				sound = 1;
				play_sample(background_music, 128, 128, 1000, TRUE);
			}
			rest(80);
		}
		
		if (key[KEY_ESC]) {
			allegro_exit();
			exit(0);	
		}
    }
}

// Get Game menu input
int getmenuinput() {
	
	// If user quits game
	if (key[KEY_ESC]) {
		allegro_exit();
		exit(0);
	}
	
	// Move cursor for selection
	// Play sound for each movement
	if (key[KEY_DOWN] && selection != max_selection) {
		play_sample(click_sound, 128, 128, 1000, FALSE);
		rectfill(screen, WIDTH/3 + 1, HEIGHT/2 + selection * 15 + 81, WIDTH/3 + 9, HEIGHT/2 + selection * 15 + 89, BLACK);
		selection++;
		rectfill(screen, WIDTH/3 + 1, HEIGHT/2 +  selection * 15 + 81, WIDTH/3 + 9, HEIGHT/2 + selection * 15 + 89, WHITE);
	}
	else if (key[KEY_UP] && selection != 0) {
		play_sample(click_sound, 128, 128, 1000, FALSE);
		rectfill(screen, WIDTH/3 + 1, HEIGHT/2 + selection * 15 + 81, WIDTH/3 + 9, HEIGHT/2 +  selection * 15 + 89, BLACK);
		selection--;
		rectfill(screen, WIDTH/3 + 1, HEIGHT/2 + selection * 15 + 81, WIDTH/3 + 9, HEIGHT/2 + selection * 15 + 89, WHITE);
	}
	else if (key[KEY_ENTER]) {
		play_sample(click_sound, 128, 128, 1000, FALSE);
		return -1;
	}
}

// Print the welcome screen menu
void welcome_screen() {
	
	// Blit the title
	blit(title, screen, 0, 0, 150, 50, title->w, title->h);
	
	// Options and insturctions
    textout_centre_ex(screen, font, "Press use your ARROW KEYS and ENTER to select an option!", WIDTH/2, HEIGHT/2 + 40, WHITE, BLACK);
    rect(screen, WIDTH/3, HEIGHT/2 + 80 , WIDTH/3 + 10, HEIGHT/2 + 90, WHITE);
    textout_ex(screen, font, "Start!", WIDTH/3 + 25, HEIGHT/2 + 82, WHITE, BLACK);
    rect(screen, WIDTH/3, HEIGHT/2 + 95, WIDTH/3 + 10, HEIGHT/2 + 105, WHITE);
    textout_ex(screen, font, "Instructions", WIDTH/3 + 25, HEIGHT/2 + 97, WHITE, BLACK);
    rect(screen, WIDTH/3, HEIGHT/2 + 110, WIDTH/3 + 10, HEIGHT/2 + 120, WHITE);
    textout_ex(screen, font, "Gamemode:", WIDTH/3 + 25, HEIGHT/2 + 112, WHITE, BLACK);
    selection = 0;
    rectfill(screen, WIDTH/3 + 1, HEIGHT/2 + 81, WIDTH/3 + 9, HEIGHT/2 + 89, WHITE);
    
    // Check if user selects Hard or Easy mode
    if (hardmode == 0) {
    	textout_ex(screen, font, "Easy", WIDTH/3 + 100, HEIGHT/2 + 112, YELLOW, BLACK);
    }
    else {
    	textout_ex(screen, font, "Hard", WIDTH/3 + 100, HEIGHT/2 + 112, YELLOW, BLACK);
    }	
}

// Draw initial start screen instructions
void draw_startscreen() {
    
    // Run welcome screen print
    welcome_screen();
    
    // Check for user input and update for user selection
    while (1) {
    	if (getmenuinput() == -1) {
    		if (selection == 0) {
    			break;
    		}
    		else if (selection == 1) {
    			instructions();
    			rectfill(screen, 0, 0, WIDTH, HEIGHT, BLACK);
    			welcome_screen();
    		}
    		else {
    			hardmode = (hardmode + 1) % 2;
    			if (hardmode == 0) {
		    		textout_ex(screen, font, "Easy", WIDTH/3 + 100, HEIGHT/2 + 112, YELLOW, BLACK);
			    }
			    else {
			    	textout_ex(screen, font, "Hard", WIDTH/3 + 100, HEIGHT/2 + 112, YELLOW, BLACK);
			    }
    		}
    	}
    	
    	// Slow game down
	    rest(100);
    }
}

//calculate X movement value based on direction angle
double calcAngleMoveX(int angle) {
    return (double) cos(angle * PI / 180);
}

//calculate Y movement value based on direction angle
double calcAngleMoveY(int angle) {
    return (double) sin(angle * PI / 180);
}

// Warp sprite if reaches end of screen
void warpsprite(sprite *spr) {
	
    //simple screen warping behavior
    int w = spr->width;
    int h = spr->height;
    
    if (spr->x < 0-w)
    {
        spr->x = SCREEN_W;
    }

    else if (spr->x > SCREEN_W)
    {
        spr->x = 0-w;
    }

    if (spr->y < 0-h)
    {
        spr->y = SCREEN_H;
    }

    else if (spr->y > SCREEN_H)
    {
        spr->y = 0-h;
    }
}

// Respawn asteroid if destoryed
void restart_asteroid(int num) {
	
	// Loop variables
	int collide;
	int i;
	
	// Check if respawn collides with other asteroids. If so, reposition
	while (1) {
		
		// Restart colision
		collide = 0;
		
		// Randomize placement
		if (rand() % 2 == 0) {
			if (rand() % 2 == 0) {
				asteroids->get(num)->x = 0;
				asteroids->get(num)->y = rand() % (SCREEN_H - asteroids->get(num)->height);
			}
			else {
				asteroids->get(num)->x = SCREEN_W - asteroids->get(num)->width;
				asteroids->get(num)->y = rand() % (SCREEN_H - asteroids->get(num)->height);
			}
		}
		else {
			if (rand() % 2 == 0) {
				asteroids->get(num)->x = rand() % (SCREEN_W - asteroids->get(num)->width);
				asteroids->get(num)->y = 0;
			}
			else {
				asteroids->get(num)->x = rand() % (SCREEN_W - asteroids->get(num)->width);
				asteroids->get(num)->y = SCREEN_H - asteroids->get(num)->height;
			}
		}
		
		// Check if collides with other sprites
		for (i = 0; i < ASTEROID_COUNT; i++) {
			if (i != num && asteroids->get(i)->collided(asteroids->get(num))) {
    			collide = 1;
				break;	
    		}
		}
		if (collide == 0) {
			break;
		}
	}
		
	// Randomize speed and set to alive
	asteroids->get(num)->velx = (rand() % 12) - 6;
	asteroids->get(num)->vely = (rand() % 12) - 6;
	asteroids->get(num)->alive = 1;	
	
	// Set health
	if (num >= SMALLASTEROID_COUNT) {
		asteroids->get(num)->health = 3;
	}
	else {
		asteroids->get(num)->health = 1;
	}
	
}

// Check collisions with other asteroids. Only in hard mode
void checkcollisions_asteroid(int num) {
	
	// Loop and position variables
    int cx1,cy1,cx2,cy2;
    int i;

	// Loop through all asteroids
    for (i=0; i<ASTEROID_COUNT; i++)
    {
    	
    	// Check if current asteroid collided with others
        if (i != num && asteroids->get(i)->collided(asteroids->get(num)))
        {
        	
            //calculate center of primary sprite
            cx1 = (int)asteroids->get(i)->x + asteroids->get(i)->width / 2;
            cy1 = (int)asteroids->get(i)->y + asteroids->get(i)->height / 2;
            
            //calculate center of secondary sprite
            cx2 = (int)asteroids->get(i)->x + asteroids->get(i)->width / 2;
            cy2 = (int)asteroids->get(i)->y + asteroids->get(i)->height / 2;
            
            //figure out which way the sprites collided
            if (cx1 <= cx2)
            {
            	
            	// Randomize in opposite directions
                asteroids->get(i)->velx = -1 * (rand() % 6) - 1;
                asteroids->get(num)->velx = rand() % 6 + 1;
                if (cy1 <= cy2)
                {
                    asteroids->get(i)->vely = -1 * (rand() % 6) - 1;
                    asteroids->get(num)->vely = rand() % 6 + 1;
                }
                else
                {
                    asteroids->get(i)->vely = rand() % 6 + 1;
                    asteroids->get(num)->vely = -1 * (rand() % 6) - 1;
                }
            }
            else
            {
                //cx1 is > cx2
                asteroids->get(i)->velx = rand() % 6 + 1;
                asteroids->get(num)->velx = -1 * (rand() % 6) - 1;
                if (cy1 <= cy2)
                {
                    asteroids->get(i)->vely = rand() % 6 + 1;
                    asteroids->get(num)->vely = -1 * (rand() % 6) - 1;
                }
                else
                {
                    asteroids->get(i)->vely = -1 * (rand() % 6) - 1;
                    asteroids->get(num)->vely = rand() % 6 + 1;
                }
            }
        }
    }
}

// Check if bullet collided with any asteroids
void checkcollisions_bullet(int num) {
	
	// Loop variable
    int i;

	// Cycle through all of the asteroids
    for (i=0; i<ASTEROID_COUNT; i++)
    {
    	
    	// Check if bullet collided with asteroid
        if (bullets->get(num)->collided(asteroids->get(i)))
        {
        	
        	// Minus asteroid health
        	asteroids->get(i)->health--;
        	
        	// Check if asteroid is destoryed.
        	if (asteroids->get(i)->health == 0) {
        		draw_sprite(buffer, explosion, bullets->get(num)->x, bullets->get(num)->y);
        		score += 100;
        		asteroids->get(i)->alive = 0;
        	}
        	
        	// Set bullet to dead
        	bullets->get(num)->alive = 0;
        	break;
        }
    }
}

// Check collisions with ship
void checkcollisions_ship() {
	
	// Loop variable
	int i;
	
	// Check if pulse is active (no injury)
	if (active_pulse == 0) {
		
		// Cycle through all asteroids
		for (i=0; i<ASTEROID_COUNT; i++)
	    {
	    	
	    	// Check if collided with asteroid
	        if (spaceship->collided(asteroids->get(i)))
	        {
	        	// Subtract health, destory asteroid and check if game over
	        	spaceship->health -= 1;
	        	asteroids->get(i)->alive = 0;
	        	if (spaceship->health == 0) {
	        		spaceship->alive = 0;
	        		gameover = 1;
	        	}
	        }
	    }	
	}
}

// Active pulse feature
void activate_pulse() {
	
	// Loop variable
	int i;
	
	// Set pulse to where the ship is
	pulse->x = spaceship->x + spaceship->width/2 - pulse->image->w/2;
	pulse->y = spaceship->y + spaceship->height/2 - pulse->image->h/2;
	
	// Draw the pulse
	draw_sprite(buffer, pulse->image, pulse->x, pulse->y);

	// Cycle through all of the asteroids. If collides, send in opposite direction
    for (i=0; i<ASTEROID_COUNT; i++)
    {
        if (pulse->collided(asteroids->get(i), 0)) {
        	asteroids->get(i)->velx = asteroids->get(i)->velx * -1;
        	asteroids->get(i)->vely = asteroids->get(i)->vely * -1;
        	asteroids->get(i)->updatePosition();
        	asteroids->get(i)->updatePosition();
        	asteroids->get(i)->updatePosition();
        	asteroids->get(i)->updatePosition();
        	asteroids->get(i)->updatePosition();
        	asteroids->get(i)->updatePosition();
        }
    }
}

// Used to move ship forward or backwards
void thrusters(int dir) {
    //shift 0-degree orientation from right-face to up-face
	spaceship->moveAngle = spaceship->faceAngle - 90;
	
    //convert negative angle to wraparound
	if (spaceship->moveAngle < 0) spaceship->moveAngle = 359 + spaceship->moveAngle;

    //adjust velocity based on angle
    if (dir == 1) {
    	spaceship->velx += calcAngleMoveX(spaceship->moveAngle) * ACCELERATION;
		spaceship->vely += calcAngleMoveY(spaceship->moveAngle) * ACCELERATION;
    }
    else {
    	spaceship->velx -= calcAngleMoveX(spaceship->moveAngle) * ACCELERATION;
		spaceship->vely -= calcAngleMoveY(spaceship->moveAngle) * ACCELERATION;
    }
	
	//keep velocity down to a reasonable speed
	if (spaceship->velx > 4.0) {
		spaceship->velx = 4.0;
	}
	
	if (spaceship->velx < -4.0) {
		spaceship->velx = -4.0;
	}
	
	if (spaceship->vely > 4.0) {
		spaceship->vely = 4.0;
	}
	
	if (spaceship->vely < -4.0) {
		spaceship->vely = -4.0;
	}
}

// Turn ship left
void turnleft() {
	// Alter angle
    spaceship->faceAngle -= STEP;
	if (spaceship->faceAngle < 0) {
		spaceship->faceAngle = 359;
	}
}

// Turn ship right
void turnright() {
	// Alter angle
    spaceship->faceAngle += STEP;
	if (spaceship->faceAngle > 359) {
		spaceship->faceAngle = 0;
	}
}

// Fire weapon
void fireweapon() {
	
	// Play sound
	play_sample(bullet_sound, 128, 128, 1000, FALSE);
	
	// Set bullet position and angle
    bullets->get(bullet_index)->x = spaceship->pointer_x - (bullets->get(bullet_index)->width / 2);
    bullets->get(bullet_index)->y = spaceship->pointer_y - (bullets->get(bullet_index)->height / 2);
    bullets->get(bullet_index)->faceAngle = spaceship->faceAngle;
    
    //shift 0-degree orientation from right-face to up-face
	bullets->get(bullet_index)->moveAngle = bullets->get(bullet_index)->faceAngle - 90;
	
    //convert negative angle to wraparound
	if (bullets->get(bullet_index)->moveAngle < 0) {
		bullets->get(bullet_index)->moveAngle = 359 + bullets->get(bullet_index)->moveAngle;
	}
	
	// Calculate velocity and set alive
    bullets->get(bullet_index)->velx = calcAngleMoveX(bullets->get(bullet_index)->moveAngle) * BULLETSPEED;
    bullets->get(bullet_index)->vely = calcAngleMoveY(bullets->get(bullet_index)->moveAngle) * BULLETSPEED;
	bullets->get(bullet_index)->alive = 1;
	
	// Cycle bullets
	bullet_index++;
	if (bullet_index > BULLET_CAP - 1) {
		bullet_index = 0;
	}
	
	// Set cooldown
	bullet_cooldown = BULLETCOOLDOWN;
}

// Update bullet
void updatebullet(int num) {
	
	// Temp position variables
    int x, y, tx, ty;
		
	//move the bullet
	bullets->get(num)->updatePosition();
    x = bullets->get(num)->x;
    y = bullets->get(num)->y;

    //stay within the screen
    if (x < 6 || x > SCREEN_W-6 || y < 20 || y > SCREEN_H-6)
    {
        bullets->get(num)->alive = 0;
        return;
    }

    // Check if collide with asteroid
    checkcollisions_bullet(num);
    
    // Draw bullet if alive
    if (bullets->get(num)->alive) {
    	rotate_sprite(buffer, bullets->get(num)->image, (int)bullets->get(num)->x, bullets->get(num)->y, 
			itofix((int)(bullets->get(num)->faceAngle / 0.7f / 2.0f)));
    }
}

// Update asteroid
void updateasteroid(int num) {
	
	// Draw correct sprite based on health and index
	if (asteroids->get(num)->health == 3 || num < SMALLASTEROID_COUNT) {
		draw_sprite(buffer, asteroids->get(num)->image, (int)asteroids->get(num)->x, asteroids->get(num)->y);
	}
	else if (asteroids->get(num)->health == 2) {
		draw_sprite(buffer, asteroids->get(num)->image2, (int)asteroids->get(num)->x, asteroids->get(num)->y);
	}
	else {
		draw_sprite(buffer, asteroids->get(num)->image3, (int)asteroids->get(num)->x, asteroids->get(num)->y);
	}
		
	//move the asteroid
	asteroids->get(num)->updatePosition();
		
	// Check if hardmode for collision is active
	if (hardmode == 1) {
		checkcollisions_asteroid(num);
	}
	
	// Warp asteroid
	warpsprite(asteroids->get(num));
}

// Update health bar
void updatehealth() {
	
	// Health outline
	rect(buffer, 60, 2, 90, 14, RED); 
	rect(buffer, 90, 2, 120, 14, RED); 
	rect(buffer, 120, 2, 150, 14, RED); 
	
	// Health filler
	if (spaceship->health > 0) {
		rectfill(buffer, 62, 4, 88, 12, RED);
	}
	
	if (spaceship->health > 1) {
		rectfill(buffer, 92, 4, 118, 12, RED);	
	}
	
	if (spaceship->health > 2) {
		rectfill(buffer, 122, 4, 148, 12, RED);
	}
}

// Update pulse cooldown bar
void updatepulse() {
	
	// Actively refill based on cooldown
	rect(buffer, WIDTH/2 - 75, 2, WIDTH/2 + 75, 14, BLUE);
	rectfill(buffer, WIDTH/2 - 73, 4, WIDTH/2 + 73 - (pulse_cooldown) * 1.46, 12, BLUE);
}

// Main update function
void update() {
	
	//Loop variable
	int i;
    
    // Status bar
    rectfill(buffer, 0, 0, WIDTH, 16, BLACK); 
    textout_ex(buffer, font, "Health:", 1, 4, WHITE, BLACK);
    textout_ex(buffer, font, "Pulse:", WIDTH/2 - 125, 4, WHITE, BLACK);
    textprintf_centre_ex(buffer, font, WIDTH - 50, 4, WHITE, BLACK, "Score: %i", score);
    
    // Update health and pulse bars
    updatehealth();
    updatepulse();
        
    // Check collisions with spaceship
    checkcollisions_ship();
        
    //move the spaceship
	spaceship->updatePosition();
	
	// Update blaster of ship
	spaceship->pointer_x = spaceship->x + spaceship->width / 2 + calcAngleMoveX(spaceship->faceAngle - 90) * (spaceship->height / 2);
	spaceship->pointer_y = spaceship->y + spaceship->height / 2 + calcAngleMoveY(spaceship->faceAngle - 90) * (spaceship->height / 2);
	
	//rotate sprite with adjust for Allegro's 16.16 fixed trig
    //(256 / 360 = 0.7), then divide by 2 radians
	rotate_sprite(buffer, spaceship->image, (int)spaceship->x, (int)spaceship->y, 
        itofix((int)(spaceship->faceAngle / 0.7f / 2.0f)));
        
    // Update pulse variables
    if (active_pulse == 0) {
    	pulse->alive = 0;
    }
    else {
    	activate_pulse();
    	active_pulse--;	
    }
    
    // Pulse cooldown countdown
    if (pulse_cooldown != 0) {
    	pulse_cooldown--;	
    }
	
	// Warp spaceship
    warpsprite(spaceship);
}

// Get game input from user
void getinput() {
	
    //hit ESC to quit
    if (key[KEY_ESC]) {
		gameover = 1;
	}
	
	// Instruction screen
	if (key[KEY_LCONTROL] && key[KEY_H]) {
		instructions();
	}
	
	// Background music toggle
	if (key[KEY_LCONTROL] && key[KEY_M]) {
		if (sound == 1) {
			sound = 0;
			stop_sample(background_music);
		}
		else {
			sound = 1;
			play_sample(background_music, 128, 128, 1000, TRUE);
		}
		rest(20);
	}
    
    //ARROW KEYS AND SPACE BAR CONTROL for spaceship
    if (key[KEY_UP]) {  
		thrusters(1);
	}
	
    if (key[KEY_DOWN]) {
		thrusters(-1);
	}
	
    if (key[KEY_LEFT]) {
		turnleft();
	}
	
    if (key[KEY_RIGHT]) {
		turnright();
	}
	
    if (key[KEY_SPACE]) {
    	if (bullet_cooldown == 0) {
    		fireweapon();
    	}
	}
	
	// Pulse active
	if (key[KEY_G]) {
		if (pulse_cooldown == 0) {
			play_sample(pulse_sound, 128, 128, 1000, FALSE);
			active_pulse = PULSE_DURATION;	
			pulse_cooldown = PULSE_COOLDOWN;
		}	
	}

    //short delay after keypress        
    rest(20);
}	

// Setup initial files
void setupscreen() {
	
	//load background buffer
	title = (BITMAP *) data[GALATICDEFENSE_BMP].dat;
	if (!title) {
		allegro_message("Error loading sprites");
		return;
	}
	
	// Load sound files
	background_music = (SAMPLE *) data[BACKGROUND_WAV].dat;
	click_sound = (SAMPLE *) data[CLICK_WAV].dat;
	bullet_sound = (SAMPLE *) data[BULLET_WAV].dat;
	pulse_sound = (SAMPLE *) data[PULSE_WAV].dat;
	
	//install a digital sound driver
    if (install_sound(DIGI_AUTODETECT, MIDI_NONE, "") != 0) {
    	allegro_message("Error initalizing sound system");
		return;	
    }
    
    // Check if sound files loaded
    if (!background_music || !click_sound || !bullet_sound || !pulse_sound) {
    	allegro_message("Error reading wave files");
    	return;
    }
	
	// Draw start screen
	draw_startscreen();
}

// Setup game
void setupgame() {
	
	//Loop variable
	int i;	
	int j;
	int collide;

	//Play background music
	play_sample(background_music, 128, 128, 1000, TRUE);

	//Create a back buffer
	buffer = create_bitmap(WIDTH, HEIGHT);
	
	//load background buffer
	background = (BITMAP *)data[BACKGROUND_BMP].dat;
	
	//load explosion bitmap
	explosion = (BITMAP *)data[EXPLOSION_BMP].dat;
	
	//Create a spaceship sprite
	spaceship = new sprite();
	if (!spaceship->load((BITMAP *) data[SPACESHIP_BMP].dat) || !background || !explosion) {
		allegro_message("Error loading sprites");
		return;
	}
	
	// Set spaceship variables
    spaceship->width = spaceship->image->w;
    spaceship->height = spaceship->image->h;
    spaceship->health = 3;
    spaceship->xdelay = 0;
    spaceship->ydelay = 0;
    spaceship->x = SCREEN_W / 2 - spaceship->width/2;
    spaceship->y = SCREEN_H / 2 - spaceship->height/2;
	spaceship->moveAngle = 0;
	spaceship->faceAngle = 0;
	
	// Set pulse variables
	pulse = new sprite();
	if (!pulse->load((BITMAP *) data[PULSE_BMP].dat)) {
		allegro_message("Error loading sprites");
		return;
	}
	
	pulse->width = pulse->image->w + 20;
	pulse->height = pulse->image->h + 20;
	pulse->x = 0;
	pulse->y = 0;
	pulse->alive = 0;
	
	// Set bullet variables
	bullets = new spritehandler();
	
	for (i = 0; i < BULLET_CAP; i++) {
		bullets->create();
		bullets->get(i)->load((BITMAP *) data[BULLET_BMP].dat);
		bullets->get(i)->width = bullets->get(i)->image->w;
		bullets->get(i)->height = bullets->get(i)->image->h;
		bullets->get(i)->xdelay = 0;
		bullets->get(i)->ydelay = 0;
		bullets->get(i)->x = 0;
		bullets->get(i)->y = 0;
	}
	
	// Set asteroid variables
	asteroids = new spritehandler();
	
	// Small asteroids
	for (i = 0; i < SMALLASTEROID_COUNT; i++) {
		asteroids->create();
		asteroids->get(i)->load((BITMAP *) data[ASTEROID1_BMP].dat);
		asteroids->get(i)->health = 1;
		asteroids->get(i)->width = asteroids->get(i)->image->w;
		asteroids->get(i)->height = asteroids->get(i)->image->h;
		asteroids->get(i)->xdelay = 1;
		asteroids->get(i)->ydelay = 1;
		asteroids->get(i)->alive = 1;
		
		// Check if initial spawn collides with other asteroids. If so, reposition
		while (1) {
			collide = 0;
			
			if (rand() % 2 == 0) {
				asteroids->get(i)->x = rand() % ((SCREEN_W / 2) - (asteroids->get(i)->width + spaceship->width));
			}
			else {
				asteroids->get(i)->x = rand() % ((SCREEN_W / 2) - (asteroids->get(i)->width + spaceship->width)) + ((SCREEN_W / 2) + (asteroids->get(i)->width + spaceship->width));
			}
			
			if (rand() % 2 == 0) {
				asteroids->get(i)->y = rand() % (SCREEN_H / 2) - (asteroids->get(i)->height + spaceship->height);
			}
			else {
				asteroids->get(i)->y = rand() % ((SCREEN_H / 2) - (asteroids->get(i)->height + spaceship->height)) + ((SCREEN_H / 2) + (asteroids->get(i)->height + spaceship->height));
			}
			
			for (j = 0; j < i; j++) {
				if (asteroids->get(j)->collided(asteroids->get(i))) {
        			collide = 1;
					break;	
        		}
			}
			
			if (collide == 0) {
				break;
			}
		}
		
		// Randomize velocity
		asteroids->get(i)->velx = (rand() % 12) - 6;
		asteroids->get(i)->vely = (rand() % 12) - 6;
	}
	
	// Large asteroids
	for (i = SMALLASTEROID_COUNT; i < ASTEROID_COUNT; i++) {
		
		asteroids->create();
		asteroids->get(i)->load((BITMAP *) data[ASTEROID20_BMP].dat);
		asteroids->get(i)->load2((BITMAP *) data[ASTEROID21_BMP].dat);
		asteroids->get(i)->load3((BITMAP *) data[ASTEROID22_BMP].dat);
		asteroids->get(i)->health = 3;
		asteroids->get(i)->width = asteroids->get(i)->image->w;
		asteroids->get(i)->height = asteroids->get(i)->image->h;
		asteroids->get(i)->xdelay = 1;
		asteroids->get(i)->ydelay = 1;
		asteroids->get(i)->alive = 1;
		
		// Check if initial spawn collides with other asteroids. If so, reposition
		while (1) {
			collide = 0;
			
			if (rand() % 2 == 0) {
				asteroids->get(i)->x = rand() % ((SCREEN_W / 2) - (asteroids->get(i)->width + spaceship->width));
			}
			else {
				asteroids->get(i)->x = rand() % ((SCREEN_W / 2) - (asteroids->get(i)->width + spaceship->width)) + ((SCREEN_W / 2) + (asteroids->get(i)->width + spaceship->width));
			}
			
			if (rand() % 2 == 0) {
				asteroids->get(i)->y = rand() % (SCREEN_H / 2) - (asteroids->get(i)->height + spaceship->height);
			}
			else {
				asteroids->get(i)->y = rand() % ((SCREEN_H / 2) - (asteroids->get(i)->height + spaceship->height)) + ((SCREEN_H / 2) + (asteroids->get(i)->height + spaceship->height));
			}
			
			for (j = 0; j < i; j++) {
				if (asteroids->get(j)->collided(asteroids->get(i))) {
	    			collide = 1;
					break;	
	    		}
			}
			
			if (collide == 0) {
				break;
			}
		}
		
		// Randomize velocity
		asteroids->get(i)->velx = calcAngleMoveX(asteroids->get(i)->moveAngle) * ((rand() % (ASTEROID_SPEED - 1)) + 1);
		asteroids->get(i)->vely = calcAngleMoveY(asteroids->get(i)->moveAngle) * ((rand() % (ASTEROID_SPEED - 1)) + 1);
	}
}

//this thread updates asteroids
void* thread0(void* data)
{
	// Loop variable
	int i;
	
    //get this thread id
    int my_thread_id = *((int*)data);

    //thread's main loop
    while(!done)
    {
        //lock the mutex to protect variables
        pthread_mutex_lock(&threadsafe);
        
        // Update asteroid is alive, else respawn it
	    for (i = 0; i < ASTEROID_COUNT; i++) {
	    	if (asteroids->get(i)->alive == 1) {
	    		updateasteroid(i);
	    	}
	    	else {
	    		restart_asteroid(i);
	    	}
	    }

        //unlock the mutex
        pthread_mutex_unlock(&threadsafe);

        //slow down (this thread only!)
    	rest(10);
    }

    // terminate the thread
    pthread_exit(NULL);

    return NULL;
}

//this thread updates bullets
void* thread1(void* data)
{
	// Loop variable
	int i;
	
    //get this thread id
    int my_thread_id = *((int*)data);

    //thread's main loop
    while(!done)
    {
        //lock the mutex to protect variables
        pthread_mutex_lock(&threadsafe);
        
        // Update bullet if alive
		for (i = 0; i < BULLET_CAP; i++) {
	    	if (bullets->get(i)->alive == 1) {
	    		updatebullet(i);
			}
	    }
	    
	    // Bullet cooldown timer
		if (bullet_cooldown > 0) {
			bullet_cooldown--;	
		}

        //unlock the mutex
        pthread_mutex_unlock(&threadsafe);

        //slow down (this thread only!)
    	rest(10);
    }

    // terminate the thread
    pthread_exit(NULL);

    return NULL;
}

//this thread updates game ship and GUI
void* thread2(void* data)
{
    //get this thread id
    int my_thread_id = *((int*)data);

    //thread's main loop
    while(!done)
    {
        //lock the mutex to protect variables
        pthread_mutex_lock(&threadsafe);
	    
	    update();

        //unlock the mutex
		pthread_mutex_unlock(&threadsafe);

        //slow down (this thread only!)
    	rest(10);
    }

    // terminate the thread
    pthread_exit(NULL);

    return NULL;
}

int main(void)
{
	
	//initialize Allegro
    allegro_init(); 
    
    //initialize the keyboard
    install_keyboard();
    
    //initalize timer
    install_timer();
    
    //initialize random seed
    srand(time(NULL));
    
    //set video mode    
    set_color_depth(16);
    
    //pthreads
    pthread_t pthread0;
    pthread_t pthread1;
    pthread_t pthread2;
    
    int threadid0 = 0;
    int threadid1 = 1;
    int threadid2 = 2;
    
    // Error variable
    int ret;
    int id;
    
    ret = set_gfx_mode(MODE, WIDTH, HEIGHT, 0, 0);
    if (ret != 0) {
	    allegro_message(allegro_error);
	    return 0;
	}
    
    // Setup datafiles
    // load the datfile
    data = load_datafile(DATA_FILE);
    
    // Error checking on load
    if (data == NULL) {
    	allegro_message("Error loading datafile");
	    return 0;
    }
    
    // Setup screen
    setupscreen();
	
	// Setup game
	setupgame();
	
	// Clear Screen
    rectfill(screen, 0, 0, WIDTH, HEIGHT, BLACK);
    
    //create the thread for Asteroid handling
    id = pthread_create(&pthread0, NULL, thread0, (void*)&threadid0);

    //create the thread for Bullet handling
    id = pthread_create(&pthread1, NULL, thread1, (void*)&threadid1);
    
    //create the thread for game ship and GUI handling
    id = pthread_create(&pthread2, NULL, thread2, (void*)&threadid2);
    
    // Game loop
    while(!gameover) {
    	
    	//lock the mutex to protect double buffer
        pthread_mutex_lock(&threadsafe);
    	
    	//refresh the screen
	    acquire_screen();
		blit(buffer, screen, 0, 0, 0, 0, WIDTH, HEIGHT);
	    release_screen();
	    
	    //check for keypresses
        if (keypressed()) {
            getinput();
		}
		
		//Clear background
		blit(background, buffer, 0, 0, 0, 0, WIDTH, HEIGHT);
		
		//unlock the mutex
        pthread_mutex_unlock(&threadsafe);
		
    }
    
    //tell threads it's time to quit
    done++;
    rest(100);
    
    //kill the mutex (thread protection)
    pthread_mutex_destroy(&threadsafe);
    
    // Clear Screen
    rectfill(screen, 0, 0, WIDTH, HEIGHT, BLACK);
    
    // Print final score
    textprintf_centre_ex(screen, font, WIDTH/2, HEIGHT/2, WHITE, BLACK, "Final User Score: %i", score);
    textout_centre_ex(screen, font, "Please Press ESC to QUIT!", WIDTH/2, HEIGHT/2 + 20, WHITE, BLACK);
    
    // Slow down game
    rest(250);
    
    // Wait until user exits game
    while(!key[KEY_ESC]) {
	};
    
    // Cleanup
    destroy_bitmap(title);
	destroy_bitmap(buffer);
	destroy_bitmap(background);
	destroy_bitmap(explosion);
	destroy_sample(background_music);
	destroy_sample(click_sound);
	destroy_sample(bullet_sound);
	destroy_sample(pulse_sound);
	delete pulse;
	delete spaceship;
    
    //end program
    allegro_exit();
    return 0;
}
END_OF_MAIN()
