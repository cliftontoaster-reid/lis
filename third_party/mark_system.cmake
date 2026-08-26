# third_party/mark_system.cmake
# Marks a third-party target's include directories as SYSTEM so that clang-tidy
# and the compiler treat its headers as non-user code (no diagnostics emitted).

if(NOT COMMAND lis_mark_system)
  function(lis_mark_system)
    foreach(_t ${ARGN})
      if(TARGET ${_t})
        get_target_property(_aliased ${_t} ALIASED_TARGET)
        if(_aliased)
          set(_t ${_aliased})
        endif()
        get_target_property(_inc ${_t} INTERFACE_INCLUDE_DIRECTORIES)
        if(_inc)
          set_target_properties(${_t} PROPERTIES
            INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${_inc}")
        endif()
      endif()
    endforeach()
    unset(_t)
    unset(_inc)
    unset(_aliased)
  endfunction()
endif()
