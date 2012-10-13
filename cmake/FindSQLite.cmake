# Copyright (C)
# 2012 - Paul Weingardt
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
#

IF(SQLITE_INCLUDE_DIR)
    SET(SQLITE_FIND_QUIETLY TRUE)
ENDIF(SQLITE_INCLUDE_DIR)

FIND_PATH(SQLITE_INCLUDE_DIR sqlite3.h
    /usr/include
    /usr/local/include)

SET(SQLITE_NAMES sqlite3)
FIND_LIBRARY(SQLITE_LIBRARY
    NAMES ${SQLITE_NAMES}
    PATHS /usr/lib /usr/local/lib
    PATH_SUFFIXES sqlite3)

IF(SQLITE_INCLUDE_DIR AND SQLITE_LIBRARY)
    SET(SQLITE_FOUND TRUE)
    SET(SQLITE_LIBRARIES ${SQLITE_LIBRARY})
ELSE(SQLITE_INCLUDE_DIR AND SQLITE_LIBRARY)
    SET(SQLITE_FOUND FALSE)
ENDIF(SQLITE_INCLUDE_DIR AND SQLITE_LIBRARY)

IF(SQLITE_FOUND)
    IF(NOT SQLITE_FIND_QUIETLY)
        MESSAGE(STATUS "Found SQLite: ${SQLITE_LIBRARY}")
    ENDIF(NOT SQLITE_FIND_QUIETLY)
ELSE(SQLITE_FOUND)
    IF(SQLITE_FIND_REQUIRED)
        MESSAGE(STATUS "Looked for SQLite libraries named ${SQLITE_NAMES}.")
        MESSAGE(FATAL_ERROR "Could NOT find SQLite3 library!")
    ELSE(SQLITE_FIND_REQUIRED)
        MESSAGE(STATUS "Optional library SQLite3 not found!")
    ENDIF(SQLITE_FIND_REQUIRED)
ENDIF(SQLITE_FOUND)

MARK_AS_ADVANCED(SQLITE_LIBRARY SQLITE_INCLUDE_DIR SQLITE_FOUND)
