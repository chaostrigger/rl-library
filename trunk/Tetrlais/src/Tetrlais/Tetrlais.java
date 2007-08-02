package Tetrlais;

import messages.MCStateResponse;
import messages.TetrlaisStateResponse;
import rlVizLib.messaging.environment.EnvironmentMessageParser;
import rlVizLib.messaging.environment.EnvironmentMessages;
import rlVizLib.messaging.interfaces.RLVizEnvInterface;
import rlVizLib.utilities.UtilityShop;
import rlVizLib.general.ParameterHolder;
import rlVizLib.general.RLVizVersion;
import rlglue.types.Action;
import rlglue.types.Observation;
import rlglue.types.Random_seed_key;
import rlglue.types.Reward_observation;
import rlglue.types.State_key;
import rlVizLib.Environments.EnvironmentBase;

public class Tetrlais extends EnvironmentBase implements RLVizEnvInterface {


	private int tet_global_score =0;
	private GameState gameState = null;

	static final int terminal_score = -10;

	int numRows=4;
	int numCols=8;

	public Tetrlais(){
		super();
		gameState=new GameState(numRows,numCols);
	}
	public Tetrlais(ParameterHolder p){
		super();
		if(p!=null){
			if(!p.isNull()){
				numRows=p.getIntParam("NumRows");
				numCols=p.getIntParam("NumCols");
			}
		}
		gameState=new GameState(numRows,numCols);

	}


	//This method creates the object that can be used to easily set different problem parameters
	public static ParameterHolder getDefaultParameters(){
		ParameterHolder p = new ParameterHolder();
		p.addIntParam("NumRows",10);
		p.addIntParam("NumCols",10);
		return p;
	}



	/*Base RL-Glue Functions*/
	public String env_init() {
		/* initialize the environment, construct a task_spec to pass on. The tetris environment
		 * has 200 binary observation variables and 1 action variable which ranges between 0 and 4. These are all 
		 * integer values. */
		/*NOTE: THE GAME STATE WIDTH AND HEIGHT MUST MULTIPLY TO EQUAL THE NUMBER OF OBSERVATION VARIABLES*/
		int numStates=gameState.getHeight()*gameState.getWidth();

		String task_spec = "2.0:e:"+numStates+"_[";
		for(int i = 0; i< numStates-1; i++)
			task_spec = task_spec + "i, ";
		task_spec = task_spec + "i]";
		for(int i=0; i<numStates;i++)
			task_spec = task_spec + "_[0,1]";
		task_spec = task_spec + ":1_[i]_[0,5]";
		//fifth action is borkerd

		return task_spec;
	}

	public Observation env_start() {
		gameState.reset();
		tet_global_score =0;

		Observation o = gameState.get_observation();

		return o;
	}

	public Reward_observation env_step(Action action) {
		Reward_observation ro = new Reward_observation();

		ro.terminal = 1;

		if(!gameState.gameOver())
		{
			ro.terminal=0;
			gameState.take_action(action);
			gameState.update();
//			ro.r = 3.0d*(gameState.get_score() - tet_global_score) - .01d;
			ro.r=1.0d;
			tet_global_score = gameState.get_score();
		}
		else{
			ro.r = Tetrlais.terminal_score;
			tet_global_score = 0;	
		}

		ro.o = gameState.get_observation();
		return ro;
	}

	public void env_cleanup() {
		// TODO Auto-generated method stub

	}

	public Random_seed_key env_get_random_seed() {
		// TODO Auto-generated method stub
		System.out.println("The Tetris Environment does not implement env_get_random_seed. Sorry.");
		return null;
	}

	public State_key env_get_state() {
		// TODO Auto-generated method stub
		System.out.println("The Tetris Environment does not implement env_get_state. Sorry.");
		return null;
	}


	public String env_message(String theMessage) {
		EnvironmentMessages theMessageObject;
		try {
			theMessageObject = EnvironmentMessageParser.parseMessage(theMessage);
		} catch (Exception e) {
			System.err.println("Someone sent Tetrlais a message that wasn't RL-Viz compatible");
			return "I only respond to RL-Viz messages!";
		}



		if(theMessageObject.canHandleAutomatically(this)){
			return theMessageObject.handleAutomatically(this);
		}

		if(theMessageObject.getTheMessageType()==rlVizLib.messaging.environment.EnvMessageType.kEnvCustom.id()){

			String theCustomType=theMessageObject.getPayLoad();

			if(theCustomType.equals("GETTETRLAISSTATE")){
				//It is a request for the state
				TetrlaisStateResponse theResponseObject=new TetrlaisStateResponse(this.tet_global_score, this.gameState.getWidth(), this.gameState.getHeight(), this.gameState.getWorldState());
				return theResponseObject.makeStringResponse();
			}
			System.out.println("We need some code written in Env Message for Tetrlais.. unknown custom message type received");
			Thread.dumpStack();

			return null;
		}

		System.out.println("We need some code written in Env Message for  Tetrlais!");
		Thread.dumpStack();

		return null;
	}

	public void env_set_random_seed(Random_seed_key arg0) {
		// TODO Auto-generated method stub

		System.out.println("The Tetris Environment does not implement env_set_random_seed. Sorry.");

	}

	public void env_set_state(State_key arg0) {
		// TODO Auto-generated method stub
		System.out.println("The Tetris Environment does not implement env_set_state. Sorry.");
	}
	/*End of Base RL-Glue Functions */


	/*RL-Viz Methods*/
	@Override
	protected Observation makeObservation() {
		// TODO Auto-generated method stub
		return gameState.get_observation();
	}
	/*End of RL-Viz Methods*/

	public RLVizVersion getTheVersionISupport() {
		// TODO Auto-generated method stub
		return new RLVizVersion(1,0);
	}

	/*Tetris Helper Functions*/

	/*End of Tetris Helper Functions*/
}