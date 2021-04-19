project(ducketleague)
add_executable(
    ${PROJECT_NAME} camera.cpp      main.cpp     openglwindow.cpp 
    ball/ball.cpp   field/field.cpp duck/duck.cpp
)
enable_abcg(${PROJECT_NAME})
