{
  description = "A flake for building thue";

  inputs.nixpkgs.url = github:NixOS/nixpkgs/nixos-23.05;

  outputs = { self, nixpkgs }:
    let
      derivation_thue = system:
        with import nixpkgs { inherit system; };
        stdenv.mkDerivation {
          pname = "thue";
          version = "0.0.1";
          src = self;
          doCheck = true;
      };
    in {
      packages.aarch64-darwin.default = derivation_thue "aarch64-darwin";
      packages.aarch64-linux.default  = derivation_thue "aarch64-linux";
      packages.i686-linux.default     = derivation_thue "i686-linux";
      packages.x86_64-darwin.default  = derivation_thue "x86_64-darwin";
      packages.x86_64-linux.default   = derivation_thue "x86_64-linux";
    };
}
