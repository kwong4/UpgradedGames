// Author: Kevin Wong
// Mappy Level Created by: Kevin Wong
// Credits and sources in Design Document

#include <stdio.h>
#include <allegro.h>
#include "mappyal.h"
#include "GetMeMyFruit.h"
#include "defines.h"

// Tile Grabber
BITMAP *grabframe(BITMAP *source, int width, int height, int startx, int starty, int columns, int frame) {
	
    BITMAP *temp = create_bitmap(width,height);
    int x = startx + (frame % columns) * width;
    int y = starty + (frame / columns) * height;
    blit(source, temp, x, y, 0, 0, width, height);
    
    return temp;
}

// Point collision with bounding box
int inside_box(int x,int y,int left,int top,int right,int bottom) {
    if (x > left && x < right && y > top && y < bottom)
        return 1;
    else
        return 0;
}

// Sprite collision with shrink and bounding box
int collided(SPRITE *current, SPRITE *other, int shrink) {
    int wa = current->x + current->width;
    int ha = current->y + current->height;
    int wb = other->x + other->width;
    int hb = other->y + other->height;

    if (inside_box(current->x, current->y, other->x+shrink, other->y+shrink, wb-shrink, hb-shrink) ||
        inside_box(current->x, ha, other->x+shrink, other->y+shrink, wb-shrink, hb-shrink) ||
        inside_box(wa, current->y, other->x+shrink, other->y+shrink, wb-shrink, hb-shrink) ||
        inside_box(wa, ha, other->x+shrink, other->y+shrink, wb-shrink, hb-shrink)) {
        	return 1;	
        }
    else {
        return 0;
    }
}

// Mappy Collision detection
int map_collided(int x, int y) {
    BLKSTR *blockdata;
	blockdata = MapGetBlock(x/mapblockwidth, y/mapblockheight);
	return blockdata->tl;
}

// Check if sprite inside of screen
int inside(SPRITE* sprite) {
	
	//Is sprite visible on screen?
    if (sprite->y > mapyoff - FRUIT_CONSTANT && sprite->y < mapyoff + HEIGHT + FRUIT_CONSTANT)
    {
    	if (sprite->x > mapxoff - FRUIT_CONSTANT && sprite->x < mapxoff + WIDTH + FRUIT_CONSTANT) {
    		return 1;	
    	}
    }
    return 0;
}

