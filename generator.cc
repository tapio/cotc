/**
 * File: generator.cc
 *
 * This file contains functions to generate the world.
 * The town generator is ported from CoolBasic sources
 * of an incomplete role playing game Knight of Faith.
 *
 * BEWARE: The code is very hackish.
 */

#include "world.hh"
#include "actor.hh"
#include "common.hh"

namespace {

	static const Tile ground(TileBuilder("Ground"));
	static const Tile grass(TileBuilder("Grass"));
	static const Tile plaza(TileBuilder("Plaza"));
	static const Tile housefloor(TileBuilder("Floor"));
	static const Tile fancyfloor(TileBuilder("Plaza"));

	static const Tile wall(TileBuilder("Wall"));
	static const Tile window(TileBuilder("Window"));
	static const Tile tree(TileBuilder("Tree"));
	static const Tile statue(TileBuilder("Tree"));

	static const Tile chair(TileBuilder("Chair"));
	static const Tile table(TileBuilder("Table"));
	static const Tile barrel(TileBuilder("Barrel"));
	static const Tile closet(TileBuilder("Closet"));
	static const Tile closet_locked(TileBuilder("Locked closet"));
	static const Tile bedL(TileBuilder("Left bed"));
	static const Tile bedR(TileBuilder("Right bed"));

	static const Tile door_open(TileBuilder("Open door"));
	static const Tile door_closed(TileBuilder("Closed door"));
	static const Tile door_locked(TileBuilder("Locked door"));

}

void World::createCity(int xhouses, int yhouses) {
	int housew = 8;
	int househ = 6;
	int streetwidth = 3;
	int offset = 1;

	if (xhouses < 0) xhouses = 10;
	if (yhouses < 0) yhouses = 10;
	width = xhouses * (housew + streetwidth + 1) + streetwidth + 2;
	height = yhouses * (househ + streetwidth + 1) + streetwidth + 2;
	int xyhouses = xhouses * yhouses;

	// Initialize tile array
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
				case 10:
					createTownHall(x1, y1, x1 + streetwidth + 2*housew, y1 + streetwidth + 2*househ); break;
				case 11:
					break;
				default:
					int fcount = (x2-x1)*(y2-y1)/randint(5,6);
					createHouse(x1 + randint(-offset,offset), y1 + randint(-offset,offset), x2 - randint(-offset,offset), y2 - randint(-offset,offset), fcount, randint(0,2)); break;
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
	const int minsize = 6;
	makeWallsAndFloor(x1,y1,x2,y2,housefloor);
	int xsize = x2-x1, ysize = y2-y1;
	int housetype = randint(0,2);
	if (housetype && (xsize > minsize || ysize > minsize)) { // Split house
		bool horiz = randbool();
		if (xsize <= minsize) horiz = true;
		else if (ysize <= minsize) horiz = false;
		int doorx, doory;
		if (horiz) { // Horizontal wall
			int wy = randint(y1+3,y2-3);
			doorx = randint(x1+1,x2-1);
			doory = wy;
			for (int i = x1; i <= x2; ++i) {
				if (i != doorx || housetype == 2) tiles[wy][i] = wall;
			}
			// Place door
			if (housetype == 1) { // Two-room house
				makeDoor(doorx,doory,randint(0,1));
				randDoor(x1,y1,x2,y2,(randint(0,9)==0));
			} else if (housetype == 2) { // Two separate houses
				makeDoor(randint(x1+1,x2-1),y1,randint(0,2));
				makeDoor(randint(x1+1,x2-1),y2,randint(0,2));
			}
			// Add furniture
			addFurniture(x1+1,y1+1,x2-1,wy-1,furnit/2);
			addFurniture(x1+1,wy+1,x2-1,y2-1,furnit/2);
		} else { // Vertical wall
			int wx = randint(x1+3,x2-3);
			doory = randint(y1+1,y2-1);
			doorx = wx;
			for (int j = y1; j <= y2; ++j) {
				if (j != doory || housetype == 2) tiles[j][wx] = wall;
			}
			// Place door
			if (housetype == 1) { // Two-room house
				makeDoor(doorx,doory,randint(0,1));
				randDoor(x1,y1,x2,y2,(randint(0,9)==0));
			} else if (housetype == 2) { // Two separate houses
				makeDoor(x1,randint(y1+1,y2-1),randint(0,2));
				makeDoor(x2,randint(y1+1,y2-1),randint(0,2));
			}
			// Add furniture
			addFurniture(x1+1,y1+1,wx-1,y2-1,furnit/2);
			addFurniture(wx+1,y1+1,x2-1,y2-1,furnit/2);
		}
	} else { // No wall / single-room house
		randDoor(x1,y1,x2,y2,randint(0,2));
		addFurniture(x1+1,y1+1,x2-1,y2-1,furnit);
	}
}


