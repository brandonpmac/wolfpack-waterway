
cmake_minimum_required(VERSION 3.1...3.28)

# Include GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# GTest: For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(
	ww_test
	
	# Test files
	sanity_test.cpp

	# Mock files
	mock/mock_si_switch.cpp
	mock/mock_si_relay.cpp
	mock/mock_si_lcd.cpp
	mock/mock_si_encoder.cpp

	# Application files
	../src/test_file.cpp
	../src/control.cpp
	../src/menu.cpp

)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_compile_definitions(TEST_BUILD)

target_link_libraries(ww_test gtest_main)

enable_testing()
include(GoogleTest)
gtest_discover_tests(ww_test)