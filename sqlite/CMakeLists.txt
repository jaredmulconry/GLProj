cmake_minimum_required(VERSION 3.1)
project(sqlite)

set(library_name ${PROJECT_NAME})

set(SQLITE_HDRS sqlite3.h sqlite3ext.h)
set(SQLITE_SRCS sqlite3.c shell.c sqlite3_all.c)
add_library(${library_name} ${SQLITE_HDRS} ${SQLITE_SRCS})