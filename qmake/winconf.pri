# No need to apply again, used TinyORM/qmake/winconf.pri through TinyOrm.pri

# Compiler and Linker options
# ---

win32-g++ {
    QMAKE_CXXFLAGS_DEBUG += -Wa,-mbig-obj
    # Avoid string table overflow
    QMAKE_CXXFLAGS_DEBUG += -O1
}
