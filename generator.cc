
#include "world.hh"
#include "actor.hh"
#include "common.hh"
#include "logger.hh"

namespace {

	static const Tile ground('.', COLOR_GREEN, !BLOCKS);
	static const Tile grass('"', COLOR_GREEN, !BLOCKS);
	static const Tile plaza(':', COLOR_GREEN, !BLOCKS);
	static const Tile housefloor('.', COLOR_YELLOW, !BLOCKS);

	static const Tile wall('#', COLOR_CYAN, BLOCKS);
	static const Tile window(TileBuilder("window"));
	static const Tile tree('T', COLOR_GREEN, BLOCKS);

	static const Tile door_open(TileBuilder("door_open"));
	static const Tile door_closed(TileBuilder("door_closed"));
	static const Tile door_locked(TileBuilder("door_locked"));

}

void World::createCity(int xhouses, int yhouses) {
	int housew = 8;
	int househ = 6;
	int streetwidth = 3;
	int offset = 1;
	int maxfurnit = 6;

	if (xhouses < 0) xhouses = 10;
	if (yhouses < 0) yhouses = 10;
	width = xhouses * (housew + streetwidth + 1) + streetwidth + 2;
	height = yhouses * (househ + streetwidth + 1) + streetwidth + 2;
	int xyhouses = xhouses * yhouses;

	for (int j = 0; j < height; j++) {
		tilerow row;
		for (int i = 0; i < width; i++) row.push_back(Tile());
		tiles.push_back(row);
	}

	// Array for special house locations
	int CityPlan[xhouses][yhouses];
	for (int j = 0; j < yhouses; ++j)
		for (int i = 0; i < xhouses; ++i)
			CityPlan[i][j] = 0;

	// Array for amounts of special buildings
	int BuildingPlan[6];
	int total = 0;
	// Create amounts for special buildings
	do {
		BuildingPlan[1] = xyhouses / randint(6,12); // plazas
		BuildingPlan[2] = randint(1,2) + randint(0,4); // inns
		BuildingPlan[3] = randint(1,2) + randint(0,4); // smiths/shops
		BuildingPlan[4] = randint(1,2) + randint(0,1) + randint(0,1); // churches
		BuildingPlan[5] = randint(1,2) + randint(0,1) + randint(0,1); // stables
		total = BuildingPlan[1]+BuildingPlan[2]+BuildingPlan[3]+BuildingPlan[4]+BuildingPlan[5];
	} while (total > xhouses * yhouses - 4); // 4 is for town hall
	// Put Town Hall in the middle
	int townhallx, townhally;
	if ((xhouses % 2) == 0) townhallx = xhouses/2; else townhallx = floor(xhouses/2.0) + randint(2);
	if ((yhouses % 2) == 0) townhally = yhouses/2; else townhally = floor(yhouses/2.0) + randint(2);
	CityPlan[townhallx][townhally] = 10;
	CityPlan[townhallx+1][townhally] = 11;
	CityPlan[townhallx][townhally+1] = 11;
	CityPlan[townhallx+1][townhally+1] = 11;
	// Position rest of the special buildings (roughly)
	for (int k = 0; k < total; ++k) {
		int i,j,b;
		while (CityPlan[i=randint(xhouses)][j=randint(yhouses)]);
		while (!BuildingPlan[b = randint(1,5)]);
		BuildingPlan[b]--;
		CityPlan[i][j] = b;
	}
	// Fine tune positioning and generate the structures
	for (int j = 0; j < yhouses; ++j) {
		for (int i = 0; i < xhouses; ++i) {
			int x1 = 1 + streetwidth + i * (housew+streetwidth+1);
			int y1 = 1 + streetwidth + j * (househ+streetwidth+1);
			int x2 = x1 + housew;
			int y2 = y1 + househ;
			switch (CityPlan[i][j]) {
				case 1:
					createPlaza(x1, y1, x2, y2); break;
				//case 2:
					//createInn(x1 + randint(0,offset/2), y1 + randint(0,offset/2), x2 - randint(0,offset/2), y2 - randint(0,offset)/2); break;
				//case 3:
					//createShop(x1 + randint(0,offset), y1 + randint(0,offset), x2 - randint(0,offset), y2 - randint(0,offset)); break;
				//case 4:
					//createChurch(x1 + randint(0,offset/2), y1 + randint(0,offset/2), x2 - randint(0,offset/2), y2 - randint(0,offset/2)); break;
				//case 5:
					//createStable(x1 + randint(0,offset), y1 + randint(0,offset), x2 - randint(0,offset), y2 - randint(0,offset)); break;
				//case 10:
					//createTownHall(x1, y1, x1 + streetwidth + 2*housesize, y1 + streetwidth + 2*housesize); break;
				case 11:
					break;
				default:
					createHouse(x1 + randint(-offset,offset), y1 + randint(-offset,offset), x2 - randint(-offset,offset), y2 - randint(-offset,offset), randint(4,maxfurnit), randint(0,1)); break;
			}
		}
	}
	// Do outer wall and make rest of tiles ground
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			if (i == 0 || j == 0 || i == width-1 || j == height-1) tiles[j][i] = wall;
			else if (tiles[j][i].ch == ' ') tiles[j][i] = ground;
		}
	}
}


