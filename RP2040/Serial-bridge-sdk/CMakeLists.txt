# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.13)

# Include the SDK import script early
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico_uart_bridge)

pico_sdk_init()

add_executable(uart_bridge uart-bridge.c usb-descriptors.c)

target_include_directories(uart_bridge PUBLIC
	./
	pico-sdk/lib/tinyusb/src)

target_link_libraries(uart_bridge
	hardware_flash
	pico_multicore
	pico_stdlib
	tinyusb_device)

pico_add_extra_outputs(uart_bridge)
