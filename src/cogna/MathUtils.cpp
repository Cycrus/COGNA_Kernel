#include "MathUtils.hpp"

#include <cmath>

using namespace COGNA;

namespace COGNA{
    float MathUtils::sigmoid(float input){
        return (float)(1.0 / (1.0 + (float)exp(-input)));
    }

    float MathUtils::linear(float input){
        return (float)input;
    }

    float MathUtils::relu(float input){
        if(input > 0) return (float)input;
        else return 0;
    }

    /***********************************************************
     * MathUtils::calculate_dynamic_gradient()
     *
     * Description: Calculates a gradient curve. The curvature depends on the
     *              source_value
     *
     * Parameters:  float   source_value    the value to be changed
     *              float   curve_factor    the steepness of the gradient
     *              long    x               the x value of the function f(x) -> often interpreted as time
     *              float   power_factor    the curvature of the gradient [0;1[->concave, [1;...]->convex
     *              int     method          ADD or SUBTRACT - indicates in what direction the gradient goes
     *
     * Return:      float  the function value of the gradient to the value time/x
     */
    float MathUtils::calculate_dynamic_gradient(float source_value,
                                                float curve_factor,
                                                double x,
                                                float power_factor,
                                                int method,
                                                float max,
                                                float min){
        float return_value = source_value + method*(source_value * curve_factor *
                             pow((x), power_factor));

        if(return_value < min){
            return_value = min;
        }
        else if(return_value > max){
            return_value = max;
        }
        return return_value;
    }

    /***********************************************************
     * MathUtils::calculate_static_gradient()
     *
     * Description: Calculates a gradient curve independent from the source value
     *
     * Parameters:  float   source_value    the value to be changed
     *              float   curve_factor    the steepness of the gradient
     *              long    x               the x value of the function f(x) -> often interpreted as time
     *              float   power_factor    the curvature of the gradient [0;1[->concave, [1;...]->convex
     *              int     method          ADD or SUBTRACT - indicates in what direction the gradient goes
     *
     * Return:      float  the function value of the gradient to the value time/x
     */
    float MathUtils::calculate_static_gradient(float source_value,
                                               float curve_factor,
                                               double x,
                                               float power_factor,
                                               int method,
                                               float max,
                                               float min){
        float return_value = source_value + method*(curve_factor *
                             pow((x), power_factor));

        if(return_value < min){
            return_value = min;
        }
        else if(return_value > max){
            return_value = max;
        }
        return return_value;
    }
}
