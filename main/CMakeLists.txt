idf_component_register(
  SRCS
    "ClockWidget.cpp"
    "EffectWidget.cpp"
    "Widget.cpp"
    "buttons.cpp"
    "clock47_main.cpp"
    "display.cpp"
    "draw.cpp"
    "network.cpp"
  INCLUDE_DIRS
    "")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 20)
