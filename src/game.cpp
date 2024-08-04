#include "game.h"
#include <iostream>
#include <condition_variable>
#include "SDL.h"
#include <cmath>
#include "map.h"


Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      map(grid_width, grid_height, "../src/data/map.txt"),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  
  PlaceFood();
  PlaceFoodNew();
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    Update();
    renderer.Render(snake, foodgrow, foodshrink, map.walls);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood() {
    int x, y;
    while (true) {
      x = random_w(engine);
      y = random_h(engine);
      // Check that the location is not occupied by a snake or other food item before placing food.
      if (!snake.SnakeCell(x, y) && !SamePoint(foodshrink, x, y) && !CollidesWithMap(x, y)) {
        foodgrow.x = x;
        foodgrow.y = y;
        break;
    }
    }
}

void Game::PlaceFoodNew() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake or other food item before placing food.
    if (!snake.SnakeCell(x, y) && !SamePoint(foodgrow, x, y) && !CollidesWithMap(x, y)) {
      foodshrink.x = x;
      foodshrink.y = y;
      break;
    }
  }
}

//Helper Function. Returns true if coordinates a(x,y) are equal to (x,y).
bool Game::SamePoint(SDL_Point a, int x, int y) {
  return (a.x == x && a.y == y); 
}

//Helper Function. Return true if two SDL_Points occupy the same coordinates.
bool Game::SamePoint(SDL_Point a, SDL_Point b) {
  return (a.x == b.x && a.y == b.y);
}

//Helper Function. Returns true if coordinates (x,y) collide with the map.
bool Game::CollidesWithMap(int x, int y) {
  for (SDL_Point wall : map.walls){
    if (SamePoint(wall, x, y)) {
      return true;
    }
  }
  return false;
}

void Game::Update() {
  if (!snake.alive) return;

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (SamePoint(foodgrow, new_x, new_y)) {
    score++;
    PlaceFood();
    snake.GrowBody(); // Grow + increase fall rate by a small amount.
    wall_shift_timer_duration -= 0.1;
  }

  if (SamePoint(foodshrink, new_x, new_y)) {
    score += 5;
    number_of_shrink_food_eaten++;
    PlaceFoodNew();
    snake.ShrinkBody(); // Shrink + increase fall rate, more if more shrink food have been eaten.
    wall_shift_timer_duration -= 0.1 * number_of_shrink_food_eaten;
  }

  if (CollidesWithMap(new_x, new_y)) {
    snake.alive = false;
  }
  
  if (!is_wall_shift_active) {
    if(shifted_y == 0) {
      map.ShiftWalls(1, 0);
      shifted_x++;
    } else {
      map.ShiftWalls(0, 1);
      shifted_y = 0;
    }
    PlaceFoodNew();
    is_wall_shift_active = true;
    wall_shift_thread = std::thread(&Game::WallShiftTimer, this);
    wall_shift_thread.detach();
    already_shifted = true;
  }
} 



void Game::WallShiftTimer() {
  auto start_time = std::chrono::high_resolution_clock::now();
  
  std::unique_lock<std::mutex> lock(shift_mutex);
  
  while(is_wall_shift_active) {
    lock.unlock();



    auto current_time = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
    if (elapsed_seconds >= wall_shift_timer_duration) {
    	  //Time to shift the walls.
        if (shifted_x == 5) {
          shifted_y = 1;
          shifted_x = 0;
        }
      	is_wall_shift_active = false;
        break;
    }

    lock.lock();
    
    //Dont check again for a bit
    con_var.wait_for(lock, std::chrono::milliseconds(int(std::floor(wall_shift_timer_duration * 1000 * 0.5))));
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }

