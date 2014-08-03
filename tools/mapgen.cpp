
#include <assert.h>
#include "mapgen.h"

//Default map settings
#define DEFAULT_MAP_HEIGHT 28
#define DEFAULT_MAP_WIDTH 150

#define DEFAULT_MIN_PLATFORM_WIDTH 2
#define DEFAULT_MAX_PLATFORM_WIDTH 5

#define DEFAULT_MIN_GAP_WIDTH 2
#define DEFAULT_MAX_GAP_WIDTH 4

#define DEFAULT_MIN_PLATFORM_POSITION_CHANGE -2
#define DEFAULT_MAX_PLATFORM_POSITION_CHANGE 2

#define DEFAULT_INITIAL_PLATFORM_HEIGHT 4

//internal stuff, careful!
#define BUFFER_SIZE 50

#define GRASSTYPE_SHORT 0
#define GRASSTYPE_TALL 1

Mapgen::Mapgen() {
  setWidth(DEFAULT_MAP_WIDTH);
  setHeight(DEFAULT_MAP_HEIGHT);
  setPlatformWidth(DEFAULT_MIN_PLATFORM_WIDTH, DEFAULT_MAX_PLATFORM_WIDTH);
  setGapWidth(DEFAULT_MIN_GAP_WIDTH, DEFAULT_MAX_GAP_WIDTH);
  setPlatformPositionChange(DEFAULT_MIN_PLATFORM_POSITION_CHANGE, DEFAULT_MAX_PLATFORM_POSITION_CHANGE);
  setMinPlatformPosition(DEFAULT_MAP_HEIGHT / 2);
  setInitialPlatformHeight(DEFAULT_INITIAL_PLATFORM_HEIGHT);

  useShortGrass();
  
  srand(time(NULL));
}

void Mapgen::setWidth(int width){iWidth = width;}
void Mapgen::setHeight(int height){iHeight = height;}
void Mapgen::setPlatformWidth(int min_width, int max_width){
  iMinPlatformWidth = min_width; 
  iMaxPlatformWidth = max_width;
}
void Mapgen::setGapWidth(int min_gap_width, int max_gap_width){
  iMinGapWidth = min_gap_width;
  iMaxGapWidth = max_gap_width;
}
void Mapgen::setPlatformPositionChange(int min, int max) {
  iMinPlatformPositionChange = min;
  iMaxPlatformPositionChange = max;
}
void Mapgen::setMinPlatformPosition(int min) { iMinPlatformPosition = min;}
void Mapgen::setInitialPlatformHeight(int initial){iInitialPlatformHeight=initial;}

void Mapgen::useShortGrass(){iGrassType = GRASSTYPE_SHORT;}
void Mapgen::useTallGrass(){iGrassType = GRASSTYPE_TALL;}

void addTileSet(TiXmlElement *map, int gid, const char* name,int tilewidth, int tileheight, const char *filename, int width, int height) {
  char buffer[BUFFER_SIZE];

  TiXmlElement *tileset = new TiXmlElement("tileset");
  map->LinkEndChild(tileset);
  tileset->SetAttribute("name",name);

  sprintf(buffer, "%d", gid);
  tileset->SetAttribute("firstgid",buffer);

  sprintf(buffer, "%d",tilewidth);
  tileset->SetAttribute("tilewidth",buffer);

  sprintf(buffer, "%d",tileheight);  
  tileset->SetAttribute("tileheight",buffer);

  TiXmlElement *image = new TiXmlElement("image");
  tileset->LinkEndChild(image);
  image->SetAttribute("source", filename);

  sprintf(buffer, "%d",height);  
  image->SetAttribute("height",buffer);

  sprintf(buffer, "%d",width);  
  image->SetAttribute("width",buffer);
}
 
void Mapgen::addBackground(TiXmlElement *map, int width, int height, const char *background_source){
  char buffer[BUFFER_SIZE];

  TiXmlElement *imagelayer = new TiXmlElement("imagelayer");
  map->LinkEndChild(imagelayer);
  
  imagelayer->SetAttribute("name","background");

  sprintf(buffer,"%d",width);
  imagelayer->SetAttribute("width",buffer);

  sprintf(buffer,"%d", height);
  imagelayer->SetAttribute("height",buffer);

  TiXmlElement *image = new TiXmlElement("image");
  imagelayer->LinkEndChild(image);

  image->SetAttribute("source",background_source);
  
}
 