void World::createPlaza(int x1, int y1, int x2, int y2) {
	bool park = randbool();
	bool style = randbool();
	for (int j = y1; j <= y2; ++j) {
		for (int i = x1; i <= x2; ++i) {
			Tile tile(ground);
			if (park) tile = randint(6) ? grass : tree;
			else if (style) tile = plaza;
			tiles[j][i] = tile;
		}
	}
}

void World::createTownHall(int x1, int y1, int x2, int y2) {
	makeWallsAndFloor(x1,y1,x2,y2,fancyfloor);
	int xx = (x1+x2)/2;
	int yy = (y1+y2)/2;
	if (randbool()) { // horizontal
		int wy1 = y1 + (y2-y1)/3;
		int wy2 = y1 + (y2-y1)/3*2;
		int wx = x1 + (x2-x1)/3*randint(1,2);
		int wx1 = x1 + randint((x2-x1)/3,(x2-x1)/3*2);
		int wx2 = x1 + randint((x2-x1)/3,(x2-x1)/3*2);
		int wx3 = x1 + randint((x2-x1)/3,(x2-x1)/3*2);
		int wx4 = x1 + randint((x2-x1)/3,(x2-x1)/3*2);
		for (int i = x1; i <= x2; i++) {
			tiles[wy1][i] = wall;
			tiles[wy2][i] = wall;
			tiles[(y1+wy1)/2][i] = wall;
			tiles[(y2+wy2)/2][i] = wall;
		}
		for (int j = wy1; j <= wy2; j++) tiles[j][wx] = wall;
		for (int j = y1; j <= (y1+wy1)/2; j++) tiles[j][wx1] = wall;
		for (int j = (y1+wy1)/2; j <= wy1; j++) tiles[j][wx2] = wall;
		for (int j = wy2; j <= (wy2+y2)/2; j++) tiles[j][wx3] = wall;
		for (int j = (wy2+y2)/2; j <= y2; j++) tiles[j][wx4] = wall;
		addFurniture( x1+1, y1+1, x2-1, wy1-1, randint(20,24), fancyfloor, 1);
		addFurniture( x1+1, wy2+1, x2-1, y2-1, randint(20,24), fancyfloor, 1);
		makeDoor(wx, (wy1+wy2)/2, 0, fancyfloor);
		makeDoor(wx1, randint(y1+1, (y1+wy1)/2-1) , 0, fancyfloor);
		makeDoor(wx2, randint((y1+wy1)/2+1, wy1-1), 0, fancyfloor);
		makeDoor(wx3, randint(wy2+1, (wy2+y2)/2-1), 0, fancyfloor);
		makeDoor(wx4, randint((wy2+y2)/2+1, y2-1) , 0, fancyfloor);
		makeDoor(randint(x1+1,wx1-1), (y1+wy1)/2 , 0, fancyfloor);
		makeDoor(randint(wx2+1,x2-1), (y1+wy1)/2 , 0, fancyfloor);
		makeDoor(randint(x1+1,wx3-1), (wy2+y2)/2 , 0, fancyfloor);
		makeDoor(randint(wx4+1,x2-1), (wy2+y2)/2 , 0, fancyfloor);
		if (wx < xx) {
			for (int i = wx+2; i <= x2-4; i+=3) {
				tiles[wy1+1][i] = statue;
				tiles[wy2-1][i] = statue;
			}
			tiles[yy-1][x2-3] = table;
			tiles[yy][x2-3] = table;
			tiles[yy+1][x2-3] = table;
			tiles[wy1+1][x2-1] = closet;
			tiles[wy2-1][x2-1] = closet;
			tiles[yy][(x1+wx)/2] = statue;
			makeDoor(x1, yy, 0, fancyfloor);
			makeDoor(randint(x1+2,wx-2), wy1, 1, fancyfloor);
			makeDoor(randint(x1+2,wx-2), wy2, 1, fancyfloor);
		} else {
			for (int i = x1+4; i <= wx-2; i+=3) {
				tiles[wy1+1][i] = statue;
				tiles[wy2-1][i] = statue;
			}
			tiles[yy-1][x1+3] = table;
			tiles[yy][x1+3] = table;
			tiles[yy+1][x1+3] = table;
			tiles[wy1+1][x1+1] = closet;
			tiles[wy2-1][x1+1] = closet;
			tiles[yy][(wx+x2)/2] = statue;
			makeDoor(x2, yy, 0, fancyfloor);
			makeDoor(randint(wx+2,x2-2), wy1, 1, fancyfloor);
			makeDoor(randint(wx+2,x2-2), wy2, 1, fancyfloor);
		}
	} else { // vertical
		int wx1 = x1 + (x2-x1)/3;
		int wx2 = x1 + (x2-x1)/3*2;
		int wy = y1 + (y2-y1)/3*randint(1,2);
		int wy1 = y1 + randint((y2-y1)/3,(y2-y1)/3*2);
		int wy2 = y1 + randint((y2-y1)/3,(y2-y1)/3*2);
		int wy3 = y1 + randint((y2-y1)/3,(y2-y1)/3*2);
		int wy4 = y1 + randint((y2-y1)/3,(y2-y1)/3*2);
		for (int j = y1; j <= y2; j++) {
			tiles[j][wx1] = wall;
			tiles[j][wx2] = wall;
			tiles[j][(x1+wx1)/2] = wall;
			tiles[j][(x2+wx2)/2] = wall;
		}
		for (int i = wx1; i <= wx2; i++) tiles[wy][i] = wall;
		for (int i = x1; i <= (x1+wx1)/2; i++) tiles[wy1][i] = wall;
		for (int i = (x1+wx1)/2; i <= wx1; i++) tiles[wy2][i] = wall;
		for (int i = wx2; i <= (wx2+x2)/2; i++) tiles[wy3][i] = wall;
		for (int i = (wx2+x2)/2; i <= x2; i++) tiles[wy4][i] = wall;
		addFurniture( x1+1, y1+1, wx1-1, y2-1, randint(20,24), fancyfloor, 1);
		addFurniture( wx2+1, y1+1, x2-1, y2-1, randint(20,24), fancyfloor, 1);
		makeDoor((wx1+wx2)/2,wy,0);
		makeDoor(randint(x1+1, (x1+wx1)/2-1) , wy1, 0, fancyfloor);
		makeDoor(randint((x1+wx1)/2+1, wx1-1), wy2, 0, fancyfloor);
		makeDoor(randint(wx2+1, (wx2+x2)/2-1), wy3, 0, fancyfloor);
		makeDoor(randint((wx2+x2)/2+1, x2-1) , wy4, 0, fancyfloor);
		makeDoor((x1+wx1)/2, randint(y1+1,wy1-1) , 0, fancyfloor);
		makeDoor((x1+wx1)/2, randint(wy2+1,y2-1) , 0, fancyfloor);
		makeDoor((wx2+x2)/2, randint(y1+1,wy3-1) , 0, fancyfloor);
		makeDoor((wx2+x2)/2, randint(wy4+1,y2-1) , 0, fancyfloor);
		if (wy < yy) {
			for (int j = wy+2; j <= y2-4; j+=3) {
				tiles[j][wx1+1] = statue;
				tiles[j][wx2-1] = statue;
			}
			tiles[y2-3][xx-1] = table;
			tiles[y2-3][xx] = table;
			tiles[y2-3][xx+1] = table;
			tiles[y2-1][wx1+1] = closet;
			tiles[y2-1][wx2-1] = closet;
			tiles[(y1+wy)/2][xx] = statue;
			makeDoor(xx, y1, 0, fancyfloor);
			makeDoor(wx1, randint(y1+2,wy-2), 1, fancyfloor);
			makeDoor(wx2, randint(y1+2,wy-2), 1, fancyfloor);
		} else {
			for (int j = y1+4; j <= wy-2; j+=3) {
				tiles[j][wx1+1] = statue;
				tiles[j][wx2-1] = statue;
			}
			tiles[y1+3][xx-1] = table;
			tiles[y1+3][xx] = table;
			tiles[y1+3][xx+1] = table;
			tiles[y1+1][wx1+1] = closet;
			tiles[y1+1][wx2-1] = closet;
			tiles[(wy+y2)/2][xx] = statue;
			makeDoor(xx, y2, 0, fancyfloor);
			makeDoor(wx1, randint(wy+2,y2-2), 1, fancyfloor);
			makeDoor(wx2, randint(wy+2,y2-2), 1, fancyfloor);
		}
	}
}

