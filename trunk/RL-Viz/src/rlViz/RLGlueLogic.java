package rlViz;


import java.util.Timer;
import java.util.TimerTask;
import java.util.Vector;

import rlVizLib.general.ParameterHolder;
import rlVizLib.general.RLVizVersion;
import rlVizLib.general.TinyGlue;
import rlVizLib.messaging.environment.EnvVersionSupportedRequest;
import rlVizLib.messaging.environment.EnvVersionSupportedResponse;
import rlVizLib.messaging.environmentShell.EnvShellListRequest;
import rlVizLib.messaging.environmentShell.EnvShellListResponse;
import rlVizLib.messaging.environmentShell.EnvShellLoadRequest;
import rlVizLib.messaging.environmentShell.EnvShellUnLoadRequest;
import rlVizLib.visualization.AbstractVisualizer;

public class RLGlueLogic {

//Singleton pattern, should make sure its thread safe
	static RLGlueLogic theGlobalGlueLogic=null;

	static RLGlueLogic getGlobalGlueLogic(){
		if(theGlobalGlueLogic==null)theGlobalGlueLogic=new RLGlueLogic();
		
		return theGlobalGlueLogic;
	}
	
	boolean debugLocal=false;

	TinyGlue myGlueState=null;
	AbstractVisualizer theEnvVisualizer=null;
	
	Timer currentTimer=null;
	
	int timeStepDelay=100;
	boolean running=false;

	
	Vector<visualizerLoadListener> envVisualizerLoadListeners=new Vector<visualizerLoadListener>();
	private RLVizVersion theEnvVersion=null;
	
	private RLGlueLogic(){}
	
	public RLVizVersion getEnvVersion(){
		return theEnvVersion;
	}

	public void step(){
		//This is not ideal.. getting bad fast
		if(theEnvVisualizer!=null)
			if(!theEnvVisualizer.isCurrentlyRunning()){
				theEnvVisualizer.startVisualizing();
			}
		myGlueState.step();
	}

	
	private EnvShellListResponse theEnvListResponseObject=null;
	public Vector<String> getEnvNameList(){
		if(debugLocal){
			Vector<String> theList=new Vector<String>();
			theList.add("Env 1");
			theList.add("Env 2");
			return theList;
		}
		//Get the Environment Names
		if(theEnvListResponseObject==null)theEnvListResponseObject=EnvShellListRequest.Execute();
		return theEnvListResponseObject.getTheEnvList();
	}

	public Vector<ParameterHolder> getEnvParamList(){
		if(debugLocal){
			Vector<ParameterHolder> theList=new Vector<ParameterHolder>();
			
			ParameterHolder p = ParameterHolder.makeTestParameterHolder();
			theList.add(p);

			p = new ParameterHolder();
			p.addIntParam("sampleParam2",7);
			theList.add(p);
			return theList;
		}
		//Get the Environment Names
		if(theEnvListResponseObject==null)theEnvListResponseObject=EnvShellListRequest.Execute();
		return theEnvListResponseObject.getTheParamList();
	}

	
	public Vector<String> getAgentList(){
		//don't have an agent list loading mechanism yet
		if(debugLocal||true){
			Vector<String> theList=new Vector<String>();
			theList.add("Agent 1");
			theList.add("Agent 2");
			theList.add("Agent 3");
			return theList;
		}
//		//Get the Environment Names
//		EnvShellListResponse theEnvListResponseObject=EnvShellListRequest.Execute();
//		return theEnvListResponseObject.getTheEnvList();
		return null;
	}

	public void loadEnvironment(String envName, ParameterHolder currentParams) {
		EnvShellLoadRequest.Execute(envName,currentParams);
		
		EnvVersionSupportedResponse versionResponse=EnvVersionSupportedRequest.Execute();
		
		//this shouldn't happen anyway
		if(versionResponse!=null)
			theEnvVersion=versionResponse.getTheVersion();
		else
			theEnvVersion=RLVizVersion.NOVERSION;
		
		myGlueState=new TinyGlue();
		theEnvVisualizer=EnvVisualizerFactory.createVisualizerFromString(envName);
		notifyEnvVisualizerListenersNewEnv();
	}



	public void start() {
		if(running)
			stop();
		running=true;
	    currentTimer = new Timer();
    
	    currentTimer.scheduleAtFixedRate(new TimerTask() {
	            public void run() {
	            	step();
	            }
	        }, 0, timeStepDelay);		
	}

	public void stop() {
		if(currentTimer!=null){
			currentTimer.cancel();
			currentTimer=null;
		}
		running=false;
		
	}

	public void setNewStepDelay(int stepDelay) {
		this.timeStepDelay=stepDelay;
		if(running)start();
	}

	public void setNewValueFunctionResolution(int theValue) {
		if(theEnvVisualizer!=null)
			theEnvVisualizer.setValueFunctionResolution(theValue);
	}

	private void notifyEnvVisualizerListenersNewEnv() {
		for (visualizerLoadListener thisListener : envVisualizerLoadListeners)
			thisListener.notifyVisualizerLoaded(theEnvVisualizer);
	}


	public void addEnvVisualizerLoadListener(visualizerLoadListener panel) {
		envVisualizerLoadListeners.add(panel);
	}

	public void unloadEnvironment() {
		notifyEnvVisualizerListenersUnloadEnv();
		theEnvVisualizer.stopVisualizing();
		theEnvVisualizer=null;
		myGlueState=null;

		
		rlglue.RLGlue.RL_cleanup();
		EnvShellUnLoadRequest.Execute();
	}

	private void notifyEnvVisualizerListenersUnloadEnv() {
		for (visualizerLoadListener thisListener : envVisualizerLoadListeners)
			thisListener.notifyVisualizerUnLoaded();
	}


}



