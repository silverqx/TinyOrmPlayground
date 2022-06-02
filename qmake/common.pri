# My variables
# ---

TINY_QT_VERSION_UNDERSCORED = $$replace(QT_VERSION, \., _)

# TINY_BUILD_SUBFOLDER - folder by release type is set in the TinyOrm.pri

CONFIG(debug, debug|release): \
    TINY_RELEASE_TYPE_CAMEL = Debug

else:CONFIG(release, debug|release): \
    force_debug_info: \
        TINY_RELEASE_TYPE_CAMEL = Profile
    else: \
        TINY_RELEASE_TYPE_CAMEL = Release