void World::createHouse(int x1, int y1, int x2, int y2, int furnit, int locked) {
	makeWallsAndFloor(x1,y1,x2,y2,housefloor);
	int housetype = randint(0,2);
	if (housetype) { // Split house
		bool horiz = randint(1);
		int doorx, doory;
		if (horiz) { // Horizontal wall
			int wy = randint(y1+3,y2-3);
			doorx = randint(x1+1,x2-1);
			doory = wy;
			for (int i = x1; i <= x2; ++i) {
				if (i != doorx || housetype == 2) tiles[wy][i] = wall;
			}
			//AddFurniture(x1+1,y1+1,x2-1,wy-1,furnit/2);
			//AddFurniture(x1+1,wy+1,x2-1,y2-1,furnit/2);
		} else { // Vertical wall
			int wx = randint(x1+3,x2-3);
			doory = randint(y1+1,y2-1);
			doorx = wx;
			for (int j = y1; j <= y2; ++j) {
				if (j != doory || housetype == 2) tiles[j][wx] = wall;
			}
			//AddFurniture(x1+1,y1+1,wx-1,y2-1,furnit/2);
			//AddFurniture(wx+1,y1+1,x2-1,y2-1,furnit/2);
		}
		// Create doors
		if (housetype == 1) { // Two-room house
			makeDoor(doorx,doory,randint(-1,1));
			randDoor(x1,y1,x2,y2,(randint(0,9)==0));
		} else if (housetype == 2) { // Two separate houses
			if (horiz) { // Horizontal wall
				makeDoor(randint(x1+1,x2-1),y1,randint(0,2));
				makeDoor(randint(x1+1,x2-1),y2,randint(0,2));
			} else { // Vertical wall
				makeDoor(x1,randint(y1+1,y2-1),randint(0,2));
				makeDoor(x2,randint(y1+1,y2-1),randint(0,2));
			}
		}

	} else { // No wall / single-room house
		//AddFurniture(x1+1,y1+1,x2-1,y2-1,furnit)
		randDoor(x1,y1,x2,y2,randint(0,2));
	}
}


void World::createPlaza(int x1, int y1, int x2, int y2) {
	bool park = randint(2);
	bool style = randint(2);
	for (int j = y1; j <= y2; ++j) {
		for (int i = x1; i <= x2; ++i) {
			Tile tile(ground);
			if (park) tile = randint(6) ? grass : tree;
			else if (style) tile = plaza;
			tiles[j][i] = tile;
		}
	}
}


