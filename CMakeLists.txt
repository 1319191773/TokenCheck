cmake_minimum_required(VERSION 3.16)

set(ANDROID_PLATFORM android-34 CACHE STRING "Android target API level")

project(TokenCheck
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Qml Quick QuickControls2)

qt_add_executable(TokenCheck
    src/main.cpp
    src/usagequery.cpp
    src/appsettings.cpp
    src/widgetbridge.cpp
    src/androidprefs.cpp
)

qt_add_resources(TokenCheck "qml_files"
    PREFIX "/"
    FILES
        src/qml/MainPage.qml
        src/qml/DetailPage.qml
        src/qml/SettingsPage.qml
        src/qml/PlatformEditPage.qml
)

target_include_directories(TokenCheck PRIVATE src)

target_link_libraries(TokenCheck PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
)

set_target_properties(TokenCheck PROPERTIES
    QT_QML_IMPORT_PATH "${CMAKE_SOURCE_DIR}/src/qml"
    QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/android"
)

if(ANDROID_ABI)
    set(ANDROID_SSL_DIR "${ANDROID_SDK_ROOT}/android_openssl/no-asm/ssl_3/${ANDROID_ABI}")
    if(EXISTS "${ANDROID_SSL_DIR}/libssl_3.so")
        set(ANDROID_SSL_STAGING "${CMAKE_CURRENT_BINARY_DIR}/ssl_libs")
        file(MAKE_DIRECTORY "${ANDROID_SSL_STAGING}")
        configure_file("${ANDROID_SSL_DIR}/libssl_3.so" "${ANDROID_SSL_STAGING}/libssl.so" COPYONLY)
        configure_file("${ANDROID_SSL_DIR}/libcrypto_3.so" "${ANDROID_SSL_STAGING}/libcrypto.so" COPYONLY)
        set_property(TARGET TokenCheck APPEND PROPERTY
            QT_ANDROID_EXTRA_LIBS
            "${ANDROID_SSL_STAGING}/libssl.so"
            "${ANDROID_SSL_STAGING}/libcrypto.so"
            "${ANDROID_SSL_DIR}/libssl_3.so"
            "${ANDROID_SSL_DIR}/libcrypto_3.so"
        )
        message(STATUS "Added OpenSSL 3 libs: ${ANDROID_SSL_DIR}")
    endif()
endif()
