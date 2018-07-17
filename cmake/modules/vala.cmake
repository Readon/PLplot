# cmake/modules/vala.cmake
#
# Vala binding configuration
#
# Copyright (C) 2006  Yindong Xiao
#
# This file is part of PLplot.
#
# PLplot is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as published
# by the Free Software Foundation; version 2 of the License.
#
# PLplot is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with the file PLplot; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

# Module for determining Vala binding configuration options

message(STATUS "WARNING: no working Vala compiler so disabling Vala binding and examples.")
# Option to enable Vala binding
if(DEFAULT_NO_BINDINGS)
  OPTION(ENABLE_vala "Enable Vala binding" OFF)
else(DEFAULT_NO_BINDINGS)
  OPTION(ENABLE_vala "Enable Vala binding" ON)
endif(DEFAULT_NO_BINDINGS)

if(NOT CMAKE_VALA_COMPILER_WORKS)  
  find_package(GLib "2.38" REQUIRED)
  find_package(Vala "0.30" REQUIRED)
  if(NOT VALA_EXECUTABLE OR NOT GLIB_FOUND)
    message(STATUS "WARNING: no working Vala compiler so disabling Vala binding and examples.")
    set(ENABLE_vala OFF CACHE BOOL "Enable Vala binding" FORCE)
  endif(NOT VALA_EXECUTABLE OR NOT GLIB_FOUND)
endif(NOT CMAKE_VALA_COMPILER_WORKS)