/*
void World::CreateTownHall(x1,y1,x2,y2)
	makeWallsAndFloor(x1,y1,x2,y2,diamondpat)
	xx = (x1+x2)/2
	yy = (y1+y2)/2
	if (randint(0,1) = 0) { // horizontal
		wy1 = y1 + (y2-y1)/3
		wy2 = y1 + (y2-y1)/3*2
		wx = x1 + (x2-x1)/3*randint(1,2)
		wx1 = x1 + randint((x2-x1)/3,(x2-x1)/3*2)
		wx2 = x1 + randint((x2-x1)/3,(x2-x1)/3*2)
		wx3 = x1 + randint((x2-x1)/3,(x2-x1)/3*2)
		wx4 = x1 + randint((x2-x1)/3,(x2-x1)/3*2)
		for (int i = x1;  <= x2
			tiles[i,wy1) = wall
			tiles[i,wy2) = wall
			tiles[i,(y1+wy1)/2) = wall
			tiles[i,(y2+wy2)/2) = wall
		}
		for (int j = wy1;  <= wy2; tiles[wx,j) = wall; }
		for (int j = y1;  <= (y1+wy1)/2; tiles[wx1,j) = wall; }
		for (int j = (y1+wy1)/2;  <= wy1; tiles[wx2,j) = wall; }
		for (int j = wy2;  <= (wy2+y2)/2; tiles[wx3,j) = wall; }
		for (int j = (wy2+y2)/2;  <= y2; tiles[wx4,j) = wall; }
		AddFurniture( x1+1, y1+1, x2-1, wy1-1, randint(20,24), diamondpat, ON)
		AddFurniture( x1+1, wy2+1, x2-1, y2-1, randint(20,24), diamondpat, ON)
		makeDoor( wx, (wy1+wy2)/2, 0, diamondpat)
		makeDoor( wx1, randint(y1+1, (y1+wy1)/2-1) , 0, diamondpat)
		makeDoor( wx2, randint((y1+wy1)/2+1, wy1-1), 0, diamondpat)
		makeDoor( wx3, randint(wy2+1, (wy2+y2)/2-1), 0, diamondpat)
		makeDoor( wx4, randint((wy2+y2)/2+1, y2-1) , 0, diamondpat)
		makeDoor( randint(x1+1,wx1-1), (y1+wy1)/2 , 0, diamondpat)
		makeDoor( randint(wx2+1,x2-1), (y1+wy1)/2 , 0, diamondpat)
		makeDoor( randint(x1+1,wx3-1), (wy2+y2)/2 , 0, diamondpat)
		makeDoor( randint(wx4+1,x2-1), (wy2+y2)/2 , 0, diamondpat)
		if (wx < xx) {
			for (int i = wx+2;  <= x2-4 Step 3
				tiles[i,wy1+1) = statue
				tiles[i,wy2-1) = statue
			}
			tiles[x2-3,yy-1 ) = table
			tiles[x2-3,yy ) = table
			tiles[x2-3,yy+1 ) = table
			tiles[x2-2,yy ) = lord
			tiles[x2-1,wy1+1) = closet
			tiles[x2-1,wy2-1) = closet
			tiles[(x1+wx)/2,yy) = statue
			makeDoor(x1,yy,0,diamondpat)
			makeDoor(randint(x1+2,wx-2),wy1,ON,diamondpat)
			makeDoor(randint(x1+2,wx-2),wy2,ON,diamondpat)
		} else {
			for (int i = x1+4;  <= wx-2 Step 3
				tiles[i,wy1+1) = statue
				tiles[i,wy2-1) = statue
			}
			tiles[x1+3,yy-1 ) = table
			tiles[x1+3,yy ) = table
			tiles[x1+3,yy+1 ) = table
			tiles[x1+2,yy ) = lord
			tiles[x1+1,wy1+1) = closet
			tiles[x1+1,wy2-1) = closet
			tiles[(wx+x2)/2,yy) = statue
			makeDoor(x2,yy,0,diamondpat)
			makeDoor(randint(wx+2,x2-2),wy1,ON,diamondpat)
			makeDoor(randint(wx+2,x2-2),wy2,ON,diamondpat)
		}
	} else { // vertical
		wx1 = x1 + (x2-x1)/3
		wx2 = x1 + (x2-x1)/3*2
		wy = y1 + (y2-y1)/3*randint(1,2)
		wy1 = y1 + randint((y2-y1)/3,(y2-y1)/3*2)
		wy2 = y1 + randint((y2-y1)/3,(y2-y1)/3*2)
		wy3 = y1 + randint((y2-y1)/3,(y2-y1)/3*2)
		wy4 = y1 + randint((y2-y1)/3,(y2-y1)/3*2)
		for (int j = y1;  <= y2
			tiles[wx1,j) = wall
			tiles[wx2,j) = wall
			tiles[(x1+wx1)/2,j) = wall
			tiles[(x2+wx2)/2,j) = wall
		}
		for (int i = wx1;  <= wx2; tiles[i,wy) = wall; }
		for (int i = x1;  <= (x1+wx1)/2; tiles[i,wy1) = wall; }
		for (int i = (x1+wx1)/2;  <= wx1; tiles[i,wy2) = wall; }
		for (int i = wx2;  <= (wx2+x2)/2; tiles[i,wy3) = wall; }
		for (int i = (wx2+x2)/2;  <= x2; tiles[i,wy4) = wall; }
		AddFurniture( x1+1, y1+1, wx1-1, y2-1, randint(20,24), diamondpat, ON)
		AddFurniture( wx2+1, y1+1, x2-1, y2-1, randint(20,24), diamondpat, ON)
		makeDoor((wx1+wx2)/2,wy,0)
		makeDoor( randint(x1+1, (x1+wx1)/2-1) , wy1, 0, diamondpat)
		makeDoor( randint((x1+wx1)/2+1, wx1-1), wy2, 0, diamondpat)
		makeDoor( randint(wx2+1, (wx2+x2)/2-1), wy3, 0, diamondpat)
		makeDoor( randint((wx2+x2)/2+1, x2-1) , wy4, 0, diamondpat)
		makeDoor( (x1+wx1)/2, randint(y1+1,wy1-1) , 0, diamondpat)
		makeDoor( (x1+wx1)/2, randint(wy2+1,y2-1) , 0, diamondpat)
		makeDoor( (wx2+x2)/2, randint(y1+1,wy3-1) , 0, diamondpat)
		makeDoor( (wx2+x2)/2, randint(wy4+1,y2-1) , 0, diamondpat)
		if (wy < yy) {
			for (int j = wy+2;  <= y2-4 Step 3
				tiles[wx1+1,j) = statue
				tiles[wx2-1,j) = statue
			}
			tiles[xx-1 ,y2-3) = table
			tiles[xx ,y2-3) = table
			tiles[xx+1 ,y2-3) = table
			tiles[xx ,y2-2) = lord
			tiles[wx1+1,y2-1) = closet
			tiles[wx2-1,y2-1) = closet
			tiles[xx,(y1+wy)/2) = statue
			makeDoor(xx,y1,0,diamondpat)
			makeDoor(wx1,randint(y1+2,wy-2),ON,diamondpat)
			makeDoor(wx2,randint(y1+2,wy-2),ON,diamondpat)
		} else {
			for (int j = y1+4;  <= wy-2 Step 3
				tiles[wx1+1,j) = statue
				tiles[wx2-1,j) = statue
			}
			tiles[xx-1 ,y1+3) = table
			tiles[xx ,y1+3) = table
			tiles[xx+1 ,y1+3) = table
			tiles[xx ,y1+2) = lord
			tiles[wx1+1,y1+1) = closet
			tiles[wx2-1,y1+1) = closet
			tiles[xx,(wy+y2)/2) = statue
			makeDoor(xx,y2,0,diamondpat)
			makeDoor(wx1,randint(wy+2,y2-2),ON,diamondpat)
			makeDoor(wx2,randint(wy+2,y2-2),ON,diamondpat)
		}
	}
}
/* */

