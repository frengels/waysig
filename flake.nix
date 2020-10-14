{
  description = "Wayland signal/listener wrapper in c++";

  inputs = { nixpkgs.url = "github:NixOS/nixpkgs"; };

  outputs = { self, nixpkgs }:
    let
      allSystems = [ "x86_64-linux" "i686-linux" "aarch64-linux" ];
      forAllSystems = f: nixpkgs.lib.genAttrs allSystems (system: f system);
      nixpkgsFor = forAllSystems (system:
        import nixpkgs {
          inherit system;
          overlays = [ self.overlay ];
        });

    in {
      overlay = final: prev: rec {
        waysig = with final;
          stdenv.mkDerivation rec {
            name = "waysig-${version}";
            version = "0.1.0";

            src = ./.;

            nativeBuildInputs = [ cmake ];

            meta = with stdenv.lib; {
              homepage = "https://github.com/dreyri/waysig";
              description =
                "C++ signal/slot implementation based on wl_signal/listener";
              license = licenses.mit;
            };
          };
      };

      packages =
        forAllSystems (system: { inherit (nixpkgsFor.${system}) waysig; });

      defaultPackage = forAllSystems (system: self.packages.${system}.waysig);

      devShell = forAllSystems (system:
        let pkgs = import nixpkgs { inherit system; };
        in with pkgs;
        mkShell {
          buildInputs = [ pkgconfig ccls cmake wayland extra-cmake-modules catch2 ];

          shellHook = ''
            echo "%compile_commands.json" > .ccls
            echo "%cpp -std=c++17" >> .ccls
            echo "-Iinclude" >> .ccls
            echo "-I${pkgs.wayland}/include >> .ccls
          '';
        });
    };
}
