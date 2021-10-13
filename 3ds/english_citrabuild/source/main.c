// Darts And Bubbles
// Half-Qilin AKA Littens4Life

#include <citro2d.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SCREEN_WIDTH  400
#define BOTTOM_SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define MAX_ACTIVE_DARTS 20
#define MAX_BUBBLES 40
#define MAX_POPPING_BUBBLES 10

// Darts and Bubbles
typedef struct
{
    C2D_Sprite spr;
    float xVel, yVel, dir, initSpeed;
    float lastX, lastY;
    bool isActive;
} Dart;
typedef struct
{
    C2D_Sprite spr;
    int bubbleType;
} Bubble;
typedef struct
{
    C2D_Sprite spr;
    int bubbleType, popTimer;
} PoppingBubble;
typedef enum
{
    inMenu,
    inInfiniteGame,
    gameOver
} GameStates;

static C2D_SpriteSheet dartSheet;
static C2D_SpriteSheet firingSheet;
static C2D_SpriteSheet nullBubbleSheet;
static C2D_SpriteSheet blueSheet;
static C2D_SpriteSheet yellowSheet;
static C2D_SpriteSheet purpleSheet;
static C2D_SpriteSheet redSheet;
static C2D_SpriteSheet buttonSheet;
static C2D_SpriteSheet buttonOutlineSheet;
static C2D_SpriteSheet cornerSheet;
static C2D_Font regularFont;
static C2D_Font lightFont;
static C2D_Font mediumFont;
static C2D_Font boldFont;
static float pi = acos(-1.0);
static Dart darts[MAX_ACTIVE_DARTS];
static size_t activeDarts = 0;
static Bubble bubbles[MAX_BUBBLES];
static size_t numBubbles = 0;
static PoppingBubble poppingBubbles[MAX_POPPING_BUBBLES];
static size_t numPoppingBubbles = 0;
wideModifier = 1;

int sign (float a) {
    int b = a > 0 ? 1 : 0;
    if (a < 0) b--;
    return b;
}

void remove_dart(Dart array[MAX_ACTIVE_DARTS], int index, int array_length) {
    int i;
    for(i = index; !(i > array_length); i++) array[i] = array[i + 1];
}
void remove_bubble(Bubble array[MAX_BUBBLES], int index, int array_length) {
    int i;
    for(i = index; !(i > array_length); i++) array[i] = array[i + 1];
}
void remove_poppingbubble(PoppingBubble array[MAX_POPPING_BUBBLES], int index, int array_length) {
    int i;
    for(i = index; !(i > array_length); i++) array[i] = array[i + 1];
}

void popBubble (C2D_Sprite spr, int bubbleType) {
    PoppingBubble* popBubble = &poppingBubbles[numPoppingBubbles];
    popBubble->spr = spr;
    popBubble->bubbleType = bubbleType;
    popBubble->popTimer = 0;
    C2D_SpriteSetDepth(&popBubble->spr, 0.0f);
    numPoppingBubbles++;
}

int makeBubble (float level) {
    if (numBubbles < MAX_BUBBLES) {
        Bubble* bubble = &bubbles[numBubbles];
        bubble->bubbleType = (int)floor(fmodf((float)rand(), (float)sqrt(sqrt(level))));
        if (bubble->bubbleType < 0) {
            bubble->bubbleType = 0;
        }
        if (bubble->bubbleType > 2) {
            bubble->bubbleType = 2;
        }
        switch (bubble->bubbleType) {
        case 0:
        {
            C2D_SpriteFromSheet(&bubble->spr, blueSheet, 0);
            break;
        }
        case 1:
        {
            C2D_SpriteFromSheet(&bubble->spr, yellowSheet, 0);
            break;
        }
        case 2:
        {
            C2D_SpriteFromSheet(&bubble->spr, purpleSheet, 0);
            break;
        }
        case 3:
        {
           C2D_SpriteFromSheet(&bubble->spr, redSheet, 0);
            break;
        }
        default:
            C2D_SpriteFromSheet(&bubble->spr, nullBubbleSheet, 0);
        }
        C2D_SpriteSetCenter(&bubble->spr, 0.5f, 0.5f);
        C2D_SpriteSetPos(&bubble->spr, (SCREEN_WIDTH/2 + 20) + floor(fmodf(rand()/10.0f, SCREEN_WIDTH/2-40)*wideModifier)/wideModifier, SCREEN_HEIGHT+20);
        C2D_SpriteScale(&bubble->spr, 2.0f, 2.0f);
        C2D_SpriteSetDepth(&bubble->spr, 0.4f);
        numBubbles++;
        switch (bubble->bubbleType) {
        case 1:
            return -10 / sqrt(sqrt(level));
        case 2:
            return -60 / sqrt(sqrt(level));
        default:
            return 0;
        }
    }
    return 0;
}