// Print Insturction screen
void instructions() {
	
	// Clear screen
	rectfill(screen, 0, 0, WIDTH, HEIGHT, BLACK);
	
	// Print game info
	textout_centre_ex(screen, font, "GAME INFO:", WIDTH/2, HEIGHT/4, WHITE, BLACK);
	textout_ex(screen, font, "- You are tasked with collecting 4 different types of fruit:", WIDTH/8, HEIGHT/4 + 20, WHITE, BLACK);
	textout_ex(screen, font, "    - Orange", WIDTH/8, HEIGHT/4 + 30, ORANGE, BLACK);
	textout_ex(screen, font, "    - Watermelon", WIDTH/8, HEIGHT/4 + 40, GREEN, BLACK);
	textout_ex(screen, font, "    - Apple", WIDTH/8, HEIGHT/4 + 50, RED, BLACK);
	textout_ex(screen, font, "    - Berries", WIDTH/8, HEIGHT/4 + 60, BLUE, BLACK);
	textout_ex(screen, font, "- Avoid enemies that may look like fruit, but move!", WIDTH/8, HEIGHT/4 + 70, WHITE, BLACK);
	textout_ex(screen, font, "- Touching an enemy will result in an instant death!", WIDTH/8, HEIGHT/4 + 80, WHITE, BLACK);
	textout_ex(screen, font, "- Easy gamemode will make enemies move at standard speed", WIDTH/8, HEIGHT/4 + 90, WHITE, BLACK);
	textout_ex(screen, font, "- Hard gamemode will make enemies move faster", WIDTH/8, HEIGHT/4 + 100, WHITE, BLACK);
	
	// Print game instructions
	textout_centre_ex(screen, font, "INSTRUCTIONS:", WIDTH/2, HEIGHT/2 + 40, WHITE, BLACK);
    textout_ex(screen, font, "1. Use the arrow keys to move your Character", WIDTH/8, HEIGHT/2 + 60, WHITE, BLACK);
    textout_ex(screen, font, "   Use the LEFT key to move LEFT", WIDTH/8, HEIGHT/2 + 70, YELLOW, BLACK);
    textout_ex(screen, font, "   Use the RIGHT key to move RIGHT", WIDTH/8, HEIGHT/2 + 80, YELLOW, BLACK);
    textout_ex(screen, font, "2. Use the SPACE bar key to JUMP", WIDTH/8, HEIGHT/2 + 90, WHITE, BLACK);
    textout_ex(screen, font, "3. Press Ctrl + h to bring up the instructions at any time!", WIDTH/8, HEIGHT/2 + 100, WHITE, BLACK);
    textout_ex(screen, font, "4. Press Ctrl + m to toggle the background music at any time!", WIDTH/8, HEIGHT/2 + 110, WHITE, BLACK);
    textout_ex(screen, font, "5. Press Esc to exit the game!", WIDTH/8, HEIGHT/2 + 120, WHITE, BLACK);
    
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
	blit(title, screen, 0, 0, 65, 50, title->w, title->h);
	
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

// Setup initial files
void setupscreen() {
	
	// Load title screen w/ Error checking
	title = (BITMAP *) data[GETMEMYFRUIT_BMP].dat;
	
	if (!title) {
		allegro_message("Error loading title screen");
		return;
	}
	
	// Load sound files
	background_music = (SAMPLE *) data[BACKGROUND_WAV].dat;
	click_sound = (SAMPLE *) data[CLICK_WAV].dat;
	jump_sound = (SAMPLE *) data[BOING_WAV].dat;
	fruit_collect_sound = (SAMPLE *) data[FANFARE_WAV].dat;
	enemy_die_sound = (SAMPLE *) data[PLUCK_WAV].dat;
	game_over_sound = (SAMPLE *) data[SHUT_OFF_WAV].dat;
	splash_sound = (SAMPLE *) data[SPLASH_WAV].dat;
	game_win_sound = (SAMPLE *) data[YAY_WAV].dat;
	
	//install a digital sound driver
    if (install_sound(DIGI_AUTODETECT, MIDI_NONE, "") != 0) {
    	allegro_message("Error initalizing sound system");
		return;	
    }
    
    // Check if sound files loaded
    if (!background_music || !click_sound || !jump_sound || !fruit_collect_sound || !enemy_die_sound || !game_over_sound || !splash_sound || !game_win_sound) {
    	allegro_message("Error reading wave files");
    	return;
    }
	
	//load level
	if (MapLoad(GAME_WORLD)) {
		allegro_message("Error loading map");
		return;
	}
	
	//create the double buffer
	buffer = create_bitmap(WIDTH, HEIGHT);
	clear(buffer);
	
	// Draw start screen
	draw_startscreen();
}

// Setup game
void setupgame() {
	
	// Loop variable
	int i, j;
	
	// Temporary variables
	BITMAP *temp;
	
	// Play Background music
	play_sample(background_music, 128, 128, 1000, TRUE);
	
	//Setup Player
	player_image[0] = (BITMAP *) data[CHARACTER_WAIT1_BMP].dat;
	player_image[1] = (BITMAP *) data[CHARACTER_WAIT2_BMP].dat;
	player_image[2] = (BITMAP *) data[CHARACTER_WALK1_BMP].dat;
	player_image[3] = (BITMAP *) data[CHARACTER_WALK2_BMP].dat;
	player_image[4] = (BITMAP *) data[CHARACTER_JUMP_BMP].dat;

	// Error checking
	if (!player_image[0] || !player_image[1] || !player_image[2] || !player_image[3] || !player_image[4]) {
		allegro_message("Error loading fruit sprites");
		return;
	}

	// Player Sprite Setup
    player = malloc(sizeof(SPRITE));
    player->x = START_POINT_X;
    player->y = START_POINT_Y;
    player->curframe= 0;
    player->framecount= 0;
    player->framedelay= FRAME_DELAY;
    player->maxframe= PLAYER_MAX_FRAME;
    player->width= player_image[0]->w;
    player->height= player_image[0]->h;
    player->alive = 1;
    
    // Setup Fruit
	fruits_image[0] = (BITMAP *) data[ORANGE_BMP].dat;
	fruits_image[1] = (BITMAP *) data[WATERMELON_BMP].dat;
	fruits_image[2] = (BITMAP *) data[APPLE_BMP].dat;
	fruits_image[3] = (BITMAP *) data[BERRIES_BMP].dat;
	
	// Error checking
	if (!fruits_image[0] || !fruits_image[1] || !fruits_image[2] || !fruits_image[3]) {
		allegro_message("Error loading fruit sprites");
		return;
	}
	
	// Fruit allocation
	for (i = 0; i < FRUIT_MAX; i++) {
		fruits[i] = malloc(sizeof(SPRITE));	
	}
	
	// Setup fruit sprite locations
	fruits[0]->x = mapblockwidth;
	fruits[0]->y = mapblockheight * 3;
	fruits[0]->width = fruits_image[0]->w;
	fruits[0]->height = fruits_image[0]->h;
	fruits[0]->data = 0;
	fruits[0]->alive = 1;
	
	fruits[1]->x = mapblockwidth * 3;
	fruits[1]->y = mapblockheight * 26;
	fruits[1]->width = fruits_image[1]->w;
	fruits[1]->height = fruits_image[1]->h;
	fruits[1]->data = 1;
	fruits[1]->alive = 1;
	
	fruits[2]->x = mapblockwidth * 116;
	fruits[2]->y = mapblockheight * 7;
	fruits[2]->width = fruits_image[2]->w;
	fruits[2]->height = fruits_image[2]->h;
	fruits[2]->data = 2;
	fruits[2]->alive = 1;
	
	fruits[3]->x = mapblockwidth * 118;
	fruits[3]->y = mapblockheight * 26;
	fruits[3]->width = fruits_image[3]->w;
	fruits[3]->height = fruits_image[3]->h;
	fruits[3]->data = 3;
	fruits[3]->alive = 1;         
    
    // Load green enemy
    temp = (BITMAP *) data[ENEMIES1_BMP].dat; 
    
    // Error checking
	if (!temp) {
		allegro_message("Error loading enemy sprites");
		return;
	}
	
	// Grabbing sprite frames
    for (i = 0; i < 6; i++) {
	    green_enemy_image[i] = grabframe(temp, 16, 16, 0, 0, 6, i);
    	
    }
    
    // Load green enemy
    temp = (BITMAP *) data[ENEMIES2_BMP].dat;
    
    // Error checking
	if (!temp) {
		allegro_message("Error loading enemy sprites");
		return;
	}
	
	// Grabbing sprite frames
    for (i = 0; i < 6; i++) {
	    orange_enemy_image[i] = grabframe(temp, 16, 16, 0, 0, 6, i);
    }
    
    // Load green enemy
    temp = (BITMAP *) data[ENEMIES3_BMP].dat;
    
    // Error checking
	if (!temp) {
		allegro_message("Error loading enemy sprites");
		return;
	}
	
	// Grabbing sprite frames
    for (i = 0; i < 6; i++) {
	    red_enemy_image[i] = grabframe(temp, 16, 16, 0, 0, 6, i);
    }
    
    // Load green enemy
    temp = (BITMAP *) data[ENEMIES4_BMP].dat;
    
    // Error checking
	if (!temp) {
		allegro_message("Error loading enemy sprites");
		return;
	}
	
	// Grabbing sprite frames
    for (i = 0; i < 7; i++) {
	    blue_enemy_image[i] = grabframe(temp, 16, 16, 0, 0, 6, i);
    }
	
	// Initalize enemies
	green_enemy = malloc(sizeof(SPRITE));
    green_enemy->x = ENEMY1_START_X;
    green_enemy->y = ENEMY1_START_Y;
    green_enemy->min_x = ENEMY1_START_X;
    green_enemy->max_x = ENEMY1_END_X;
    green_enemy->dir = 1;
    green_enemy->curframe= 0;
    green_enemy->framecount= 0;
    green_enemy->framedelay= FRAME_DELAY_ENEMY;
    green_enemy->maxframe= 5;
    green_enemy->width= green_enemy_image[0]->w;
    green_enemy->height= green_enemy_image[0]->h;
    green_enemy->alive = 1;
    
    orange_enemy = malloc(sizeof(SPRITE));
    orange_enemy->x = ENEMY2_START_X;
    orange_enemy->y = ENEMY2_START_Y;
    orange_enemy->min_x = ENEMY2_START_X;
    orange_enemy->max_x = ENEMY2_END_X;
    orange_enemy->dir = 1;
    orange_enemy->curframe= 0;
    orange_enemy->framecount= 0;
    orange_enemy->framedelay= FRAME_DELAY_ENEMY;
    orange_enemy->maxframe= 5;
    orange_enemy->width= orange_enemy_image[0]->w;
    orange_enemy->height= orange_enemy_image[0]->h;
    orange_enemy->alive = 1;
    
    red_enemy = malloc(sizeof(SPRITE));
    red_enemy->x = ENEMY3_END_X;
    red_enemy->y = ENEMY3_START_Y;
    red_enemy->min_x = ENEMY3_START_X;
    red_enemy->max_x = ENEMY3_END_X;
    red_enemy->dir = -1;
    red_enemy->curframe= 0;
    red_enemy->framecount= 0;
    red_enemy->framedelay= FRAME_DELAY_ENEMY;
    red_enemy->maxframe= 5;
    red_enemy->width= red_enemy_image[0]->w;
    red_enemy->height= red_enemy_image[0]->h;
    red_enemy->alive = 1;
    
    blue_enemy = malloc(sizeof(SPRITE));
    blue_enemy->x = ENEMY4_END_X;
    blue_enemy->y = ENEMY4_START_Y;
    blue_enemy->min_x = ENEMY4_START_X;
    blue_enemy->max_x = ENEMY4_END_X;
    blue_enemy->dir = -1;
    blue_enemy->curframe= 0;
    blue_enemy->framecount= 0;
    blue_enemy->framedelay= FRAME_DELAY_ENEMY;
    blue_enemy->maxframe= 5;
    blue_enemy->width= blue_enemy_image[0]->w;
    blue_enemy->height= blue_enemy_image[0]->h;
    blue_enemy->alive = 1;
}

// Player walking position and animation update
void walk(int dir) {
	
	// Find direction and move accordingly
	facing = dir; 
    player->x+= 2 * dir; 
    
    // Animate Player w/ Delay
    if (++player->framecount > player->framedelay) {
    	player->framecount=0;
    	if (player->curframe > WALKFRAME_MAX || player->curframe < WALKFRAME_MIN) {
			player->curframe = 	WALKFRAME_MIN;
		}
        if (++player->curframe > WALKFRAME_MAX) {
        	player->curframe=WALKFRAME_MIN;
        }
    }
}

// Enemy walking position and animation update
void walk_enemies(SPRITE *enemy) {
	
	// Generate random number
	int random = rand() % 30;
	
	if (fruit_collected > 0) {
		if (random == 0) {
			enemy->dir = enemy->dir * - 1;
		}
	}
	
	if (fruit_collected > 1) {
		int xdifference = player->x - enemy->x;
		int ydifference = player->y - enemy->y;
		
		if (abs(xdifference) < 75 && abs(ydifference) < 50) {
			if (xdifference > 0) {
				enemy->dir = 1;
			}
			else {
				enemy->dir = -1;
			}
		}
	}
	
	// Check if speed should be fast or not
	if (hardmode == 1) {
		enemy->x+= HARD_MODE * enemy->dir; 
	}
	else {
		enemy->x+= EASY_MODE * enemy->dir;
	}
	
	if (fruit_collected > 2) {
		enemy->x+= enemy->dir;
	}
    
    // Turning point of enemy movement
    if (enemy->x > enemy->max_x || enemy->x < enemy->min_x) {
    	enemy->dir = enemy->dir * -1;
    	
    	if (enemy->x > enemy->max_x) {
    		enemy->x = enemy->max_x;
    	}
    	
    	if (enemy->x < enemy->min_x) {
    		enemy->x = enemy->min_x;
    	}
    }
    
    // Frame delay animation
    if (++enemy->framecount > enemy->framedelay) {
    	enemy->framecount=0;
    	if (++enemy->curframe > enemy->maxframe) {
			enemy->curframe = 0;
		}
    }
}

// Update Enemies
void update_enemies() {
	
	// Position variables
	int x1,y1,x2,y2;
	
	// Player coordinates
	x1 = player->x;
    y1 = player->y;
    x2 = x1 + player->width;
    y2 = y1 + player->height;
	
	// Draw enemies
    if (green_enemy->alive == 1) {
    		
    	// Update enemy position and animation
    	walk_enemies(green_enemy);
    	
		// Get enemy bounding rectangle
		if (inside_box(green_enemy->x + green_enemy->width/2, green_enemy->y + green_enemy->height/2, x1, y1, x2, y2)) {
			play_sample(enemy_die_sound, 128, 128, 1000, FALSE);
			player->alive = 0;
			gameover = 1;
		}

		// Check if within screen to draw
		if (inside(green_enemy)) {
			if (green_enemy->dir == -1) {
				draw_sprite(buffer, green_enemy_image[green_enemy->curframe],
	        		green_enemy->x - mapxoff, green_enemy->y - mapyoff);
			}
			else {
				draw_sprite_h_flip(buffer, green_enemy_image[green_enemy->curframe],
	        		green_enemy->x - mapxoff, green_enemy->y - mapyoff);
			}
    	}
	}
    	
    if (red_enemy->alive == 1) {
    		
    	// Update enemy position and animation
    	walk_enemies(red_enemy);
    	
		//get enemy bounding rectangle
		if (inside_box(red_enemy->x + red_enemy->width/2, red_enemy->y + red_enemy->height/2, x1, y1, x2, y2)) {
			play_sample(enemy_die_sound, 128, 128, 1000, FALSE);
			player->alive = 0;
			gameover = 1;
		}

		// Check if within screen to draw
		if (inside(red_enemy)) {
			if (red_enemy->dir == -1) {
		        draw_sprite(buffer, red_enemy_image[red_enemy->curframe],
		        	red_enemy->x - mapxoff, red_enemy->y - mapyoff);
	    	}
	    	else {
	    		draw_sprite_h_flip(buffer, red_enemy_image[red_enemy->curframe],
		        	red_enemy->x - mapxoff, red_enemy->y - mapyoff);
	    	}
    	}
	}
	
	if (orange_enemy->alive == 1) {
    		
    	// Update enemy position and animation
    	walk_enemies(orange_enemy);
    		
		//get enemy bounding rectangle
		if (inside_box(orange_enemy->x + orange_enemy->width/2, orange_enemy->y + orange_enemy->height/2, x1, y1, x2, y2)) {
			play_sample(enemy_die_sound, 128, 128, 1000, FALSE);
			player->alive = 0;
			gameover = 1;
		}

		// Check if within screen to draw
		if (inside(orange_enemy)) {
			if (orange_enemy->dir == -1) {
	        	draw_sprite(buffer, orange_enemy_image[orange_enemy->curframe],
	        		orange_enemy->x - mapxoff, orange_enemy->y - mapyoff);
	    	}
	    	else {
	    		draw_sprite_h_flip(buffer, orange_enemy_image[orange_enemy->curframe],
	        		orange_enemy->x - mapxoff, orange_enemy->y - mapyoff);
	    	}
    	}
	}
	
	if (blue_enemy->alive == 1) {
    		
    	// Update enemy position and animation
    	walk_enemies(blue_enemy);
    		
		//get enemy bounding rectangle
		if (inside_box(blue_enemy->x + blue_enemy->width/2, blue_enemy->y + blue_enemy->height/2, x1, y1, x2, y2)) {
			play_sample(enemy_die_sound, 128, 128, 1000, FALSE);
			player->alive = 0;
			gameover = 1;
		}

		// Check if within screen to draw
		if (inside(blue_enemy)) {
			if (blue_enemy->dir == -1) {
		        draw_sprite(buffer, blue_enemy_image[blue_enemy->curframe],
		        	blue_enemy->x - mapxoff, blue_enemy->y - mapyoff);
			}
			else {
				draw_sprite_h_flip(buffer, blue_enemy_image[blue_enemy->curframe],
		        	blue_enemy->x - mapxoff, blue_enemy->y - mapyoff);
			}
    	}
	}
}

// Player wait animation
void wait() {
	
	// Update Player animation w/ delay
	if (++player->framecount > player->framedelay) {
    	player->framecount=0;
		if (player->curframe > WAITFRAME_MAX || player->curframe < WAITFRAME_MIN) {
			player->curframe = 	WAITFRAME_MIN;
		}
		else if (++player->curframe > WAITFRAME_MAX) {
			player->curframe = WAITFRAME_MIN;
		}
	}
}

// General update function
void update() {
	
	// Loop variable
	int i;
	
	// Position variables
	int x1,y1,x2,y2;
	
	// Old coordinates
	oldpy = player->y; 
    oldpx = player->x;
	
	// Handle jumping
    if (jump==JUMPIT)
    { 
        if (!map_collided(player->x + player->width/2, 
            player->y + player->height + 5))
            jump = 0; 
    }
    else
    {
    	player->curframe = JUMPFRAME;
        player->y -= jump/3; 
        jump--; 
    }

	if (jump<0) { 
        if (map_collided(player->x + player->width/2, 
            player->y + player->height))
		{ 
            jump = JUMPIT; 
            while (map_collided(player->x + player->width/2, 
                player->y + player->height))
                player->y -= 2; 
        } 
    }
	
	// Check for collided with foreground tiles
	if (facing == -1) { 
        if (map_collided(player->x, player->y + player->height)) 
            player->x = oldpx; 
    }
	else { 
        if (map_collided(player->x + player->width, 
            player->y + player->height)) 
            player->x = oldpx; 
    }
	
    // Update the map scroll position
	mapxoff = player->x + player->width/2 - WIDTH/2 + 10;
	mapyoff = player->y + player->height/2 - HEIGHT/2 + 10;


    // Avoid moving beyond the map edge
	if (mapxoff < 0) {
		mapxoff = 0;
	}
	if (mapxoff > (mapwidth * mapblockwidth - WIDTH)) {
		mapxoff = mapwidth * mapblockwidth - WIDTH;
	}
	if (mapyoff < 0) {
        mapyoff = 0;
    }
	if (mapyoff > (mapheight * mapblockheight - HEIGHT)) {
        mapyoff = mapheight * mapblockheight - HEIGHT;
    }
    
    // Avoid player moving beyoard map edge (left and right)
    if (player->x < 0) {
    	player->x = 0;	
    }
    if (player->x > (mapwidth * mapblockwidth - player->width)) {
    	player->x = (mapwidth * mapblockwidth - player->width);
    }
    if ((player->y + player->height) > (mapheight * mapblockheight - (mapblockheight / 2))) {
    	play_sample(splash_sound, 128, 128, 1000, FALSE);
    	gameover = 1;
    	return;
    }
    if (player->y < 0) {
    	player->y = 0;	
    }

	if (oldpy == player->y & oldpx == player->x) {
		waitcount++;
		
		if (waitcount > WAITCOUNT_MAX) {
			wait();	
			waitcount = 0;
		}
	}
	else {
		waitcount = 0;	
	}

    // Draw the background tiles
	MapDrawBG(buffer, mapxoff, mapyoff, 0, 0, WIDTH-1, HEIGHT-1);

    // Draw foreground tiles
	MapDrawFG(buffer, mapxoff, mapyoff, 0, 0, WIDTH-1, HEIGHT-1, 0);

	// Fruit Collection Status
	rect(buffer, WIDTH - 156, 1, WIDTH - 4, 41, BLACK);
	rect(buffer, WIDTH - 155, 2, WIDTH - 5, 40, BLACK);
	textout_ex(buffer, font, "FRUIT COLLECTED: ", WIDTH - 150, 5, BLACK, -1);

	// Player coordinates
	x1 = player->x;
    y1 = player->y;
    x2 = x1 + player->width;
    y2 = y1 + player->height;

	// Draw fruit
    for (i = 0; i < FRUIT_MAX; i++) {
    	
    	if (fruits[i]->alive == 1) {
    		
			//get fruit bounding rectangl
    		if (inside_box(fruits[i]->x + fruits[i]->width/2, fruits[i]->y + fruits[i]->height/2, x1, y1, x2, y2)) {
    			play_sample(fruit_collect_sound, 128, 128, 1000, FALSE);
    			fruits[i]->alive = 0;
    			fruit_collected++;
    		}

    		if (inside(fruits[i])) {
		        draw_sprite(buffer, fruits_image[i],
		        fruits[i]->x - mapxoff, fruits[i]->y - mapyoff);
	    	}
    	}
    	else {
    		draw_sprite(buffer, fruits_image[i],
		        WIDTH - 125 + 25 * i, 15);
    	}
    }
    
    // Update enemies
    update_enemies();

    // Draw the player's sprite
	if (facing == 1) {
		if (player->y > player->width * -1) {
			draw_sprite(buffer, player_image[player->curframe], 
            	(player->x-mapxoff), (player->y-mapyoff+1));
		}
    }
	else {
		if (player->y > player->width * -1) {
        draw_sprite_h_flip(buffer, player_image[player->curframe], 
            (player->x-mapxoff), (player->y-mapyoff));
        }    
    }
    
    // Update sound cooldown
    if (sound_cooldown > 0) {
    	sound_cooldown--;
    }
}

// Get game input from user
void getinput() {
	
    //hit ESC to quit
    if (key[KEY_ESC]) {
		allegro_exit();
		exit(0);
	}
	
	// Instruction screen
	if (key[KEY_LCONTROL] && key[KEY_H]) {
		instructions();
	}
	
	// Background music toggle
	if (key[KEY_LCONTROL] && key[KEY_M]) {
		if (sound_cooldown == 0) {
			sound_cooldown = COOLDOWN;
			if (sound == 1) {
				sound = 0;
				stop_sample(background_music);
			}
			else {
				sound = 1;
				play_sample(background_music, 128, 128, 1000, TRUE);
			}
		}
	}
    
    //ARROW KEYS AND SPACE BAR CONTROL
    if (key[KEY_LEFT]) {
		walk(-1);
	}	
    else if (key[KEY_RIGHT]) {
		walk(1);
	}
	
	
    if (key[KEY_SPACE]) {
    	if (jump==JUMPIT) {
    		play_sample(jump_sound, 128, 128, 1000, FALSE);
    		jump = 30;
    	}
	}
}

int main(void) {

	// Looping variable
	int i;

	//initialize Allegro
    allegro_init(); 
    
    //initialize the keyboard
    install_keyboard(); 
    
    //initalize timer
    install_timer();
    
    //initialize random seed
    srand(time(NULL));
    
    // Set Video mode
    set_color_depth(16);
    
    // Error variable
    int ret;
    
    // Graphics Error checking
    ret = set_gfx_mode(MODE, WIDTH, HEIGHT, 0, 0);
    if (ret != 0) {
	    allegro_message(allegro_error);
	    return;
	}
    
    //load the datfile
    data = load_datafile("data.dat");
    
    // Setup screen
    setupscreen();

	setupgame();

    // Main loop
	while (!gameover)
	{

		// Check for keypresses
        if (keypressed()) {
            getinput();
		}

		// Update
		update();

		// Check win condition
		if (fruit_collected == 4) {
			win = 1;
			gameover = 1;	
		}

        // Blit the double buffer 
		vsync();
        acquire_screen();
		blit(buffer, screen, 0, 0, 0, 0, WIDTH-1, HEIGHT-1);
        release_screen();

		rest(10);
	}
	
	// Clear Screen
    rectfill(screen, 0, 0, WIDTH, HEIGHT, BLACK);
    
    // Stop background
    stop_sample(background_music);
    
    // Print final result w/ Sound
	if (win == 1) {
		play_sample(game_win_sound, 128, 128, 1000, FALSE);
		textout_centre_ex(screen, font, "Congratulations! You have collected all the fruit!", WIDTH/2, HEIGHT/2, WHITE, BLACK);
	}
	else {
		play_sample(game_over_sound, 128, 128, 1000, FALSE);
		textprintf_centre_ex(screen, font, WIDTH/2, HEIGHT/2, WHITE, BLACK, "Sorry you have only collected %i fruit. Please try again!", fruit_collected);
	}

	textout_centre_ex(screen, font, "Please Press ESC to QUIT!", WIDTH/2, HEIGHT/2 + 20, WHITE, BLACK);

	// Slow down game
    rest(250);
    
    // Wait until user exits game
    while(!key[KEY_ESC]) {
	};

	// Cleanup
    free(player);
    free(red_enemy);
	free(orange_enemy);
	free(blue_enemy);
	free(green_enemy);
    
	destroy_bitmap(buffer);
	destroy_bitmap(title);
	destroy_bitmap(blue_enemy_image[6]);
	
	for (i = 0; i < PLAYER_MAX_FRAME; i++) {
    	destroy_bitmap(player_image[i]);
    } 
	
	for (i = 0; i < FRUIT_MAX; i++) {
		destroy_bitmap(fruits_image[i]);
		free(fruits[i]);
	}
	
	for (i = 0; i < 6; i++) {
		destroy_bitmap(orange_enemy_image[i]);
		destroy_bitmap(red_enemy_image[i]);
		destroy_bitmap(blue_enemy_image[i]);
		destroy_bitmap(green_enemy_image[i]);
	}
	
	destroy_sample(background_music);
	destroy_sample(click_sound);
	destroy_sample(jump_sound);
	destroy_sample(fruit_collect_sound);
	destroy_sample(enemy_die_sound);
	destroy_sample(game_over_sound);
	destroy_sample(splash_sound);
	destroy_sample(game_win_sound);
	
	MapFreeMem();
	allegro_exit();
	return 0;
}
END_OF_MAIN()
