# CMakeList.txt: tcp_server 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("tcp_server")
# 将源代码添加到此项目的可执行文件。
aux_source_directory(. DIR_SRCS)
add_executable (tcp_server ${DIR_SRCS})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET tcp_server PROPERTY CXX_STANDARD 17
  )
endif()

# TODO: 如有需要，请添加测试并安装目标。





# boost 库 
# 查找 Boost 库的组件
find_package(Boost REQUIRED COMPONENTS system thread json)  # 设置了${Boost_LIBRARIES}这个变量
# 包含 Boost 头文件路径
target_include_directories(tcp_server PRIVATE ${Boost_INCLUDE_DIRS})
# 链接 Boost 库
target_link_libraries(tcp_server PRIVATE ${Boost_LIBRARIES} )



# mysql 库
# 为目标指定头文件路径
target_include_directories(tcp_server PRIVATE /usr/include)
# 硬编码库文件路径并链接
target_link_libraries(tcp_server PRIVATE mysqlcppconn )



#poco 库
# 查找 Poco 库
find_package(Poco REQUIRED Net Foundation NetSSL)

# 链接 Poco 库
target_link_libraries(tcp_server PRIVATE Poco::Net Poco::Foundation Poco::NetSSL)




# 复制配置文件 在重新生成的时候才会复制
add_custom_command(
    TARGET tcp_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/config.ini
            $<TARGET_FILE_DIR:tcp_server>/config.ini
    COMMENT "Updating config.ini if different"
)
