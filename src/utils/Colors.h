#ifndef SUNDOWNDRIVE_UTILS_COLORS_H
#define SUNDOWNDRIVE_UTILS_COLORS_H

#include <glm/glm.hpp>

namespace Colors {
    // sky and atmosphere
    constexpr glm::vec3 SunsetOrange   {1.0f, 0.45f, 0.15f};
    constexpr glm::vec3 SunsetPink     {1.0f, 0.55f, 0.60f};
    constexpr glm::vec3 DuskPurple     {0.35f, 0.15f, 0.45f};
    constexpr glm::vec3 SkyBlue        {0.40f, 0.60f, 0.85f};
    constexpr glm::vec3 HorizonYellow  {1.0f, 0.85f, 0.35f};

    // terrain and nature
    constexpr glm::vec3 GrassGreen     {0.25f, 0.55f, 0.20f};
    constexpr glm::vec3 EarthBrown     {0.40f, 0.28f, 0.15f};
    constexpr glm::vec3 DarkEarth      {0.20f, 0.10f, 0.05f};

    // road and urban
    constexpr glm::vec3 AsphaltGray    {0.30f, 0.30f, 0.32f};
    constexpr glm::vec3 RoadLineWhite  {0.95f, 0.95f, 0.90f};
    constexpr glm::vec3 RoadLineYellow {1.0f, 0.85f, 0.10f};

    // lighting
    constexpr glm::vec3 SunLight       {1.0f, 0.85f, 0.60f};
    constexpr glm::vec3 LampWarm       {1.0f, 0.90f, 0.65f};
    constexpr glm::vec3 AmbientDim     {0.15f, 0.10f, 0.12f};

    // general
    constexpr glm::vec3 White          {1.0f, 1.0f, 1.0f};
    constexpr glm::vec3 Black          {0.0f, 0.0f, 0.0f};
}

#endif // SUNDOWNDRIVE_UTILS_COLORS_H
