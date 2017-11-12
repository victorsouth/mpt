/* ============================================ *
 * (c) 2016 Гаврилов Артем Сергеевич, АС-13-04  *
 * ============================================ *
 */
 
#include "avr/io.h"
#include <util/delay.h>
#include "tft.h"

#define LEFT_EYE_POSX 10
#define LEFT_EYE_POSY 10
#define EYE_SIZE 40

#define RIGHT_EYE_POSX 80
#define RIGHT_EYE_POSY  10

#define PUPIL_RADIUS 15

#define LEFT_SIDE_OF_NOSE 60
#define RIGHT_SIDE_OF_NOSE 70
#define TOP_SIDE_OF_NOSE 60
#define BOTTOM_SIDE_OF_NOSE 110

#define BUTTON_BLINKING 0b00000001
#define BUTTON_SMILE 0b00000010

void drawLeftEye()
{

    FillRect(LEFT_EYE_POSX, LEFT_EYE_POSY,
		LEFT_EYE_POSX + EYE_SIZE, LEFT_EYE_POSY + EYE_SIZE, YELLOW);

	FillCircle(LEFT_EYE_POSX + EYE_SIZE / 2,
		LEFT_EYE_POSY + EYE_SIZE / 2, PUPIL_RADIUS, RED);
}

void drawRightEye(char isBlinking)
{
	if (isBlinking) {
		FillRect(RIGHT_EYE_POSX, RIGHT_EYE_POSY + 15,
			RIGHT_EYE_POSX + EYE_SIZE, RIGHT_EYE_POSY + EYE_SIZE - 15, YELLOW);

			HLine(RIGHT_EYE_POSX,RIGHT_EYE_POSX + EYE_SIZE,
			 RIGHT_EYE_POSY + EYE_SIZE / 2, BLACK);
		return;
	}

	FillRect(RIGHT_EYE_POSX, RIGHT_EYE_POSY,
		RIGHT_EYE_POSX + EYE_SIZE, RIGHT_EYE_POSY + EYE_SIZE, YELLOW);
	FillCircle(RIGHT_EYE_POSX + EYE_SIZE / 2,
		RIGHT_EYE_POSY + EYE_SIZE / 2, PUPIL_RADIUS, RED);
}

void drawNose()
{
	FillRect(LEFT_SIDE_OF_NOSE, TOP_SIDE_OF_NOSE, RIGHT_SIDE_OF_NOSE,
	 BOTTOM_SIDE_OF_NOSE, YELLOW);
}

void drawMouth(char isFunny)
{
	char leftSideOfFirstPiece = 20;

	char rightSideOfFirstPiece = 30;

	char rightSideOfSecondPiece = 100;
	char rightSideOfThirdPiece = 110;

	char topSideOfLateral = 0;
	char bottomSideOfLateral = 0;

	char topSideOfSecond = 130;
	char bottomSideOfSecond = 145;

	if (isFunny){
		topSideOfLateral = topSideOfSecond - 5;
		bottomSideOfLateral = bottomSideOfSecond - 5;
	} else {
		topSideOfLateral = topSideOfSecond + 5;
		bottomSideOfLateral = bottomSideOfSecond + 5;
	}

	FillRect(leftSideOfFirstPiece, topSideOfLateral,
		rightSideOfFirstPiece,bottomSideOfLateral , YELLOW);

	FillRect(rightSideOfFirstPiece, topSideOfSecond,
		rightSideOfSecondPiece, bottomSideOfSecond, YELLOW);

	FillRect(rightSideOfSecondPiece, topSideOfLateral,
		rightSideOfThirdPiece, bottomSideOfLateral, YELLOW);
}

void drawRobot(char isFunny, char isBlinking)
{
	drawLeftEye();

	drawRightEye(isBlinking);

	drawNose();

	drawMouth(isFunny);
}

char detect_buttons_in_delay(int delay)
{
	static char previous_port = 0xFF;
	int quant_count = delay / 10;
	for (int quant_number = 0; quant_number < quant_count; ++quant_number) {
		char current_port = PINB;
		char buttons = ~current_port & previous_port;
		previous_port = current_port;
		if (buttons)
			return buttons;
		_delay_ms(10);
	}
	return 0x00;
}

int main()
{
	PORTB = 0xFF;
	DDRB = 0x00;

	InitTFT();
	
	char isFunny = 1, isBlinking = 0;
	
	drawRobot(isFunny, isBlinking);
		
	while (1){
		char buttons = detect_buttons_in_delay(100);
		if (buttons & BUTTON_BLINKING) {
			
			if (!isFunny && isBlinking)
				continue;
			ClearScreen();
			
			isFunny = !isFunny;
			drawRobot(isFunny, isBlinking);
		}
		if (buttons & BUTTON_SMILE) {
			
			if (!isFunny && !isBlinking)
				continue;
			
			ClearScreen();
			isBlinking = !isBlinking;
			drawRobot(isFunny, isBlinking);
		}
	}
}