/*
void World::CreateInn(x1,y1,x2,y2)
	makeWallsAndFloor(x1,y1,x2,y2,floor)
	if (randint(0,1) = 0) { // horizontal
		yy = floor((y1+y2)/2)
		if (randint(0,1) = 0) {
			makeDoor(x1,yy)
			for (int i = x1+2;  <= x2-4 Step 3
				for (int j = y1+2;  <= y2-2 Step 3
					tiles[j][i] = table
					tiles[i-1+2*randint(1,0),j) = chair
					tiles[i,j-1+2*randint(1,0)) = chair
					tiles[i-1+2*randint(1,0),j) = chair
					tiles[i,j-1+2*randint(1,0)) = chair
				}
			}
			tiles[x1 ,yy-1) = inn
			tiles[x1 ,yy+1) = inn
			tiles[x2-2,yy-1) = table
			tiles[x2-2,yy ) = table
			tiles[x2-2,yy+1) = table
			tiles[x2-1,y1+1) = barrel
			tiles[x2-1,y2-1) = barrel
			if (randint(0,1)) { tiles[x2-randint(1,2),y1+randint(1,2)) = barrel
			if (randint(0,1)) { tiles[x2-randint(1,2),y2-randint(1,2)) = barrel
			tiles[x2-3+2*randint(1,0),yy-1+randint(0,2)) = innkeeper
		} else {
			makeDoor(x2,yy)
			for (int i = x1+4;  <= x2-2 Step 3
				for (int j = y1+2;  <= y2-2 Step 3
					tiles[j][i] = table
					tiles[i-1+2*randint(1,0),j) = chair
					tiles[i,j-1+2*randint(1,0)) = chair
					tiles[i-1+2*randint(1,0),j) = chair
					tiles[i,j-1+2*randint(1,0)) = chair
				}
			}
			tiles[x2 ,yy-1) = inn
			tiles[x2 ,yy+1) = inn
			tiles[x1+2,yy-1) = table
			tiles[x1+2,yy ) = table
			tiles[x1+2,yy+1) = table
			tiles[x1+1,y1+1) = barrel
			tiles[x1+1,y2-1) = barrel
			if (randint(0,1)) { tiles[x1+randint(1,2),y1+randint(1,2)) = barrel
			if (randint(0,1)) { tiles[x1+randint(1,2),y2-randint(1,2)) = barrel
			tiles[x1+3-2*randint(1,0),yy-1+randint(0,2)) = innkeeper
		}
	} else { // vertical
		xx = floor((x1+x2)/2)
		if (randint(0,1) = 0) {
			makeDoor(xx,y1)
			for (int i = x1+2;  <= x2-2 Step 3
				for (int j = y1+2;  <= y2-4 Step 3
					tiles[j][i] = table
					tiles[i-1+2*randint(1,0),j) = chair
					tiles[i,j-1+2*randint(1,0)) = chair
					tiles[i-1+2*randint(1,0),j) = chair
					tiles[i,j-1+2*randint(1,0)) = chair
				}
			}
			tiles[xx-1,y1 ) = inn
			tiles[xx+1,y1 ) = inn
			tiles[xx-1,y2-2) = table
			tiles[xx ,y2-2) = table
			tiles[xx+1,y2-2) = table
			tiles[x1+1,y2-1) = barrel
			tiles[x2-1,y2-1) = barrel
			if (randint(0,1)) { tiles[x1+randint(1,2),y2-randint(1,2)) = barrel
			if (randint(0,1)) { tiles[x2-randint(1,2),y2-randint(1,2)) = barrel
			tiles[xx-1+randint(0,2),y2-3+2*randint(1,0)) = innkeeper
		} else {
			makeDoor(xx,y2)
			for (int i = x1+2;  <= x2-2 Step 3
				for (int j = y1+4;  <= y2-2 Step 3
					tiles[j][i] = table
					tiles[i-1+2*randint(1,0),j) = chair
					tiles[i,j-1+2*randint(1,0)) = chair
					tiles[i-1+2*randint(1,0),j) = chair
					tiles[i,j-1+2*randint(1,0)) = chair
				}
			}
			tiles[xx-1,y2) = inn
			tiles[xx+1,y2) = inn
			tiles[xx-1,y1+2) = table
			tiles[xx ,y1+2) = table
			tiles[xx+1,y1+2) = table
			tiles[x1+1,y1+1) = barrel
			tiles[x2-1,y1+1) = barrel
			if (randint(0,1)) { tiles[x1+randint(1,2),y1+randint(1,2)) = barrel
			if (randint(0,1)) { tiles[x2-randint(1,2),y1+randint(1,2)) = barrel
			tiles[xx-1+randint(0,2),y1+3-2*randint(1,0)) = innkeeper
		}
	}
}
/* */

