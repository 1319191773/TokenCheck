cmake_minimum_required(VERSION 3.16)

if(ANDROID)
    set(ANDROID_PLATFORM android-34 CACHE STRING "Android target API level")
endif()

project(TokenCheckAndroid
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Qml Quick QuickControls2)

# ---- Shared sources ----
set(COMMON_SOURCES
    src/main.cpp
    src/usagequery.cpp
    src/appsettings.cpp
    src/platform_registry.cpp
    src/securestorage.cpp
    src/platforms/glm/glm_handler.cpp
    src/platforms/deepseek/deepseek_handler.cpp
)

# ---- Platform-specific sources ----
set(PLATFORM_SOURCES "")
if(ANDROID)
    list(APPEND PLATFORM_SOURCES
        src/widgetbridge.cpp
        src/androidprefs.cpp
    )
endif()

if(IOS)
    enable_language(OBJCXX)
    list(APPEND PLATFORM_SOURCES
        src/iosappgroups.mm
        src/securestorage_ios.mm
    )
endif()

qt_add_executable(TokenCheckAndroid
    ${COMMON_SOURCES}
    ${PLATFORM_SOURCES}
)

qt_add_resources(TokenCheckAndroid "qml_files"
    PREFIX "/"
    FILES
        src/qml/qmldir
        src/qml/Theme.qml
        src/qml/MainPage.qml
        src/qml/MainContentPage.qml
        src/qml/DetailPage.qml
        src/qml/SettingsPage.qml
        src/qml/PlatformEditPage.qml
        src/platforms/glm/qml/qmldir
        src/platforms/glm/qml/GlmCard.qml
        src/platforms/glm/qml/GlmDetail.qml
        src/platforms/deepseek/qml/qmldir
        src/platforms/deepseek/qml/DeepSeekCard.qml
        src/platforms/deepseek/qml/DeepSeekDetail.qml
)

target_include_directories(TokenCheckAndroid PRIVATE src)

target_link_libraries(TokenCheckAndroid PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
)

set_target_properties(TokenCheckAndroid PROPERTIES
    QT_QML_IMPORT_PATH "${CMAKE_SOURCE_DIR}/src/qml;${CMAKE_SOURCE_DIR}/src/platforms/glm/qml;${CMAKE_SOURCE_DIR}/src/platforms/deepseek/qml"
)

# ---- Android-specific ----
if(ANDROID)
    set_target_properties(TokenCheckAndroid PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/android"
    )
    if(ANDROID_ABI)
        set(ANDROID_SSL_DIR "${ANDROID_SDK_ROOT}/android_openssl/no-asm/ssl_3/${ANDROID_ABI}")
        if(EXISTS "${ANDROID_SSL_DIR}/libssl_3.so")
            set(ANDROID_SSL_STAGING "${CMAKE_CURRENT_BINARY_DIR}/ssl_libs")
            file(MAKE_DIRECTORY "${ANDROID_SSL_STAGING}")
            configure_file("${ANDROID_SSL_DIR}/libssl_3.so" "${ANDROID_SSL_STAGING}/libssl.so" COPYONLY)
            configure_file("${ANDROID_SSL_DIR}/libcrypto_3.so" "${ANDROID_SSL_STAGING}/libcrypto.so" COPYONLY)
            set_property(TARGET TokenCheckAndroid APPEND PROPERTY
                QT_ANDROID_EXTRA_LIBS
                "${ANDROID_SSL_STAGING}/libssl.so"
                "${ANDROID_SSL_STAGING}/libcrypto.so"
                "${ANDROID_SSL_DIR}/libssl_3.so"
                "${ANDROID_SSL_DIR}/libcrypto_3.so"
            )
            message(STATUS "Added OpenSSL 3 libs: ${ANDROID_SSL_DIR}")
        endif()
    endif()
endif()

# ---- iOS-specific ----
if(IOS)
    set_target_properties(TokenCheckAndroid PROPERTIES
        MACOSX_BUNDLE ON
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/ios/Info.plist"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.tokencheck.ios"
        MACOSX_BUNDLE_BUNDLE_NAME "TokenCheck"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
        OUTPUT_NAME "TokenCheck"
    )
    target_compile_definitions(TokenCheckAndroid PRIVATE
        Q_OS_IOS
    )
    target_link_libraries(TokenCheckAndroid PRIVATE
        "-framework Foundation"
        "-framework UIKit"
        "-framework Security"
    )
endif()
