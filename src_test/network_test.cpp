#include "DataWriter.hpp"
#include "NeuralNetwork.hpp"

#include <cstdio>

using namespace COGNA;

#define POSITIVE_RESULT 1
#define NEGATIVE_RESULT 0

class Environment{
    public:
        int _machines[2];
        int _reward;

        Environment();
        int try_machine(int num);
};

Environment::Environment(){
    _machines[0] = 70;
    _machines[1] = 30;
}

int Environment::try_machine(int num){
    if(_machines[num] > random()%100){
        return POSITIVE_RESULT;
    }
    return NEGATIVE_RESULT;
}

/***********************************************************
 * main()
 *
 * Description: Main function of program
 *
 * Return:  int     Error code of program
 */
int main(){
    srandom(time(NULL));

    struct timeval time;
    long start_time, end_time, d_time = 0;
    long p_end_time = 0;
    const long TIME_BETWEEN_STEPS = 1;
    start_time = get_time_microsec(time);

    long start_action_time = get_time_microsec(time);
    int learning_counter = 0;

    Environment env;
    NeuralNetwork *nn = new NeuralNetwork();

    /* Learning Parameter */
    nn->_parameter->activation_backfall_curvature = 1.00f;
    nn->_parameter->activation_backfall_steepness = 0.04f;

    nn->_parameter->short_habituation_curvature = 0.65f;
    nn->_parameter->short_habituation_steepness = 0.07f;
    nn->_parameter->short_sensitization_curvature = 0.65f;
    nn->_parameter->short_sensitization_steepness = 0.07f;

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

    nn->_parameter->habituation_threshold = 0.001f;
    nn->_parameter->sensitization_threshold = 5.000f;

    nn->_parameter->long_learning_weight_reduction_curvature = 0.5f;
    nn->_parameter->long_learning_weight_reduction_steepness = 0.5f;
    nn->_parameter->long_learning_weight_backfall_curvature = 1.0f;
    nn->_parameter->long_learning_weight_backfall_steepness = 0.00000006f;

    nn->_parameter->transmitter_change_curvature = 1.02f;
    nn->_parameter->transmitter_change_steepness = 0.02f;
    nn->_parameter->transmitter_backfall_curvature = 1.00f;
    nn->_parameter->transmitter_backfall_steepness = 0.0000001f;

    /* Assembly of connection system */
    for(unsigned i=1; i<=23; i++){
        if(i==4 || i==12 || i==8 || i==9 || i==16 || i==17)
            nn->add_neuron(3.0f);
        else
            nn->add_neuron(0.001f);
    }

    Connection *machine_connection[2] = {NULL, NULL};

    DataWriter *output_handler = new DataWriter();
    if(DATA_ANALYTIC_OUTPUT){
        if(output_handler->open_file() == ERROR_CODE) goto exit;
    }

    if(!nn->add_neuron_connection(1, 2, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!(machine_connection[0] = nn->add_neuron_connection(2, 5, 0.9f, EXCITATORY, FUNCTION_RELU, LEARNING_HABISENS))) goto exit;
    if(!nn->add_neuron_connection(1, 4, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(3, 4, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(4, 5, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(5, 6, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(5, 14, 1.0f, INHIBITORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(6, 7, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(6, 20, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(6, 21, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(20, 9, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(21, 8, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;

    if(!nn->add_neuron_connection(1, 10, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!(machine_connection[1] = nn->add_neuron_connection(10, 13, 1.1f, EXCITATORY, FUNCTION_RELU, LEARNING_HABISENS))) goto exit;
    if(!nn->add_neuron_connection(1, 12, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(11, 12, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(12, 13, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(13, 14, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(13, 6, 1.0f, INHIBITORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(14, 15, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(14, 22, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(14, 23, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(22, 17, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(23, 16, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;

    if(!nn->add_neuron_connection(18, 8, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(19, 9, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_synaptic_connection(8, 2, 5, 0.3f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_synaptic_connection(9, 2, 5, 0.3f, INHIBITORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;

    if(!nn->add_neuron_connection(18, 16, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_neuron_connection(19, 17, 1.0f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_synaptic_connection(16, 10, 13, 0.3f, EXCITATORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;
    if(!nn->add_synaptic_connection(17, 10, 13, 0.3f, INHIBITORY, FUNCTION_RELU, LEARNING_NONE)) goto exit;

    //if(nn->set_random_neuron_activation(3, 200000, 2.1f) == ERROR_CODE) goto exit;
    //if(nn->set_random_neuron_activation(11, 200000, 2.1f) == ERROR_CODE) goto exit;

    if(nn->setup_network() == ERROR_CODE) goto exit;

    /* Start of network loop */
    printf("Neural network started successfully!\n\n*********************START*********************\n\n");

    do{
        if(p_end_time - start_action_time > 1000000){
            nn->init_activation(1, 1.0f);
            start_action_time = get_time_microsec(time);
            learning_counter++;
        }

        if(d_time >= TIME_BETWEEN_STEPS){
            start_time = get_time_microsec(time);
            if(DEBUG_MODE && DEB_BASE)
                nn->print_activation();

            nn->feed_forward();
            if(DATA_ANALYTIC_OUTPUT){
                output_handler->write_data(nn);
            }

            int temp_result = -10;
            if(nn->neuron_is_active(7)){
                temp_result = env.try_machine(0);
                printf("Machine 1 was triggered and COGNA was ");
                if(temp_result == POSITIVE_RESULT) printf("rewarded    :)\n");
                else printf("punished    :(\n");
                printf("Weight machine 0 = %.5f\n", machine_connection[0]->short_weight);
                printf("Weight machine 1 = %.5f\n\n", machine_connection[1]->short_weight);
            }
            if(nn->neuron_is_active(15)){
                temp_result = env.try_machine(1);
                printf("Machine 2 was triggered and COGNA was ");
                if(temp_result == POSITIVE_RESULT) printf("rewarded    :)\n");
                else printf("punished    :(\n");
                printf("Weight machine 0 = %.5f\n", machine_connection[0]->short_weight);
                printf("Weight machine 1 = %.5f\n\n", machine_connection[1]->short_weight);
            }

            switch(temp_result){
                case POSITIVE_RESULT:
                    nn->init_activation(18, 2.99f);
                    break;
                case NEGATIVE_RESULT:
                    nn->init_activation(19, 2.99f);
                    break;
            }

            p_end_time = get_time_microsec(time);

            if((p_end_time-start_time) > 100){
                //printf("Time required for step: %ld\n\n", p_end_time-start_time);
            }
        }
        // Time Calculation TODO Put into function
        end_time = get_time_microsec(time);
        d_time = end_time - start_time;
    }while(learning_counter <= 20);

    printf("\n\n#########################################\n\n");
    if(machine_connection[0]->short_weight > machine_connection[1]->short_weight){
        printf("The agent thinks, that machine 0 is the most lucrative...\n");
        if(env._machines[0] > env._machines[1]){
            printf("And it was right!\n");
        }
        else{
            printf("And it was wrong!\n");
        }
    }
    else{
        printf("The agent thinks, that machine 1 is the most lucrative...\n");
        if(env._machines[1] > env._machines[0]){
            printf("And it was right!\n");
        }
        else{
            printf("And it was wrong!\n");
        }
    }
    printf("\n");
    printf("Weight machine 0 = %.5f\n", machine_connection[0]->short_weight);
    printf("Weight machine 1 = %.5f\n", machine_connection[1]->short_weight);

    delete nn;
    delete output_handler;
    return SUCCESS_CODE;

    exit:
    delete nn;
    delete output_handler;
    return ERROR_CODE;
}
