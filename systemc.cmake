#!cmake .

# SystemC specific needs
set(CMAKE_PREFIX_PATH $ENV{SYSTEMC_HOME})
find_package(SystemCLanguage CONFIG REQUIRED)
set (CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD} CACHE STRING
     "C++ standard to build all targets. Supported values are 98, 11, and 14.")
set (CMAKE_CXX_STANDARD_REQUIRED ${SystemC_CXX_STANDARD_REQUIRED} CACHE BOOL
     "The with CMAKE_CXX_STANDARD selected C++ standard is a requirement.")
link_libraries(SystemC::systemc)

# Things that affect all targets
set(shared_env LD_LIBRARY_PATH=${CMAKE_PREFIX_PATH}/lib DYLD_LIBRARY_PATH=${CMAKE_PREFIX_PATH}/lib)
add_custom_target(run
   time env ${shared_env} ./$<TARGET_FILE_NAME:engine.x> $(ARGS) 2>&1 | tee engine.log
   COMMAND time env ${shared_env} ./$<TARGET_FILE_NAME:engine+.x> $(ARGS) 2>&1 | tee engine+.log
   COMMENT "Running program"
   USES_TERMINAL
   )
