project(billiard)

add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp balls/balls.cpp board/board.cpp
                               stick/stick.cpp holes/holes.cpp
                               )

enable_abcg(${PROJECT_NAME})
