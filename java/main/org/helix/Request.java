package org.helix;

import java.util.HashMap;

/**
 *
 * @author sergey
 */
public class Request {
    private HashMap<String, String> headers = new HashMap<String, String>();
    private String url;
    private String method;
    private String body;
    

    public HashMap<String, String> getHeaders() {
        return headers;
    }

    public String getUrl() {
        return url;
    }

    public String getMethod() {
        return method;
    
}
    public String getBody() {
        return body;
    }
    
    
    
    
        
}
