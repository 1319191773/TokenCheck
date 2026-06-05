cmake_minimum_required(VERSION 3.14)

project(TokenCheckPC
    VERSION 2.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Network)

set(TS_FILES
    ${CMAKE_SOURCE_DIR}/translations/TokenCheckPC_zh_CN.ts
)

find_package(Qt5 OPTIONAL_COMPONENTS LinguistTools)
if(Qt5LinguistTools_FOUND)
    qt5_add_translation(QM_FILES ${TS_FILES})
else()
    set(QM_FILES "")
    foreach(_ts ${TS_FILES})
        get_filename_component(_ts_name ${_ts} NAME_WE)
        set(_qm ${CMAKE_CURRENT_BINARY_DIR}/${_ts_name}.qm)
        find_program(LRELEASE_EXECUTABLE lrelease HINTS "${Qt5_DIR}/../../../bin")
        if(LRELEASE_EXECUTABLE)
            add_custom_command(OUTPUT ${_qm}
                COMMAND ${LRELEASE_EXECUTABLE} ${_ts} -qm ${_qm}
                DEPENDS ${_ts}
            )
            list(APPEND QM_FILES ${_qm})
        endif()
    endforeach()
endif()

set(APP_ICON_RESOURCE "${CMAKE_CURRENT_BINARY_DIR}/app.rc")
configure_file(${CMAKE_SOURCE_DIR}/res/app.rc.in ${APP_ICON_RESOURCE} @ONLY)

add_executable(TokenCheckPC
    src/main.cpp
    src/core/appsettings.cpp
    src/core/datamanager.cpp
    src/core/platform_registry.cpp
    src/network/usagequery.cpp
    src/platforms/glm/glm_handler.cpp
    src/platforms/deepseek/deepseek_handler.cpp
    src/ui/floatingball.cpp
    src/ui/mainwindow.cpp
    src/ui/trayiconmanager.cpp
    src/ui/settingsdialog.cpp
    src/ui/globalhotkey.cpp
    src/ui/theme.cpp
    src/ui/component/clickablecard.cpp
    src/ui/component/progressbar.cpp
    src/ui/component/hotkeybutton.cpp
    src/ui/component/colorbutton.cpp
    src/ui/component/mydoubleslider.cpp
    src/ui/component/noscrollcombobox.cpp
    src/ui/component/noscrollspinbox.cpp
    ${QM_FILES}
    ${APP_ICON_RESOURCE}
    res/resources.qrc
)

target_include_directories(TokenCheckPC PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/network
    ${CMAKE_SOURCE_DIR}/src/platforms/glm
    ${CMAKE_SOURCE_DIR}/src/platforms/deepseek
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/ui/component
)

target_link_libraries(TokenCheckPC PRIVATE
    Qt5::Widgets
    Qt5::Network
)

if(QM_FILES)
    add_custom_command(TARGET TokenCheckPC POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${QM_FILES} $<TARGET_FILE_DIR:TokenCheckPC>
    )
endif()

set(OPENSSL_DIR "${CMAKE_SOURCE_DIR}/res")
set(OPENSSL_DLLS
    "${OPENSSL_DIR}/libssl-1_1-x64.dll"
    "${OPENSSL_DIR}/libcrypto-1_1-x64.dll"
)
add_custom_command(TARGET TokenCheckPC POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${OPENSSL_DIR}/libssl-1_1-x64.dll"
        "$<TARGET_FILE_DIR:TokenCheckPC>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${OPENSSL_DIR}/libcrypto-1_1-x64.dll"
        "$<TARGET_FILE_DIR:TokenCheckPC>"
)

install(TARGETS TokenCheckPC
    RUNTIME DESTINATION bin
)
install(FILES ${OPENSSL_DLLS}
    DESTINATION bin
)

if(WIN32)
    set_target_properties(TokenCheckPC PROPERTIES
        WIN32_EXECUTABLE ON
    )
endif()
