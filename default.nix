{ pkgs ? import <nixpkgs> { }
, system ? builtins.currentSystem
}:

pkgs.stdenv.mkDerivation rec {
  name = "void";
  version = "master";
  src = ./.;
  nativeBuildInputs = with pkgs; [
    libsForQt5.qt5.qmake
    libsForQt5.qt5.wrapQtAppsHook
    pkgconfig
  ];
  buildInputs = with pkgs; [
    cryptopp
    libsForQt5.qt5.qtmultimedia
    libsForQt5.qt5.qtsvg
    libsForQt5.qt5.qtwebchannel
    libsForQt5.qt5.qtwebengine
    nspr
    nss
    openssl
    zlib
    icu
  ];
}
