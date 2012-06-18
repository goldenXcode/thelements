/*
 * saveload.c
 * --------------------------
 * Defines functions relating to saving
 * and loading from files.
 */

#include "saveload.h"
#include <android/log.h>

const char *get_filename_ext(const char *filename) {
	const char *dot = strrchr(filename, '.');
	if(!dot || dot == filename) return "";
	return dot;
}
char saveState(char* saveLoc)
{
	// Lock the mutex so that we don't continue to update
	__android_log_write(ANDROID_LOG_INFO, "TheElements", "Starting save");
	pthread_mutex_lock(&update_mutex);
	__android_log_write(ANDROID_LOG_INFO, "TheElements", "Got mutex");

	char retVal = saveStateLogic(saveLoc);

	// Unlock the mutex before quitting
	pthread_mutex_unlock(&update_mutex);
	__android_log_write(ANDROID_LOG_INFO, "TheElements", "Finished save");

	return retVal;
}
char saveStateLogic(char* saveLoc)
{
	FILE* fp;
	/*
	//Strings for file location
	char timestamp[64], saveLoc[256];

	time_t rawtime;
	if (type == NORMAL_SAVE)
	{
		//Generate the filename based on date and time
		time(&rawtime);
		strftime(timestamp, 255, "%Y-%m-%d-%H:%M", localtime(&rawtime));
		strcpy(saveLoc, ROOT_FOLDER);
		strcat(saveLoc, SAVES_FOLDER);
		strcat(saveLoc, timestamp);
		strcat(saveLoc, SAVE_EXTENSION);
	}
	else if (type == TEMP_SAVE)
	{
		//Generate the temp save filename
		strcpy(saveLoc, ROOT_FOLDER);
		strcat(saveLoc, SAVES_FOLDER);
		strcat(saveLoc, TEMP_SAVE_FILE);
		strcat(saveLoc, SAVE_EXTENSION);
	}
	 */
	fp = fopen(saveLoc, "w");

	if (fp != NULL)
	{
		int i, counterX, counterY, numElementsToBeSaved;
		struct Particle* tempParticle;
		char* elementSaveFilter;
		elementSaveFilter = (char*)malloc(numElements * sizeof(char));

		//Initialize elementSaveFilter
		for(i = 0; i < numElements; i++)
		{
			elementSaveFilter[i] = 0;
		}

		//Preprocessing loop
		numElementsToBeSaved = 0;
		for (counterX = 0; counterX < workWidth; counterX++)
		{
			for (counterY = 0; counterY < workHeight; counterY++)
			{
				tempParticle = allCoords[getIndex(counterX, counterY)];
				//If the particle is a custom element, indicate that we need to save this element
				if(tempParticle && tempParticle->element->index >= NUM_BASE_ELEMENTS && elementSaveFilter[tempParticle->element->index] != 1)
				{
					elementSaveFilter[tempParticle->element->index] = 1;
					numElementsToBeSaved++;
				}
			}
		}

		//First write how many custom elements will be saved
		fprintf(fp, "%d\n\n", numElementsToBeSaved);

		//Save the custom elements that need to be saved
		/* Save format:
		 * index
		 * name
		 * state startingTemp lowestTemp highestTemp
		 * lowerElement->index higherElement->index
		 * red green blue
		 * TODO: specials
		 * density fallVel inertia
		 */
		for (i = 0; i < numElements; i++)
		{
			if(elementSaveFilter[i] == 1)
			{
				fprintf(fp, "%d\n", i);
				fprintf(fp, "%s\n", elements[i]->name);
				fprintf(fp, "%d %d %d %d\n",
						elements[i]->state,
						elements[i]->startingTemp,
						elements[i]->lowestTemp,
						elements[i]->highestTemp);
				fprintf(fp, "%d %d\n",
						elements[i]->lowerElement->index,
						elements[i]->higherElement->index);
				fprintf(fp, "%d %d %d\n",
						elements[i]->red,
						elements[i]->green,
						elements[i]->blue);
				//TODO: Specials & useElementSpecialVals
				fprintf(fp, "%d %d %d\n\n",
						elements[i]->density,
						elements[i]->fallVel,
						elements[i]->inertia);
			}
		}

		//Save the dimensions
		fprintf(fp, "%d %d\n\n", workWidth, workHeight);

		//Save the max specials (for backward compatibility if needed)
		fprintf(fp, "%d\n\n", MAX_SPECIALS);

		//Save the particles
		/* Save format:
		 * (x y xVel yVel heat element->index) ...
		 *   .
		 *   .
		 *   .
		 */
		for (counterY = 0; counterY < workHeight; counterY++)
		{
			for (counterX = 0; counterX < workWidth; counterX++)
			{
				tempParticle = allCoords[getIndex(counterX, counterY)];
				if(tempParticle)
				{
					fprintf(fp, "(%f %f %d %d %d %d)",
							tempParticle->x,
							tempParticle->y,
							tempParticle->xVel,
							tempParticle->yVel,
							tempParticle->heat,
							tempParticle->element->index);
					if (!tempParticle->element->useElementSpecialVals)
					{
						fprintf(fp, "[");
						for (i = 0; i < MAX_SPECIALS; i++)
						{
							fprintf(fp, "%d ", tempParticle->specialVals[i]);
						}
						fprintf(fp, "]");
					}
				}
				else
				{
					fprintf(fp, "()");
				}
			}
			fprintf(fp, "\n");
		}

		fclose(fp);
		return TRUE;
	}
	if(fp)
	{
		fclose(fp);
	}
	return FALSE;
}

