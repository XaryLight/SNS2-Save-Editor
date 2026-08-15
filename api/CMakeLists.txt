# 设置 Qt 路径
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(
        Qt6
        REQUIRED
        COMPONENTS
        Gui
        Core
        Widgets
)

# 自动收集 api 目录下所有的子目录，并设置为头文件搜索路径
file(GLOB API_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*/*.*"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.*"
)
#file(GLOB API_SUBDIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} LIST_DIRECTORIES true ${CMAKE_CURRENT_SOURCE_DIR}/*)
# 过滤掉文件，只保留目录，并拼接成绝对路径列表
#foreach(subdir ${API_SUBDIRS})
#    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${subdir})
#        list(APPEND API_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir})
#    endif()
#endforeach()

# 将这些源文件编译成一个名为 "api" 的静态库
add_library(api STATIC ${CMAKE_CURRENT_SOURCE_DIR} ${API_SOURCES})

target_include_directories(api
        PUBLIC
        ${API_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}
)
# 链接 Qt6::Widgets 库 (新增这一行)
target_link_libraries(api PUBLIC
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
)