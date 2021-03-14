import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Scanner;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;




public class FinalProjTester {
	
	
	// REMOVES DOUBLE QUOTE
	private static String trim(String arg) 
	{
		String s = arg.substring(1, arg.length()-1);
		return s;
	}
	// GETS/READ JSON
	private static void getJson(String file)
	{	
		JSONParser parse = new JSONParser();
		try
		{
			Object Obj = parse.parse(new FileReader(file));
			JSONArray toJsonArray = (JSONArray) Obj; //type casting to Json Object
			System.out.println(toJsonArray);
		}
		catch(FileNotFoundException e){e.printStackTrace();}
		catch(ParseException e){e.printStackTrace();}
		catch(IOException e){e.printStackTrace();}
	}
	
	//SEND HTTP REQUEST
	private static HttpURLConnection connect(String argUrl)
	{
		HttpURLConnection connection = null;
	     try 
	     {
	    	 URL url = new URL(argUrl);
	    	 connection = (HttpURLConnection) url.openConnection();
	    	 
	     }
	     catch (IOException e) {
	    	 e.printStackTrace();
	    	 System.out.println("I/O Error");
	     }
	     return connection;
	}
	
	//WRITES A JSON FILE
	private static void writeIt(HttpURLConnection conn)
	{
		try
		{
			BufferedReader in = new BufferedReader(new InputStreamReader(conn.getInputStream()));
			String line;
	        FileWriter	writer = new FileWriter(new File("JSONFile.json"));
	        while ((line = in.readLine()) != null)
	        {
	        	writer.write(line);
	        }
			writer.close();
	        in.close();
	        System.out.println("writer and in have been closed");
		}
		catch (IOException e) {e.printStackTrace();}
		
	}
	//READS JSON FROM THE COMMAND LINE
	public static String readIt(String input)
	{
		//maybe use
		Object doThis = null;
		try 
		{
			JSONParser parse = new JSONParser();
			Object Obj = parse.parse(input);
			JSONObject toJsonObj = (JSONObject) Obj;
			doThis = toJsonObj.get("command");
			
		} catch(ParseException e){e.printStackTrace();}
		return doThis.toString();
	}
	
	
	public static void main(String[] args) {
		if( args.length == 0) {
			System.out.println("No argument was given");
			//have a return here
		}
		String trimmedURL = trim(args[1]); 
		System.out.println("URL has been trimmed");
		HttpURLConnection stream = connect(trimmedURL);
		System.out.println("Established connection");
		writeIt(stream);
		System.out.println("Json has been written");
		
		//Executes a loop where it will get JSON commands
		//from the webserver from its standard input.
		String input;
		BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
		try 
		{
			while((input = reader.readLine()) != null)
			{
				String command = readIt(input);
				if(command.equals("quit"))
				{
					System.out.println("{\"status\":\"quitting\"}");
					break;
				}
				else
				{
					System.out.println(command);
				}
			}
					
		}
		catch(IOException e){e.printStackTrace();}
		
		
	}

}