void Mapgen::populate_mapbuffer(int *mapbuffer, int width, int height) {

  int max_fillheight = height - 1;

  memset(mapbuffer, 0, width * height);  //assumes 0 is empty
  
  int current_height = height - iInitialPlatformHeight;
  int current_col = 0;                   //here we are 0 indexed

  int old_height;
  
  while(current_col < width) {
    old_height = current_height;

    int panelwidth = rand() % (iMaxPlatformWidth - iMinPlatformWidth) + iMinPlatformWidth;

    //        printf("rand times (%d - %d) + %d\n",iMaxPlatformPositionChange, iMinPlatformPositionChange, iMinPlatformPositionChange);

    int delta_height = rand() % (iMaxPlatformPositionChange - iMinPlatformPositionChange) + iMinPlatformPositionChange;

    //    printf("Current height = %d   Delta height %d",current_height, delta_height);
    current_height = current_height + delta_height;                         //adjust height
    current_height = std::min(current_height, max_fillheight);              //block illegal platform positions
    current_height = std::max(current_height, iMinPlatformPosition);    //block illegal platform positions
    //    printf(" final height %d\n",current_height);

    int end_col = std::min(width-1, current_col + panelwidth);              //write the data
    //    printf("current_col = %d    end_col = %d   current_height = %d\n",current_col, end_col, current_height);
    
    int firstcol = 1;                                        //true if this is first column of platform
    //    int ascending = current_height > old_height;
    //    int descending = current_height < old_height;

    for(;current_col <= end_col; current_col++) {
      for(int row = current_height; row < height; row++) {
	if(row==current_height) {
	  if(firstcol) {
	    switch(iGrassType) {
	    case GRASSTYPE_SHORT:
		if(rand()%2)
		  mapbuffer[row*width + current_col] = 52;
		else
		  mapbuffer[row*width + current_col] = 54;
		break;
	    case GRASSTYPE_TALL:
	      mapbuffer[row*width + current_col] = 103;
	      break;
	    default:
	      assert(0);
	    }

	  } else if(current_col==end_col) {
	    switch(iGrassType) {
	    case GRASSTYPE_SHORT:
		if(rand()%2)
		  mapbuffer[row*width + current_col] = 51;
		else
		  mapbuffer[row*width + current_col] = 53;
	      break;
	    case GRASSTYPE_TALL:
		switch (rand()%3) {
		case 2:
		  mapbuffer[row*width + current_col] = 104;
		  break;
		case 1:
		  mapbuffer[row*width + current_col] = 105;
		  break;
		case 0:
		  mapbuffer[row*width + current_col] = 102;
		  break;
		}
	      break;
	    default:
	      assert(0);
	    }
	  } else {
	    switch(iGrassType) {
	    case GRASSTYPE_SHORT:
		mapbuffer[row*width + current_col] = 6 + rand() % 45;
		break;
	    case GRASSTYPE_TALL:
		mapbuffer[row*width + current_col] = 56 + rand() % 46;
		//		printf("%d\n",		mapbuffer[row*width + current_col]);
		break;
	    default:
	      assert(0);
	    }
	  }
	  //	  printf("Setting (%d, %d) = 3\n",row, current_col);
	} else {
	  mapbuffer[row*width + current_col] = 1 + rand() % 4;
	  //	  printf("Setting (%d, %d) = 9\n",row, current_col);
	}
      }
      firstcol = 0;
    }
    
    int gap_length = rand() % (iMaxGapWidth - iMinGapWidth) + iMinGapWidth;
    current_col = current_col + gap_length; 
  }
}


void Mapgen::populate_doorbuffer(int *doorbuffer, int *mapbuffer, int width, int height) {
  int door_row;
  
  memset(mapbuffer, 0, width * height);  //assumes 0 is empty


  int door1, door2, door3, door4;

  //FIND WHERE THE ROW WHERE THE DOOR SHOULD GO       this is awful
  for(door1 = 0; mapbuffer[door1 * width + width-2]==0 && door1 < height - 1; door1++) {
    assert((door1 + 1) * width + width - 1 <= width * height);
  }
  for(door2 = 0; mapbuffer[door2 * width + width-3]==0 && door2 < height - 1; door2++) {
    assert((door2 + 1) * width + width - 1 <= width * height);
  }
  for(door3 = 0; mapbuffer[door3 * width + width-4]==0 && door3 < height - 1; door3++) {
    assert((door3 + 1) * width + width - 1 <= width * height);
  }
  for(door4 = 0; mapbuffer[door4 * width + width-5]==0 && door4 < height - 1; door4++) {
    assert((door4 + 1) * width + width - 1 <= width * height);
  }
  door_row = std::min(std::min(std::min(door1, door2), door3), door4);
  
  int door_initial_col = width - 5;
  
  int k = 106;

  for(int i = 4; i >= 0; i--) {
    for(int j = 0; j < 4; j++) {
      //      fprintf(stderr, "populating at %d \n",(door_row - j) * width + door_initial_col + j);
      assert((door_row - i) * width + door_initial_col + j >= 0);
      assert((door_row - i) * width + door_initial_col + j < iWidth * iHeight);

      doorbuffer[(door_row - i) * width + door_initial_col + j] = k;
      k = k + 1;

    }
  }
  
}