void World::createInn(int x1, int y1, int x2, int y2) {
/*
	makeWallsAndFloor(x1,y1,x2,y2,floor);
	if (randint(0,1) = 0) { // horizontal
		yy = floor((y1+y2)/2);
		if (randint(0,1) = 0) {
			makeDoor(x1,yy);
			for (int i = x1+2;  <= x2-4 Step 3
				for (int j = y1+2;  <= y2-2 Step 3
					tiles[j][i] = table;
					tiles[i-1+2*randint(1,0),j) = chair;
					tiles[i,j-1+2*randint(1,0)) = chair;
					tiles[i-1+2*randint(1,0),j) = chair;
					tiles[i,j-1+2*randint(1,0)) = chair;
				}
			}
			tiles[x1 ,yy-1) = inn;
			tiles[x1 ,yy+1) = inn;
			tiles[x2-2,yy-1) = table;
			tiles[x2-2,yy ) = table;
			tiles[x2-2,yy+1) = table;
			tiles[x2-1,y1+1) = barrel;
			tiles[x2-1,y2-1) = barrel;
			if (randint(0,1)) tiles[x2-randint(1,2),y1+randint(1,2)) = barrel;
			if (randint(0,1)) tiles[x2-randint(1,2),y2-randint(1,2)) = barrel;
			tiles[x2-3+2*randint(1,0),yy-1+randint(0,2)) = innkeeper;
		} else {
			makeDoor(x2,yy)
			for (int i = x1+4;  <= x2-2 Step 3
				for (int j = y1+2;  <= y2-2 Step 3
					tiles[j][i] = table;
					tiles[i-1+2*randint(1,0),j) = chair;
					tiles[i,j-1+2*randint(1,0)) = chair;
					tiles[i-1+2*randint(1,0),j) = chair;
					tiles[i,j-1+2*randint(1,0)) = chair;
				}
			}
			tiles[x2 ,yy-1) = inn;
			tiles[x2 ,yy+1) = inn;
			tiles[x1+2,yy-1) = table;
			tiles[x1+2,yy ) = table;
			tiles[x1+2,yy+1) = table;
			tiles[x1+1,y1+1) = barrel;
			tiles[x1+1,y2-1) = barrel;
			if (randint(0,1)) { tiles[x1+randint(1,2),y1+randint(1,2)) = barrel;
			if (randint(0,1)) { tiles[x1+randint(1,2),y2-randint(1,2)) = barrel;
			tiles[x1+3-2*randint(1,0),yy-1+randint(0,2)) = innkeeper;
		}
	} else { // vertical
		xx = floor((x1+x2)/2);
		if (randint(0,1) = 0) {
			makeDoor(xx,y1);
			for (int i = x1+2;  <= x2-2 Step 3
				for (int j = y1+2;  <= y2-4 Step 3
					tiles[j][i] = table;
					tiles[i-1+2*randint(1,0),j) = chair;
					tiles[i,j-1+2*randint(1,0)) = chair;
					tiles[i-1+2*randint(1,0),j) = chair;
					tiles[i,j-1+2*randint(1,0)) = chair;
				}
			}
			tiles[xx-1,y1 ) = inn;
			tiles[xx+1,y1 ) = inn;
			tiles[xx-1,y2-2) = table;
			tiles[xx ,y2-2) = table;
			tiles[xx+1,y2-2) = table;
			tiles[x1+1,y2-1) = barrel;
			tiles[x2-1,y2-1) = barrel;
			if (randint(0,1)) tiles[x1+randint(1,2),y2-randint(1,2)) = barrel;
			if (randint(0,1)) tiles[x2-randint(1,2),y2-randint(1,2)) = barrel;
			tiles[xx-1+randint(0,2),y2-3+2*randint(1,0)) = innkeeper;
		} else {
			makeDoor(xx,y2);
			for (int i = x1+2;  <= x2-2 Step 3
				for (int j = y1+4;  <= y2-2 Step 3
					tiles[j][i] = table;
					tiles[i-1+2*randint(1,0),j) = chair;
					tiles[i,j-1+2*randint(1,0)) = chair;
					tiles[i-1+2*randint(1,0),j) = chair;
					tiles[i,j-1+2*randint(1,0)) = chair;
				}
			}
			tiles[xx-1,y2) = inn;
			tiles[xx+1,y2) = inn;
			tiles[xx-1,y1+2) = table;
			tiles[xx ,y1+2) = table;
			tiles[xx+1,y1+2) = table;
			tiles[x1+1,y1+1) = barrel;
			tiles[x2-1,y1+1) = barrel;
			if (randint(0,1)) tiles[x1+randint(1,2),y1+randint(1,2)) = barrel;
			if (randint(0,1)) tiles[x2-randint(1,2),y1+randint(1,2)) = barrel;
			tiles[xx-1+randint(0,2),y1+3-2*randint(1,0)) = innkeeper;
		}
	}
*/
}

