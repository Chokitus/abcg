project(billiard)

add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp balls/balls.cpp
                               stick/stick.cpp board/board.cpp)

enable_abcg(${PROJECT_NAME})
