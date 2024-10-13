#ifndef FOOD_MANAGER_H
#define FOOD_MANAGER_H

#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include "SDL.h"
#include "messagequeue.h"

struct FoodItem {
  int x;
  int y;
  int id;
  std::chrono::time_point<std::chrono::steady_clock> creation_time;

  // Constructor to initialize the creation time
  FoodItem(int x, int y, int id)
      : x(x), y(y), id(id), creation_time(std::chrono::steady_clock::now()) {}
};

class FoodManager {
 public:
  FoodManager(int grid_width, int grid_height);

  void Start(int food_count);

  MessageQueue<FoodItem> &GetQueue() { return food_queue; }

 private:
  void CreateAndRemoveFood(int id);

  int active_food_count = 0;
  std::mutex food_mutex;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_h;
  std::uniform_int_distribution<int> random_w;
  MessageQueue<FoodItem> food_queue;
};

#endif  // FOOD_MANAGER_H
