import java.net.MalformedURLException;
import java.net.*;
import java.io.*;

public class ContentInfo {
	public static void main(String[] args) {
		if (args.length<1) System.out.println("Nothing was given.");
		
		else {
		
			for (int i = 0; i< args.length; i++)
			{
				try
				{
					URL u = new URL(args[i]);
					URLConnection uc = u.openConnection();
					for (int j = 1; ;j++)
					{
						//if statement for content type
						//if statement for content size
						String header = uc.getHeaderField(j);
					
						if (uc.getHeaderFieldKey(j).equalsIgnoreCase("Content-type"))
						{
							if (header == null) System.out.println(uc.getHeaderFieldKey(j) + ": ");
                            System.out.println(uc.getHeaderFieldKey(j) + ": " + header);
                            System.out.println("Content-Length: " + uc.getContentLength());
                            break;
						}
						//if (uc.getHeaderFieldKey(j).equalsIgnoreCase("Content-length"))
						//{
						//	if (header == null) System.out.println(uc.getHeaderFieldKey(j) + ": -1");
						//	System.out.println(uc.getHeaderFieldKey(j) + ": " + header);
						//	break;
						//}
					}
                }catch(MalformedURLException e){
                    System.err.println(args[0] + "is not parseable");
				}catch(IOException e) {
					System.err.println(e);
				}
			}
		}

	}
}