#include "NeuralNetwork.hpp"
#include "HelperFunctions.hpp"

#include <cstdio>
#include <ctime>

/***********************************************************
 * main()
 *
 * Description: Main function of program
 *
 * Return:  int     Error code of program
 */
int main(){
    int key_ret = 0;

    struct timeval time;
    long start_time, end_time, d_time;
    long p_end_time;
    const long TIME_BETWEEN_STEPS = 1;//500 * 1000;
    start_time = utils::get_time_microsec(time);

    // const int MOTONEURON = 7;
    const int PROBE = 7;
    COGNA::NeuralNetwork *nn = new COGNA::NeuralNetwork();

    /* Learning Parameter */
    nn->_parameter->activation_backfall_curvature = 1.00f;
    nn->_parameter->activation_backfall_steepness = 0.047f;

    nn->_parameter->short_habituation_curvature = 1.02f;
    nn->_parameter->short_habituation_steepness = 0.09f;
    nn->_parameter->short_sensitization_curvature = 1.02f;
    nn->_parameter->short_sensitization_steepness = 0.09f;

    nn->_parameter->long_habituation_curvature = 0.35f;
    nn->_parameter->long_habituation_steepness = 0.00005f;
    nn->_parameter->long_sensitization_curvature = 1.02f;
    nn->_parameter->long_sensitization_steepness = 0.0001f;

    nn->_parameter->short_dehabituation_curvature = 1.00f;
    nn->_parameter->short_dehabituation_steepness = 0.00000005f;
    nn->_parameter->short_desensitization_curvature = 1.00f;
    nn->_parameter->short_desensitization_steepness = 0.00000005f;

    nn->_parameter->long_dehabituation_curvature = 1.00f;
    nn->_parameter->long_dehabituation_steepness = 0.000000000001f;
    nn->_parameter->long_desensitization_curvature = 1.00f;
    nn->_parameter->long_desensitization_steepness = 0.000000000001f;

    nn->_parameter->presynaptic_potential_curvature = 0.60f;
    nn->_parameter->presynaptic_potential_steepness = 0.3f;
    nn->_parameter->presynaptic_backfall_curvature = 1.00f;
    nn->_parameter->presynaptic_backfall_steepness = 0.0000002f;

    nn->_parameter->habituation_threshold = 2.0f;
    nn->_parameter->sensitization_threshold = 4.0f;

    nn->_parameter->long_learning_weight_reduction_curvature = 0.5f;
    nn->_parameter->long_learning_weight_reduction_steepness = 0.5f;
    nn->_parameter->long_learning_weight_backfall_curvature = 1.0f;
    nn->_parameter->long_learning_weight_backfall_steepness = 0.00000006f;

    nn->_parameter->transmitter_change_curvature = 1.02f;
    nn->_parameter->transmitter_change_steepness = 0.02f;
    nn->_parameter->transmitter_backfall_curvature = 1.00f;
    nn->_parameter->transmitter_backfall_steepness = 0.0000001f;

    /* Assembly of connection system */
    for(unsigned i=1; i<=7; i++){
        nn->add_neuron(0.1f);
    }

    if(!nn->add_neuron_connection(1, 2, 1.0f, COGNA::EXCITATORY, COGNA::FUNCTION_RELU,
        COGNA::LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(2, 5, 1.0f, COGNA::EXCITATORY, COGNA::FUNCTION_RELU,
        COGNA::LEARNING_HABISENS)) goto exit;
    if(!nn->add_neuron_connection(3, 4, 1.0f, COGNA::EXCITATORY, COGNA::FUNCTION_RELU,
        COGNA::LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(4, 5, 1.0f, COGNA::EXCITATORY, COGNA::FUNCTION_RELU,
        COGNA::LEARNING_HABISENS)) goto exit;
    if(!nn->add_neuron_connection(6, 5, 1.0f, COGNA::EXCITATORY, COGNA::FUNCTION_RELU,
        COGNA::LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(5, 7, 1.0f, COGNA::EXCITATORY, COGNA::FUNCTION_RELU,
        COGNA::LEARNING_NONE)) goto exit;
    if(!nn->add_synaptic_connection(2, 4, 5, 1.0f, COGNA::NONDIRECTIONAL, COGNA::FUNCTION_RELU,
        COGNA::LEARNING_NONE)) goto exit;
    if(!nn->add_synaptic_connection(4, 2, 5, 1.0f, COGNA::NONDIRECTIONAL, COGNA::FUNCTION_RELU,
        COGNA::LEARNING_NONE)) goto exit;

    if(nn->set_random_neuron_activation(6, 20, 1.0) == COGNA::ERROR_CODE) goto exit;

    if(nn->setup_network() == COGNA::ERROR_CODE) goto exit;

    /* Start of network loop */
    printf("Neural network started successfully!\n\n*********************START*********************\n\n");

    do{
        key_ret = getc(stdin);
        switch(key_ret){
            case '7':
                nn->init_activation(3, 1.0f);
                break;
            case '8':
                nn->init_activation(3, 3.0f);
                break;
            case '9':
                nn->init_activation(3, 5.0f);
                break;

            case '1':
                nn->init_activation(1, 1.0f);
                break;
            case '2':
                nn->init_activation(1, 3.0f);
                break;
            case '3':
                nn->init_activation(1, 5.0f);
                break;
        }

        if(d_time >= TIME_BETWEEN_STEPS){
            start_time = utils::get_time_microsec(time);
            if(COGNA::DEBUG_MODE && COGNA::DEB_BASE)
                nn->print_activation();

            nn->feed_forward();

            /* Useless without sound.
            if(nn->neuron_is_active(MOTONEURON)){
                if(nn->get_neuron_activation(MOTONEURON) <= 2.0f)
                    play_wav(MAJOR_2, 100);
                else if(nn->get_neuron_activation(MOTONEURON) > 2.0f &&
                        nn->get_neuron_activation(MOTONEURON) <= 2.5f)
                    play_wav(MAJOR_3, 100);
                else if(nn->get_neuron_activation(MOTONEURON) > 2.5f &&
                        nn->get_neuron_activation(MOTONEURON) <= 3.0f)
                    play_wav(MAJOR_4, 100);
                else if(nn->get_neuron_activation(MOTONEURON) > 3.0f &&
                        nn->get_neuron_activation(MOTONEURON) <= 3.5f)
                    play_wav(MAJOR_5, 100);
                else if(nn->get_neuron_activation(MOTONEURON) > 3.5f &&
                        nn->get_neuron_activation(MOTONEURON) <= 4.0f)
                    play_wav(MAJOR_6, 100);
                else if(nn->get_neuron_activation(MOTONEURON) > 4.0f &&
                        nn->get_neuron_activation(MOTONEURON) <= 4.5f)
                    play_wav(MAJOR_7, 100);
                else
                    play_wav(MAJOR_8, 100);
            }
            */

            p_end_time = utils::get_time_microsec(time);
            if(nn->get_neuron_activation(PROBE) > 0.05f)
                printf("Neuron %d activation: %.3f\n", PROBE, nn->get_neuron_activation(PROBE));
            if((p_end_time-start_time) > 100){
                printf("Time required for step: %ld\n\n", p_end_time-start_time);
            }
        }
        // Time Calculation TODO Put into function
        end_time = utils::get_time_microsec(time);
        d_time = end_time - start_time;
    }while(key_ret != '\b');

    delete nn;
    return COGNA::SUCCESS_CODE;

    exit:
    delete nn;
    return COGNA::ERROR_CODE;
}
