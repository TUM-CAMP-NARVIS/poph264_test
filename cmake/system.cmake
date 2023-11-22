set(
  SYSTEM_LIBRARIES
  Comctl32
  D2d1
  D3d11
  D3d12
  Dbghelp
  Dinput8
  Dwmapi
  Dwrite
  Dxgi
  Dxguid
  Msi
  Rpcrt4
  Shell32
  Shlwapi
  User32
  WindowsApp
  mfuuid
  ws2_32
  crypt32
  secur32
  bcrypt
  ncrypt
  userenv
  ntdll
  iphlpapi
  runtimeobject
  oleaut32
  advapi32
  kernel32
)

foreach(LIBRARY ${SYSTEM_LIBRARIES})
  add_library("System::${LIBRARY}" INTERFACE IMPORTED GLOBAL)
  set_property(
    TARGET "System::${LIBRARY}"
    PROPERTY IMPORTED_LIBNAME "${LIBRARY}"
  )
endforeach()