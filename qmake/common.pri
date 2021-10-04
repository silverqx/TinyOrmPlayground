# My variables
# ---

TINY_QT_VERSION_UNDERSCORED = $$replace(QT_VERSION, \., _)
CONFIG(release, debug|release) {
    debug_and_release: TINY_RELEASE_TYPE = $$quote(/release)
    TINY_RELEASE_TYPE_CAMEL = Release
}
else:CONFIG(debug, debug|release) {
    debug_and_release: TINY_RELEASE_TYPE = $$quote(/debug)
    TINY_RELEASE_TYPE_CAMEL = Debug
}
