import java.io.*;
import java.net.*;
import java.util.regex.*;

class	FinalProjTester
{

  public static String getPage	(URL	url)
  {
    String	toReturn	= "";
    InputStream	in		= null;

    try
    {

      in = url.openStream();

      // buffer the input to increase performance
      in = new BufferedInputStream(in);

      // chain the InputStream to a Reader
      Reader r = new InputStreamReader(in);
      int    c;

      while ((c = r.read()) != -1 )
      {
	toReturn += (char)c;
      }
    }
    catch (IOException ex)
    {
      System.err.println(ex);
    }
    finally
    {
      if (in != null)
      {
	try
	{
	  in.close();
	}
	catch (IOException e)
	{
	  // ignore
	}
      }
    }

    return(toReturn);
  }

  public static void chooseVar	(QueryString	query,
				 BufferedReader	reader,
				 String		varName,
				 String[]	choiceArray
				)
				throws IOException
  {
    int		arrayIndex;

    do
    {
      System.out.println(varName + ":");

      for  (int i = 0;  i < choiceArray.length;  i++)
      {
	System.out.println(i + ": " + choiceArray[i]);
      }

      System.out.print("Your choice: ");

      try
      {
	arrayIndex	= Integer.parseInt(reader.readLine());
      }
      catch (NumberFormatException err)
      {
	arrayIndex	= -1;
      }
    }
    while  ( (arrayIndex < 0) || (arrayIndex >= choiceArray.length) );

    query.add(varName,choiceArray[arrayIndex]);
  }


  public static void main (String args[])
  {
    if (args.length < 1)
    {
      System.err.println("Usage:\tjava FinalProjTester <url>\n");
      System.exit(1);
    }

    String	urlString	= args[0];
    String[]	xAxisArray	= {"pl_orbper","pl_orbeccen","pl_bmassj",
    				   "pl_radj","st_dist","st_teff","st_mass",
				   "st_rad"
    				  };
    String[]	yAxisArray	= {"pl_orbper","pl_orbeccen","pl_bmassj",
    				   "pl_radj","st_dist","st_teff","st_mass",
				   "st_rad"
    				  };

    try
    {
      BufferedReader	reader	= new BufferedReader
					(new InputStreamReader(System.in));
      QueryString	query	= new QueryString();

      chooseVar(query,reader,"xAxis",xAxisArray);
      chooseVar(query,reader,"yAxis",yAxisArray);

      URL		url	= new URL(urlString);
      String		protocol= url.getProtocol();
      String		host	= url.getHost();
      String		path	= "exoplanets.swt";
      int		port	= url.getPort();

      url			= new URL(protocol,
					  host,
					  port,
					  path + "?" + query
					 );
      String		text	= getPage(url);
      PrintWriter	out	= new PrintWriter("downloaded.html");

      System.out.println(urlString);
      System.out.println(text);
      out.println(text);
      out.close();
    }
    catch (MalformedURLException ex)
    {
      System.err.println(args[0] + " is not a parseable URL");
    }
    catch (IOException ex)
    {
      System.err.println("IOException: " + ex);
    }
  }

}
