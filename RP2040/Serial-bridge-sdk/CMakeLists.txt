# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.13)

# Include the SDK import script early
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(catsniffer_uart_bridge)

pico_sdk_init()

add_executable(${PROJECT_NAME} main.c usb-uart-controller.c usb-descriptors.c catsniffer.c)

target_include_directories(${PROJECT_NAME} PUBLIC
	./
	pico-sdk/lib/tinyusb/src)

target_link_libraries(${PROJECT_NAME}
	hardware_flash
	hardware_gpio
	pico_multicore
	pico_stdlib
	tinyusb_device)

pico_add_extra_outputs(catsniffer_uart_bridge)
