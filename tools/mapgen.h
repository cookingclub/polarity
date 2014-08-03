#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include <algorithm>

#include "tinyxml.h"

#define TILEWIDTH 32
#define TILEHEIGHT 32

class Mapgen {
public:
Mapgen();


void generateMap(const char *output_file);

// *****************  customize settings *********************
void setWidth(int width);
void setHeight(int height);

void setPlatformWidth(int min_width, int max_width);
void setGapWidth(int min_gap_width, int max_gap_width);

//Note Position = maxheight - height.       i.e. height is from bottom, position is measured from top
void setPlatformPositionChange(int min_delta, int max_delta);
void setMinPlatformPosition(int min);
void setInitialPlatformHeight(int initial_height);

void useShortGrass();
void useTallGrass();

protected:
int iWidth, iHeight;

int iMinPlatformWidth, iMaxPlatformWidth;
int iMinGapWidth, iMaxGapWidth;
int iMinPlatformPositionChange, iMaxPlatformPositionChange;
int iMinPlatformPosition;
int iInitialPlatformHeight;

int iGrassType;

void populate_mapbuffer(int *mapbuffer, int width, int height);
 void populate_doorbuffer(int *doorbuffer, int *mapbuffer, int width, int height);
void addBackground(TiXmlElement *map, int width, int height, const char *background_source);
 void addGrass(TiXmlElement *map, int *mapbuffer, int width, int height);
 void addDoor(TiXmlElement *map, int *mapbuffer, int width, int height);
 void addMap(TiXmlDocument *doc);
 
};
  
