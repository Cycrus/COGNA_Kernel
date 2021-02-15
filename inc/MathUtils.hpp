#ifndef INCLUDE_MATHUTILS_HPP
#define INCLUDE_MATHUTILS

#include "Constants.hpp"

namespace COGNA{
    class MathUtils{
        public:
            static float sigmoid(float input);
            static float relu(float input);
            static float linear(float input);

            static float calculate_dynamic_gradient(float source_value,
                                                    float curve_factor,
                                                    double x,
                                                    float power_factor,
                                                    int method=ADD,
                                                    float max=100.0f,
                                                    float min=0.0f);
            static float calculate_static_gradient(float source_value,
                                                   float curve_factor,
                                                   double x,
                                                   float power_factor,
                                                   int method=ADD,
                                                   float max=100.0f,
                                                   float min=0.0f);
    };
}

#endif /* INCLUDE_MATHUTILS */
