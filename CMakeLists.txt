cmake_minimum_required(VERSION 3.11)
project(DNSTester VERSION 1.4)

find_package(PkgConfig REQUIRED)
set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src)

INSTALL(FILES src/resource/icon/dns-tester.png
	DESTINATION share/pixmaps
	)
INSTALL(FILES dns-tester.desktop
		DESTINATION share/applications
		)


set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "DNS测速并更换软件")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jia Qingtong (wanywhn@qq.com)")

include(CPack)