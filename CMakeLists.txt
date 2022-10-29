add_library(serial src/SerialPort.cpp)

install(TARGETS serial DESTINATION lib)
install(FILES src/SerialPort.h DESTINATION include)