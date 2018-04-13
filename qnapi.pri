unix:!macx {
    OUTDIR = $${PWD}
    INSTALL_PREFIX = /usr
}

macx {
    OUTDIR = $${PWD}/macx
}

win32 {
    OUTDIR = $${PWD}/win32
    INSTALL_PREFIX = $${OUTDIR}/out
}
