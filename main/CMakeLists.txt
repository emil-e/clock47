idf_component_register(
  SRCS
    "${CMAKE_BINARY_DIR}/web/web.cpp"
    "ClockWidget.cpp"
    "Color.cpp"
    "DateWidget.cpp"
    "GenericTextWidget.cpp"
    "StackWidget.cpp"
    "TextWidget.cpp"
    "Widget.cpp"
    "buttons.cpp"
    "clock47_main.cpp"
    "display.cpp"
    "draw.cpp"
    "effect.cpp"
    "fastmath.cpp"
    "httpserver.cpp"
    "message.cpp"
    "mode.cpp"
    "network.cpp"
    "property.cpp"
    "ui.cpp"
  INCLUDE_DIRS
    "")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 20)
