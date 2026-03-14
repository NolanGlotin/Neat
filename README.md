# Neat : NeuroEvolution of Augmenting Topologies in C

A C implementation of the NEAT algorithm from scratch, which evolves neural networks by modifying both their weights and their structure.

## Features
- Evolve both the weights and the structure of neural networks
- Speciation to maintain diversity in the population
- Support for various activation functions
- Configurable parameters for mutation rates, population size, etc.
- Easy-to-use API for creating and evolving neural networks
- Logging of the training process for analysis and visualization
- Example implementation of the algorithm on the inverted pendulum problem

## Neat algorithm overview

The NEAT algorithm (NeuroEvolution of Augmenting Topologies, Stanley, 2002) starts with a population of simple neural networks, each with only input and output neurons. The algorithm then iteratively evolves the population through selection, mutation, and crossover. The fitness function is used to evaluate the performance of each network, and the best-performing networks are selected for reproduction. The algorithm applies various mutation operators to create new networks, and it also uses crossover to combine the genetic material of two parent networks to create offspring.

### Fitness function
The fitness function is a crucial component of the NEAT algorithm, as it guides the evolution of the neural networks. It should evaluate how well the network performs on the task at hand, and it can be designed in various ways depending on the specific problem you are trying to solve.

### Speciation
The NEAT algorithm uses speciation to maintain diversity in the population of neural networks. This is achieved by grouping similar networks into species based on their genetic distance. The algorithm then applies selection and reproduction within each species, which helps to preserve innovative structures and prevent premature convergence.

### Mutation and Crossover
The NEAT algorithm applies various mutation operators to evolve the neural networks. These include:
- Weight mutation: Randomly perturbing the weights of the connections in the network.
- Structural mutation: Adding new nodes or connections to the network.
- Crossover: Combining the genetic material of two parent networks to create offspring. 

