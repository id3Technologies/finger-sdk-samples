cmake_minimum_required(VERSION 2.8.12)

project(id3FingerSdkCppSamples)

macro(postBuildCopyDirectory target dst_dir)
    set(dirs_list ${ARGN})
    foreach (src_dir ${dirs_list})
        add_custom_command(
                TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${src_dir}
                ${dst_dir})
    endforeach ()
endmacro()

set(ID3FINGER_SDK_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../../sdk/)

include_directories(${ID3FINGER_SDK_ROOT_DIR}include)

if (LINUX_BUILD)
    set(LINK_DIR ${ID3FINGER_SDK_ROOT_DIR}bin/linux/x64)
elseif (WINDOWS_BUILD)
    set(LINK_DIR ${ID3FINGER_SDK_ROOT_DIR}bin/windows/x64)
endif ()

link_directories(${LINK_DIR})

add_executable(id3FingerRecognitionSampleNIST id3FingerRecognitionSampleNIST.cpp)
target_link_libraries(id3FingerRecognitionSampleNIST id3Finger)

postBuildCopyDirectory(id3FingerRecognitionSampleNIST $<TARGET_FILE_DIR:id3FingerRecognitionSampleNIST> ${LINK_DIR})
#postBuildCopyDirectory(id3FingerRecognitionSampleNIST $<TARGET_FILE_DIR:id3FingerRecognitionSampleNIST>/data ${CMAKE_CURRENT_LIST_DIR}/../data)
#postBuildCopyDirectory(id3FingerRecognitionSampleNIST $<TARGET_FILE_DIR:id3FingerRecognitionSampleNIST>/models ${CMAKE_CURRENT_LIST_DIR}/../sdk/models)