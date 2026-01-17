cmake_minimum_required(VERSION 4.1.1)

#https://github.com/Kitware/CMake/blob/v4.1.1/Help/dev/experimental.rst

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD ON)

project(course_select LANGUAGES CXX)

# set(MODULE_LIB_DIR1 /opt/utils/modules)  # 模块库的目录的设置
add_executable(course_select
    #math.cpp                              # 模块实现单元
    main.cpp

)

set(PRESENTATION
   #表现层
    registrar.ui.cppm
)
set(APPLICATION
    #应用层
    registrar.cppm
    base.cppm
)
set(DAMAIN
    #领域层
    registrar.student.cppm
    registrar.teacher.cppm
    registrar.sectary.cppm
    registrar.course.cppm
)
set(DATA
    #数据管理层
    registrar.database.cppm
)
source_group("presentation" FILES ${PRESENTATION})
source_group("application" FILES ${APPLICATION})
source_group("domain" FILES ${DAMAIN})
source_group("data" FILES ${DATA})
target_sources(course_select  PRIVATE FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS                              # 配置BASE_DIRS，用于组织构建输出目录，以便构建不同目录中(同名的)模块文件.
        #${MODULE_LIB_DIR1}
        ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${PRESENTATION} ${APPLICATION} ${DAMAIN} ${DATA}
)
target_compile_features(course_select PRIVATE cxx_std_23)













































# target_link_directories(course_select PRIVATE /opt/utils/lib)  # -L /opt/utils/lib 设置链接路径
# target_link_libraries(course_select PRIVATE utils)             # -lutils           设置链接库名

# set_target_properties(course_select PROPERTIES
#     INSTALL_RPATH_USE_LINK_PATH  True                           # 使用链接路径作为安装文件的RPath，等价于"-Wl,-rpath=/opt/utils/lib"
# )

#add_library(course_select SHARED
#  utils.cpp
#)

#set_target_properties( course_select PROPERTIES
#    PUBLIC_HEADER utils.cppm
#    VERSION   2.5.1
#    SOVERSION 2
#    LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/lib"
#)

#install(TARGETS course_select
#    LIBRARY DESTINATION /opt/utils/lib
#    PUBLIC_HEADER DESTINATION  /opt/utils/modules
#)

#add_custom_command(TARGET course_select
#    POST_BUILD
#    COMMAND cmake --install "${CMAKE_CURRENT_BINARY_DIR}"
#)
