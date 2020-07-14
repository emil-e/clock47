file(READ "index.html" WEBPAGE)
configure_file("web.cpp.in" "web.cpp")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "index.html")