char loadState(char* loadLoc)
{
	//Lock the mutex before loading so that we don't update frames while loading
	__android_log_write(ANDROID_LOG_INFO, "TheElements", "Starting loadstate");
	pthread_mutex_lock(&update_mutex);
	__android_log_write(ANDROID_LOG_INFO, "TheElements", "Got mutex lock");

	char retVal = loadStateLogic(loadLoc);

	//Unlock the mutex before quitting
	pthread_mutex_unlock(&update_mutex);
	__android_log_write(ANDROID_LOG_INFO, "TheElements", "Finishing loadstate");

	return retVal;
}
char loadStateLogic(char* loadLoc)
{
	char buffer[100];
	sprintf(buffer, "loadState: %s", loadLoc);
	__android_log_write(ANDROID_LOG_INFO, "TheElements", buffer);
	FILE* fp;
	/*
	char loadLoc[256];
	//Set up the filename
	if (type == NORMAL_SAVE)
	{
		if(sizeof(filename)/sizeof(char) > 256)
		{
			return FALSE;
		}

		strcpy(loadLoc, filename);
	}
	else if (type == TEMP_SAVE)
	{
		strcpy(loadLoc, ROOT_FOLDER);
		strcat(loadLoc, SAVES_FOLDER);
		strcat(loadLoc, TEMP_SAVE_FILE);
		strcat(loadLoc, SAVE_EXTENSION);
	}
	else if (type == DEMO_SAVE)
	{
		strcpy(loadLoc, ROOT_FOLDER);
		strcat(loadLoc, SAVES_FOLDER);
		strcat(loadLoc, DEMO_SAVE_FILE);
		strcat(loadLoc, SAVE_EXTENSION);
	}
	 */
	//Load the file for reading
	fp = fopen(loadLoc, "r");

	//Clear the screen and set up some temp variables
	arraySetup();
	elementSetup();
	gameSetup();

	int numElementsSaved, i, j, elementIndex, lowerElementIndex, higherElementIndex,
	sizeX, sizeY, fileMaxSpecials, tempSpecialVal, charsRead, failed;
	struct Element* tempElement;
	struct Particle* tempParticle;
	char lookAhead;

	if(fp != NULL)
	{
		if((charsRead = fscanf(fp, "%d\n\n", &numElementsSaved)) == EOF || charsRead < 1) { return FALSE; }

		int* tempMap;
		int* tempElMap;
		tempElMap = (int*)malloc ( MAX_ELEMENTS * sizeof(int) );
		tempMap = (int*)malloc( numElementsSaved * 3 * sizeof(int) );
		elements = realloc( elements, (numElements + numElementsSaved) * sizeof( struct Element* ) );

		for(i = 0; i < numElementsSaved; i++)
		{
			if((charsRead = fscanf(fp, "%d", &elementIndex) == EOF || charsRead < 1)) {return FALSE;}

			tempElement = (struct Element*) malloc(sizeof(struct Element));
			if((charsRead = fscanf(fp, "%s", &tempElement->name)) == EOF || charsRead < 1) {return FALSE;}
			if((charsRead = fscanf(fp, "%d", &tempElement->state)) == EOF || charsRead < 1) {return FALSE;}
			if((charsRead = fscanf(fp, "%d", &tempElement->startingTemp)) == EOF || charsRead < 1) {return FALSE;}
			if((charsRead = fscanf(fp, "%d", &tempElement->lowestTemp)) == EOF || charsRead < 1) {return FALSE;}
			if((charsRead = fscanf(fp, "%d", &tempElement->highestTemp)) == EOF || charsRead < 1) {return FALSE;}
			if((charsRead = fscanf(fp, "%d", &lowerElementIndex)) == EOF || charsRead < 1) {return FALSE;}
			if((charsRead = fscanf(fp, "%d", &higherElementIndex)) == EOF || charsRead < 1) {return FALSE;}
			if((charsRead = fscanf(fp, "%d", &tempElement->red)) == EOF || charsRead < 1) {return FALSE;}
			if((charsRead = fscanf(fp, "%d", &tempElement->green)) == EOF || charsRead < 1) {return FALSE;}
			if((charsRead = fscanf(fp, "%d", &tempElement->blue)) == EOF || charsRead < 1) {return FALSE;}
			if((charsRead = fscanf(fp, "%d", &tempElement->density)) == EOF || charsRead < 1) {return FALSE;}
			if((charsRead = fscanf(fp, "%d", &tempElement->fallVel)) == EOF || charsRead < 1) {return FALSE;}
			if((charsRead = fscanf(fp, "%d", &tempElement->inertia)) == EOF || charsRead < 1) {return FALSE;}
			tempMap[3*i] = elementIndex;
			tempMap[3*i + 1] = lowerElementIndex;
			tempMap[3*i + 2] = higherElementIndex;
			tempElMap[elementIndex] = i + numElements;
			elements[elementIndex] = tempElement;
		}
		for (i = 0; i < numElementsSaved; i++)
		{
			elements[ tempMap[3*i] ]->lowerElement = tempMap[3*i + 1] > NUM_BASE_ELEMENTS ? 
					elements[ tempElMap[tempMap[3*i + 1]] ] : elements[tempMap[3*i + 1]];
			elements[ tempMap[3*i] ]->lowerElement = tempMap[3*i + 2] > NUM_BASE_ELEMENTS ? 
					elements[tempElMap[tempMap[3*i + 2]] ]: elements[tempMap[3*i + 2]];
		}
		free(tempMap);
		free(tempElMap);

		numElements +=  numElementsSaved;


		// Get the dimensions
		if((charsRead = fscanf(fp, "%d %d\n\n", &sizeX, &sizeY)) == EOF || charsRead < 2) {return FALSE;}

		// Get the max specials at the time of this file
		if((charsRead = fscanf(fp, "%d\n\n", &fileMaxSpecials)) == EOF || charsRead < 1) {return FALSE;}

		//Make sure saves are portable from different dimensions
		if(sizeX > workWidth)
		{
			sizeX = workWidth;
		}
		if(sizeY > workHeight)	
		{
			sizeY = workHeight;
		}


		while(!feof(fp))
		{
			// Check for all particles used
			if(loq <= 0) {return TRUE;}

			// Scan in the initial character
			if((charsRead = fscanf(fp, "%c", &lookAhead)) == EOF || charsRead < 1) {return FALSE;}

			// If it's not '(' then something is malformed, move on
			if(lookAhead != '(') {continue;}


			// Go back to before the open paren
			fseek(fp, -1, SEEK_CUR);
			// Try to read in a particle
			tempParticle = avail[loq-1];
			if((charsRead = fscanf(fp, "(%f %f %d %d %d %d)", &tempParticle->x,
					&tempParticle->y,
					&tempParticle->xVel,
					&tempParticle->yVel,
					&tempParticle->heat,
					&elementIndex)) == EOF
					|| charsRead < 6) {continue;}

			// We succeeded, so decrement loq to remove the particle from being available
			loq--;

			// Save the integral coordinates for writing to allCoords
			i = (int)tempParticle->x;
			j = (int)tempParticle->y;
			if (i >= sizeX || j >= sizeY) { return FALSE; }

			// Keep the element handy
			if (elementIndex >= numElements) { return FALSE; }
			tempElement = elements[elementIndex];

			// If we don't use the element's specialVals, then we need to try to read in more
			if (!tempElement->useElementSpecialVals)
			{
				failed = FALSE;
				// Check that our lookahead is correct
				if((charsRead = fscanf(fp, "%c", &lookAhead)) != EOF && charsRead == 1 && lookAhead == '[')
				{
					int k;
					for (k = 0; k < fileMaxSpecials; k++)
					{
						if((charsRead = fscanf(fp, "%d ", &tempSpecialVal)) == EOF || charsRead < 1)
						{
							failed = TRUE;
							break;
						}

						if (k < MAX_SPECIALS)
						{
							tempParticle->specialVals[k] = tempSpecialVal;
						}
					}

					// Skip past the closing ']'
					fseek(fp, 1, SEEK_CUR);
				}
				else
				{
					// Put the lookAhead char back on the stream
					fseek(fp, -1, SEEK_CUR);
					failed = TRUE;
				}

				if (failed)
				{
					int k;
					for (k = 0; k < MAX_SPECIALS; k++)
					{
						tempParticle->specialVals[k] = tempElement->specialVals[k];
					}
				}
			}

			// Set the element and make the particle set
			tempParticle->element = tempElement;
			tempParticle->set = TRUE;

			// Set the allCoords and bitmap color
			allCoords[getIndex(i, j)] = tempParticle;
			setBitmapColor(tempParticle->x, tempParticle->y, tempElement);
		}

		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

char saveTempToFile(char* saveLoc)
{
	__android_log_write(ANDROID_LOG_INFO, "TheElements", "saveTempToFile");
	FILE* saveFile = fopen(saveLoc, "wb");
	char tempLoc[256];
	strcpy(tempLoc, ROOT_FOLDER);
	strcat(tempLoc, SAVES_FOLDER);
	strcat(tempLoc, TEMP_SAVE);
	strcat(tempLoc, SAVE_EXTENSION);
	FILE* tempFile = fopen(tempLoc, "rb");
	__android_log_write(ANDROID_LOG_INFO, "TheElements", "Opened the files");

	char temp;

	while(!feof(tempFile))
	{
		temp = fgetc(tempFile);
		if(ferror(tempFile))
		{
			__android_log_write(ANDROID_LOG_ERROR, "TheElements", "Error reading from temp.sav file");
			clearerr(tempFile);
			return FALSE;
		}
		else if (!feof(tempFile))
		{
			fputc(temp, saveFile);
			if(ferror(saveFile))
			{
				__android_log_write(ANDROID_LOG_ERROR, "TheElements", "Error writing to save file");
				clearerr(saveFile);
				return FALSE;
			}
		}
	}

	fclose(saveFile);
	fclose(tempFile);

	return TRUE;
}

char loadFileToTemp(char* loadLoc)
{
	FILE* loadFile = fopen(loadLoc, "rb");
	char tempLoc[256];
	strcpy(tempLoc, ROOT_FOLDER);
	strcat(tempLoc, SAVES_FOLDER);
	strcat(tempLoc, TEMP_SAVE);
	strcat(tempLoc, SAVE_EXTENSION);
	FILE* tempFile = fopen(tempLoc, "wb");

	char temp;

	while(!feof(loadFile))
	{
		temp = fgetc(loadFile);
		if(ferror(loadFile))
		{
			__android_log_write(ANDROID_LOG_ERROR, "TheElements", "Error reading from save file");
			clearerr(loadFile);
			break;
		}
		else if (!feof(loadFile))
		{
			fputc(temp, tempFile);
			if(ferror(tempFile))
			{
				__android_log_write(ANDROID_LOG_ERROR, "TheElements", "Error writing to temp.sav file");
				clearerr(tempFile);
				break;
			}
		}
	}

	fclose(loadFile);
	fclose(tempFile);
}

char removeTempSave(void)
{
	char saveLoc[256];
	//Generate the temp save filename
	strcpy(saveLoc, ROOT_FOLDER);
	strcat(saveLoc, SAVES_FOLDER);
	strcat(saveLoc, TEMP_SAVE);
	strcat(saveLoc, SAVE_EXTENSION);

	//Remove the file at that location
	if(remove(saveLoc) == 0)
	{
		return TRUE;
	}

	return FALSE;
}
//TODO:
/*
char saveCustomElement(struct Element* cCElement)
{
	char saveLoc[256];
	strcpy(saveLoc, ROOT_FOLDER);
	strcat(saveLoc, ELEMENTS_FOLDER);
	strcat(saveLoc, createdCustomElement->name);
	strcat(saveLoc, ELEMENT_EXTENSION);

	FILE* saveFile;

	saveFile = fopen(saveFile,"wb");

	fprintf(saveFile, cCElement->name );
	fprintf(saveFile, " %d", cCElement->startingTemp,
						cCElement->lowestTemp,
						cCElement->highestTemp,
						cCElement->lowerElement->index,
						cCElement->higherElement->index,
						cCElement->)



	fclose(saveFile);
	//TODO: Save to saveLoc

	return FALSE;
}
 */

//Saves the custom element to the correct file
char saveCustomElementHash(char* elementHash)
{
	char saveLoc[256];
	char name[256];

	//Extract the name out of the elementHash, there might be a better way to do this but w/e, particularly I don't think the second while is needed but meh
	strcpy(name,elementHash);
	int i = 0;
	while ( elementHash[i] != ' ')
	{
		i++;
	}
	while( i < 256 )
	{
		name[i] = 0;
	}

	strcpy(saveLoc, ROOT_FOLDER);
	strcat(saveLoc, ELEMENTS_FOLDER);
	strcat(saveLoc, name);
	strcat(saveLoc, ELEMENT_EXTENSION);

	FILE* saveFile;

	saveFile = fopen(saveLoc,"wb");
	fprintf( saveFile,elementHash);
	fclose(saveFile);


	return FALSE;
}

char loadCustomElements(void)
{
	glob_t data;
	char loadLoc[256];
	char saveLoc[256];
	//Load the file names of everything in the loading directory
	strcpy(loadLoc, ROOT_FOLDER);
	strcpy(saveLoc, ROOT_FOLDER);
	strcat(loadLoc, ELEMENTS_FOLDER);
	strcat(saveLoc, ELEMENTS_FOLDER);
	strcat(saveLoc, LIST_SAVE);

	strcat(saveLoc, LIST_EXTENSION);

	FILE* sp = fopen(saveLoc, "wb");

	DIR* mydir = opendir(loadLoc);

	struct dirent *entry = NULL;

	while((entry = readdir(mydir))) /* If we get EOF, the expression is 0 and the loop stops. */
	{
		if ( !strcmp(get_filename_ext(entry->d_name),ELEMENT_EXTENSION))
		{
			char location[256];
			strcpy( location, loadLoc);
			strcat( location, entry->d_name);
			loadCustomElement( location );
			fprintf(sp, entry->d_name);
			fprintf(sp, "\n");
		}
	}

	closedir(mydir);
	/*
  switch( glob("*.*", 0, NULL, &data ) )
  {
    case 0:
      break;
    case GLOB_NOSPACE:
    	__android_log_write(ANDROID_LOG_INFO, "TheElements", "Out of Memory");
      break;
    case GLOB_ABORTED:
    	__android_log_write(ANDROID_LOG_INFO, "TheElements", "Reading Error");
      break;
    case GLOB_NOMATCH:
    	__android_log_write(ANDROID_LOG_INFO, "TheElements", "No Files Found");
      break;
    default:
      break;
  }

  __android_log_write(ANDROID_LOG_INFO, "TheElements", "saveload end ");
  int i;
  for(i=0; i<data.gl_pathc; i++)
  {
	  loadCustomElement( data.gl_pathv[i] );
	      fprintf(sp, data.gl_pathv[i]);
	      fprintf(sp, "\n");
	      __android_log_write(ANDROID_LOG_INFO, "TheElements", "saveload printing");
  }
  __android_log_write(ANDROID_LOG_INFO, "TheElements", "saveload end2");

  globfree( &data );
	 */
	fclose(sp);
	return FALSE;
}
char loadCustomElement(char* loadLoc)
{
	FILE* fp;
	fp = fopen(loadLoc, "rb");

	int lowerElementIndex, higherElementIndex;

	struct Element* tempCustom = (struct Element*) malloc(sizeof(struct Element));
	if(fscanf(fp, "%s", &tempCustom->name) == EOF) {return FALSE;}
	if(fscanf(fp, "%d", &tempCustom->base) == EOF) {return FALSE;}
	if(fscanf(fp, "%d", &tempCustom->state) == EOF) {return FALSE;}
	if(fscanf(fp, "%d", &tempCustom->startingTemp) == EOF) {return FALSE;}
	if(fscanf(fp, "%d", &tempCustom->lowestTemp) == EOF) {return FALSE;}
	if(fscanf(fp, "%d", &tempCustom->highestTemp) == EOF) {return FALSE;}
	if(fscanf(fp, "%d", &lowerElementIndex) == EOF) {return FALSE;}
	if(fscanf(fp, "%d", &higherElementIndex) == EOF) {return FALSE;}
	if(fscanf(fp, "%d", &tempCustom->red) == EOF) {return FALSE;}
	if(fscanf(fp, "%d", &tempCustom->green) == EOF) {return FALSE;}
	if(fscanf(fp, "%d", &tempCustom->blue) == EOF) {return FALSE;}
	if(fscanf(fp, "%d", &tempCustom->density) == EOF) {return FALSE;}
	if(fscanf(fp, "%d", &tempCustom->fallVel) == EOF) {return FALSE;}
	if(fscanf(fp, "%d", &tempCustom->inertia) == EOF) {return FALSE;}

	tempCustom->collisions = malloc( NUM_BASE_ELEMENTS * sizeof(char));
	tempCustom->specials = malloc( MAX_SPECIALS*sizeof(char));
	tempCustom->specialVals = malloc( MAX_SPECIALS*sizeof(char));

	int i;
	for (i = 0; i < NUM_BASE_ELEMENTS;i++ )
	{
		fscanf(fp,"%d",&tempCustom->collisions[i]);
	}
	for (i = 0; i < MAX_SPECIALS;i++ )
	{
		fscanf(fp,"%d",&tempCustom->specials[i]);
		fscanf(fp,"%d",&tempCustom->specialVals[i]);
	}

	struct Element** tempElementArray;
	tempElementArray = malloc(numElements*sizeof( struct Element*));
	for ( i = 0; i < numElements; i++)
	{
		tempElementArray[i] = elements[i];
	}
	free(elements);
	elements = malloc((numElements+1)*sizeof( struct Element*));
	for ( i = 0; i < numElements; i++)
	{
		elements[i] = tempElementArray[i];
	}

	tempCustom->index = numElements;
	tempCustom->higherElement = elements[higherElementIndex];
	tempCustom->lowerElement = elements[lowerElementIndex];

	//Increment number of Elements and add the newest element in
	numElements++;
	elements[numElements-1] = tempCustom;

	return FALSE;
}