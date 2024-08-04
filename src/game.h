#ifndef GAME_H
#define GAME_H

#include <random>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "map.h"


class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;

 private:
  Snake snake;
  SDL_Point foodgrow;
  SDL_Point foodshrink;
  Map map;


  double wall_shift_timer_duration = 3;
  int number_of_shrink_food_eaten{0};

  int shifted_x{0};
  int shifted_y{0};
 
  bool is_wall_shift_active{false};
  bool already_shifted{false};

  std::thread wall_shift_thread;
  std::mutex shift_mutex;
  std::condition_variable con_var;

  void WallShiftTimer();

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};
  
  void PlaceFood();
  void PlaceFoodNew();
  void Update();

  bool SamePoint(SDL_Point a, int x, int y);
  bool SamePoint(SDL_Point a, SDL_Point b);
  bool CollidesWithMap(int x, int y);
};

#endif