/*
void World::CreateShop(x1,y1,x2,y2)
	makeWallsAndFloor(x1,y1,x2,y2,floor)
	// inner wall
	if (randint(0,1) = 0) { // horizontal
		wy = randint(y1+3,y2-3)
		doorx = randint(x1+1,x2-1)
		doory = wy
		for (int i = x1;  <= x2
			if (i != doorx) { tiles[i,wy) = wall
		}
		if (wy < (y1+y2)/2) { tempside = 0 } else { tempside = 1
		if (tempside = 0) {
			AddFurniture(x1+1,y1+1,x2-1,wy-1,randint(3,5))
			for (int i = 1;  <= randint(5,10)
				if (randint(0,2) = 0) { temptile = barrel } else { temptile = closet_locked
				if (randint(1,5) = 5) { temptile = table
				tiles[randint(x1+1,x2-1),randint(wy+1,y2-1)) = temptile
			}
			tiles[randint(x1+1,x2-1),randint(wy+1,y2-1)) = shopkeeper
			doorx2 = randint(x1+1,x2-1)
			doory2 = y2
		} else {
			AddFurniture(x1+1,wy+1,x2-1,y2-1,randint(3,5))
			for (int i = 1;  <= randint(5,10)
				if (randint(0,2) = 0) { temptile = barrel } else { temptile = closet_locked
				if (randint(1,5) = 5) { temptile = table
				tiles[randint(x1+1,x2-1),randint(y1+1,wy-1)) = temptile
			}
			tiles[randint(x1+1,x2-1),randint(y1+1,wy-1)) = shopkeeper
			doorx2 = randint(x1+1,x2-1)
			doory2 = y1
		}
		tiles[doorx2-1+2*randint(0,1),doory2) = shop
	} else { // vertical
		wx = randint(x1+3,x2-3)
		doory = randint(y1+1,y2-1)
		doorx = wx
		for (int j = y1;  <= y2
			if (j != doory) { tiles[wx,j) = wall
		}
		if (wx < (x1+x2)/2) { tempside = 0 } else { tempside = 1
		if (tempside = 0) {
			AddFurniture(x1+1,y1+1,wx-1,y2-1,randint(3,5))
			for (int i = 1;  <= randint(5,10)
				if (randint(0,2) = 0) { temptile = barrel } else { temptile = closet_locked
				if (randint(1,5) = 5) { temptile = table
				tiles[randint(wx+1,x2-1),randint(y1+1,y2-1)) = temptile
			}
			tiles[randint(wx+1,x2-1),randint(y1+1,y2-1)) = shopkeeper
			doorx2 = x2
			doory2 = randint(y1+1,y2-1)
		} else {
			AddFurniture(wx+1,y1+1,x2-1,y2-1,randint(3,5))
			for (int i = 1;  <= randint(5,10)
				if (randint(0,2) = 0) { temptile = barrel } else { temptile = closet_locked
				if (randint(1,5) = 5) { temptile = table
				tiles[randint(x1+1,wx-1),randint(y1+1,y2-1)) = temptile
			}
			tiles[randint(x1+1,wx-1),randint(y1+1,y2-1)) = shopkeeper
			doorx2 = x1
			doory2 = randint(y1+1,y2-1)
		}
		tiles[doorx2,doory2-1+2*randint(0,1)) = shop
	}
	makeDoor(doorx,doory,1)
	makeDoor(doorx2,doory2)
}
/* */

