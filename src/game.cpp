#include "game.h"
#include <iostream>
#include "SDL.h"

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)),
      _food_manager(grid_width, grid_height) {
  PlaceFood();
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

    // Render the current game state (snake and food)
    {
      std::lock_guard<std::mutex> lock(
          food_mutex);  // Lock to access food items
      renderer.Render(snake, active_food);
    }

    // Maintain consistent FPS
    frame_end = SDL_GetTicks();
    frame_duration = frame_end - frame_start;
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }

    // After every second, update the window title.
    frame_count++;
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

void Game::PlaceFood() { _food_manager.Start(5); }

void Game::ManageFood() {
  // Lock the mutex to safely manage the food list
  std::lock_guard<std::mutex> lock(food_mutex);

  // Get the current time to check for expired food
  auto now = std::chrono::steady_clock::now();

  // Remove expired food items and keep valid ones
  std::vector<FoodItem> new_food_list;
  for (auto &food : active_food) {
    auto age = std::chrono::duration_cast<std::chrono::seconds>(
        now - food.creation_time);
    if (age.count() < 10) {
      new_food_list.push_back(std::move(food));  // Keep valid food
    } else {
      std::cout << "Food expired and removed\n";
    }
  }
  active_food = std::move(new_food_list);  // Update the active food list

  // Try to receive new food from the queue
  auto food_opt = _food_manager.GetQueue().try_receive();
  if (food_opt) {
    active_food.push_back(std::move(*food_opt));  // Add new food to the list
  }
}

void Game::Update() {
  if (!snake.alive) return;

  snake.Update();

  // Manage food (check for expired items, receive new food)
  ManageFood();

  // Check if the snake eats any food item
  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  std::lock_guard<std::mutex> lock(food_mutex);  // Lock to access the food list
  auto it = std::find_if(active_food.begin(), active_food.end(),
                         [new_x, new_y](const FoodItem &food) {
                           return food.x == new_x && food.y == new_y;
                         });

  if (it != active_food.end()) {
    std::cout << "Snake ate food! Score: " << ++score << "\n";
    snake.GrowBody();
    snake.speed += 0.02;
    active_food.erase(it);  // Remove the eaten food
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }