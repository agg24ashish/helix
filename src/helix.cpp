#include <jni.h>
#include "Helix.h"

#include  <stdlib.h>
#include <propeller/HttpServer.h>

using namespace propeller;

static JavaVM* vm = NULL;

struct ThreadContext
{
    JNIEnv* env;
    jobject handler;
};

class Handler : public Server::EventHandler
{
public:

    virtual void onRequest( const Request& request, Response& response, sys::ThreadPool::Worker&  thread )
    {
         
         
         http::Request& httpRequest = ( http::Request& )request;
         http::Response& httpResponse = ( http::Response& )response;
         

        ThreadContext* context = ( ThreadContext* ) thread.data();
        
        if ( !context )
        {
            //
            //  create thread context
            //
            context = new ThreadContext();
            JNIEnv* env;
            vm->AttachCurrentThread( (void **) &env, NULL );
            
            jclass klass = env->FindClass( "org/helix/Server" );

            context->handler =  env->CallStaticObjectMethod( 
                klass, 
                env->GetStaticMethodID( klass, "getHandler", "()Lorg/helix/Handler;" )
            );
            
            context->env = env;
            
            thread.setData( context );
            
        }
        
        //
        //  create request object
        //
        jclass requestClass = context->env->FindClass( "org/helix/Request" );
        
        jobject requestObject =  context->env->NewObject( 
            requestClass,
            context->env->GetMethodID( requestClass, "<init>", "()V" )    
            );
        
        context->env->NewLocalRef( requestObject );
        
        jobject headersObject = context->env->GetObjectField( 
                requestObject, 
                context->env->GetFieldID( 
                  requestClass,
                  "headers",
                  "Ljava/util/HashMap;"
                )
            );
            
        for (http::Request::HeaderMap::const_iterator i = httpRequest.headers().begin(); i != httpRequest.headers().end(); i++ )
        {
            context->env->CallObjectMethod( 
                headersObject, 
                context->env->GetMethodID( context->env->FindClass("java/util/HashMap"), "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;"),
                context->env->NewStringUTF( i->first.c_str() ),
                context->env->NewStringUTF( i->second.c_str() )
            );
        }
        
        
        context->env->SetObjectField( 
            requestObject,
            context->env->GetFieldID( requestClass, "url", "Ljava/lang/String;"),
            context->env->NewStringUTF( httpRequest.uri() )
        );
        
        context->env->SetObjectField( 
            requestObject,
            context->env->GetFieldID( requestClass, "method", "Ljava/lang/String;"),
            context->env->NewStringUTF( httpRequest.method() )
        );
        
        context->env->SetObjectField( 
            requestObject,
            context->env->GetFieldID( requestClass, "body", "Ljava/lang/String;"),
            context->env->NewStringUTF( httpRequest.body() )
        );
        
        //
        //  create response object
        //
       jclass responseClass = context->env->FindClass( "org/helix/Response" );
        
        jobject responseObject =  context->env->NewObject( 
            responseClass,
            context->env->GetMethodID( responseClass, "<init>", "()V" )    
            );
        
        context->env->NewLocalRef( responseObject );
        
        context->env->CallVoidMethod( 
            context->handler,
            context->env->GetMethodID( context->env->FindClass( "org/helix/Handler" ), "onRequest", "(Lorg/helix/Request;Lorg/helix/Response;)V" ),
            requestObject,
            responseObject
        );
        
        headersObject = context->env->GetObjectField( 
                responseObject, 
                context->env->GetFieldID( 
                    responseClass,
                    "headers",
                    "Ljava/util/HashMap;"
                    )
            );
        
        jobject entrySet = context->env->CallObjectMethod(
                headersObject,
                context->env->GetMethodID( context->env->FindClass( "java/util/HashMap" ), "entrySet", "()Ljava/util/Set;" )
                );
        
        
        jobject iterator = context->env->CallObjectMethod(
                entrySet,
                context->env->GetMethodID( context->env->FindClass( "java/util/Set" ), "iterator", "()Ljava/util/Iterator;" )
                );
        
        jclass iteratorClass = context->env->FindClass( "java/util/Iterator" );
        jmethodID hasNextMethod = context->env->GetMethodID( iteratorClass, "hasNext", "()Z" );    
        jmethodID nextMethod = context->env->GetMethodID( iteratorClass, "next", "()Ljava/lang/Object;" );    
        
        int status = context->env->GetIntField( 
            responseObject,
            context->env->GetFieldID( responseClass, "status", "I" )
            );
        
        httpResponse.setStatus( status );
      
        for ( ;; )
        {
            
            jboolean hasNext = context->env->CallBooleanMethod(
                iterator,
                hasNextMethod
            );
            
            if ( !hasNext )
            {
                break;
            }   
            
            jobject entry = context->env->CallObjectMethod( 
                    iterator,
                    nextMethod
                  );    
            
            jobject key = context->env->CallObjectMethod( 
                    entry, 
                    context->env->GetMethodID( context->env->FindClass( "java/util/Map$Entry"), "getKey", "()Ljava/lang/Object;")
                    );
            
            jobject value = context->env->CallObjectMethod( 
                    entry, 
                    context->env->GetMethodID( context->env->FindClass( "java/util/Map$Entry"), "getValue", "()Ljava/lang/Object;")
                    );
            
            
            
            const char* keyString = context->env->GetStringUTFChars( ( jstring ) key, 0 );
            const char* valueString = context->env->GetStringUTFChars( ( jstring ) value, 0 );
            
            
            httpResponse.addHeader( keyString, valueString );
            
            context->env->ReleaseStringUTFChars( ( jstring ) key, keyString );
            context->env->ReleaseStringUTFChars( ( jstring ) value, valueString );
        }
        
        jobject body = context->env->GetObjectField( 
            responseObject, 
            context->env->GetFieldID( responseClass, "body", "Ljava/lang/String;")
            );
        
        
        const char* bodyString = context->env->GetStringUTFChars( ( jstring ) body, 0 );
        httpResponse.setBody( bodyString );
        context->env->ReleaseStringUTFChars( ( jstring ) body, bodyString );
        
        context->env->DeleteLocalRef( requestObject );
        context->env->DeleteLocalRef( responseObject );
        
    }
    
    
};


/*
 * Class:     org_helix_Server
 * Method:    create
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_helix_Server_create( JNIEnv* env, jobject obj, jint port )
{
    env->GetJavaVM( &vm );

    Handler* handler = new Handler();
    
    env->SetLongField( 
        obj, 
        env->GetFieldID( env->GetObjectClass( obj ), "handlerPointer", "J" ),
        ( long ) handler 
    );

    http::Server*  server = new http::Server( 8000,  ( Server::EventHandler& ) *handler );
    
    env->SetLongField( 
        obj, 
        env->GetFieldID( env->GetObjectClass( obj ), "serverPointer", "J" ), 
        ( long ) server 
    );    
}

/*
 * Class:     org_helix_Server
 * Method:    start
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_helix_Server_start( JNIEnv* env, jobject obj )  
{
    Handler* handler = ( Handler* )env->GetLongField( 
        obj, 
        env->GetFieldID( env->GetObjectClass( obj ), "handlerPointer", "J" )
    );
    
    http::Server* server = ( http::Server* )env->GetLongField( 
        obj, 
        env->GetFieldID( env->GetObjectClass( obj ), "serverPointer", "J" )
    );
    
    int readTimeout = env->GetIntField( 
        obj, 
        env->GetFieldID( env->GetObjectClass( obj ), "readTimeout", "I" )
    );
    
    server->setConnectionReadTimeout( ( unsigned int ) readTimeout );
    
    int writeTimeout = env->GetIntField( 
        obj, 
        env->GetFieldID( env->GetObjectClass( obj ), "writeTimeout", "I" )
    );
    
    server->setConnectionWriteTimeout( ( unsigned int ) writeTimeout );
    
    int connectionThreadCount = env->GetIntField( 
        obj, 
        env->GetFieldID( env->GetObjectClass( obj ), "connectionThreadCount", "I" )
    );
    
    server->setConnectionThreadCount( ( unsigned int ) connectionThreadCount );

    int poolThreadCount = env->GetIntField( 
        obj, 
        env->GetFieldID( env->GetObjectClass( obj ), "poolThreadCount", "I" )
    );
    
    server->setPoolThreadCount( ( unsigned int ) poolThreadCount );
    
    
    
    server->start();
    
}

