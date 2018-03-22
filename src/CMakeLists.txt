cmake_minimum_required(VERSION 2.8)

project(xiaomi_desk_lamp LANGUAGES C)

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR xtensia-lx106)
SET(CMAKE_CROSSCOMPILING 1)

set(CMAKE_C_STANDARD 99)

SET(CMAKE_C_COMPILER xtensa-lx106-elf-gcc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I. -DICACHE_FLASH -mlongcalls -Wall -Wextra")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Teagle.app.v6.ld")

set(ESP8266_LIBS -nostdlib -Wl,--start-group main net80211 wpa lwip pp phy c pwm -Wl,--end-group gcc)

add_executable(${PROJECT_NAME} "main.c" "leds.c" "encoder.c")
target_link_libraries(${PROJECT_NAME} ${ESP8266_LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE ./)

add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND esptool.py elf2image ${PROJECT_NAME}
)

add_custom_target(flash
  DEPENDS ${PROJECT_NAME}
  COMMAND esptool.py -b 1500000 write_flash 0 ${PROJECT_NAME}-0x00000.bin 0x10000 ${PROJECT_NAME}-0x10000.bin
)
