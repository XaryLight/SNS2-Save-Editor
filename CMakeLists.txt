cmake_minimum_required(VERSION 3.5...4.2)
set(PROJECT_NAME "SNS2-Editor")
project(${PROJECT_NAME})# LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# LLD 链接器
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld")


find_package(
        Qt6
        REQUIRED
        COMPONENTS
        Gui
        Core
        Widgets
)

add_subdirectory(api)

add_executable(${PROJECT_NAME} main.cpp resources/resources.qrc)

target_link_libraries(${PROJECT_NAME}
        api
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

set(TS_FILES
        data/i18n/en_ts.ts
        data/i18n/zh_ts.ts
)

# --- 翻译文件处理部分 (更兼容的写法) ---

set(TS_FILES
        data/i18n/en_ts.ts
        data/i18n/zh_ts.ts
)

find_program(LRELEASE_EXE lrelease HINTS "${Qt6_DIR}/bin")

if(LRELEASE_EXE)
    foreach(TS_FILE ${TS_FILES})
        # 1. 获取文件的绝对路径，避免相对路径导致的查找问题
        get_filename_component(ABS_TS_PATH "${TS_FILE}" REALPATH)

        # 2. 提取文件名 (例如: en_ts.ts)
        get_filename_component(BASE_NAME "${ABS_TS_PATH}" NAME)

        # 3. 手动去掉 .ts 后缀，得到 en_ts
        string(REPLACE ".ts" "" TS_NAME_NO_EXT "${BASE_NAME}")

        # 4. 获取该文件所在的目录 (例如: data/i18n)
        get_filename_component(TS_DIR "${ABS_TS_PATH}" DIRECTORY)

        # 5. 构建目标 .qm 文件路径 (例如: data/i18n/en_ts.qm)
        set(QM_FILE "${TS_DIR}/${TS_NAME_NO_EXT}.qm")

        # 6. 添加自定义命令
        add_custom_command(
                OUTPUT "${QM_FILE}"
                # 注意：这里强制使用引号，防止路径中有空格的错误
                # 输入必须是 .ts 文件，输出目标是 .qm
                COMMAND ${LRELEASE_EXE} "${ABS_TS_PATH}"
                DEPENDS "${ABS_TS_PATH}"
                COMMENT "Compiling: ${ABS_TS_PATH} -> ${QM_FILE}"
        )

        # 7. 将生成的 .qm 关联到项目
        target_sources(${PROJECT_NAME} PRIVATE "${QM_FILE}")
    endforeach()
else()
    message(WARNING "lrelease tool not found!")
endif()

# WIN32
if(WIN32)
    if(MSVC)
        target_link_options(${PROJECT_NAME} PRIVATE /SUBSYSTEM:WINDOWS)
    else()
        target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
    endif()
endif()

# 部署 Qt DLL
find_program(WINDEPLOYQT windeployqt HINTS "${CMAKE_PREFIX_PATH}/bin")
if(WINDEPLOYQT)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${WINDEPLOYQT} --no-translations "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.exe"
            COMMENT "Deploying Qt runtime dependencies..."
    )
else()
    message(WARNING "windeployqt not found! Qt dependencies will be missing.")
endif()