void World::createChurch(int x1, int y1, int x2, int y2) {
/*
	makeWallsAndFloor(x1,y1,x2,y2,fancyfloor,1);
	if (randint(0,1) = 0) { // horizontal
		yy = floor((y1+y2)/2);
		if (randint(0,1) = 0) {
			makeDoor(x1,yy);
			for (int i = x1+2;  <= x2-4
				for (int j = y1+2;  <= y2-2
					if (j != yy) { tiles[j][i] = chair;
				}
			}
			tiles[x1 ,yy-1) = church;
			tiles[x1 ,yy+1) = church;
			tiles[x2-2,yy-1) = table;
			tiles[x2-2,yy ) = table;
			tiles[x2-2,yy+1) = table;
			tiles[x2-1,y1+1) = closet;
			tiles[x2-1,y2-1) = closet;
			tiles[x2 ,yy ) = window;
			tiles[x2-3+2*randint(1,0),yy-1+randint(0,2)) = priest;
		} else {
			makeDoor(x2,yy)
			for (int i = x1+4;  <= x2-2
				for (int j = y1+2;  <= y2-2
					if (j != yy) tiles[j][i] = chair;
				}
			}
			tiles[x2 ,yy-1) = church;
			tiles[x2 ,yy+1) = church;
			tiles[x1+2,yy-1) = table;
			tiles[x1+2,yy ) = table;
			tiles[x1+2,yy+1) = table;
			tiles[x1+1,y1+1) = closet;
			tiles[x1+1,y2-1) = closet;
			tiles[x1 ,yy ) = window;
			tiles[x1+3-2*randint(1,0),yy-1+randint(0,2)) = priest;
		}
		for (int i = x1+2;  <= x2-2 Step 2
			tiles[i,y1) = window;
			tiles[i,y2) = window;
		}
	} else { // vertical
		xx = floor((x1+x2)/2)
		if (randint(0,1) = 0) {
			makeDoor(xx,y1)
			for (int i = x1+2;  <= x2-2
				for (int j = y1+2;  <= y2-4
					if (i != xx) tiles[j][i] = chair;
				}
			}
			tiles[xx-1,y1 ) = church;
			tiles[xx+1,y1 ) = church;
			tiles[xx-1,y2-2) = table;
			tiles[xx ,y2-2) = table;
			tiles[xx+1,y2-2) = table;
			tiles[x1+1,y2-1) = closet;
			tiles[x2-1,y2-1) = closet;
			tiles[xx ,y2 ) = window;
			tiles[xx-1+randint(0,2),y2-3+2*randint(1,0)) = priest;
		} else {
			makeDoor(xx,y2)
			for (int i = x1+2;  <= x2-2
				for (int j = y1+4;  <= y2-2
					if (i != xx) tiles[j][i] = chair;
				}
			}
			tiles[xx-1,y2) = church;
			tiles[xx+1,y2) = church;
			tiles[xx-1,y1+2) = table;
			tiles[xx ,y1+2) = table;
			tiles[xx+1,y1+2) = table;
			tiles[x1+1,y1+1) = closet;
			tiles[x2-1,y1+1) = closet;
			tiles[xx ,y1 ) = window;
			tiles[xx-1+randint(0,2),y1+3-2*randint(1,0)) = priest;
		}
		for (int j = y1+2;  <= y2-2 Step 2
			tiles[x1,j) = window;
			tiles[x2,j) = window;
		}
	}
*/
}