/*
void World::CreateStable(x1,y1,x2,y2)
	makeWallsAndFloor(x1,y1,x2,y2,floor)
	// inner wall
	if (randint(0,1) = 0) { // horizontal wall
		wy = randint(y1+3,y2-3)
		doorx = randint(x1+1,x2-1)
		doory = wy
		for (int i = x1;  <= x2
			if (i != doorx) { tiles[i,wy) = wall
		}
		if (randint(0,1) = 0) {
			AddFurniture(x1+1,y1+1,x2-1,wy-1,randint(3,5))
			for (int i = 1;  <= randint(3,10)
				tiles[randint(x1+1,x2-1),randint(wy+1,y2-1)) = horsetile
			}
			stableboyX = randint(x1+1,x2-1); stableboyY = randint(wy+1,y2-1)
			doorx2 = randint(x1+2,x2-2)
			doory2 = y2
		} else {
			AddFurniture(x1+1,wy+1,x2-1,y2-1,randint(3,5))
			for (int i = 1;  <= randint(3,10)
				tiles[randint(x1+1,x2-1),randint(y1+1,wy-1)) = horsetile
			}
			stableboyX = randint(x1+1,x2-1); stableboyY = randint(y1+1,wy-1)
			doorx2 = randint(x1+2,x2-2)
			doory2 = y1
		}
		makeDoor(doorx2 ,doory2,-1)
		makeDoor(doorx2-1,doory2,-1)
		makeDoor(doorx2+1,doory2,-1)
		tiles[doorx2-2+4*randint(0,1),doory2) = stable
	} else { // vertical wall
		wx = randint(x1+3,x2-3)
		doory = randint(y1+1,y2-1)
		doorx = wx
		for (int j = y1;  <= y2
			if (j != doory) { tiles[wx,j) = wall
		}
		if (randint(0,1) = 0) {
			AddFurniture(x1+1,y1+1,wx-1,y2-1,randint(3,5))
			for (int i = 1;  <= randint(3,10)
				tiles[randint(wx+1,x2-1),randint(y1+1,y2-1)) = horsetile
			}
			stableboyX = randint(wx+1,x2-1); stableboyY = randint(y1+1,y2-1)
			doorx2 = x2
			doory2 = randint(y1+2,y2-2)
		} else {
			AddFurniture(wx+1,y1+1,x2-1,y2-1,randint(3,5))
			for (int i = 1;  <= randint(3,10)
				tiles[randint(x1+1,wx-1),randint(y1+1,y2-1)) = horsetile
			}
			stableboyX = randint(x1+1,wx-1); stableboyY = randint(y1+1,y2-1)
			doorx2 = x1
			doory2 = randint(y1+2,y2-2)
		}
		makeDoor(doorx2,doory2 ,-1)
		makeDoor(doorx2,doory2-1,-1)
		makeDoor(doorx2,doory2+1,-1)
		tiles[doorx2,doory2-2+4*randint(0,1)) = stable
	}
	makeDoor(doorx,doory,1)
	tiles[stableboyX,stableboyY) = stableboy
}
/* */

