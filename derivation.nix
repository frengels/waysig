{ stdenv, pkgconfig, meson, ninja }:

stdenv.mkDerivation rec {
    name = "waysig-${version}";
    version = "0.1.0";

    src = ./.;

    nativeBuildInputs = [ meson ninja pkgconfig ];

    meta = with stdenv.lib; {
        homepage = "https://github.com/dreyri/waysig";
        description = "C++ signal/slot implementation based on wl_signal/listener";
        license = licenses.mit;
    };
}