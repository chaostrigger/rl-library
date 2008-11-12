package org.rlcommunity.environment.acrobot.visualizer;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.geom.AffineTransform;

import java.awt.geom.Ellipse2D;
import java.awt.geom.Rectangle2D;
import rlVizLib.visualization.PollingVizComponent;

public class AcrobotBotComponent implements PollingVizComponent {
	private AcrobotVisualizer acroVis; 
	static final int joint1X = 50;
	static final int joint1Y = 30;
	static final int leg1length =25;
	static final int leg2length =leg1length;
	static final int circleSize1 =6;
	static final int circleSize2 = 4;
	static final int circleSize3 = 2;
	
        int lastUpdateStep=-1;
        
	public AcrobotBotComponent(AcrobotVisualizer acrobotVisualizer) {
		acroVis = acrobotVisualizer;
	}

	public void render(Graphics2D g) {
	    AffineTransform saveAT = g.getTransform();
            g.setColor(Color.WHITE);
            g.fill(new Rectangle(1,1));
   	    g.scale(.01, .01);
            
            g.setColor(Color.green);
            int goalY=joint1Y-leg1length;
            g.drawLine(0, goalY, 100, goalY);
	    g.setColor(Color.BLACK);
            
		int joint2X = (int) (leg1length*Math.sin(acroVis.getTheta1())+ joint1X);
		int joint2Y = (int) (leg1length*Math.cos(acroVis.getTheta1())+joint1Y);
		g.drawLine(joint1X, joint1Y, joint2X, joint2Y);
//Draw the first joint circle
                g.setColor(Color.BLUE);
                g.fill(new Ellipse2D.Float((float)joint1X-circleSize1/2,(float)joint1Y-circleSize1/2,circleSize1,circleSize1));

                int joint3X = (int) (leg2length*Math.cos(Math.PI/2-acroVis.getTheta2()-acroVis.getTheta1()) + joint2X);
		int joint3Y= (int) (leg2length*Math.sin(Math.PI/2-acroVis.getTheta1()-acroVis.getTheta2()) + joint2Y);
	    g.setColor(Color.BLACK);
		g.drawLine(joint2X, joint2Y, joint3X, joint3Y);
//Second circle
                g.setColor(Color.BLUE);
                g.fill(new Ellipse2D.Float((float)joint2X-circleSize2/2,(float)joint2Y-circleSize2/2,circleSize2,circleSize2));

                //                System.out.printf("(%d %d) --> (%d %d)\n",joint2X,joint2Y,joint3X,joint3Y);
                
//Feet
                g.setColor(Color.CYAN);
                g.fill(new Ellipse2D.Float((float)joint3X-circleSize3/2,(float)joint3Y-circleSize3/2,circleSize3,circleSize3));
	    g.setTransform(saveAT);
	}

	public boolean update() {
            int thisUpdateStep=acroVis.getTheGlueState().getTotalSteps();
            
            if(thisUpdateStep>lastUpdateStep){
                lastUpdateStep=thisUpdateStep;
                System.out.println("Printing step: "+lastUpdateStep);
                return false;
            }
            return false;
	}

}
