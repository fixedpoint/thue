{
  description = "A flake for building thue";

  inputs.nixpkgs.url = github:NixOS/nixpkgs;

  outputs = { self, nixpkgs }: {

    packages.x86_64-linux.default =
      with import nixpkgs { system = "x86_64-linux"; };
      stdenv.mkDerivation {
        pname = "thue";
        version = "0.0.1";
        src = self;
        buildPhase = "make";
        doCheck = true;
        checkPhase = "make check";
        installPhase = ''
          install -d $out/bin
          install -m 0555 thue $out/bin
        '';
      };

    packages.aarch64-darwin.default =
      with import nixpkgs { system = "aarch64-darwin"; };
      stdenv.mkDerivation {
        pname = "thue";
        version = "0.0.1";
        src = self;
        buildPhase = "make";
        doCheck = true;
        checkPhase = "make check";
        installPhase = ''
          install -d $out/bin
          install -m 0555 thue $out/bin
        '';
      };
  };
}
