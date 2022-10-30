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
};
