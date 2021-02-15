#ifndef INCLUDE_CONSTANTS_HPP
#define INCLUDE_CONSTANTS_HPP

const bool DATA_ANALYTIC_OUTPUT = true;

const bool DEBUG_MODE = false;
const bool DEB_BASE = true;
const bool DEB_HABITUATION = false;
const bool DEB_SENSITIZATION = false;
const bool DEB_TRANSMITTER = false;
const bool DEB_NEURON_BACKFALL = false;
const bool DEB_LONG_LEARNING_WEIGHT = false;
const bool DEB_PRESYNAPTIC = false;

const int SUCCESS_CODE = 0;
const int ERROR_CODE = -1;

const int MICROSECOND_FACTOR = 1000000;
const int MIN_NEURON_ID = 1;

const int MIN_TRANSMITTER_WEIGHT = 0;
const int MAX_TRANSMITTER_WEIGHT = 3;

const float MIN_LONG_LEARNING_WEIGHT = 0.0f;
const float MAX_LONG_LEARNING_WEIGHT = 1.0f;

const long MAX_CHANCE = 100000000;

const float DEFAULT_TRANSMITTER_WEIGHT = 1.0f;
const float DEFAULT_PRESYNAPTIC_POTENTIAL = 1.0f;

const int ADD = 1;
const int SUBTRACT = -1;

const int EXCITATORY = 1;
const int INHIBITORY = -1;
const int NONDIRECTIONAL = 0;

const int FUNCTION_SIGMOID = 1;
const int FUNCTION_LINEAR = 2;
const int FUNCTION_RELU = 3;

const int LEARNING_NONE = 1;
const int LEARNING_HABITUATION = 2;
const int LEARNING_SENSITIZATION = 3;
const int LEARNING_HABISENS = 4;

const int POSITIVE_INFLUENCE = 1;
const int NEGATIVE_INFLUENCE = -1;

const int NO_TRANSMITTER = -1;
const int STD_TRANSMITTER = 0;

#endif /* INCLUDE_CONSTANTS_HPP */