void makeBubbleMenu () {
    if (numBubbles < MAX_BUBBLES) {
        Bubble* bubble = &bubbles[numBubbles];
        bubble->bubbleType = (int)floor(fmodf((float)rand(), pi));
        if (bubble->bubbleType < 0) {
            bubble->bubbleType = 0;
        }
        if (bubble->bubbleType > 2) {
            bubble->bubbleType = 2;
        }
        switch (bubble->bubbleType) {
        case 0:
        {
            C2D_SpriteFromSheet(&bubble->spr, blueSheet, 0);
            break;
        }
        case 1:
        {
            C2D_SpriteFromSheet(&bubble->spr, yellowSheet, 0);
            break;
        }
        case 2:
        {
            C2D_SpriteFromSheet(&bubble->spr, purpleSheet, 0);
            break;
        }
        default:
            C2D_SpriteFromSheet(&bubble->spr, nullBubbleSheet, 0);
        }
        C2D_SpriteSetCenter(&bubble->spr, 0.5f, 0.5f);
        C2D_SpriteSetPos(&bubble->spr, 23 + floor(fmodf(rand()/10.0f, SCREEN_WIDTH-46)*wideModifier)/wideModifier, SCREEN_HEIGHT+20);
        C2D_SpriteScale(&bubble->spr, 2.0f, 2.0f);
        C2D_SpriteSetDepth(&bubble->spr, 0.4f);
        numBubbles++;
    }
}
int makeBubbleType (int type) {
    if (numBubbles < MAX_BUBBLES) {
        Bubble* bubble = &bubbles[numBubbles];
        bubble->bubbleType = type;
        switch (bubble->bubbleType) {
        case 0:
        {
            C2D_SpriteFromSheet(&bubble->spr, blueSheet, 0);
            break;
        }
        case 1:
        {
            C2D_SpriteFromSheet(&bubble->spr, yellowSheet, 0);
            break;
        }
        case 2:
        {
            C2D_SpriteFromSheet(&bubble->spr, purpleSheet, 0);
            break;
        }
        case 3:
        {
    	   C2D_SpriteFromSheet(&bubble->spr, redSheet, 0);
            break;
        }
        default:
            C2D_SpriteFromSheet(&bubble->spr, nullBubbleSheet, 0);
        }
        C2D_SpriteSetCenter(&bubble->spr, 0.5f, 0.5f);
        C2D_SpriteSetPos(&bubble->spr, (SCREEN_WIDTH/2 + 20) + floor(fmodf(rand()/10.0f, SCREEN_WIDTH/2-40)*wideModifier)/wideModifier, SCREEN_HEIGHT+20);
        C2D_SpriteScale(&bubble->spr, 2.0f, 2.0f);
        C2D_SpriteSetDepth(&bubble->spr, 0.4f);
        numBubbles++;
    }
    return 0;
}


void makeDart (float dir, float x, float y, float speed) {
    Dart* dart = &darts[activeDarts];
    C2D_SpriteFromSheet(&dart->spr,	dartSheet, 1);
    C2D_SpriteSetCenter(&dart->spr, 0.5f, 0.5f);
    C2D_SpriteScale(&dart->spr, 2.0f, 2.0f);
    C2D_SpriteSetPos(&dart->spr, x, y);
    C2D_SpriteSetRotation(&dart->spr, dir);
    C2D_SpriteSetDepth(&dart->spr, 0.5f);
    dart->yVel = sin(dir) * speed;
    dart->xVel = cos(dir) * speed;
    dart->initSpeed = speed;
    dart->isActive = true;
    activeDarts++;
}

long long doMovement (long long score, unsigned long level) {
    // Move darts
    for (size_t i = 0; i < activeDarts; i++) {
        Dart* dart = &darts[i];
        C2D_SpriteMove(&dart->spr, dart->xVel, dart->yVel);
        float xLoc = dart->spr.params.pos.x;
        float yLoc = dart->spr.params.pos.y;
        dart->dir = atan(dart->yVel / dart->xVel);
        if (dart->xVel < 0) dart->dir += pi;
        if (dart->xVel == 0) {
            if (dart-> yVel > 0) dart->dir = pi / 2;
            else dart->dir = - pi / 2;
        }
        C2D_SpriteSetRotation(&dart->spr, dart->dir);
        dart->yVel += 0.1f;
        if ((xLoc < -10.0f) || (xLoc > SCREEN_WIDTH + 10.0f) || (yLoc > SCREEN_HEIGHT + 10.0f)) {
            remove_dart(&darts, i, activeDarts);
            activeDarts--;
            i--;
            continue;
        }
    }

    // Move bubbles
    int bubbleType = 0;
    for (size_t i = 0; i < numBubbles; i++) {
        Bubble* bubble = &bubbles[i];
        bubbleType = bubble->bubbleType;
        C2D_SpriteMove(&bubble->spr, 0, -4.0f/3.0f);
        if (bubble->spr.params.pos.y < -20) {
            remove_bubble(&bubbles, i, numBubbles);
            switch (bubbleType) {
                case 3: {
                    score += ceil(sqrt(level) * 2.0f);
                }
                case 2: {
                    score -= floor(sqrt(sqrt(level)) * 2.0f);
                    break;
                }
                default: score -= floor(sqrt(sqrt(level)));
            }
            numBubbles--;
            i--;
        }
    }
    
    return score;
}

void updateAnimation() {
    // Change the popped bubble images
    for (size_t i = 0; i < numPoppingBubbles; i++) {
        PoppingBubble* bubble = &poppingBubbles[i];
        int x = bubble->spr.params.pos.x;
        int y = bubble->spr.params.pos.y;
        bubble->popTimer++;
        if (bubble->popTimer % 3 == 0) {
            if (floor(bubble->popTimer/5) > 1) {
                remove_poppingbubble(&poppingBubbles, i, numPoppingBubbles);
                numPoppingBubbles--;
                i--;
                continue;
            }
            switch (bubble->bubbleType) {
            case 0:
            {
                C2D_SpriteFromSheet(&bubble->spr, blueSheet, floor(bubble->popTimer/5+1));
                break;
            }
            case 1:
            {
                C2D_SpriteFromSheet(&bubble->spr, yellowSheet, floor(bubble->popTimer/5+1));
                break;
            }
            case 2:
            {
                C2D_SpriteFromSheet(&bubble->spr, purpleSheet, floor(bubble->popTimer/5+1));
                break;
            }
            case 3:
            {
                C2D_SpriteFromSheet(&bubble->spr, redSheet, floor(bubble->popTimer/5+1));
                break;
            }
            default:
                C2D_SpriteFromSheet(&bubble->spr, nullBubbleSheet, floor(bubble->popTimer/5+1));
            }
            C2D_SpriteSetCenter(&bubble->spr, 0.5f, 0.5f);
            C2D_SpriteSetPos(&bubble->spr, x, y);
            C2D_SpriteScale(&bubble->spr, 2.0f, 2.0f);
        }
    }
}

