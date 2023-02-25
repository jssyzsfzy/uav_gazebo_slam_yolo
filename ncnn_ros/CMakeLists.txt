cmake_minimum_required(VERSION 3.0.2)
project(ncnn_ros)

## Compile as C++11, supported in ROS Kinetic and newer
set(CMAKE_CXX_STANDARD 17)
# set(OpenCV_DIR /usr/local/share/OpenCV)
set(OpenCV_DIR /usr/local/lib/cmake/opencv4)

# 在这里设置
# /home/fuhua/soft/ncnn/build/install/lib/cmake/ncnn
# 还有一点需要复制文件到这里覆盖
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default")
set(ncnn_DIR /home/fuhua/soft/ncnn/build/install/lib/cmake/ncnn)
# set(realsense2_DIR /opt/ros/noetic/lib/x86_64-linux-gnu/cmake/realsense2)
find_package(ncnn REQUIRED)
# find_package(WiringPi REQUIRED)
find_package(catkin REQUIRED COMPONENTS
  OpenCV
  roscpp
  rospy
  std_msgs
  ncnn
  message_runtime
  message_generation
  cv_bridge
  image_transport
  sensor_msgs
  move_base_msgs
  tf2
  actionlib
  actionlib_msgs
  # realsense2
)
# include_directories(${WIRINGPI_INCLUDE_DIRS})


find_package(OpenCV REQUIRED)
include_directories(include ${OpenCV_INCLUDE_DIRS})
add_action_files(
  FILES
  pose_add.action
  goal.action
)

add_message_files(
  FILES
  box.msg
  control.msg
)
add_service_files(
  FILES
  goal.srv
)
generate_messages(
  DEPENDENCIES
  actionlib_msgs
  std_msgs
)

catkin_package(
  INCLUDE_DIRS include
  CATKIN_DEPENDS actionlib actionlib_msgs roscpp rospy std_msgs message_runtime
)
# link_directories(
#   lib
#   ${catkin_LIB_DIRS}
# )

include_directories(
  include
  ${wiringpi_include}
  ${catkin_INCLUDE_DIRS}
)

add_library(ncnn_ros SHARED
            src/yolo-fastestv2.cpp)

target_link_libraries(ncnn_ros
  ncnn
  ${OpenCV_LIBRARIES}
  ${catkin_LIBRARIES}
)

catkin_install_python(PROGRAMS scripts/get_control.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

add_executable(demo src/demo.cpp)
target_link_libraries(demo
  ncnn_ros
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
  
)

add_executable(sub_box src/sub_box.cpp)
add_dependencies(sub_box ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(sub_box
  ncnn_ros
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)

add_executable(rotors_control src/rotors_control.cpp)
target_link_libraries(rotors_control
  ncnn_ros
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)
add_executable(img_link src/img_link.cpp)
target_link_libraries(img_link
  ncnn_ros
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)
add_executable(get_one_img src/get_one_img.cpp)
target_link_libraries(get_one_img
  ncnn_ros
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)

add_executable(get_img_world src/get_img_world.cpp)
target_link_libraries(get_img_world
  ncnn_ros
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)

add_executable(pub_goal src/pub_goal.cpp)
add_dependencies(pub_goal ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(pub_goal
  ncnn_ros
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)

add_executable(path2pose src/path2pose.cpp)
target_link_libraries(path2pose
  ncnn_ros
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)
add_executable(travel src/travel.cpp)
target_link_libraries(travel
  ncnn_ros
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)

add_executable(cmd_pose src/cmd_pose.cpp)
add_dependencies(cmd_pose ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(cmd_pose
  ncnn_ros
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)

add_executable(test_demo src/test_demo.cpp)
add_dependencies(test_demo ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(test_demo
  ncnn_ros
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)

add_executable(get_fishcamera src/get_fishcamera.cpp)
target_link_libraries(get_fishcamera
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)

