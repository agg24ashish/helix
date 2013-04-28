package org.helix;
import java.util.Iterator;
import java.util.Map;


class  TestHandler implements Handler
{
    public void onRequest( Request request, Response response )
    {
        System.out.println("request");
        
        Iterator it = request.getHeaders().entrySet().iterator();
        
        while (it.hasNext()) {
            Map.Entry pairs = (Map.Entry)it.next();
            System.out.println(pairs.getKey() + " = " + pairs.getValue());
        }
        
        System.out.println(String.format("%s %s %s", request.getUrl(), request.getMethod(), request.getBody()));
        
        response.setBody("test");
        response.addHeader("test", "test"); 
        
        
    }
}
        
class Test {
    public static void main(String[] args) {
       Server server = new Server(8000);
        
       server.start( new TestHandler() );
        
        
       
    }
        
}
