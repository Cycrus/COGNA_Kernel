# COGNA

## Currently not further developed, because of switch to the official Nengo neuro engineering framework.

<h1>Idea</h1>
  The idea behind COGNA (Cognitive Oriented Generalized Neural Approach) is to emulate basic neurochemical principles from various neurological studies and combine them in one network architecture.
  
<h1>Concept</h1>
  Generally a COGNA network contains multiple neuron objects which are connected with dedicated connection objects. Each neuron can have multiple connections to different neurons, but only one connection to a single other neuron. Neurons can also have connections to other connections to modulate their activation and learning behavior.
  
  The present neural network allows for realistic simulation of biological neural systems. It uses a collection of digital neurons which communicate with each other with an analogous code. The idea is derived from spiking neural networks, which means, that time plays a role in this network type. When an input is processed, it does not reach the output instantaneously, rather than one array of connections after another. This approach allows for runtime manipulation of connections and activations based on updated information and memory.
  
The network has unique features which are listed below:

    • Excitatory & inhibitory connections:
    State of the art neural networks use only excitatory connections, since they are sufficient for rather simple networks used for classification or limited tasks.
    The present network allows for excitatory and inhibitory connections.
    This means, that firing neurons not always activate receiving neurons, but (if predefined) they can also reduce activation from receiving neurons.
    This can be used for processing contrasts, which is an important ability for real life decision making.
    
    • Biologically inspired learning:
    In its current state, connections and weights in the present network cannot be adjusted using backpropagation or other commonly used learning algorithms.
    It rather uses biologically inspired methods to dynamically adjust its connectivity to its current environment.
    Two learning methods have currently been selected: habituation and sensitization. Habituation is a mechanism where neural connections weaken, when activated weakly multiple times.
    Sensitization is the opposite where connections strenghten after strong activations.
    They allow for simple learning behavior.
    However, it is hypothesized, that with those two learning methods more complex methods, such as operant conditioning, can be constructed in multineural approaches.
    
    • Presynaptic connections:
    A connection coming from a neuron can connect to another connection. This allows for biasing the connection.
    It does not change the threshold of a neuron, but it can increase or decrease the activation coming from this neuron when it fires.
    This allows for implementation of biological learning mechanisms, such as classical conditioning.
    
    • Neurotransmitters:
    Every connection works with a neurotransmitter. Every connection can have a different neurotransmitter.
    The amount of specific transmitters can be increased or decreased during runtime when certain neurons fire.
    This simulates biological neurotransmitters and hormone systems and can influence the general connectivity of the network based on various situational cues.
  
<h1>Architecture</h1>
  Contrary to most other network architectures not all connections are activated simultaneously, rather than one step after another. This allows for including time as a variable in the processing of information. When a neuron is activated, it activates all following neurons, those activate all next neurons and so on.
  This happens by storing pointers to all connections, which must be activated currently inside a global array. All connections which are inside of this array fire by clearing the array saving all next connections inside of it, if the activation threshold of a neuron is exceeded.