int main(int argc, char* argv[]) {
    // Init libs
    romfsInit();
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    srand(time(NULL));

    // Global vars
    GameStates state = inMenu;
    GameStates switchTo = inMenu;
    touchPosition touch;
    C2D_TextBuf textBuffer = C2D_TextBufNew(100);
    bool initialSwitchStuff = false;

    // Static vars
    C2D_Text startGameText;
    C2D_Text exitText;
    C2D_TextBuf staticTextBuffer = C2D_TextBufNew(64);
    C2D_Text gameOverText;
    C2D_Text introText1;
    C2D_Text introText2;
    C2D_Text introText3;
    C2D_Text halfQilinText;
    unsigned int saveVersion = 0;
    unsigned int currentSaveVersion = 4;

    // Game vars
    long double timer = 0.0f;
    float dir = 0.0f;
    int framesSinceLastBubble = 0;
    unsigned long level = 1;
    long long score = 0;
    long long highScore = 0;
    long long gameHighScore = 0;
    float tX = 0;
    float tY = 0;
    float dX = 0;
    float dY = 0;
    float dirX = 0;
    float dirY = 0;
    float dist = 0;
    float dist_temp = 0;
    int redBubbleDartsShot = 0;
    bool updatedPosition = false;
    float linetimer = 0;
    C2D_Text scoreText;
    C2D_Text levelText;
    float textWidth;
    float textHeight;
    char intconvert[40];
    unsigned long long totalDartsShot = 0;
    unsigned long long dartsShot = 0;
    C2D_Sprite startDartSprite;

    // Menu vars
    bool isTopHighlighted = false;
    bool isExitHighlighted = false;
    bool exitGame = false;
    C2D_Text highScoreText;
    C2D_Text dartsText;
    int exitGameTimer = 0;
    
    // Game Over screen stuff
    bool resetGameTap = false;
    C2D_Text sessionScoreText;
    C2D_Text exitGOText;

    // Save data
    FILE* saveData = fopen("saves/bubblesAndDarts.txt","r");

    if (saveData != NULL && fscanf(saveData, "%u~", &saveVersion) == 1) {
    	if (saveVersion == 2) {
    		if (fscanf(saveData, "%x|", &highScore) == 1) {
            	highScore = ~highScore;
        	}
    	}
        if (saveVersion == 3) {
            if (fscanf(saveData, "%llu|", &highScore) == 1) {
                highScore = ~highScore;
            }
        }
        if (saveVersion == 4) {
            if (fscanf(saveData, "%llx|", &highScore) == 1) {
                highScore = ~highScore;
            }
        }
        if (saveVersion >= 2) {
            if (fscanf(saveData, "%llu;", &totalDartsShot) == 1) {
                totalDartsShot = ~totalDartsShot;
            }
        }
    }
    fclose(saveData);
    saveData = fopen("saves/bubblesAndDarts.txt", "w");
    fprintf(saveData, "%u~%llx|%llu;", currentSaveVersion, ~highScore, ~totalDartsShot);
    fclose(saveData);

    // Wide mode (not needed)

    // Create screens
    C3D_RenderTarget* top_main = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    // Create tints
    C2D_ImageTint highlightedTint;
    C2D_PlainImageTint(&highlightedTint, C2D_Color32(0, 254, 255, 255), 1.0f);
    C2D_ImageTint blackTint;
    C2D_PlainImageTint(&blackTint, C2D_Color32(0, 0, 0, 255), 1.0f);

    // Load graphics
    dartSheet = C2D_SpriteSheetLoad("romfs:/gfx/dart.t3x");
    firingSheet = C2D_SpriteSheetLoad("romfs:/gfx/dart_firing.t3x");
    blueSheet = C2D_SpriteSheetLoad("romfs:/gfx/blueBubbles.t3x");
    yellowSheet = C2D_SpriteSheetLoad("romfs:/gfx/yellowBubbles.t3x");
    purpleSheet = C2D_SpriteSheetLoad("romfs:/gfx/purpleBubbles.t3x");
    redSheet = C2D_SpriteSheetLoad("romfs:/gfx/redBubbles.t3x");
    nullBubbleSheet = C2D_SpriteSheetLoad("romfs:/gfx/nullBubbles.t3x");
    buttonSheet = C2D_SpriteSheetLoad("romfs:/gfx/menuButtons.t3x");
    buttonOutlineSheet = C2D_SpriteSheetLoad("romfs:/gfx/menuButtonOutlines.t3x");
    cornerSheet = C2D_SpriteSheetLoad("romfs:/gfx/corners.t3x");
    if (!dartSheet || !firingSheet || !blueSheet || !yellowSheet || !purpleSheet || !nullBubbleSheet || !buttonSheet || !buttonOutlineSheet || !redSheet || !cornerSheet) svcBreak(USERBREAK_PANIC);

    // Load fonts
    regularFont = C2D_FontLoad("romfs:/gfx/Farro-Regular.bcfnt");
    lightFont = C2D_FontLoad("romfs:/gfx/Farro-Light.bcfnt");
    mediumFont = C2D_FontLoad("romfs:/gfx/Farro-Medium.bcfnt");
    boldFont = C2D_FontLoad("romfs:/gfx/Farro-Bold.bcfnt");
    if (!regularFont || !lightFont || !boldFont || !mediumFont) svcBreak(USERBREAK_PANIC);

    // Static text
    C2D_TextFontParse(&exitText, mediumFont, staticTextBuffer, "Exit");
    C2D_TextFontParse(&startGameText, mediumFont, staticTextBuffer, "Play Game");
    C2D_TextFontParse(&gameOverText, mediumFont, staticTextBuffer, "Game Over");
    C2D_TextFontParse(&exitGOText, regularFont, staticTextBuffer, "Tap to return to menu");
    C2D_TextFontParse(&introText1, boldFont, staticTextBuffer, "Bubbles");
    C2D_TextFontParse(&introText2, boldFont, staticTextBuffer, "and");
    C2D_TextFontParse(&introText3, boldFont, staticTextBuffer, "Darts");
    C2D_TextFontParse(&halfQilinText, regularFont, staticTextBuffer, "By Half-Qilin");

    // Main loop
    while (aptMainLoop())
    {
        timer += 1 / 60.0;
        hidScanInput();
        hidTouchRead(&touch);

        // Get input data
        u32 kDown = hidKeysDown();
        u32 kHeld = hidKeysHeld();
        u32 kUp = hidKeysUp();

        switch (state) {
            case gameOver: {
                if (initialSwitchStuff) {
                    resetGameTap = false;
                    initialSwitchStuff = false;
                }
                if (kDown & KEY_TOUCH) resetGameTap = true;
                if (kUp & KEY_TOUCH) {
                    if (resetGameTap) {
                        switchTo = inMenu;
                    }
                }
                break;
            }
        case inInfiniteGame: {
            // In Game
            // Dart spawing logic
            if (kDown & KEY_TOUCH) {
                tX = touch.px;
                tY = touch.py;
                updatedPosition = false;
            }
            linetimer -= 0.1;
            if (kHeld & KEY_TOUCH) {
                dX = touch.px-tX;
                dY = touch.py-tY;
                dist_temp = sqrt(dX*dX+dY*dY);
                if (dist_temp > 200) dist_temp = 200;
                if (dist_temp > 3.75) {
                    updatedPosition = true;
                    dir = atan(dY/dX);
                    dist = dist_temp;
                    if (dX < 0) dir += pi;
                    C2D_SpriteSetRotation(&startDartSprite, dir);
                    dirX = cos(dir);
                    dirY = sin(dir);
                }
                linetimer = 1;
    			C2D_SpriteFromSheet(&startDartSprite, firingSheet, floor(sqrt(dist_temp) / sqrt(199.0f) * 4));
            } else {
    			C2D_SpriteFromSheet(&startDartSprite, firingSheet, 0);
            }
    		C2D_SpriteSetCenter(&startDartSprite, 0.5f, 0.5f);
   			C2D_SpriteScale(&startDartSprite, 2.0f, 2.0f);
    		C2D_SpriteSetPos(&startDartSprite, 24, SCREEN_HEIGHT-24);
    		C2D_SpriteSetRotation(&startDartSprite, dir);
    		C2D_SpriteSetDepth(&startDartSprite, 1.0f);
            if ((kUp & KEY_TOUCH) && activeDarts < MAX_ACTIVE_DARTS) {
                makeDart(dir, 24.0f, SCREEN_HEIGHT-24.0f, sqrt(dist));
                redBubbleDartsShot++;
                dartsShot++;
                totalDartsShot++;
            	// Make a red bubble after some shots, randomly
            	if (redBubbleDartsShot > (rand()%200)+100) {
            		makeBubbleType(3);
            		redBubbleDartsShot -= 200;
            		framesSinceLastBubble = 10;
            	}
            }

            // Spawn a bubble randomly
            framesSinceLastBubble++;
            if (framesSinceLastBubble > floor((rand()%600)+60/sqrt(level))) {
                framesSinceLastBubble = makeBubble(level);
            }

            score = doMovement(score, level);

            // Do bubble collisions
            for (size_t i = 0; i < numBubbles; i++) {
                Bubble* bubble = &bubbles[i];
                int x = bubble->spr.params.pos.x;
                int y = bubble->spr.params.pos.y;
                int radius = 16;
                if (bubble->bubbleType == 3) radius = 20;
                for (size_t j = 0; j < activeDarts; j++) {
                    Dart* dart = &darts[j];
                    int dx = dart->spr.params.pos.x;
                    int dy = dart->spr.params.pos.y;
                    float distX = (x-(dx+(float)cos(dart->dir)));
                    float distY = (y-(dy+(float)sin(dart->dir)));
                    float distT = sqrt(distX * distX+distY * distY);
                    if (!(dart->isActive)) {
                        if (distT - radius < 5) {
                            C2D_SpriteMove(&dart->spr, 0, -4.0f/3.0f);
                            dart->isActive = false;
                            dart->dir = sqrt(dart->xVel*dart->xVel+dart->yVel*dart->yVel);
                            dart->xVel = -distX/distT*(dart->dir)*4/5;
                            dart->yVel = -distY/distT*(dart->dir)*4/5;
                            dart->dir = atan(dart->yVel / dart->xVel);
                            if (dart->xVel < 0) dart->dir += pi;
                            C2D_SpriteSetRotation(&dart->spr, dart->dir);
                        }
                    } else if (distT - radius < 5) {
                        switch (bubble->bubbleType) {
                        case 0: {
                            score += 2;
                            break;
                        }
                        case 1: {
                            score += 3;
                            break;
                        }
                        case 2: {
                            score += 2;
                            break;
                        }
                        case 3: {
                        	score -= floor(sqrt(level));
                        	break;
                        }
                        default:
                            score += 1;
                        }
                        popBubble(bubble->spr, bubble->bubbleType);
                        if (bubble->bubbleType != 0) {
                            C2D_SpriteFromSheet(&dart->spr,	dartSheet, 2);
                            C2D_SpriteScale(&dart->spr, 2.0f, 2.0f);
                            C2D_SpriteSetCenter(&dart->spr, 0.5f, 0.5f);
                            C2D_SpriteSetPos(&dart->spr, dx, dy-4.0f/3.0f);
                            dart->isActive = false;
                            dart->dir = sqrt(dart->xVel*dart->xVel+dart->yVel*dart->yVel);
                            dart->xVel = -distX/distT*(dart->dir)*4/5;
                            dart->yVel = -distY/distT*(dart->dir)*4/5;
                            dart->dir = atan(dart->yVel / dart->xVel);
                            if (dart->xVel < 0) dart->dir += pi;
                            C2D_SpriteSetRotation(&dart->spr, dart->dir);
                            C2D_SpriteSetDepth(&dart->spr, 0.5f);
                        }
                        if (bubble->bubbleType != 2) {
                            remove_bubble(&bubbles, i, numBubbles);
                            numBubbles--;
                            i--;
                            break;
                        } else {
                            C2D_SpriteFromSheet(&bubble->spr, blueSheet, 0);
                            bubble->bubbleType = 0;
                            C2D_SpriteScale(&bubble->spr, 2.0f, 2.0f);
                            C2D_SpriteSetCenter(&bubble->spr, 0.5f, 0.5f);
                            C2D_SpriteSetPos(&bubble->spr, x, y);
                            C2D_SpriteSetDepth(&bubble->spr, 0.4f);
                            break;
                        }
                    }
                }
            }

            // Scoring logic
            if (score > gameHighScore) {
                gameHighScore = score;
            }
            if (level * 30 <= gameHighScore) {
                level = floor(gameHighScore/30) + 1;
            }
            if (gameHighScore > highScore) {
                highScore = gameHighScore;
            }
            if (level >= 25) {
                if (score <= (((signed long long)gameHighScore) - ceil(sqrt(sqrt(level))))) {
                    switchTo = gameOver;
                    break;
                }
            } else {
                if (score <= ((signed long long)gameHighScore) - 5) {
                    switchTo = gameOver;
                    break;
                }
            }

            updateAnimation();
            
            break;
        }
        case inMenu: {
        	if (initialSwitchStuff) {
        		// Save
        		saveData = fopen("saves/bubblesAndDarts.txt", "w");
    			fprintf(saveData, "%u~%llx|%llu;", currentSaveVersion, ~highScore, ~totalDartsShot);
    			fclose(saveData);
    			initialSwitchStuff = false;
                // Clear bubbles and darts
                for (size_t i = 0; i < activeDarts; i++) {
                    remove_dart(&darts, i, activeDarts);
                    activeDarts--;
                    i--;
                }
                for (size_t i = 0; i < numBubbles; i++) {
                    remove_bubble(&bubbles, i, numBubbles);
                    numBubbles--;
                    i--;
                }
                for (size_t i = 0; i < numPoppingBubbles; i++) {
                    remove_poppingbubble(&poppingBubbles, i, numPoppingBubbles);
                    numPoppingBubbles--;
                    i--;
                }
        	}
        	isTopHighlighted = false;
            isExitHighlighted = false;
        	if (kHeld & KEY_TOUCH) {
        		tX = touch.px;
        		tY = touch.py;
        		isTopHighlighted = C2D_Clamp(tX+1, 10, 310) == tX+1 && C2D_Clamp(tY+1, 10, 60) == tY+1;
                isExitHighlighted = C2D_Clamp(tX+1, 10, 310) == tX+1 && C2D_Clamp(tY+1, 180, 230) == tY+1;
        	}
        	if (kUp & KEY_TOUCH) {
        		if (C2D_Clamp(tX+1, 10, 310) == tX+1 && C2D_Clamp(tY+1, 10, 60) == tY+1) {
        			switchTo = inInfiniteGame;
        			level = 1;
        			score = 0;
                    gameHighScore = 0;
        			dir = 0.0f;
        			framesSinceLastBubble = -60;
        			redBubbleDartsShot = 0;
        			dartsShot = 0;
                    for (size_t i = 0; i < activeDarts; i++) {
                        remove_dart(&darts, i, activeDarts);
                        activeDarts--;
                        i--;
                    }
                    for (size_t i = 0; i < numBubbles; i++) {
                        remove_bubble(&bubbles, i, numBubbles);
                        numBubbles--;
                        i--;
                    }
                    for (size_t i = 0; i < numPoppingBubbles; i++) {
                        remove_poppingbubble(&poppingBubbles, i, numPoppingBubbles);
                        numPoppingBubbles--;
                        i--;
                    }
        			break;
        		}
                if (C2D_Clamp(tX+1, 10, 310) == tX+1 && C2D_Clamp(tY+1, 180, 230) == tY+1) {
                    exitGame = true;
                    break;
                }
        	}
            // Menu visuals
            // Spawn bubbles and darts
            framesSinceLastBubble++;
            if (framesSinceLastBubble > floor((rand()%150)+15)) {
                framesSinceLastBubble = 0;
                makeBubbleMenu();
                if (activeDarts < MAX_ACTIVE_DARTS) makeDart(fmodf(((float)rand())/32767, pi*2), (SCREEN_WIDTH/2 + 20) + floor(fmodf(rand()/10.0f, SCREEN_WIDTH/2-40)*wideModifier)/wideModifier, -24.0f, fmodf(((float)rand())/32767, pi*2));
            }
            
            doMovement(0, 0);
            
            // Do bubble collisions
            for (size_t i = 0; i < numBubbles; i++) {
                Bubble* bubble = &bubbles[i];
                int x = bubble->spr.params.pos.x;
                int y = bubble->spr.params.pos.y;
                int radius = 16;
                if (bubble->bubbleType == 3) radius = 20;
                for (size_t j = 0; j < activeDarts; j++) {
                    Dart* dart = &darts[j];
                    int dx = dart->spr.params.pos.x;
                    int dy = dart->spr.params.pos.y;
                    float distX = (x-(dx+(float)cos(dart->dir)));
                    float distY = (y-(dy+(float)sin(dart->dir)));
                    float distT = sqrt(distX * distX+distY * distY);
                    if (!(dart->isActive)) {
                        if (distT - radius < 5) {
                            C2D_SpriteMove(&dart->spr, 0, -4.0f/3.0f);
                            dart->isActive = false;
                            dart->dir = sqrt(dart->xVel*dart->xVel+dart->yVel*dart->yVel);
                            dart->xVel = -distX/distT*(dart->dir)*4/5;
                            dart->yVel = -distY/distT*(dart->dir)*4/5;
                            dart->dir = atan(dart->yVel / dart->xVel);
                            if (dart->xVel < 0) dart->dir += pi;
                            C2D_SpriteSetRotation(&dart->spr, dart->dir);
                        }
                    } else if (distT - radius < 5) {
                        popBubble(bubble->spr, bubble->bubbleType);
                        if (bubble->bubbleType != 0) {
                            C2D_SpriteFromSheet(&dart->spr, dartSheet, 2);
                            C2D_SpriteScale(&dart->spr, 2.0f, 2.0f);
                            C2D_SpriteSetCenter(&dart->spr, 0.5f, 0.5f);
                            C2D_SpriteSetPos(&dart->spr, dx, dy-4.0f/3.0f);
                            dart->isActive = false;
                            dart->dir = sqrt(dart->xVel*dart->xVel+dart->yVel*dart->yVel);
                            dart->xVel = -distX/distT*(dart->dir)*4/5;
                            dart->yVel = -distY/distT*(dart->dir)*4/5;
                            dart->dir = atan(dart->yVel / dart->xVel);
                            if (dart->xVel < 0) dart->dir += pi;
                            C2D_SpriteSetRotation(&dart->spr, dart->dir);
                            C2D_SpriteSetDepth(&dart->spr, 0.5f);
                        }
                        if (bubble->bubbleType != 2) {
                            remove_bubble(&bubbles, i, numBubbles);
                            numBubbles--;
                            i--;
                            break;
                        } else {
                            C2D_SpriteFromSheet(&bubble->spr, blueSheet, 0);
                            bubble->bubbleType = 0;
                            C2D_SpriteScale(&bubble->spr, 2.0f, 2.0f);
                            C2D_SpriteSetCenter(&bubble->spr, 0.5f, 0.5f);
                            C2D_SpriteSetPos(&bubble->spr, x, y);
                            C2D_SpriteSetDepth(&bubble->spr, 0.4f);
                            break;
                        }
                    }
                }
            }
            
            updateAnimation();
        	break;
        }
        default: break;
        }
        
        // Prepare Rendering
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

        // Main Screen Render
        C2D_SceneBegin(top_main);
        switch (state) {
        case inInfiniteGame: {
            C2D_TargetClear(top_main, C2D_Color32(255, 255, 255, 255));
            // Popping Bubbles
            for (size_t i = 0; i < numPoppingBubbles; i++) {
                C2D_DrawSprite(&poppingBubbles[i].spr);
            }
            // Moving Bubbles
            for (size_t i = 0; i < numBubbles; i++) {
                C2D_DrawSprite(&bubbles[i].spr);
            }
            // Indicator Line
            C2D_DrawLine(24, SCREEN_HEIGHT-24, C2D_Color32f(0.0f, 0.0f, 0.0f, 0.5f * linetimer), 24 + dirX*sqrt(dist)*10, (SCREEN_HEIGHT-24) + dirY*sqrt(dist)*10, C2D_Color32f(0.0f, 0.0f, 0.0f, 0.0f), 2, 0.0f);
            // Moving Darts
            for (size_t i = 0; i < activeDarts; i++) {
                C2D_DrawSprite(&darts[i].spr);
            }
            // Starting Dart
            C2D_DrawSprite(&startDartSprite);
            break;
        }
        case inMenu: {
            C2D_TargetClear(top_main, C2D_Color32(255-ceil(255*(exitGameTimer/15.0f)), 255-ceil(255*(exitGameTimer/15.0f)), 255-ceil(255*(exitGameTimer/15.0f)), 255));
        	// Popping Bubbles
            for (size_t i = 0; i < numPoppingBubbles; i++) {
                C2D_DrawSprite(&poppingBubbles[i].spr);
            }
            // Moving Bubbles
            for (size_t i = 0; i < numBubbles; i++) {
                C2D_DrawSprite(&bubbles[i].spr);
            }
            // Moving Darts
            for (size_t i = 0; i < activeDarts; i++) {
                C2D_DrawSprite(&darts[i].spr);
            }
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(buttonSheet, 0), -218.5f, SCREEN_HEIGHT - 16.5f, 0.5f, &blackTint, 2.0f, 2.0f);
        	C2D_DrawText(&introText2, C2D_AlignCenter, SCREEN_WIDTH / 2.0f - 20, SCREEN_HEIGHT/4.0f + 2.5 * (sinl(fmodl(timer, pi*2)) + cosl(fmodl(timer, pi*2))), 1.0f, 1.0f, 1.0f);
            C2D_DrawText(&introText1, C2D_AlignRight, SCREEN_WIDTH / 2.0f - 50, SCREEN_HEIGHT/4.0f + 5 * sinl(fmodl(timer, pi*2)), 1.0f, 1.0f, 1.0f);
            C2D_DrawText(&introText3, C2D_AlignLeft, SCREEN_WIDTH / 2.0f + 10, SCREEN_HEIGHT/4.0f + 5 * cosl(fmodl(timer, pi*2)), 1.0f, 1.0f, 1.0f);
            C2D_DrawText(&halfQilinText, C2D_AlignLeft | C2D_AtBaseline | C2D_WithColor, 3.0f, SCREEN_HEIGHT-3.0f, 1.0f, 0.4f, 0.4f, C2D_Color32(255, 255, 255, 255));
        	break;
        }
        default: break;
        }
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(cornerSheet, 1), 0, 0, 1.0f, NULL, 2.0f, 2.0f);
        if (state != inMenu) C2D_DrawImageAt(C2D_SpriteSheetGetImage(cornerSheet, 0), SCREEN_WIDTH - 8, 0, 1.0f, NULL, 2.0f, 2.0f);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(cornerSheet, 3), 0, SCREEN_HEIGHT - 8, 1.0f, NULL, 2.0f, 2.0f);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(cornerSheet, 2), SCREEN_WIDTH - 8, SCREEN_HEIGHT - 8, 1.0f, NULL, 2.0f, 2.0f);
        if (exitGame) {
        	exitGameTimer++;
        	C2D_Fade(C2D_Color32(0, 0, 0, ceil(255*(exitGameTimer/15.0f))));
        }

        // Bottom Scene Render
        C2D_SceneBegin(bottom);

        switch (state) {
        case inInfiniteGame: {
        	C2D_TargetClear(bottom, C2D_Color32(255, 255, 255, 255));
            // Pointer Line
            if (updatedPosition) {
                C2D_DrawLine(tX, tY, C2D_Color32f(0.0f, 0.0f, 0.0f, 0.3f * linetimer), tX + dirX*dist, tY + dirY*dist, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f * linetimer), 2, 0.0f);
                C2D_DrawLine(tX + dirX*dist + (sqrt(dist)/sqrt(300)*7.5+5) * cos(dir-0.75*pi), tY + dirY*dist + (sqrt(dist)/sqrt(300)*7.5+5) * sin (dir-0.75*pi), C2D_Color32f(0.0f, 0.0f, 0.0f, 0.5f * linetimer), tX + dirX*dist, tY + dirY*dist, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f * linetimer), 2, 0.2f);
                C2D_DrawLine(tX + dirX*dist + (sqrt(dist)/sqrt(300)*7.5+5) * cos(0.75*pi + dir), tY + dirY*dist + (sqrt(dist)/sqrt(300)*7.5+5) * sin(0.75*pi + dir), C2D_Color32f(0.0f, 0.0f, 0.0f, 0.5f * linetimer), tX + dirX*dist, tY + dirY*dist, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f * linetimer), 2, 0.2f);
                C2D_DrawTriangle(tX + dirX*dist, tY + dirY*dist, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f * linetimer), tX + dirX*dist + (sqrt(dist)/sqrt(300)*7.5+5) * cos(0.75*pi + dir), tY + dirY*dist + (sqrt(dist)/sqrt(300)*7.5+5) * sin(0.75*pi + dir), C2D_Color32f(0.0f, 0.0f, 0.0f, 0.0f), 
                		tX + dirX*dist + (sqrt(dist)/sqrt(300)*7.5+5) * cos(dir-0.75*pi), tY + dirY*dist + (sqrt(dist)/sqrt(300)*7.5+5) * sin (dir-0.75*pi), C2D_Color32f(0.0f, 0.0f, 0.0f, 0.0f), 0.1f);
            }
            // Score Text
            snprintf(intconvert, 20, "%lld", score);
            C2D_TextFontParse(&scoreText, lightFont, textBuffer, intconvert);
            snprintf(intconvert, 16, "Level %ld", level);
            C2D_TextFontParse(&levelText, regularFont, textBuffer, intconvert);
            C2D_DrawText(&scoreText, C2D_AlignLeft, 5.0f, 0.0f, 1.0f, 1.0f, 1.0f);
            C2D_DrawText(&levelText, C2D_AlignLeft, 5.0f, 30.0f, 1.0f, 0.8f, 0.8f);
            C2D_TextBufClear(textBuffer);
            break;
        }
        case inMenu: {
        	C2D_TargetClear(bottom, C2D_Color32(255-ceil(255*(exitGameTimer/15.0f)), 255-ceil(255*(exitGameTimer/15.0f)), 255-ceil(255*(exitGameTimer/15.0f)), 255));
        	C2D_TextGetDimensions(&startGameText, 0.7f, 0.7f, &textWidth, &textHeight);
            snprintf(intconvert, 39, "High Score: %lld", highScore);
            C2D_TextFontParse(&highScoreText, regularFont, textBuffer, intconvert);
            if (isTopHighlighted) {
            	C2D_DrawImageAt(C2D_SpriteSheetGetImage(buttonSheet, 0), 10.0f, 10.0f, 0.0f, &highlightedTint, 2.0f, 2.0f);
        		C2D_DrawImageAt(C2D_SpriteSheetGetImage(buttonOutlineSheet, 0), 10.0f, 10.0f, 0.1f, &blackTint, 2.0f, 2.0f);
        		C2D_DrawText(&startGameText, C2D_WithColor | C2D_AlignCenter | C2D_AtBaseline, 160.0f, 36.0f, 0.2f, 0.7f, 0.7f, C2D_Color32(0, 0, 0, 255));
                C2D_DrawText(&highScoreText, C2D_WithColor | C2D_AlignCenter, 160.0f, 36.0f, 0.2f, 0.5f, 0.5f, C2D_Color32(0, 0, 0, 255));
        	} else {
				C2D_DrawImageAt(C2D_SpriteSheetGetImage(buttonSheet, 0), 10.0f, 10.0f, 0.0f, &blackTint, 2.0f, 2.0f);
        		C2D_DrawText(&startGameText, C2D_WithColor | C2D_AlignCenter | C2D_AtBaseline, 160.0f, 36.0f, 0.2f, 0.7f, 0.7f, C2D_Color32(0, 254, 255, 255));
                C2D_DrawText(&highScoreText, C2D_WithColor | C2D_AlignCenter, 160.0f, 36.0f, 0.2f, 0.5f, 0.5f, C2D_Color32(0, 254, 255, 255));
        	};
            C2D_TextGetDimensions(&exitText, 0.7f, 0.7f, &textWidth, &textHeight);
            if (isExitHighlighted) {
                C2D_DrawImageAt(C2D_SpriteSheetGetImage(buttonSheet, 0), 10.0f, 180.0f, 0.0f, &highlightedTint, 2.0f, 2.0f);
                C2D_DrawImageAt(C2D_SpriteSheetGetImage(buttonOutlineSheet, 0), 10.0f, 180.0f, 0.1f, &blackTint, 2.0f, 2.0f);
                C2D_DrawText(&exitText, C2D_WithColor | C2D_AlignCenter | C2D_AtBaseline, 160.0f, 204.0f + textHeight/2, 0.2f, 0.7f, 0.7f, C2D_Color32(0, 0, 0, 255));
            } else {
                C2D_DrawImageAt(C2D_SpriteSheetGetImage(buttonSheet, 0), 10.0f, 180.0f, 0.0f, &blackTint, 2.0f, 2.0f);
                C2D_DrawText(&exitText, C2D_WithColor | C2D_AlignCenter | C2D_AtBaseline, 160.0f, 204.0f + textHeight/2, 0.2f, 0.7f, 0.7f, C2D_Color32(0, 254, 255, 255));
            }
            snprintf(intconvert, 33, "%llu Darts Thrown", totalDartsShot);
        	C2D_TextFontParse(&dartsText, regularFont, textBuffer, intconvert);
        	C2D_DrawText(&dartsText, C2D_AlignCenter, 160.0f, 114.0f, 0.2f, 0.5f, 0.5f);
            C2D_TextBufClear(textBuffer);
            break;
        }
        case gameOver: {
            snprintf(intconvert, 39, "You scored %lld points!", gameHighScore);
            C2D_TextFontParse(&sessionScoreText, regularFont, textBuffer, intconvert);
            C2D_TargetClear(bottom, C2D_Color32(255, 255, 255, 255));
            C2D_DrawText(&gameOverText, C2D_AlignCenter | C2D_AtBaseline, 160.0f, 110.0f, 0.2f, 1.0f, 1.0f);
            C2D_DrawText(&sessionScoreText, C2D_AlignCenter, 160.0f, 110.0f, 0.2f, 0.5f, 0.5f);
            C2D_DrawText(&exitGOText, C2D_AlignCenter | C2D_AtBaseline, 160.0f, SCREEN_HEIGHT-3.0f, 0.2f, 0.4f, 0.4f);
            C2D_TextBufClear(textBuffer);
            break;
        }
        default: break;
        }
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(cornerSheet, 1), 0, 0, 1.0f, NULL, 2.0f, 2.0f);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(cornerSheet, 0), BOTTOM_SCREEN_WIDTH - 8, 0, 1.0f, NULL, 2.0f, 2.0f);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(cornerSheet, 3), 0, SCREEN_HEIGHT - 8, 1.0f, NULL, 2.0f, 2.0f);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(cornerSheet, 2), BOTTOM_SCREEN_WIDTH - 8, SCREEN_HEIGHT - 8, 1.0f, NULL, 2.0f, 2.0f);

        // Exit the game if needed
        if (exitGame && exitGameTimer > 15) break;
        
        if (exitGame) {
        	exitGameTimer++;
        	C2D_Fade(C2D_Color32(0, 0, 0, ceil(255*(exitGameTimer/15.0f))));
        }

        C3D_FrameEnd(0);

        if (switchTo != state) {
            initialSwitchStuff = true;
            state = switchTo;
        }
    }

    // Delete graphics and fonts
    C2D_SpriteSheetFree(dartSheet);
    C2D_SpriteSheetFree(firingSheet);
    C2D_SpriteSheetFree(blueSheet);
    C2D_SpriteSheetFree(yellowSheet);
    C2D_SpriteSheetFree(purpleSheet);
    C2D_SpriteSheetFree(nullBubbleSheet);
    C2D_FontFree(regularFont);
    C2D_FontFree(lightFont);
    C2D_FontFree(boldFont);
    C2D_FontFree(mediumFont);

    // Save data
    saveData = fopen("saves/bubblesAndDarts.txt", "w");
    fprintf(saveData, "%u~%llx|%llu;", currentSaveVersion, ~highScore, ~totalDartsShot);
    fclose(saveData);

    // Deinit libs
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    romfsExit();
    return 0;
}
