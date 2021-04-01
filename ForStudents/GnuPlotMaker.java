import java.io.BufferedReader;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.*;
import java.io.*;
//import com.google.gson.*;
import org.json.simple.*;
import org.json.simple.parser.*;

public class GnuPlotMaker {
	// REMOVES DOUBLE QUOTE
		private static String trim(String arg) 
		{
			String s = arg.substring(1, arg.length()-1);
			return s;
		}
		
	//SEND HTTP REQUEST
		private static HttpURLConnection connect(String argUrl) throws Exception
		{
			HttpURLConnection connection = null;
			URL url = new URL(argUrl);
			connection = (HttpURLConnection) url.openConnection();
		    return connection;
		}
		
	//WRITES A JSON FILE
		private static void saveIt(HttpURLConnection conn) throws Exception
		{
			BufferedReader in = new BufferedReader(new InputStreamReader(conn.getInputStream()));
			//System.out.println("buffer reader was created.");
			String line;
		    FileWriter writer = new FileWriter(new File("JSONFile.json"));
			//System.out.println("File Writer was created.");
			while ((line = in.readLine()) != null)
		    {
				writer.write(line);
		        //System.out.println(line);
		    }
			writer.close();
		    in.close();
		}
		
	//READS JSON FROM THE COMMAND LINE
		public static String readIt(String input, String key) throws Exception
		{
			//maybe use
			Object doThis = null;
			JSONParser parse = new JSONParser();
			Object Obj = parse.parse(input);
			JSONObject toJsonObj = (JSONObject) Obj;
			doThis = toJsonObj.get(key);
			return doThis.toString();
		}
		
	// GETS/READ JSON FOR GNU PLOT 
		private static void writeXY(String file, String x, String y) throws Exception
		{	
			JSONParser parse = new JSONParser();
			Object Obj = parse.parse(new FileReader(file));
			//System.out.println("i am here");
			JSONArray toJsonArray = (JSONArray) Obj;//type casting to Json Array
			//System.out.println("i am here 2");
				
			FileWriter	writer = new FileWriter(new File("data.txt"));
			//System.out.println("i am here 3");
			for(int i = 0; i<toJsonArray.size(); i++)
			{
				JSONObject toJsonObj = (JSONObject) toJsonArray.get(i);
				Object xVar = toJsonObj.get(x);
				Object yVar = toJsonObj.get(y);
				if( xVar == null || yVar == null) i++;
				else 
				{
				String line = (xVar + " " + yVar+"\n");
				//System.out.println(line);
				writer.write(line);
				} 
			}
			writer.close();
		}
		
	//WRITES GNU FILE CONTAINING GNU COMMANDS
		private static void initGnuFile(String x,String y) throws Exception
		{
			String line = String.format("set term png size 640,480\n"
					                  + "set output \"scatterPlot.png\"\n"
					                  + "set title \"%s as a fnc of %s\"\n"
					                  + "plot \"./data.txt\" with points pt 1\n"
					                  + "quit", 
					                  x, y); //where x and y are the x and y axis
			FileWriter	writer = new FileWriter(new File("scatter.gnu"));
			writer.write(line);
			writer.close();
		
		}
	//FORKS A NEW PROCESS TO RUN GNU PLOT	
		private static void forkIt() throws Exception
		{
			
				Runtime runtimeProcess = Runtime.getRuntime();
				Process process = runtimeProcess.exec("/usr/bin/gnuplot ./scatter.gnu");
				process.waitFor();
		}
		
	//MAINNIE MAIN
		public static void main(String[] args) throws Exception
		{
			if( args.length == 0) {
				throw new IllegalArgumentException();
			}
			String trimmedURL = trim(args[0]);
		    //System.out.println(trimmedURL); //for testing purposes
			HttpURLConnection stream = connect(trimmedURL);
			//System.out.println("connection accomplished"); //for testing purposes
			saveIt(stream);
			//System.out.println("The json file was saved");
			String input;
			BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
			//System.out.println("Please enter a command"); //for testing purposes
			while((input = reader.readLine()) != null)
			{
				String command = readIt(input,"command");
				if(command.contains("quit"))
				{
					System.out.println("{\"status\":\"quitting\"}");
					break;
				}
				else if (command.contains("plot"))
				{
					String x = readIt(input, "xAxis");
					String y = readIt(input, "yAxis");
					writeXY("JSONFile.json",x,y);
				//	System.out.println("data has been written");
					initGnuFile(x,y);
					forkIt();
			       		PrintWriter writer = new PrintWriter(System.out); 
					writer.write("{\"status\":\"success\",\"imageFilename\":\"scatterPlot.png\",\"height\":360,\"width\":480}");
					writer.flush();		
					//{System.out.println("{\"status\":\"success\",\"imageFilename\":\"scatterPlot.png\",\"height\":360,\"width\":480}");
					//{"status":"success","imageFilename":"scatterPlot.png","height":360,"width":480}
				}
				else {System.err.println("AHHHH");}
				}		
		}
}
