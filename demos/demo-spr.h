
#include "unity.h"
#include <cc65.h>

#if defined __APPLE2__
	unsigned char spriteColors[] = { BLUE, RED, GREEN, YELLOW };    //  Note: colors are pre-assigned in the sprite sheet
#elif defined __ATARI__
	unsigned char spriteColors[] = { BLUE, RED, GREEN, YELLOW, GREY };  //  Note: 5th color is shared between all sprites
#elif defined __ATMOS__
	unsigned char spriteColors[] = { CYAN, MBLUE, LGREEN, GREY };
#elif defined __CBM__
	unsigned char spriteColors[] = { BLUE, RED, GREEN, YELLOW, 0, 0, 0, 0 };	// Main sprite color
	unsigned char sharedColors[] = { CYAN, BLACK };		// Shared sprite colors
#endif

unsigned char buffer[20];

int DemoSPR(void) 
{
	unsigned char i, frame;
	unsigned int xpos, ypos, angle;
	clock_t timer = clock();

	// Initialize sfx and bitmap
	InitSFX();
	InitBitmap();
	LoadBitmap("stadium.map");
	EnterBitmapMode();
	
	// Print some extra info
	paperColor = GREY; 
	inkColor = BLACK; 
	PrintStr(0, LAST_LINE, "STADIUM");	
	paperColor = BLACK; 
	for (i=0; i<4; i++) {
		inkColor = spriteColors[i]; 
		PrintLogo(8*(i+1)+5, LAST_LINE, i);
		PrintNum(8*(i+1)+2, LAST_LINE, i+1);
		PrintStr(8*(i+1), LAST_LINE, "PL");
	}
	
	// Initialize sprites
#if defined __APPLE2__
	// number of rows, number of frames
	InitSprites(5, 64);
#elif defined __ATARI__
	// number of rows, unique colors of sprites 0-4
	InitSprites(13, spriteColors);
#elif defined __ATMOS__
	// number of rows, unique colors of sprites 0-3
	InitSprites(8, spriteColors);
#elif defined __CBM__
	// unique colors of sprites 0-7, shared colors of all sprites 
	InitSprites(spriteColors, sharedColors);
#endif

	// Enable sprites
	for (i=0; i<4; i++) {
		EnableSprite(i);
#if defined __ATARI__
		EnableSprite(4+i);	// Flicker sprites used for 2nd color (tires)
#endif
	}

	// Animate sprites
	while (!kbhit()) {
#if defined __APPLE2__
		tick();	// Apple 2 has no clock, so simulate ticks!
#endif
        // Update sprites position
        if (clock()>timer) {
            timer = clock();
            angle += 3;
            for (i=0; i<4; i++) {
                xpos = 230+cc65_cos((angle+i*90)%360)/5;
                ypos = 90+cc65_sin((angle+i*90)%360)/6;
                frame = ((12-(angle+(i+1)*90))%360)/23;
				LocateSprite(xpos, ypos);
#if defined __APPLE2__
				UpdateSprite(i, (i*16)+frame);	// Point to sprite data associated with each player color
#elif defined __ATARI__
				UpdateSprite(i, frame);			// Normal sprite for 1st color (body)
				UpdateSprite(4+i, 16+frame);	// Flicker sprite for 2nd color (tires)	
#elif defined __ATMOS__
				UpdateSprite(i, frame);			// Single coloured sprite (colour applies to second scan line)
#elif defined __CBM__
				UpdateSprite(i, frame);			// Single sprite including 1 unique + 2 shared colors
#endif
				EngineSFX(1, 300);
            }       
        }
	}
	
	// Black-out screen and clear key
	DisableSprite(-1);	// "-1" disables all sprites
	ExitBitmapMode();
	StopSFX();
	cgetc();
	
    // Done
    return EXIT_SUCCESS;	
}