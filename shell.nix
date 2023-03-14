{ pkgs ? import <nixpkgs> { }
, system ? builtins.currentSystem
}:

pkgs.mkShell rec {
  buildInputs = with pkgs; [
    (yarn.overrideAttrs (finalAttrs: previousAttrs: {
      buildInputs = [ nodejs-slim-14_x ];
    }))
    cryptopp
    libsForQt5.qt5.qmake
    libsForQt5.qt5.qtmultimedia
    libsForQt5.qt5.qtsvg
    libsForQt5.qt5.qtwebchannel
    libsForQt5.qt5.qtwebengine
    libsForQt5.qt5.wrapQtAppsHook
    nspr
    nss
    openssl
    pkgconfig
    python3
    zlib
  ];
}
