package org.helix;
/**
 *
 * @author sergey
 */
public interface Handler {
    abstract void onRequest( Request request, Response response );
    
}
