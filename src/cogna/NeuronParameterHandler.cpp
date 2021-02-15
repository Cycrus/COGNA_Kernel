#include "NeuronParameterHandler.hpp"

using namespace COGNA;

namespace COGNA{
    NeuronParameterHandler::NeuronParameterHandler(){
        activation_threshold = 0.0f;

        max_activation = 50.0f;
        min_activation = 0.0f;

        activation_backfall_curvature = 0.0f;
        activation_backfall_steepness = 0.0f;

        transmitter_change_curvature = 0.0f;
        transmitter_change_steepness = 0.0f;

        influenced_transmitter = NO_TRANSMITTER;
        transmitter_influence_direction = POSITIVE_INFLUENCE;
    }
}
