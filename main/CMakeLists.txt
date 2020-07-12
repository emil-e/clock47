idf_component_register(
  SRCS
    "ClockWidget.cpp"
    "EffectWidget.cpp"
    "StackWidget.cpp"
    "TextWidget.cpp"
    "Widget.cpp"
    "buttons.cpp"
    "clock47_main.cpp"
    "display.cpp"
    "draw.cpp"
    "httpserver.cpp"
    "network.cpp"
    "ui.cpp"
  INCLUDE_DIRS
    "")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 20)
