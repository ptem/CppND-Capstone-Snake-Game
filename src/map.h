#ifndef MAP_H
#define MAP_H

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "SDL.h"

enum class MapTile { Air = 0, Wall = 1 };

class Map {

 public:
   
   //Public-facing walls. Vector of points that will be rendered and to have collision with snake/food checked against.
   std::vector<SDL_Point> walls;

   Map(int grid_width, int grid_height, std::string map_file);

   // Do not want the Map object to be copied elsewhere as for larger games this will be intensive, but movement is okay.
   Map(const Map&) = delete;
   Map(Map&&) = default;
   Map& operator=(const Map&) = delete;
   Map& operator=(Map&&) = default;
   ~Map() = default;

   void AddWall(int x, int y);
   void ShiftWalls(int x, int y);
  //  std::vector<SDL_Point> GetWallNeighbors();
   
 private:
   std::string map_file;

   std::vector<std::vector<MapTile>> map;

  //  std::vector<SDL_Point> neighbors;

   int grid_width;
   int grid_height;

   // Reads a parsed line of the map file, returning a vector of MapTiles.
   std::vector<MapTile> ParseLine(std::string line);

   // Loads the map file and subsequently parses it.
   std::vector<std::vector<MapTile>> LoadMap();

   // Decodes an entry in the map file.
   MapTile GetTileFromChar(char c);

   // Builds the Walls from the parsed map for use by the rest of the game.
   void BuildWalls();
};

#endif