You can see the original paper for more details on the algorithm: [Stanley, 2002](https://nn.cs.utexas.edu/downloads/papers/stanley.ec02.pdf).

## Installation
### Build and install the library (Linux/MacOS) :

```bash
make build
sudo make install
make clean
```

### To uninstall :

```bash
sudo make uninstall
```

## Usage
To run the Neat algorithm, call the `neat` function as follows:

```c
#include <neat/neat.h>
...
network_t *model = neat(input_number, output_number, fitness_function, &fitness, log_path);
```
Where:
- `input_number` is the number of input neurons in the network.
- `output_number` is the number of output neurons in the network.
- `fitness_function` is a pointer to the function that evaluates the fitness of a network (takes a network as input and returns a float representing its fitness).
- `&fitness` is a pointer to a float variable where the fitness of the best network will be stored.
- `log_path` is the path to a file where the progress of the algorithm will be logged. The log file will contain the following columns: generation number, best fitness, average fitness, and species count.
- `model` is a pointer to the best network found by the algorithm.

Note that you do not need to specify the number of hidden neurons, as the algorithm will evolve the structure of the network over time.

## Configuration

The Neat algorithm can be configured using the `config_t` structure defined in `config.h`. The parameters can be accessed through the `neat_config` global variable. By default, the parameters are set to standard values. 
To change the parameters, simply modify the values in the `neat_config` variable before calling the `neat` function, for example:
```c
neat_config.population_size = 200;
```

Here are all the currently available parameters:

- General parameters:
  - `generation_number`: The maximum number of generations to run the algorithm.
  - `fitness_threshold`: The fitness threshold for stopping the algorithm.
  - `population_size`: The number of networks in the population.
  - `survival_rate`: The percentage of the population that can reproduce
  - `elitism`: The percentage of the population that is guaranteed to survive to the next generation.
- Neural network parameters:
  - `network_weight_range`: The range of the weights in the networks.
  - `network_activation_function`: The activation function used in the neural networks (some are provided in `utils.h`).
- Mutation parameters:
  - `mutation_replace_weight_proba`: The probability of replacing a weight with a new random value.
  - `mutation_perturb_weight_proba`: The probability of perturbing a weight.
  - `mutation_add_connection_proba`: The probability of adding a new connection.
  - `mutation_delete_connection_proba`: The probability of removing a connection.
  - `mutation_add_neuron_proba`: The probability of adding a new node.
  - `mutation_delete_neuron_proba`: The probability of removing a node.
  - `mutation_toggle_enabled_proba`: The probability of switching the enabled/disabled state of a connection.
  - `mutation_perturb_weight_factor`: The factor by which a weight is perturbed when applying the perturbation mutation.
  - `mutation_new_connection_tries`: The number of attempts to find two unconnected nodes when adding a new connection.
  - `mutation_new_node_tries`: The number of attempts to find an enabled connection to split when adding a new node.
- Speciation parameters:
  - `speciation_c1`: The coefficient for excess genes in the compatibility distance calculation.
  - `speciation_c2`: The coefficient for disjoint genes in the compatibility distance calculation.
  - `speciation_c3`: The coefficient for average weight differences in the compatibility distance calculation.
  - `speciation_compatibility_threshold`: The compatibility distance threshold for determining whether two networks belong to the same species.
  - `speciation_stagnation_threshold`: The number of generations without improvement after which a species is considered stagnant and is not allowed to reproduce.

  
## The neural network API
The library provides an API for creating and manipulating neural networks. Here are some of the key functions:

- `network_t *create_network(float (*activation)(float))`: Creates a new neural network with the specified activation function (some are provided in `utils.h`).
- `void free_network(network_t *network)`: Frees the memory allocated for a neural network.
- `void feed_forward(network_t *network, float *inputs, float *outputs)`: Performs a forward pass through the network with the given inputs and stores the outputs in the provided array.
- `void save_network(network_t *network, const char *filename)`: Saves a neural network to a file.
- `network_t *load_network(const char *filename)`: Loads a neural network from a file.

## Example

![Inverted Pendulum](examples/inverted_pendulum/demo.gif)

Let's equilibrate an inverted pendulum on a cart using the Neat algorithm ! The possible actions are to apply a force to the cart in either direction and the fitness function will be defined as the time the pendulum remains balanced. The complete code for this example can be found in the `examples` directory.

```c
// Training a model
void train_model(char *model_output) {
    srand(time(NULL));
    network_t *model = neat(
        INPUT_NB,
        OUTPUT_NB,
        fitness_function,
        NULL,
        "log.csv"
    );
    save_network(model, model_output);
    free_network(model);
}

// Testing the model
void test_model(char *model_name) {
    // Loading the model
    network_t *model = load_network(model_name);
    float inputs[INPUT_NB];
    float outputs[OUTPUT_NB];

    // Initializing the pendulum
    inverted_pendulum_t pendulum = {0};
    reset_pendulum(&pendulum);
    float duration = 0.0f;
    init_window();

    while(!window_should_close()) {
        float dt = get_frame_time();
        duration += dt;

        // Running forward pass through the network
        inject_inputs(pendulum, inputs);
        feed_forward(model, inputs, outputs);

        // Controlling the pendulum using the network's output
        if (CONTROL_CART_SPEED)
            update_pendulum_speed(dt, outputs[0]*CART_SPEED, &pendulum);
        else
            update_pendulum_acceleration(dt, outputs[0]*CART_ACCELERATION, &pendulum);

        // Drawing the pendulum
        draw(pendulum);
    }
    close_window();
    free_network(model);
}

// Main function
int main(void) {
    train_model("output.txt");
    test_model("output.txt");
    return EXIT_SUCCESS;
}
```

The `train_model` function trains a model using the Neat algorithm and saves it to a file. The `test_model` function loads the trained model, runs it on the inverted pendulum environment, and logs the state of the pendulum over time.

### Compilation
This project uses Raylib to render the simulation.
Make you have Raylib and neat installed and then use the makefile :

```bash
cd examples/inverted_pendulum
make
./main
```

The training should only take a few seconds and will generate a file named `output.txt` containing the best model found by the algorithm, and a file named `log.csv` containing the report of the training process. The simulation will open in a new window, allowing you to visualize the performance of the trained model. It can happen that the model fails to balance the pendulum, in this case you can simply run the test again.

## Disclaimer
This implementation of the NEAT algorithm is intended for educational purposes and may contain bugs or inefficiencies.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Author
Nolan Glotin 2025