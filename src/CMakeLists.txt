set(CMD_NAME dnstester)

set(CMAKE_AUTOMOC ON)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

set(QTPKG Core Widgets Gui Network DBus)

list(TRANSFORM QTPKG PREPEND "Qt5")

foreach(ITEM ${QTPKG})
    find_package(${ITEM})
endforeach()
pkg_check_modules(Dtk REQUIRED IMPORTED_TARGET dtkwidget)
pkg_check_modules(Dtk REQUIRED IMPORTED_TARGET dtkcore)

set(SRC main.cpp mainwindow.cpp mainwindow.h
        networkutils.cpp networkutils.h
        DnsTableModel.cpp DnsTableModel.h)

qt5_wrap_ui(FORMS mainwindow.ui)

add_executable(${CMD_NAME} ${SRC} ${FORMS} resource.qrc)
list(TRANSFORM QTPKG REPLACE "Qt5" "Qt5::")
foreach(ITEM ${QTPKG})
    target_link_libraries(${CMD_NAME} PUBLIC ${ITEM})
        endforeach()

target_include_directories(${CMD_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${CMD_NAME} PUBLIC PkgConfig::Dtk)

set_target_properties(${CMD_NAME} PROPERTIES AUTOMOC TRUE)
set_target_properties(${CMD_NAME} PROPERTIES AUTORCC TRUE)

INSTALL(TARGETS ${CMD_NAME}
        RUNTIME
        DESTINATION bin
        )
