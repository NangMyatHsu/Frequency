cmake_minimum_required(VERSION 3.5)

project(lecture1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Good practice to disable GNU extensions

# --- Improved Qt finding logic ---
# Try to find Qt6 first, then fallback to Qt5 if Qt6 is not found.
# This avoids redundant find_package calls and makes the logic clearer.
find_package(Qt6 COMPONENTS Widgets SerialPort PrintSupport OPTIONAL_COMPONENTS OpenGL) # Add OpenGL if qcustomplot uses it
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets SerialPort PrintSupport OpenGL REQUIRED) # Add OpenGL if qcustomplot uses it
    set(QT_VERSION_MAJOR 5) # Explicitly set for consistency
else()
    set(QT_VERSION_MAJOR 6) # Explicitly set for consistency
endif()

# Check if Qt was found at all
if(NOT Qt${QT_VERSION_MAJOR}_FOUND)
    message(FATAL_ERROR "Could not find Qt ${QT_VERSION_MAJOR} or Qt5. Please ensure Qt is installed and discoverable by CMake.")
endif()

# Define the list of source files, including UI and header files
# CMake's AUTO_UI, AUTO_MOC, AUTO_RCC will handle these correctly.
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    qcustomplot.h
    qcustomplot.cpp
    mainwindow.ui
    frame.qrc # CMake's AUTOUIC will process this
)

# --- Consolidated executable/library creation ---
# Use the appropriate Qt macro or standard CMake command.
# For Android, Qt provides `qt_add_library` which handles Android specifics.

if(ANDROID)
    # For Android, Qt recommends qt_add_library with target_link_libraries for Qt modules
    # if you're building a shared library (like an Android app's native part).
    # If this is the main application, 'add_executable' with android_bundle might be better.
    # Given the project name 'lecture1' and typical app structure, 'add_executable' with bundling is more likely.
    # If 'lecture1' is meant to be a library linked by another app, then 'qt_add_library'.
    # Assuming it's an app for now based on 'main.cpp'.
    qt_add_executable(lecture1
        ${PROJECT_SOURCES}
        # For Android, you might need an AndroidManifest.xml and other assets.
        # Add 'ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android' if you have a custom Android project structure.
    )
    # For Android deployment, you'll likely need qt_android_add_deploy_configuration
    # and qt_android_add_dependencies if you have specific Android dependencies.
    # This is beyond just linking Qt modules but important for a complete Android app.
else()
    # For desktop platforms, use qt_add_executable for Qt6, and add_executable for Qt5.
    # The `MANUAL_FINALIZATION` and `qt_finalize_executable` pattern is specific to Qt6's `qt_add_executable`.
    if(${QT_VERSION_MAJOR} EQUAL 6)
        qt_add_executable(lecture1
            MANUAL_FINALIZATION # Required for qt_finalize_executable later
            ${PROJECT_SOURCES}
        )
    else() # Qt5
        add_executable(lecture1
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# --- Link Qt modules ---
# Use the found Qt version's components for linking.
# Ensure PrintSupport is linked as well, as it was in your find_package call.
target_link_libraries(lecture1 PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::SerialPort
    Qt${QT_VERSION_MAJOR}::PrintSupport # Add PrintSupport here
    # If qcustomplot requires OpenGL, add it:
    # Qt${QT_VERSION_MAJOR}::OpenGL # This component name can vary (e.g., Qt5::OpenGL, Qt6::Gui for QOpenGLWidget)
)

# Add any additional libraries required by qcustomplot if it's not purely Qt
# For example, if it uses system OpenGL, you might need to find_package(OpenGL) and link it.
# e.g., target_link_libraries(lecture1 PRIVATE OpenGL::GL)

# --- Set target properties ---
# These properties are mostly for desktop bundle creation.
set_target_properties(lecture1 PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# --- Finalize Qt6 executable ---
# This step is only for Qt6's qt_add_executable when MANUAL_FINALIZATION is used.
if(${QT_VERSION_MAJOR} EQUAL 6)
    qt_finalize_executable(lecture1)
endif()

# --- Optional: Deploy Qt runtime (for desktop) ---
# This helps in creating self-contained application bundles.
# Consider adding this for deployment if you're not using system-wide Qt.
# if(NOT ANDROID) # Or if NOT WIN32 AND NOT APPLE (as bundle properties handle these)
#    # For Qt6:
#    # qt_generate_deploy_app_dependencies(lecture1)
#    # For Qt5:
#    # install(TARGETS lecture1 DESTINATION .) # Basic install target
#    # install(CODE "
#    #    file(GLOB QT_PLUGINS \"${QT_PLUGINS_DIR}/*\")
#    #    install(FILES \${QT_PLUGINS} DESTINATION \${CMAKE_INSTALL_PREFIX}/plugins)
#    #    " COMPONENT Runtime)
# endif()
