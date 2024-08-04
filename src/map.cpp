#include "map.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "SDL.h"

Map::Map(int grid_width, int grid_height, std::string map_file) {
    this->grid_width = grid_width;
    this->grid_height = grid_height;
    this->map_file = map_file;

    map = LoadMap();
    std::cout << "Initialized Map Class \n";
    BuildWalls();
}

std::vector<std::vector<MapTile>> Map::LoadMap() {
    std::vector<std::vector<MapTile>> mappy;

    std::cout << "Loading Map " << map_file << "\n";
    std::ifstream loaded_file (map_file);
    std::string line;

    if (loaded_file.is_open()) {

        int rows = 0;
        while (getline (loaded_file, line)) {
            mappy.push_back(Map::ParseLine(std::move(line)));
            rows++;
        }

        std::cout << "Parsed " << rows << " rows. \n";
    } else {
        std::cout << "Unable to open file. \n";
    }

    return mappy;
}

std::vector<MapTile> Map::ParseLine(std::string line){
    std::vector<char> v(line.begin(), line.end());
    std::vector<MapTile> row;

    int i = 0;
    int num_walls = 0;
    for (char c : v) {
        MapTile tile = GetTileFromChar(c);
        row.push_back(tile);
        i++;
        if (tile == MapTile::Wall){
            num_walls++;
        }
    }
    return row;
}

MapTile Map::GetTileFromChar(char c) {
    switch(c) {
        case '0':
            return MapTile::Air;
        case '1':
            return MapTile::Wall;

        default:
            return MapTile::Air;
    }
}

void Map::BuildWalls() {
    walls.clear();
    for (int i = 0; i < map.size(); i++) {
        for (int j = 0; j < map[i].size(); j++) {
            if (map[i][j] == MapTile::Wall) {
                SDL_Point wall;
                wall.x = j;
                wall.y = i;
                walls.push_back(wall);
            }
        }
    }
}

void Map::AddWall(int x, int y) {
    if (map[x][y] != MapTile::Wall) {
        map[x][y] = MapTile::Wall;
        BuildWalls();
        // std::cout << "Added new wall block at (" << x << "," << y << "). \n";
    }
}

void Map::ShiftWalls(int x, int y) {
    if (x != 0) {
        std::rotate(map.begin(), map.end()-x, map.end());
    }
    if (y != 0) {
        for (int i = 0; i < map.size(); i++) {
            std::rotate(map[i].begin(), map[i].end()-y, map[i].end());
        }
    }

    BuildWalls();
}