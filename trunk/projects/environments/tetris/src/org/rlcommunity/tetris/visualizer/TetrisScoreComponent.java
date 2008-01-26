/*
Copyright 2007 Brian Tanner
http://rl-library.googlecode.com/
brian@tannerpages.com
http://brian.tannerpages.com

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

package org.rlcommunity.tetris.visualizer;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.geom.AffineTransform;

import rlVizLib.visualization.VizComponent;

public class TetrisScoreComponent implements VizComponent{
	private TetrisVisualizer tetVis = null;
	
	int lastScore=0;

	private int lastUpdateTimeStep=-1;

	public TetrisScoreComponent(TetrisVisualizer ev){
		this.tetVis = ev;
		lastScore=-1;
	}

	public void render(Graphics2D g) {
		//This is some hacky stuff, someone better than me should clean it up
		Font f = new Font("Verdana",0,8);     
		g.setFont(f);
	    //SET COLOR
	    g.setColor(Color.BLACK);
	    //DRAW STRING
	    AffineTransform saveAT = g.getTransform();
   	    g.scale(.01, .01);
	    g.drawString("Lines: " +tetVis.getScore(),0.0f, 10.0f);
	    g.drawString("E/S/T: " +tetVis.getEpisodeNumber()+"/"+tetVis.getTimeStep()+"/"+tetVis.getTotalSteps(),0.0f, 20.0f);
	    g.drawString("CurrentPiece: " + tetVis.getCurrentPiece(), 0.0f, 30.0f);
	    g.setTransform(saveAT);
	}

	public boolean update() {
int currentTimeStep=tetVis.getGlueState().getTotalSteps();

		if(currentTimeStep!=lastUpdateTimeStep){
			tetVis.updateAgentState(false);
			lastUpdateTimeStep=currentTimeStep;
			return true;
		}
		return false;
	}
	
	
}