namespace {
	bool freePosition(int x, int y, tilearray& tiles, Tile floortype) {
		return tiles[y][x] == floortype
		   && (tiles[y-1][x] == floortype)
		   && (tiles[y+1][x] == floortype)
		   && (tiles[y][x+1] == floortype)
		   && (tiles[y][x-1] == floortype);
	}
}


void World::addFurniture(int x1, int y1, int x2, int y2, int furnit, Tile floortype, bool nobed) {
	int furniturecount = 0;
	while (furniturecount < furnit) {
		int i, j, cnt = 0;
		do {
			if (furnit > 4) {
				// Divide to the room
				i = randint((x1+x2)/2,x2);
				j = randint((y1+y2)/2,y2);
				if (furniturecount < furnit*.75) { i = randint(x1, (x1+x2)/2); j = randint((y1+y2)/2, y2); }
				if (furniturecount < furnit*.50) { i = randint((x1+x2)/2, x2); j = randint(y1, (y1+y2)/2); }
				if (furniturecount < furnit*.25) { i = randint(x1, (x1+x2)/2); j = randint(y1, (y1+y2)/2); }
			} else {
				i = randint(x1,x2);
				j = randint(y1,y2);
			}
			if (++cnt > 30) return;
		} while (!freePosition(i,j,tiles,floortype));
		furniturecount++;
		// Bed?
		if ((randint(1,3) == 1) && !nobed) {
			nobed = true;
			cnt = 0;
			while (cnt < 30) {
				cnt++;
				i = randint(x1,x2-1);
				j = randint(y1,y2);
				if (freePosition(i,j,tiles,floortype) && freePosition(i+1,j,tiles,floortype)) {
					tiles[j][i] = bedL;
					tiles[j][i+1] = bedR;
					break;
				}
			}
			furniturecount++;
		// Things placed next to a wall
		} else if (j == y1 || j == y2 || i == x1 || i == x2) {
			if (randint(1,4) == 1) tiles[j][i] = randbool() ? table : chair; else tiles[j][i] = closet;
			if (tiles[j][i] == closet && randint(1,3) == 1) tiles[j][i] = barrel;
			if (tiles[j][i] == closet && randint(0,1) == 1) tiles[j][i] = closet_locked;
		// Things going to the center
		} else {
			if (randint(1,3) == 1) {
				tiles[j][i] = randbool() ? table : chair;
				if (randint(1,3) == 1) tiles[j][i] = barrel;
			} else {
				tiles[j][i] = table;
				int newchairs = 0, tries = 0;
				while (furniturecount < furnit && newchairs < 2 && tries < 10) {
					bool vaxis = randbool();
					int dx = vaxis ? 0 : randint(-1,1);
					int dy = vaxis ? randint(-1,1) : 0;
					if (tiles[j+dy][i+dx] == floortype) {
						tiles[j+dy][i+dx] = chair;
						furniturecount++; newchairs++;
					}
					tries++;
				}
			}
		}
	}
}


