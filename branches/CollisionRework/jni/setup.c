/*
 * setup.c
 * -----------------------------
 * Defines the setup function, which initializes
 * all of the arrays and variables necessary.
 */

#include "setup.h"
#include <android/log.h>

void gameSetup()
{
	//__android_log_write(ANDROID_LOG_INFO, "TheElements", "gameSetup");
	int i, j;
	loq = MAX_POINTS;
	unsigned char backgroundRed = cAtmosphere->backgroundRed;
	unsigned char backgroundGreen = cAtmosphere->backgroundGreen;
	unsigned char backgroundBlue = cAtmosphere->backgroundBlue;

	cElement = elements[NORMAL_ELEMENT];

	//Unset all the particles
	for(i = 0; i < MAX_POINTS; i++)
	{
		avail[i] = particles[i];
		particles[i]->set = FALSE;
	}

	//Clear allCoords and our pixels array
	for(i = 0; i < stupidTegra; i++)
	{
		for(j = 0; j < workHeight; j++)
		{
			if(i < workWidth)
			{
				allCoords[getIndex(i, j)] = NULL;
			}
			colors[3 * getColorIndex(i, j)] = backgroundRed;
			colors[3 * getColorIndex(i, j) + 1] = backgroundGreen;
			colors[3 * getColorIndex(i, j) + 2] = backgroundBlue;
		}
	}
	/* Network stuff not needed
	for (j = 0; j < 8; j++)
	{
		username[j] = 0;
		password[j] = 0;
	}
	userlength = 0;
	passlength = 0;
	*/
}

//Set up all the variable sized arrays
void arraySetup()
{
	//__android_log_write(ANDROID_LOG_INFO, "TheElements", "arraySetup");
	int i;
	int temp = 1;
	//Variables for special size and special value size, because these are variable-sized multidimensional arrays
	
	//Calculate the number of pixels
	int points = stupidTegra * workHeight;

	//Make sure everything is deallocated
	free(colors);
	free(allCoords);

	//Allocate memory
	colors = malloc(3 * points * sizeof(char));
	allCoords = malloc(workWidth * workHeight * sizeof(struct Particle*)); //Two dimensional array, so when calling use allcoords[getIndex(x, y)];

}

void atmosphereSetup()
{
	free(cAtmosphere);
	cAtmosphere = (struct Atmosphere*) malloc (sizeof(struct Atmosphere));

	cAtmosphere->heat = DEFAULT_ATMOSPHERE_TEMP;
	cAtmosphere->gravity = DEFAULT_ATMOSPHERE_GRAVITY;
	cAtmosphere->backgroundRed = DEFAULT_RED;
	cAtmosphere->backgroundGreen = DEFAULT_GREEN;
	cAtmosphere->backgroundBlue = DEFAULT_BLUE;
	cAtmosphere->borderLeft = DEFAULT_BORDER_LEFT;
	cAtmosphere->borderTop = DEFAULT_BORDER_TOP;
	cAtmosphere->borderRight = DEFAULT_BORDER_RIGHT;
	cAtmosphere->borderBottom = DEFAULT_BORDER_BOTTOM;
}

void elementSetup()
{
	//TODO: Load custom elements	__android_log_write(ANDROID_LOG_INFO, "TheElements", "arraysetup end");

	//Calculate numElements
	//Calculate special size
	//Calculate special value size
	numElements = NUM_BASE_ELEMENTS; //Changed later

	//Free and reallocate the elements array
	free(elements);
	elements = malloc(numElements * sizeof(struct Element*));

	//Allocate and initialize all the elements
	struct Element* tempElement;
	int i;
	for(i = 0; i < numElements; i++)
	{
		if(i < NUM_BASE_ELEMENTS)
		{
			tempElement = (struct Element*) malloc(sizeof(struct Element));
			elements[i] = tempElement;
			tempElement->index = i;
			tempElement->red = baseRed[i];
			tempElement->green = baseGreen[i];
			tempElement->blue = baseBlue[i];
			tempElement->fallVel = baseFallVel[i];
			tempElement->density = baseDensity[i];
			tempElement->state = baseState[i];
			tempElement->specials = baseSpecial[i];
			tempElement->specialVals = (char*) malloc(2 * sizeof(char));
				tempElement->specialVals[0] = baseSpecialValue[i][0];
				tempElement->specialVals[1] = baseSpecialValue[i][1];
			tempElement->inertia = baseInertia[i];
			tempElement->startingTemp = baseStartingTemp[i];
			tempElement->highestTemp = baseHighestTemp[i];
			tempElement->lowestTemp = baseLowestTemp[i];
		}
		else
		{
			//TODO: Gotta load the thing from the array, allocate the memory for it, and store the pointer here
		}

		cElement = elements[NORMAL_ELEMENT];
	}

	//Resolve heat pointers
	for(i = 0; i < numElements; i++)
	{
		if(i < NUM_BASE_ELEMENTS)
		{
			elements[i]->lowerElement = elements[baseLowerElement[i]];
			elements[i]->higherElement = elements[baseHigherElement[i]];
		}
		else
		{
			//TODO: Load the higher/lower elements and assign them
		}
	}
}

void particleSetup()
{
	//__android_log_write(ANDROID_LOG_INFO, "TheElements", "particleSetup");
	int i;
	for(i = 0; i < MAX_POINTS; i++)
	{
		particles[i] = (struct Particle*) malloc(sizeof(struct Particle));
	}
}
