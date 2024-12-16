#!/bin/bash
set -e

BUILD_DIR=build
mkdir $BUILD_DIR
pushd $BUILD_DIR
cmake ../
make -j4

cat <<EOF >server.desktop
[Desktop Entry]
Type=Application
Name=QtgRPCTestServer
Exec=server
Icon=default
Comment=Comment
Terminal=true
Categories=Utility;
EOF

linuxdeployqt server.desktop -appimage 


cat <<EOF >client.desktop
[Desktop Entry]
Type=Application
Name=QtgRPCTestClient
Exec=client
Icon=default
Comment=Comment
Terminal=true
Categories=Utility;
EOF

linuxdeployqt client.desktop -appimage -qmldir=../client

popd