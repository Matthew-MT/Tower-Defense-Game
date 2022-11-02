#pragma once

namespace game {
    enum TileType {
        Empty = 0,
        Wall = 1,
        Spawn = 2,
        Base = 3,
        TurretType = 4
    };

    /*enum TurretType{
        Gatling = 0
    };*/

    enum TargetType {
        Closest = 0,
        First = 1,
        Strongest = 2
    };

    enum Option {
        None = 0,
        Right = 1,
        Left = 2,
        Easy = 3,
        Normal = 4,
        Hard = 5,
        Fun = 6
    };
};
