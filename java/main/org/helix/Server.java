package org.helix;
    
import java.util.HashMap;
import java.util.Map;

public class Server {    
    
    private int connectionThreadCount = 5;
    private int poolThreadCount = 20;
    private int readTimeout = 30;
    private int writeTimeout = 30;

    public int getReadTimeout() {
        return readTimeout;
    }

    public void setReadTimeout(int readTimeout) {
        this.readTimeout = readTimeout;
    }

    public int getWriteTimeout() {
        return writeTimeout;
    }

    public void setWriteTimeout(int writeTimeout) {
        this.writeTimeout = writeTimeout;
    }
    
    public int getConnectionThreadCount() {
        return connectionThreadCount;
    }

    public void setConnectionThreadCount(int connectionThreadCount) {
        this.connectionThreadCount = connectionThreadCount;
    }

    public int getPoolThreadCount() {
        return poolThreadCount;
    }

    public void setPoolThreadCount(int poolThreadCount) {
        this.poolThreadCount = poolThreadCount;
    }
    
    private long handlerPointer;
    private long serverPointer;
    
    private static Handler handlerStatic;
 
    private native void create(int port);
    private native void start();
    private native void test(Request request, Response response);
    private native Map.Entry test1();
    
    public void start(Handler handler){
        handlerStatic = handler;
        this.start();
    }
    
    private static Handler getHandler() {
        return handlerStatic;
    }
    
    public Server(int port) {
        create(port);
    }

    static {
        System.loadLibrary("helix");
    }        

}