void World::randDoor(int x1, int y1, int x2, int y2, int locked) {
	int doorx, doory, rnd1;
	do {
		doorx = randint(x1+1,x2-1);
		doory = randint(y1+1,y2-1);
		rnd1 = randint(0,1);
		if (randbool()) doorx = rnd1*x1 + (!rnd1)*x2;
		else doory = rnd1*y1 + (!rnd1)*y2;
	} while (std::abs((int)tiles[doory-1][doorx].blocks_movement +
	                  (int)tiles[doory+1][doorx].blocks_movement +
	                  (int)tiles[doory][doorx-1].blocks_movement +
	                  (int)tiles[doory][doorx+1].blocks_movement) > 3);
	makeDoor(doorx,doory,locked);
}


void World::makeDoor(int doorx, int doory, int locked, Tile floortype) {
	if (locked == 0) tiles[doory][doorx] = door_open;
	else if (locked == 1) tiles[doory][doorx] = door_closed;
	else if (locked == 2) tiles[doory][doorx] = door_locked;
	else tiles[doory][doorx] = floortype;
	// Below checks not needed, doors placed before furniture
	//if (tiles[doory-1][doorx] == bedL) tiles[doory-1][doorx+1] = floortype;
	//if (tiles[doory-1][doorx] == bedR) tiles[doory-1][doorx-1] = floortype;
	//if (tiles[doory+1][doorx] == bedL) tiles[doory+1][doorx+1] = floortype;
	//if (tiles[doory+1][doorx] == bedR) tiles[doory+1][doorx-1] = floortype;
	//if (tiles[doory][doorx+1] == bedL) tiles[doory][doorx+2] = floortype;
	//if (tiles[doory][doorx-1] == bedR) tiles[doory][doorx-2] = floortype;
	//if (tiles[doory-1][doorx].blocks_movement) tiles[doory-1][doorx] = floortype;
	//if (tiles[doory+1][doorx].blocks_movement) tiles[doory+1][doorx] = floortype;
	//if (tiles[doory][doorx+1].blocks_movement) tiles[doory][doorx+1] = floortype;
	//if (tiles[doory][doorx-1].blocks_movement) tiles[doory][doorx-1] = floortype;
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
