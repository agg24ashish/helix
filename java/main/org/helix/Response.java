package org.helix;

import java.util.HashMap;   
import java.util.Iterator;
import java.util.Map;

/**
 *
 * @author sergey
 */
public class Response {
    private HashMap<String, String> headers = new HashMap<String, String>();
    
    private String body = "";
    private int status = 200;

    public int getStatus() {
        return status;
    }

    public void setStatus(int status) {
        this.status = status;
    }
    
    public String getBody() {
        return body;
    }

    public void setBody(String body) {
        this.body = body;
    } 
   
    public void addHeader(String name, String value) {
        
        this.headers.put(name, value);
    }
}