/*
void World::CreateChurch(x1,y1,x2,y2)
	makeWallsAndFloor(x1,y1,x2,y2,diamondpat,1)
	if (randint(0,1) = 0) { // horizontal
		yy = floor((y1+y2)/2)
		if (randint(0,1) = 0) {
			makeDoor(x1,yy)
			for (int i = x1+2;  <= x2-4
				for (int j = y1+2;  <= y2-2
					if (j != yy) { tiles[j][i] = chair
				}
			}
			tiles[x1 ,yy-1) = church
			tiles[x1 ,yy+1) = church
			tiles[x2-2,yy-1) = table
			tiles[x2-2,yy ) = table
			tiles[x2-2,yy+1) = table
			tiles[x2-1,y1+1) = closet
			tiles[x2-1,y2-1) = closet
			tiles[x2 ,yy ) = window
			tiles[x2-3+2*randint(1,0),yy-1+randint(0,2)) = priest
		} else {
			makeDoor(x2,yy)
			for (int i = x1+4;  <= x2-2
				for (int j = y1+2;  <= y2-2
					if (j != yy) { tiles[j][i] = chair
				}
			}
			tiles[x2 ,yy-1) = church
			tiles[x2 ,yy+1) = church
			tiles[x1+2,yy-1) = table
			tiles[x1+2,yy ) = table
			tiles[x1+2,yy+1) = table
			tiles[x1+1,y1+1) = closet
			tiles[x1+1,y2-1) = closet
			tiles[x1 ,yy ) = window
			tiles[x1+3-2*randint(1,0),yy-1+randint(0,2)) = priest
		}
		for (int i = x1+2;  <= x2-2 Step 2
			tiles[i,y1) = window
			tiles[i,y2) = window
		}
	} else { // vertical
		xx = floor((x1+x2)/2)
		if (randint(0,1) = 0) {
			makeDoor(xx,y1)
			for (int i = x1+2;  <= x2-2
				for (int j = y1+2;  <= y2-4
					if (i != xx) { tiles[j][i] = chair
				}
			}
			tiles[xx-1,y1 ) = church
			tiles[xx+1,y1 ) = church
			tiles[xx-1,y2-2) = table
			tiles[xx ,y2-2) = table
			tiles[xx+1,y2-2) = table
			tiles[x1+1,y2-1) = closet
			tiles[x2-1,y2-1) = closet
			tiles[xx ,y2 ) = window
			tiles[xx-1+randint(0,2),y2-3+2*randint(1,0)) = priest
		} else {
			makeDoor(xx,y2)
			for (int i = x1+2;  <= x2-2
				for (int j = y1+4;  <= y2-2
					if (i != xx) { tiles[j][i] = chair
				}
			}
			tiles[xx-1,y2) = church
			tiles[xx+1,y2) = church
			tiles[xx-1,y1+2) = table
			tiles[xx ,y1+2) = table
			tiles[xx+1,y1+2) = table
			tiles[x1+1,y1+1) = closet
			tiles[x2-1,y1+1) = closet
			tiles[xx ,y1 ) = window
			tiles[xx-1+randint(0,2),y1+3-2*randint(1,0)) = priest
		}
		for (int j = y1+2;  <= y2-2 Step 2
			tiles[x1,j) = window
			tiles[x2,j) = window
		}
	}
}
/* */

/*
void World::AddFurniture(x1,y1,x2,y2,furnit,Tile floortype=12,nobed=0)
	furniturecount = 0
	While furniturecount < furnit
		RandFurnit:
		i = randint((x1+x2)/2,x2); j = randint((y1+y2)/2,y2)
		if (furniturecount < furnit*.75) { i = randint(x1 , (x1+x2)/2); j = randint((y1+y2)/2, y2)
		if (furniturecount < furnit*.50) { i = randint((x1+x2)/2, x2); j = randint(y1 , (y1+y2)/2)
		if (furniturecount < furnit*.25) { i = randint(x1 , (x1+x2)/2); j = randint(y1 , (y1+y2)/2)
		// i = randint(x1,x2); j = randint(y1,y2)
		if (tiles[j][i] != floortype) { Goto RandFurnit
		furniturecount+1
		if ((furniturecount = 1 || randint(1,10) = 1) && nobed = 0) {
			int temp = 0;
			do {
				temp++;
				i = randint(x1,x2-1); j = randint(y1,y2)
				if (temp > 30) { Goto nospace
			} while (!!! tiles[j][i] = floor && tiles[i+1,j) = floortype
			tiles[j][i] = bedL
			tiles[i+1,j) = bedR
			nospace:
		} else if ( (j = y1 || j = y2 || i = x1 || i = x2) {
			if (randint(1,4) = 1) { tiles[j][i] = randint(table,chair) } else { tiles[j][i] = closet
			if (tiles[j][i] = closet && randint(1,3) = 1) { tiles[j][i] = barrel
			if (tiles[j][i] = closet && randint(0,1) = 1) { tiles[j][i] = closet_locked
		} else {
			if (randint(1,4) = 4) {
				tiles[j][i] = randint(table,chair)
				if (randint(1,4) = 4) { tiles[j][i] = barrel
			} else {
				tiles[j][i] = table
				rnd1 = randint(0,1)
				rnd2 = randint(0,1)
				if (rnd2 = 0) {
					if (tiles[i-1+rnd1*2,j) = floortype) {
						tiles[i-1+rnd1*2,j) = chair
					} else if ( (tiles[i-1+(!rnd1)*2,j) = floortype) {
						tiles[i-1+(!rnd1)*2,j) = chair
					} else if ( (tiles[i,j-1+rnd1*2) = floortype) {
						tiles[i,j-1+rnd1*2) = chair
					} else if ( (tiles[i,j-1+(!rnd1)*2) = floortype) {
						tiles[i,j-1+(!rnd1)*2) = chair
					}
				} else {
					if (tiles[i,j-1+rnd1*2) = floortype) {
						tiles[i,j-1+rnd1*2) = chair
					} else if ( (tiles[i,j-1+(!rnd1)*2) = floortype) {
						tiles[i,j-1+(!rnd1)*2) = chair
					} else if ( (tiles[i-1+rnd1*2,j) = floortype) {
						tiles[i-1+rnd1*2,j) = chair
					} else if ( (tiles[i-1+(!rnd1)*2,j) = floortype) {
						tiles[i-1+(!rnd1)*2,j) = chair
					}
				}
				furniturecount+1
			}
		}
	Wend
}
*/

