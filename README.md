# Snake Game

## Overview

This **Snake Game** is a modern C++ implementation that expands on the traditional snake game by introducing **multiple food items that dynamically appear and disappear** on the grid. The project showcases advanced C++ concepts such as **multi-threading, message queues**, and **mutex-protected shared resources**. Additionally, the game leverages the **SDL2 library** for graphics rendering and event handling.

---

## Key Features

- **Dynamic Food System**:
  - Multiple food items are generated asynchronously using **threads**.
  - Food items disappear after 10 seconds if not consumed, introducing a sense of urgency.
  - We use **`std::mutex`** to ensure thread-safe access to the food list and prevent race conditions.
- **Asynchronous Communication**:

  - The `FoodManager` class utilizes a **message queue** (`MessageQueue<FoodItem>`) to send and receive food items between the main game loop and the food generator threads.

- **Consistent Frame Rate**:

  - The game maintains a **60 FPS frame rate** by carefully managing input, updates, and rendering times.
  - **SDL2** handles graphics rendering and user input with minimal latency.

- **Enhanced Gameplay Mechanics**:
  - The snake grows longer and speeds up every time it consumes food.
  - Food generation is capped to prevent flooding the grid, with **no more than 2 active food items** at a time.

---

## How This Game Is Different

1. **Dynamic Food Handling**:

   - In a traditional snake game, only one food item exists at a time. Here, **multiple food items appear and disappear periodically**.
   - **Food Expiration**: If a food item is not consumed within **10 seconds**, it disappears. This creates time-sensitive challenges for the player.
   - **Thread-Safe Generation**: We use **multi-threading** to generate food items asynchronously, managed by `FoodManager`. The food generator uses **randomized intervals (5-10 seconds)** to introduce unpredictability.

2. **Efficient Synchronization with Mutex**:

   - To prevent race conditions when accessing the shared food list, **mutex locks** ensure thread-safe updates between the `Game` and `Renderer` classes.
   - The `ManageFood()` function in the `Game` class **removes expired food items** safely, ensuring no invalid food is rendered.

3. **Threading with Message Queues**:

   - Food items are sent from **food generator threads to the main game loop** using a **custom `MessageQueue`**.
   - The queue ensures smooth communication between threads without blocking the game loop.

4. **Smooth Graphics and Input Handling with SDL2**:
   - The game is built with **SDL2** to handle window creation, rendering, and user input efficiently.
   - Supports **WASD** and **arrow keys** for movement, allowing flexible control.

---

## Installation

### Dependencies

- **SDL2**: Required for rendering and input handling.
- **CMake**: Build system generator.
- **g++ or clang++**: C++ compiler.

---

### Installation Instructions Using `make`

1. **Clone the repository**:

   ```bash
   git clone https://github.com/regokan/snake-game.git
   cd snake-game
   ```

2. **Install dependencies** using the `setup` target:

   - For Linux (Debian-based):
     ```bash
     make setup-debian
     ```
   - For macOS:
     ```bash
     make setup-mac
     ```

3. **Build the project**:

   ```bash
   make build
   cd build
   ```

4. **Run the Game**:

   ```bash
   ./SnakeGame
   ```

5. **Format the code** (optional):

   ```bash
   make format
   ```

6. **Build with debugging symbols** (optional):

   ```bash
   make debug
   ```

7. **Clean up build artifacts**:
   ```bash
   make clean
   ```

---

## High-Level Structure

The project consists of the following core classes:

- **`Game`**:

  - Manages the game state, including snake updates and food management.
  - Uses **`ManageFood()`** to track food expiration and prevent stale items from rendering.
  - Runs the **main game loop**, handling input, updates, and rendering at a consistent frame rate.

- **`FoodManager`**:

  - Generates food asynchronously using **multiple threads**.
  - Uses **mutex locks** to synchronize access to the shared food list.
  - Caps the **maximum number of active food items to 2**, ensuring the game grid isn't overwhelmed.

- **`MessageQueue`**:

  - A custom **thread-safe message queue** used to communicate between food generator threads and the main game loop.
  - Provides **blocking and non-blocking receive methods** to prevent deadlocks.

- **`Renderer`**:

  - Handles **SDL2 rendering** of the snake, food items, and the game window.
  - Uses **the vector of active food items** passed from the `Game` class to render food dynamically.
  - Updates the **window title with the current score and FPS** every second.

- **`Snake`**:
  - Manages the snake’s movement and body updates.
  - Implements **body growth and speed increases** when food is consumed.
  - Handles **collision detection** to determine if the snake dies.

---

## Project Structure

- **`src/`**: Source files implementing game logic.
- **`include/`**: Header files for each class.
- **`CMakeLists.txt`**: CMake configuration file for building the project.
- **`Makefile`**: Makefile to manage setup, build, and formatting tasks.

---

## Example: Food Handling in Detail

### **Food Generation Logic**

1. **FoodManager** runs a thread for each food item with a **5-10 second delay** between spawns.
2. Each food item is sent to the `MessageQueue` to ensure thread-safe communication.
3. The **maximum number of active food items** at any point is **2**. If more than 2 food items are generated, the system waits for existing items to expire or be consumed.

### **Food Expiration**

- Each food item has a **10-second lifespan**. If not consumed, it is removed from the active list.
- The `ManageFood()` function in `Game` checks the **creation time of each food item** and removes expired items during each update cycle.

---

## Example Code Snippets

### **FoodManager::CreateAndRemoveFood**

```cpp
void FoodManager::CreateAndRemoveFood(int id) {
  while (true) {
    std::lock_guard<std::mutex> lock(food_mutex);
    FoodItem food{random_w(engine), random_h(engine), id};
    std::cout << "Creating food item " << id << " at (" << food.x << ", " << food.y << ")\n";
    food_queue.send(std::move(food));

    std::this_thread::sleep_for(std::chrono::seconds(5 + rand() % 6));
  }
}
```

### **Game::ManageFood**

```cpp
void Game::ManageFood() {
  std::lock_guard<std::mutex> lock(food_mutex);

  auto now = std::chrono::steady_clock::now();
  std::vector<FoodItem> new_food_list;

  for (auto &food : active_food) {
    auto age = std::chrono::duration_cast<std::chrono::seconds>(now - food.creation_time);
    if (age.count() < 10) {
      new_food_list.push_back(std::move(food));
    } else {
      std::cout << "Food expired and removed\n";
    }
  }
  active_food = std::move(new_food_list);

  auto food_opt = _food_manager.GetQueue().try_receive();
  if (food_opt) {
    active_food.push_back(std::move(*food_opt));
  }
}
```

---

## Conclusion

This **Snake Game** extends the traditional gameplay with **dynamic food items** and **multi-threaded food generation**. With **thread-safe message passing**, **food expiration management**, and **real-time rendering with SDL2**, it offers a more challenging and engaging experience for players.
