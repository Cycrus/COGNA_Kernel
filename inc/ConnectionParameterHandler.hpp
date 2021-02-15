#ifndef INCLUDE_CONNECTIONPARAMETERHANDLER_HPP
#define INCLUDE_CONNECTIONPARAMETERHANDLER_HPP

class ConnectionParameterHandler{
    public:
        int activation_type;      /* Defines if connection is inhibitory or excitatory */
        int activation_function;  /* Defines what activation function the connection uses */
        int learning_type;        /* Defines the kind of learning the connection is capable of */
        int transmitter_type;     /* Type of neurotransmitter used for transmitting action potential */

        float max_weight;
        float min_weight;

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

        ConnectionParameterHandler();
};

#endif /* INCLUDE_CONNECTIONPARAMETERHANDLER_HPP */