void Mapgen::addGrass(TiXmlElement *map, int *mapbuffer, int width, int height) {
  char buffer[BUFFER_SIZE];
  
  TiXmlElement *layer = new TiXmlElement("layer");
  map->LinkEndChild(layer);
  
  layer->SetAttribute("name","Grass");

  sprintf(buffer,"%d",width);
  layer->SetAttribute("width",buffer);

  sprintf(buffer,"%d", height);
  layer->SetAttribute("height",buffer);

  TiXmlElement *data = new TiXmlElement("data");
  layer->LinkEndChild(data);


  for(int row = 0; row < height; row++) {
    for(int col = 0; col < width; col++) {
      TiXmlElement *tile = new TiXmlElement("tile");
      data->LinkEndChild(tile);
      sprintf(buffer, "%d", mapbuffer[row * width + col]);
      tile->SetAttribute("gid",buffer);
    }
  }
}


void Mapgen::addDoor(TiXmlElement *map, int *mapbuffer, int width, int height) {
  char buffer[BUFFER_SIZE];
  
  TiXmlElement *layer = new TiXmlElement("layer");
  map->LinkEndChild(layer);
  
  layer->SetAttribute("name","Static");

  sprintf(buffer,"%d",width);
  layer->SetAttribute("width",buffer);

  sprintf(buffer,"%d", height);
  layer->SetAttribute("height",buffer);

  TiXmlElement *data = new TiXmlElement("data");
  layer->LinkEndChild(data);


  for(int row = 0; row < height; row++) {
    for(int col = 0; col < width; col++) {
      TiXmlElement *tile = new TiXmlElement("tile");
      data->LinkEndChild(tile);
      sprintf(buffer, "%d", mapbuffer[row * width + col]);
      tile->SetAttribute("gid",buffer);
    }
  }
}

void Mapgen::addMap(TiXmlDocument *doc){
  
  char buffer[BUFFER_SIZE];

  TiXmlElement *map = new TiXmlElement("map");
  doc->LinkEndChild(map);
  map->SetAttribute("version","1.0");
  map->SetAttribute("orientation","orthogonal");

  sprintf(buffer,"%d", iWidth);
  map->SetAttribute("width",buffer);
  sprintf(buffer,"%d", iHeight);
  map->SetAttribute("height",buffer);
  map->SetAttribute("tilewidth","32");
  map->SetAttribute("tileheight","32");
  

  addTileSet(map,1,"fill",32,32,"../graphics/fill.png",64,64);
  addTileSet(map,5,"dood",64,192,"../graphics/dood.png",64,192);
  addTileSet(map,6,"grass",32,64,"../graphics/grass.png",1600,64);
  addTileSet(map,56,"grassfore",32,96,"../graphics/grassfore.png",1600,96);
  addTileSet(map,106,"door",32,32,"../graphics/door.png",128,160);
  addTileSet(map,126,"solid",32,32,"../graphics/solid.png",64,32);
  addTileSet(map,128,"edges",32,32,"../graphics/edges.png",160,96);



  addBackground(map, iWidth, iHeight, "../graphics/background.png");

  int *mapbuffer = new int[iWidth*iHeight];
  populate_mapbuffer(mapbuffer, iWidth, iHeight);
  addGrass(map, mapbuffer, iWidth, iHeight);

  int *doorbuffer = new int[iWidth*iHeight];
  populate_doorbuffer(doorbuffer, mapbuffer, iWidth, iHeight);
  addDoor(map, doorbuffer, iWidth, iHeight);
}


void Mapgen::generateMap(const char *output_file )
{
  TiXmlDocument doc;
  TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
  doc.LinkEndChild( decl );
  addMap(&doc);
  doc.SaveFile(output_file);
}

int main() {
  Mapgen m;
  m.setWidth(800);
  m.setHeight(28);   //28



  m.setPlatformWidth(10, 20);
  m.setGapWidth(2, 7);
  //  m.setPlatformPositionChange(DEFAULT_MIN_PLATFORM_POSITION_CHANGE, DEFAULT_MAX_PLATFORM_POSITION_CHANGE);
  //  m.setMinPlatformPosition(DEFAULT_MAP_HEIGHT / 2);
  //  m.setInitialPlatformHeight(DEFAULT_INITIAL_PLATFORM_HEIGHT);


  m.useShortGrass();
  m.useTallGrass();
  
  m.generateMap("asdf.tmx");
}

