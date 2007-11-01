# Build RL-Glue so that each part will be communicating via network communication

RL-GLUE = ./rl-glue/RL-Glue
BUILD_PATH = ../obj/obj.rlglue
BIN_PATH = ..

CC      = g++  
CFLAGS  = -I$(RL-GLUE)/ -ansi -pedantic -Wall
LDFLAGS =

GLUE_OBJECTS = RL_glue.o RL_network.o RL_server_agent.o RL_server_environment.o RL_server_experiment.o

# Link everything together
RL_glue: $(GLUE_OBJECTS)
	$(CC) -o $(BIN_PATH)/$@ $(addprefix $(BUILD_PATH)/, $(GLUE_OBJECTS))

# Build the main interface. 
RL_glue.o: $(RL-GLUE)/RL_glue.c
	$(CC) $(CFLAGS) -c $< -o $(BUILD_PATH)/$@

# Build the network stuff
RL_network.o: $(RL-GLUE)/Network/RL_network.c
	$(CC) $(CFLAGS) -c $< -o $(BUILD_PATH)/$@

# Compile the thing that make the Glue think its talking to an agent when its really talking over a socket
# If you wanted to compile the agent into the glue, you wouldn't need to compile this, or link to it. Instead
# you would just compile and link in your agent	
RL_server_agent.o: $(RL-GLUE)/Network/Glue/RL_server_agent.c 
	$(CC) $(CFLAGS) -c $< -o $(BUILD_PATH)/$@

# Compile the thing that make the Glue think its talking to an environment when its really talking over a socket
# If you wanted to compile the environment into the glue, you wouldn't need to compile this, or link to it. Instead
# you would just compile and link in your environment	
RL_server_environment.o: $(RL-GLUE)/Network/Glue/RL_server_environment.c 
	$(CC) $(CFLAGS) -c $< -o $(BUILD_PATH)/$@

# Compile the thing that make the Glue think its talking to an experiment when its really talking over a socket
# If you wanted to compile the experiment into the glue, you wouldn't need to compile this, or link to it. Instead
# you would just compile and link in your experiment	
RL_server_experiment.o: $(RL-GLUE)/Network/Glue/RL_server_experiment.c
	$(CC) $(CFLAGS) -c $< -o $(BUILD_PATH)/$@
