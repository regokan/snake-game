#include "controller.h"
#include <iostream>
#include "SDL.h"
#include "snake.h"

void Controller::ChangeDirection(Snake &snake, Snake::Direction input,
                                 Snake::Direction opposite) const {
  if (snake.direction != opposite || snake.size == 1) snake.direction = input;
  return;
}

void Controller::HandleInput(bool &running, Snake &snake) const {
  SDL_Event e;
  const float speed_increment_percent = 0.25f;
  const float max_speed = 0.5f;

  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      running = false;
    } else if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
        case SDLK_UP:
        case SDLK_w:
          if (snake.direction == Snake::Direction::kUp) {
            snake.speed = std::min(snake.speed * (1 + speed_increment_percent), max_speed);
          } else {
            ChangeDirection(snake, Snake::Direction::kUp, Snake::Direction::kDown);
          }
          break;

        case SDLK_DOWN:
        case SDLK_s:
          if (snake.direction == Snake::Direction::kDown) {
            snake.speed = std::min(snake.speed * (1 + speed_increment_percent), max_speed);
          } else {
            ChangeDirection(snake, Snake::Direction::kDown, Snake::Direction::kUp);
          }
          break;

        case SDLK_LEFT:
        case SDLK_a:
          if (snake.direction == Snake::Direction::kLeft) {
            snake.speed = std::min(snake.speed * (1 + speed_increment_percent), max_speed);
          } else {
            ChangeDirection(snake, Snake::Direction::kLeft, Snake::Direction::kRight);
          }
          break;

        case SDLK_RIGHT:
        case SDLK_d:
          if (snake.direction == Snake::Direction::kRight) {
            snake.speed = std::min(snake.speed * (1 + speed_increment_percent), max_speed);
          } else {
            ChangeDirection(snake, Snake::Direction::kRight, Snake::Direction::kLeft);
          }
          break;
      }
    } else if (e.type == SDL_KEYUP) {
      snake.speed = snake.initial_speed;  // Reset speed to initial
    }
  }
}
