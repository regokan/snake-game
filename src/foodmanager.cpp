#include "foodmanager.h"

FoodManager::FoodManager(int grid_width, int grid_height)
    : random_w(0, grid_width - 1),
      random_h(0, grid_height - 1),
      engine(dev()) {}

void FoodManager::Start(int food_count) {
  for (int i = 0; i < food_count; ++i) {
    std::thread(&FoodManager::CreateAndRemoveFood, this, i).detach();
  }
}

void FoodManager::CreateAndRemoveFood(int id) {
  while (true) {
    std::this_thread::sleep_for(
        std::chrono::seconds(5 + rand() % 6));  // Wait 5-10 seconds

    // Use mutex to safely check and update the active food count
    std::lock_guard<std::mutex> lock(food_mutex);
    FoodItem food{random_w(engine), random_h(engine), id};
    std::cout << "Creating food item " << id << " at (" << food.x << ", "
              << food.y << ")\n";

    food_queue.send(std::move(food));
    active_food_count++;  // Increment active food count
  }
}
