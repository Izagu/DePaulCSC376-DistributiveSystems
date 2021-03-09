
import java.net.MalformedURLException;
import java.net.URL;

public class URLInfo {
	public static void main(String[] args) {
		if (args.length<1) System.out.println("Nothing was given.");
		
		else getUrlInfo(args[0]);
	}
	private static void getUrlInfo(String url) {
		
		try
		{
			URL u = new URL(url);
			String proto = u.getProtocol();
			System.out.println("Protocol:  " + proto );
		}catch(MalformedURLException e) {
			System.out.println("Protocol:   " );
		}
		try
		{
			URL u = new URL(url);
			String usr = u.getUserInfo();
			System.out.println("User Info:  " + usr );
		}catch(MalformedURLException e) {
			System.out.println("User Info:   null" );
		}
		try
		{
			URL u = new URL(url);
			String host = u.getHost();
			System.out.println("Host:  " + host );
		}catch(MalformedURLException e) {
			System.out.println("Host:   null" );
		}
		try
		{
			URL u = new URL(url);
			int port = u.getPort();
			System.out.println("Port:  " + port );
		}catch(MalformedURLException e) {
			System.out.println("Port:  -1 " );
		}
		try
		{
			URL u = new URL(url);
			String path = u.getPath();
			System.out.println("Path:  " + path );
		}catch(MalformedURLException e) {
			System.out.println("Path:   " );
		}
		try
		{
			URL u = new URL(url);
			String query = u.getQuery();
			System.out.println("Query:  " + query );
		}catch(MalformedURLException e) {
			System.out.println("Query:   " );
		}
	}
}