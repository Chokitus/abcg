project(rocketleague)
add_executable(
    ${PROJECT_NAME} camera.cpp      main.cpp     openglwindow.cpp 
    ball/ball.cpp   field/field.cpp car/car.cpp
)
enable_abcg(${PROJECT_NAME})
