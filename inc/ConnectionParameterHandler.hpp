#ifndef INCLUDE_CONNECTIONPARAMETERHANDLER_HPP
#define INCLUDE_CONNECTIONPARAMETERHANDLER_HPP

class ConnectionParameterHandler{
    public:
        int activation_type;      /* Defines if connection is inhibitory or excitatory */
        int activation_function;  /* Defines what activation function the connection uses */
        int learning_type;        /* Defines the kind of learning the connection is capable of */
        int transmitter_type;     /* Type of neurotransmitter used for transmitting action potential */

        ConnectionParameterHandler();
};

#endif /* INCLUDE_CONNECTIONPARAMETERHANDLER_HPP */
