#ifndef GAME_H
#define GAME_H

#include <mutex>
#include <vector>
#include "controller.h"
#include "foodmanager.h"
#include "renderer.h"

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;

 private:
  Snake snake;
  FoodManager _food_manager;
  std::vector<FoodItem> active_food;  // Store active food items

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  std::mutex food_mutex;
  int score{0};

  void PlaceFood();
  void Update();
  void ManageFood();
};

#endif