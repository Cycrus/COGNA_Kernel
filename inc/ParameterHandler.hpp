#ifndef INCLUDE_PARAMETERHANDLER_HPP
#define INCLUDE_PARAMETERHANDLER_HPP

class ParameterHandler{
    public:
        float max_weight;
        float min_weight;

        float activation_backfall_curvature;
        float activation_backfall_steepness;

        float short_habituation_curvature;
        float short_habituation_steepness;
        float short_sensitization_curvature;
        float short_sensitization_steepness;
        float short_dehabituation_curvature;
        float short_dehabituation_steepness;
        float short_desensitization_curvature;
        float short_desensitization_steepness;

        float long_habituation_curvature;
        float long_habituation_steepness;
        float long_sensitization_curvature;
        float long_sensitization_steepness;
        float long_dehabituation_curvature;
        float long_dehabituation_steepness;
        float long_desensitization_curvature;
        float long_desensitization_steepness;

        float presynaptic_potential_curvature;
        float presynaptic_potential_steepness;
        float presynaptic_backfall_curvature;
        float presynaptic_backfall_steepness;

        float long_learning_weight_reduction_curvature;
        float long_learning_weight_reduction_steepness;
        float long_learning_weight_backfall_curvature;
        float long_learning_weight_backfall_steepness;

        float habituation_threshold;
        float sensitization_threshold;

        float transmitter_change_curvature;
        float transmitter_change_steepness;
        float transmitter_backfall_curvature;
        float transmitter_backfall_steepness;

        ParameterHandler();
};

#endif /* INCLUDE_PARAMETERHANDLER_HPP */
