cmake_minimum_required(VERSION 3.15)
project(mdl_module)

set(CMAKE_CXX_STANDARD 20)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

add_executable(
        mdl_module
        test.cpp
#        mdlmodule.cpp

        base/BaseAttr.cpp base/BaseAttr.h
        base/BaseSeq.cpp base/BaseSeq.h
        base/BaseEvt.cpp base/BaseEvt.h
        base/json.hpp
        base/FileLoader.cpp base/FileLoader.h
        base/BaseDS.cpp base/BaseDS.h

        mdl_module/CodeTable.cpp mdl_module/CodeTable.h
        mdl_module/Cover.cpp mdl_module/Cover.h
        mdl_module/Defines.h
        mdl_module/mdl_enter.cpp mdl_module/mdl_enter.h
        mdl_module/Attribute.h
        mdl_module/GetOpt.cpp mdl_module/GetOpt.h
        mdl_module/MathUtil.cpp mdl_module/MathUtil.h
        mdl_module/Event.cpp mdl_module/Event.h
        mdl_module/Node.cpp mdl_module/Node.h
        mdl_module/Params.h
        mdl_module/Pattern.cpp mdl_module/Pattern.h
        mdl_module/Sequence.cpp mdl_module/Sequence.h
        mdl_module/Common.h
        mdl_module/PatternTable.cpp
        mdl_module/Window.cpp mdl_module/Window.h
)

target_link_libraries(mdl_module ${PYTHON_LIBRARIES})