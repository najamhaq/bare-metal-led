@echo off
if exist build (
  rmdir /s /q build
  echo build directory removed
) else (
  echo build directory does not exist
)

