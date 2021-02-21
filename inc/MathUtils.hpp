/**
 * @file MathUtils.hpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief A class containing static utility math functions
 *
 * **Note:**
 * Contains different activation functions and the gradient calculations.
 *
 * @date 2021-02-16
 *
 */

#ifndef INCLUDE_MATHUTILS_HPP
#define INCLUDE_MATHUTILS

#include "Constants.hpp"

namespace COGNA{
    /**
     * @brief Class containing various static math utility functions.
     */
    class MathUtils{
        public:
            /**
             * @brief Function calculating Sigmoid activation function.
             *
             * @param input    Input value for the function.
             *
             * @return         Result of function.
             */
            static float sigmoid(float input);

            /**
             * @brief Function calculating ReLu activation function.
             *
             * @param input    Input value for the function.
             *
             * @return         Result of function.
             */
            static float relu(float input);

            /**
             * @brief Function calculating Linear activation function.
             *
             * @param input    Input value for the function.
             *
             * @return         Result of function.
             */
            static float linear(float input);

            /**
             * @brief Calculates the dynamic gradient function.
             *
             * Is dynamically dependent from the source value.
             * It is used in most activation and learning behavior.
             *
             * @param source_value    Value which should be manipulated.
             * @param curve_factor    Defines how wide the curve of the function is.
             * @param x               X value of function (e.g. time or activation).
             * @param power_factor    Defines how steep the function is.
             * @param method          Can be ADD or SUBTRACT. Defines if function increases or decreases.
             * @param max             Max value the function can return.
             * @param min             Min value the function can return.
             *
             * @return         Result of function.
             */
            static float calculate_dynamic_gradient(float source_value,
                                                    float curve_factor,
                                                    double x,
                                                    float power_factor,
                                                    int method=ADD,
                                                    float max=100.0f,
                                                    float min=0.0f);

            /**
             * @brief Calculates the static gradient function.
             *
             * Is not dependent from the source value.
             * It is used in most backdrop behaviors, such as dehabituation.
             *
             * @param source_value    Value which should be manipulated.
             * @param curve_factor    Defines how wide the curve of the function is.
             * @param x               X value of function (e.g. time or activation).
             * @param power_factor    Defines how steep the function is.
             * @param method          Can be ADD or SUBTRACT. Defines if function increases or decreases.
             * @param max             Max value the function can return.
             * @param min             Min value the function can return.
             *
             * @return         Result of function.
             */
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