void World::randDoor(int x1, int y1, int x2, int y2, int locked) {
	int doorx, doory, rnd1;
	do {
		doorx = randint(x1+1,x2-1);
		doory = randint(y1+1,y2-1);
		rnd1 = randint(0,1);
		if (randint(0,1) == 1) doorx = rnd1*x1 + (!rnd1)*x2;
		else doory = rnd1*y1 + (!rnd1)*y2;
	//} while ((tiles[doorx-1][doory])>wall) + (tiles[doorx+1,doory)>wall) + (tiles[doorx,doory-1)>wall) + (tiles[doorx,doory+1)>wall) < 3);
	} while (false);
	makeDoor(doorx,doory,locked);
}



void World::makeDoor(int doorx, int doory, int locked, Tile floortype) {
	if (locked == 0) tiles[doory][doorx] = door_open;
	else if (locked == 1) tiles[doory][doorx] = door_closed;
	else if (locked == 2) tiles[doory][doorx] = door_locked;
	else tiles[doory][doorx] = floortype;
	//if (tiles[doorx,doory-1) = bedL) tiles[doorx+1,doory-1) = floortype;
	//if (tiles[doorx,doory-1) = bedR) tiles[doorx-1,doory-1) = floortype;
	//if (tiles[doorx,doory+1) = bedL) tiles[doorx+1,doory+1) = floortype;
	//if (tiles[doorx,doory+1) = bedR) tiles[doorx-1,doory+1) = floortype;
	//if (tiles[doorx+1,doory) = bedL) tiles[doorx+2,doory ) = floortype;
	//if (tiles[doorx-1,doory) = bedR) tiles[doorx-2,doory ) = floortype;
	//if (tiles[doory][doorx] => table && tiles[doorx-1,doory) < lord) tiles[doorx-1,doory) = floortype;
	//if (tiles[doory][doorx] => table && tiles[doorx+1,doory) < lord) tiles[doorx+1,doory) = floortype;
	//if (tiles[doory][doorx] => table && tiles[doorx,doory-1) < lord) tiles[doorx,doory-1) = floortype;
	//if (tiles[doory][doorx] => table && tiles[doorx,doory+1) < lord) tiles[doorx,doory+1) = floortype;
}


void World::makeWallsAndFloor(int x1, int y1, int x2, int y2, Tile floortype, bool nowindows, bool nowalls) {
	// floor
	for (int j = y1+1; j < y2; ++j) {
		for (int i = x1+1; i < x2; ++i) {
			tiles[j][i] = floortype;
		}
	}
	// walls
	if (!nowalls) {
		for (int i = x1; i <= x2; ++i) { // horizontal
			if (randint(1,10) == 1 && i != x1 && i != x2 && nowindows == 0) tiles[y1][i] = window; else tiles[y1][i] = wall;
			if (randint(1,10) == 1 && i != x1 && i != x2 && nowindows == 0) tiles[y2][i] = window; else tiles[y2][i] = wall;
		}
		for (int j = y1; j <= y2; ++j) { // vertical
			if (randint(1,10) == 1 && j != y1 && j != y2 && nowindows == 0) tiles[j][x1] = window; else tiles[j][x1] = wall;
			if (randint(1,10) == 1 && j != y1 && j != y2 && nowindows == 0) tiles[j][x2] = window; else tiles[j][x2] = wall;
		}
	}
}
